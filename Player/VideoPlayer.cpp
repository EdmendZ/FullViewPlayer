#include "VideoPlayer.h"
#include <iostream>

VideoPlayer::VideoPlayer(const char* filePath) : filePath(filePath) {}

bool VideoPlayer::initialize() {
    pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, filePath, nullptr, nullptr) != 0) {
        std::cerr << "Could not open video file." << std::endl;
        return false;
    }

    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
        std::cerr << "Could not find stream information." << std::endl;
        return false;
    }

    for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1) {
        std::cerr << "Could not find a video stream." << std::endl;
        return false;
    }

    AVCodecParameters* pCodecPar = pFormatCtx->streams[videoStream]->codecpar;
    const AVCodec* pCodec = avcodec_find_decoder(pCodecPar->codec_id);
    if (pCodec == nullptr) {
        std::cerr << "Unsupported codec!" << std::endl;
        return false;
    }

    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (!pCodecCtx) {
        std::cerr << "Failed to allocate the codec context!" << std::endl;
        return false;
    }

    if (avcodec_parameters_to_context(pCodecCtx, pCodecPar) < 0) {
        std::cerr << "Couldn't copy codec context." << std::endl;
        return false;
    }

    if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) {
        std::cerr << "Couldn't open codec." << std::endl;
        return false;
    }

    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();
    if (pFrameRGB == nullptr || pFrame == nullptr) {
        std::cerr << "Could not allocate frame." << std::endl;
        return false;
    }

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 32);
    buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1);

    sws_ctx = sws_getContext(
        pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
        pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pCodecCtx->width, pCodecCtx->height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    return true;
}

bool VideoPlayer::decodeNextFrame() {
    AVPacket packet;
    if (av_read_frame(pFormatCtx, &packet) >= 0) {
        if (packet.stream_index == videoStream) {
            int ret = avcodec_send_packet(pCodecCtx, &packet);
            if (ret >= 0) {
                ret = avcodec_receive_frame(pCodecCtx, pFrame);
                if (ret >= 0) {
                    sws_scale(
                        sws_ctx,
                        (uint8_t const* const*)pFrame->data,
                        pFrame->linesize,
                        0,
                        pCodecCtx->height,
                        pFrameRGB->data,
                        pFrameRGB->linesize
                    );
                    av_packet_unref(&packet);
                    return true;
                }
            }
        }
        av_packet_unref(&packet);
    }
    return false;
}

void VideoPlayer::updateTexture() {
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pCodecCtx->width, pCodecCtx->height, GL_RGB, GL_UNSIGNED_BYTE, pFrameRGB->data[0]);
}

GLuint VideoPlayer::getTextureID() {
    return textureID;
}

void VideoPlayer::cleanup() {
    av_free(buffer);
    av_frame_free(&pFrameRGB);
    av_frame_free(&pFrame);
    avcodec_free_context(&pCodecCtx);
    avformat_close_input(&pFormatCtx);
    sws_freeContext(sws_ctx);
}
