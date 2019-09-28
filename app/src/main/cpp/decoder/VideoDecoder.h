//
// Created by weiqianghu on 2019/9/2.
//

#ifndef VIDEOPLAYER_VIDEODECODER_H
#define VIDEOPLAYER_VIDEODECODER_H

#ifndef UINT64_C
#define UINT64_C(value)__CONCAT(value,ULL)
#endif

#ifndef INT64_MIN
#define INT64_MIN  (-9223372036854775807LL - 1)
#endif

#ifndef INT64_MAX
#define INT64_MAX    9223372036854775807LL
#endif

#include <list>
#include <vector>
#include <jni.h>
#include "DecoderRequestHeader.h"
#include "../common/opengl_media/MovieFrame.h"
#include "../video_player/texture_uploader/TextureFrameUploader.h"
#include "../common/opengl_media/texture/TextureFrame.h"
#include <EGL/egl.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

using namespace std;

#ifndef SUBSCRIBE_VIDEO_DATA_TIME_OUT
#define SUBSCRIBE_VIDEO_DATA_TIME_OUT 20 * 1000
#endif

#ifndef DECODE_PROBESIZE_DEFAULT_VALUE
#define DECODE_PROBESIZE_DEFAULT_VALUE 50 * 1024
#endif

#define DECODER_HEADER_FORCE_ROTATE            "header_force_rotate"
#define DECODER_HEADER_FORCE_FPS            "header_force_fps"

typedef struct {
    long long beginOpen;
    float successOpen;
    float firstScreenTimeMills;
    float failOpen;
    int failOpenType;
    float duration;
    vector<float> retryOpen;
    vector<float> videoQueueFull;
    vector<float> videoQueueEmpty;
    //	vector<float> videoLoading;		// 加载中，正在缓冲
    //	vector<float> videoContinue;	// 缓冲结束，开始播放
    //	float stopPlay;                 // 结束
} BuriedPoint;

class VideoDecoder {
protected:
    int subscribeTimeOutTimeMills{};
    DecoderRequestHeader *requestHeader{};
    float seek_seconds{};
    int64_t audio_stream_duration{};

    TextureFrameUploader *textureFrameUploader{};
    pthread_mutex_t mLock{};
    pthread_cond_t mCondition{};

    UploaderCallback *mUploaderCallback{};

    //保存需要读入的文件的格式信息，比如流的个数以及流数据等
    AVFormatContext *pFormatCtx{};

    int openInput();

    bool hasAllCodecParameters();

    virtual int openFormatInput(char *videoSourceURI);

    virtual void initFFMpegContext();

    virtual bool isNeedRetry();

    virtual int initAnalyzeDurationAndProbesize(int *max_analyze_durations, int analyzeDurationSize,
                                                int probesize, bool fpsProbeSizeConfigured);

    bool isNetworkPath(char *videoSourceURI) {
        bool result = false;
        int length = 0;
        char *path = videoSourceURI;
        char c = ':';
        while (*path != '\0' && *path != c) {
            length++;
            path++;
        }
        if (*path == c) {
            char *scheme = new char[length];
            char *schemeTmp = scheme;
            while (length--) {
                *(schemeTmp++) = *(videoSourceURI++);
            }
            *(schemeTmp++) = '\0';
            if (!strcmp(scheme, "rtmp") || !strcmp(scheme, "RTMP")) {
                result = true;
            }
            delete[] scheme;
        }
        return result;
    }

public:
    int connectionRetry = 0;

    bool seek_req{};
    bool seek_resp{};
    bool isSubscribe{};
    bool isOpenInputSuccess{};
    AVIOInterruptCB int_cb{};
    long long readLatestFrameTimemills{};
    bool isTimeout{};
    bool is_Network{};
    bool is_eof{};
    float position{};
    AVCodecContext *videoCodecContext{};
    AVCodec *videoCodec{};
    AVFrame *videoFrame{};
    int width{};
    int height{};
    int degres{};
    float fps{};
    float videoTimeBase{};
    std::list<int> *videoStreams{};
    int videoStreamIndex{};

    /** 音频流解码变量 **/
    AVCodecContext *audioCodecCtx{};
    AVCodec *audioCodec{};
    AVFrame *audioFrame{};
    std::list<int> *audioStreams{};
    int audioStreamIndex{};
    float audioTimeBase{};
    SwrContext *swrContext{};
    void *swrBuffer{};
    int swrBufferSize{};

    /** 字幕流解码变量 **/
    std::list<int> *subtitleStreams{};
    int subtitleStreamIndex{};
    int artworkStreamIndex{};

    int openVideoStream();

    int openVideoStream(int streamIndex);

    int openAudioStream();

    int openAudioStream(int streamIndex);

    //判断codecContext的sampleFmt是否是AV_SAMPLE_FMT_S16的
    bool audioCodecIsSupported(AVCodecContext *audioCodecCtx);

    std::list<int> *collectStreams(enum AVMediaType codecType);

    void avStreamFPSTimeBase(AVStream *st, float defaultTimeBase, float *pFPS, float *pTimeBase);

    VideoFrame *handleVideoFrame();

    void copyFrameData(uint8_t *dst, uint8_t *src, int width, int height, int lineSize);

    AudioFrame *handleAudioFrame();

    void closeSubtitleStream();

    void closeAudioStream();

public:
    VideoDecoder();

    VideoDecoder(JavaVM *g_jvm, jobject obj);

    virtual ~VideoDecoder();

    void uploadTexture();

    void signalDecodeThread();

    virtual int openFile(DecoderRequestHeader *requestheader);

    void startUploader(UploaderCallback *uploaderCallback);

    std::list<MovieFrame *> decodeFrames(float minDuration);

    std::list<MovieFrame *> decodeFrames(float minDuration, int *decodeVideoErrorStata);

    bool decodeVideoTexIdByPosition(float position);

    bool hasSeekReq() {
        return seek_req;
    }

    bool hasSeekResp() {
        return seek_resp;
    }

    EGLContext getTextureFrameUploaderEGLContext() {
        if (nullptr != textureFrameUploader) {
            return textureFrameUploader->getEGLContext();
        }
        return nullptr;
    }

    void setSeekReq(bool seekReqParam) {
        seek_req = seekReqParam;
        if (seek_req) {
            seek_resp = false;
        }
    }

    bool isOpenInput() {
        return isOpenInputSuccess;
    }

    void stopSubScribe() {
        subscribeTimeOutTimeMills = -1;
        isSubscribe = false;
    }

    bool isSubscribed() {
        return isSubscribe;
    }

    static int interrupt_cb(void *ctx);

    int detectInterrupted();

    inline void interrupt() {
        subscribeTimeOutTimeMills = -1;
    }

    bool isEof() {
        return isVideoOutputEOF;
    }

    virtual bool isNetwork() {
        is_Network = false;
        return is_Network;
    }

    bool validAudio() {
        return audioStreamIndex != -1;
    }

    bool validVideo() {
        return videoStreamIndex != -1;
    }

    bool validSubtitles() {
        return subtitleStreamIndex != -1;
    }

    void setPosition(float seconds);

    void closeFile();

    float getPosition() {
        return position;
    }

    float getDuration() {
        if (!pFormatCtx) {
            return 0;
        }
        if (pFormatCtx->duration == AV_NOPTS_VALUE) {
            return -1;
        }
        return (float) pFormatCtx->duration / AV_TIME_BASE;
    }

    int getVideoRotateHint() {
        return degres;
    }

    int getVideoFrameWidth() {
        if (videoCodecContext) {
            return videoCodecContext->width;
        }
        return -1;
    };

    float getVideoFPS() {
        return fps;
    };

    /** 获得视频帧的高度 **/
    int getVideoFrameHeight() {
        if (videoCodecContext) {
            return videoCodecContext->height;
        }
        return -1;
    };

    int getAudioChannels() {
        if (audioCodecCtx) {
            return audioCodecCtx->channels;
        }
        return -1;
    }

    int getAudioSampleRate() {
        if (audioCodecCtx) {
            return audioCodecCtx->sample_rate;
        }
        return -1;
    };

    /** 解码音频 **/
    bool
    decodeAudioFrames(AVPacket *packet, std::list<MovieFrame *> *result, float &decodedDuration,
                      float minDuration, int *decodeVideoErrorState);

    /** flush音频 **/
    void flushAudioFrames(AVPacket *audioPacket, std::list<MovieFrame *> *result, float minDuration,
                          int *decodeVideoErrorState);

protected:
    virtual TextureFrameUploader *createTextureFrameUploader() = 0;

    virtual void seek_frame();

    virtual bool decodeVideoFrame(AVPacket packet, int *decodeVideoErrorState) = 0;

    virtual void flushVideoFrames(AVPacket packet, int *decodeVideoErrorState) = 0;

    virtual void closeVideoStream();

public:
    virtual float updateTexImage(TextureFrame *textureFrame) = 0;

public:
    JavaVM *g_jvm{};
    jobject obj{};
    bool isVideoOutputEOF{};
    bool isAudioOutputEOF{};
};


#endif //VIDEOPLAYER_VIDEODECODER_H
