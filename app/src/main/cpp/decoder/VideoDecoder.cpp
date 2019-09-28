//
// Created by weiqianghu on 2019/9/2.
//

#include <pthread.h>
#include "VideoDecoder.h"
#include "../Log.h"

#define min(a, b)  (((a) < (b)) ? (a) : (b))

VideoDecoder::VideoDecoder(JavaVM *g_jvm, jobject obj) {
    this->g_jvm = g_jvm;
    this->obj = obj;

    connectionRetry = 0;
}

VideoDecoder::VideoDecoder() = default;

VideoDecoder::~VideoDecoder() = default;

static float update_tex_image_callback(TextureFrame *textureFrame, void *context) {
    auto *videoDecoder = (VideoDecoder *) context;
    return videoDecoder->updateTexImage(textureFrame);
}

static void signal_decoder_thread_callback(void *context) {
    auto *videoDecoder = (VideoDecoder *) context;
    videoDecoder->signalDecodeThread();
}

void VideoDecoder::initFFMpegContext() {
    avcodec_register_all();
    av_register_all();
}

int VideoDecoder::openFile(DecoderRequestHeader *requestheader) {
    isSubscribe = true;
    isOpenInputSuccess = false;
    position = 0.0f;
    seek_req = false;
    seek_resp = false;
    pFormatCtx = nullptr;
    subscribeTimeOutTimeMills = SUBSCRIBE_VIDEO_DATA_TIME_OUT;

    videoCodecContext = nullptr;
    videoFrame = nullptr;
    videoStreams = nullptr;

    swrContext = nullptr;
    swrBuffer = nullptr;
    audioCodecCtx = nullptr;
    audioStreams = nullptr;
    audioFrame = nullptr;

    textureFrameUploader = nullptr;

    subtitleStreams = nullptr;
    this->requestHeader = requestheader;

    this->initFFMpegContext();

    int errorCode = openInput();
    if (errorCode > 0) {
        int videoErr = openVideoStream();
        int audioErr = openAudioStream();
        if (videoErr < 0 && audioErr < 0) {
            errorCode = -1;
        } else {
            subtitleStreams = collectStreams(AVMEDIA_TYPE_SUBTITLE);
        }
    } else {
        LOGE("open input failed, have to return");
        return errorCode;
    }

    isOpenInputSuccess = true;
    isVideoOutputEOF = false;
    isAudioOutputEOF = false;
    return errorCode;
}

void VideoDecoder::startUploader(UploaderCallback *uploaderCallback) {
    mUploaderCallback = uploaderCallback;
    textureFrameUploader=createTextureFrameUploader();

}

