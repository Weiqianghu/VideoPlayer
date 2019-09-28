//
// Created by weiqianghu on 2019/9/8.
//

#ifndef VIDEOPLAYER_TEXTUREFRAMEUPLOADER_H
#define VIDEOPLAYER_TEXTUREFRAMEUPLOADER_H

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <sys/types.h>
#include "../common/CommonTools.h"
#include "../../common/egl_core/EGLCore.h"
#include "../../common/opengl_media/texture/TextureFrame.h"
#include "../../common/opengl_media/texture_copier/TextureFrameCopier.h"

#define OPENGL_VERTEX_COORDNATE_CNT            8

static GLfloat DECODER_COPIER_GL_VERTEX_COORDS[8] = {
        -1.0f, -1.0f,    // 0 top left
        1.0f, -1.0f,    // 1 bottom left
        -1.0f, 1.0f,  // 2 bottom right
        1.0f, 1.0f,    // 3 top right
};

static GLfloat DECODER_COPIER_GL_TEXTURE_COORDS_NO_ROTATION[8] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
};

static GLfloat DECODER_COPIER_GL_TEXTURE_COORDS_ROTATED_90[8] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
};

static GLfloat DECODER_COPIER_GL_TEXTURE_COORDS_ROTATED_180[8] = {
        1.0, 1.0,
        0.0, 1.0,
        1.0, 0.0,
        0.0, 0.0,
};
static GLfloat DECODER_COPIER_GL_TEXTURE_COORDS_ROTATED_270[8] = {
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
};

class UploaderCallback {
public:
    virtual void processVideoFrame(GLuint inputTexId, int width, int height, float positionnt) = 0;

    virtual void processAudioData(short *samples, int size, float position, byte **buffer) = 0;

    virtual void onSeekCallback(float seek_seconds) = 0;

    virtual void initFromUploaderGLContext(EGLCore *eglCore) = 0;

    virtual void destroyFromUploaderGLContext() = 0;
};

class TextureFrameUploader {
public:
    TextureFrameUploader();

    virtual ~TextureFrameUploader();

    bool start(int videoWidth, int videoHeight, int degress);

    void signalFrameAvaliable();

    void stop();

    EGLContext getEGLContext() {
        if (nullptr != eglCore) {
            return eglCore->getContext();
        }
        return nullptr;
    }

    typedef float (*update_tex_image_callback)(TextureFrame *textureFrame, void *context);

    typedef void (*signal_decode_thread_callback)(void *context);

    virtual void registerUpdateTexImageCallback(
            float (*update_tex_image_callback)(TextureFrame *textureFrame, void *context),
            void (*signal_decode_thread_callback)(void *context), void *context);

    void setUploaderCallback(UploaderCallback *pUploaderCallback) {
        mUploaderCallback = pUploaderCallback;
    }

protected:
    bool isInitial;
    EGLCore *eglCore;
    EGLSurface copyTexSurface;
    int videoWidth;
    int videoHeight;

    GLfloat *vertexCoords;
    GLfloat *textureCoords;

    UploaderCallback *mUploaderCallback;
    TextureFrameCopier *textureFrameCopier;

    GLuint outputTexId;
    GLuint mFBO;

    TextureFrame *textureFrame;
    update_tex_image_callback updateTexImageCallback;
    signal_decode_thread_callback signalDecodeThreadCallback;
    void *updateTexImageContext;
    enum RenderThreadMessage {
        MSG_NONE = 0, MSG_WINDOW_SET, MSG_RENDER_LOOP_EXIT
    };

    pthread_t _threadId;
    pthread_mutex_t mLock;
    pthread_cond_t mCondition;
    enum RenderThreadMessage _msg;

    static void *threadStartCallback(void *myself);

    void renderLoop();

    virtual void destroy();

    virtual bool initialize();

    virtual void drawFrame();

    float updateTexImage();

    void signalDecodeThread();
};


#endif //VIDEOPLAYER_TEXTUREFRAMEUPLOADER_H
