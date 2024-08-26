#pragma once
#include <GL/glew.h>
#include <string>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

class VideoPlayer {
public:
    VideoPlayer(const char* filePath);
    bool initialize();
    bool decodeNextFrame();
    void updateTexture();
    GLuint getTextureID();
    void cleanup();
private:
    const char* filePath;
    AVFormatContext* pFormatCtx = nullptr;
    AVCodecContext* pCodecCtx = nullptr;
    AVFrame* pFrame = nullptr;
    AVFrame* pFrameRGB = nullptr;
    SwsContext* sws_ctx = nullptr;
    uint8_t* buffer = nullptr;
    int videoStream = -1;
    GLuint textureID = 0; // 将 textureID 初始化为 0
};
