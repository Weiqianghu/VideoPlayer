//
// Created by weiqianghu on 2019/9/8.
//

#include <pthread.h>
#include <unistd.h>
#include "TextureFrameUploader.h"
#include "../../Log.h"

TextureFrameUploader::TextureFrameUploader() : _msg(MSG_NONE), copyTexSurface(nullptr) {
    LOGI("TextureFrameUploader create instance");
    eglCore = nullptr;
    textureFrame = nullptr;
    outputTexId = static_cast<GLuint>(-1);
    isInitial = false;
    vertexCoords = new GLfloat[OPENGL_VERTEX_COORDNATE_CNT];
    textureCoords = new GLfloat[OPENGL_VERTEX_COORDNATE_CNT];
    pthread_mutex_init(&mLock, nullptr);
    pthread_cond_init(&mCondition, nullptr);
}

TextureFrameUploader::~TextureFrameUploader() {
    LOGI("TextureFrameUploader instance destroyed");
    pthread_mutex_destroy(&mLock);
    pthread_cond_destroy(&mCondition);
    delete[] vertexCoords;
    delete[] textureCoords;
}

bool TextureFrameUploader::start(int videoWidth, int videoHeight, int degress) {
    LOGI("Creating TextureFrameUploader thread");
    this->videoWidth = videoWidth;
    this->videoHeight = videoHeight;
    memcpy(vertexCoords, DECODER_COPIER_GL_VERTEX_COORDS,
           sizeof(GLfloat) * OPENGL_VERTEX_COORDNATE_CNT);
    switch (degress) {
        case 90:
            memcpy(textureCoords, DECODER_COPIER_GL_TEXTURE_COORDS_ROTATED_90,
                   sizeof(GLfloat) * OPENGL_VERTEX_COORDNATE_CNT);
            break;
        case 180:
            memcpy(textureCoords, DECODER_COPIER_GL_TEXTURE_COORDS_ROTATED_180,
                   sizeof(GLfloat) * OPENGL_VERTEX_COORDNATE_CNT);
            break;
        case 270:
            memcpy(textureCoords, DECODER_COPIER_GL_TEXTURE_COORDS_ROTATED_270,
                   sizeof(GLfloat) * OPENGL_VERTEX_COORDNATE_CNT);
            break;
        default:
            memcpy(textureCoords, DECODER_COPIER_GL_TEXTURE_COORDS_NO_ROTATION,
                   sizeof(GLfloat) * OPENGL_VERTEX_COORDNATE_CNT);
            break;
    }
    _msg = MSG_WINDOW_SET;
    pthread_create(&_threadId, nullptr, threadStartCallback, this);
    return true;
}

void TextureFrameUploader::signalFrameAvaliable() {
    while (!isInitial || _msg == MSG_WINDOW_SET || nullptr == eglCore) {
        usleep(100 * 1000);
    }
    pthread_mutex_lock(&mLock);
    pthread_cond_signal(&mCondition);
    pthread_mutex_unlock(&mLock);
}

void TextureFrameUploader::stop() {
    LOGI("Stopping TextureFrameUploader Render thread");
    pthread_mutex_lock(&mLock);
    _msg = MSG_RENDER_LOOP_EXIT;
    pthread_cond_signal(&mCondition);
    pthread_mutex_unlock(&mLock);

    pthread_join(_threadId, nullptr);
    LOGI("TextureFrameUploader Render thread stopped");
}

void *TextureFrameUploader::threadStartCallback(void *myself) {
    auto *processor = (TextureFrameUploader *) myself;
    processor->renderLoop();
    pthread_exit(nullptr);
}

void TextureFrameUploader::renderLoop() {
    bool renderingEnabled = true;
    LOGI("render loop");
    while (renderingEnabled) {
        pthread_mutex_lock(&mLock);
        switch (_msg) {
            case MSG_WINDOW_SET:
                LOGI("receive msg MSG_WINDOW_SET");
                isInitial = initialize();
                break;
            case MSG_RENDER_LOOP_EXIT:
                LOGI("receive msg MSG_RENDER_LOOP_EXIT");
                renderingEnabled = false;
                destroy();
                break;
            default:
                break;
        }
        _msg = MSG_NONE;
        if (nullptr != eglCore) {
            this->signalDecodeThread();
            pthread_cond_wait(&mCondition, &mLock);
            eglCore->makeCurrent(copyTexSurface);
            this->drawFrame();
        }
        pthread_mutex_unlock(&mLock);
    }
    LOGI("Render loop exits");
}

bool TextureFrameUploader::initialize() {
    eglCore = new EGLCore();
    LOGI("TextureFrameUploader use sharecontext");
    eglCore->initWithSharedContext();
    LOGI("after TextureFrameUploader use sharecontext");

    copyTexSurface = eglCore->createOffscreenSurface(videoWidth,videoHeight);
    eglCore->makeCurrent(copyTexSurface);
    glGenFramebuffers(1,&mFBO);
    glGenTextures(1, &outputTexId);
    glBindTexture(GL_TEXTURE_2D, outputTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, videoWidth, videoHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (mUploaderCallback){
        mUploaderCallback->initFromUploaderGLContext(eglCore);
    }

    eglCore->makeCurrent(copyTexSurface);
    LOGI("leave TextureFrameUploader::initialize");

    return true;
}