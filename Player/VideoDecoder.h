#ifndef VIDEODECODER_H
#define VIDEODECODER_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include <string>

class VideoDecoder {
public:
    VideoDecoder(const std::string& filePath);
    ~VideoDecoder();

    bool initialize();
    bool decodeFrame();
    uint8_t* getFrameData();
    int getWidth() const;
    int getHeight() const;
    double getFrameRate() const;
    void printVideoInfo() const;


private:
    std::string filePath;
    AVFormatContext* pFormatCtx;
    AVCodecContext* pCodecCtx;
    AVFrame* pFrame;
    AVFrame* pFrameRGB;
    struct SwsContext* swsCtx;
    uint8_t* buffer;
    int videoStream;
};

#endif // VIDEODECODER_H
