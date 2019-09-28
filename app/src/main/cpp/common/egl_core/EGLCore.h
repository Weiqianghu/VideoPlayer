//
// Created by weiqianghu on 2019/9/8.
//

#ifndef VIDEOPLAYER_EGLCORE_H
#define VIDEOPLAYER_EGLCORE_H


#include <EGL/egl.h>

typedef EGLBoolean (EGLAPIENTRYP PFNEGLPRESENTATIONTIMEANDROIDPROC)(EGLDisplay display,
                                                                    EGLSurface surface,
                                                                    khronos_stime_nanoseconds_t time);

class EGLCore {
public:
    EGLCore();

    virtual ~EGLCore();

    bool init();

    bool init(EGLContext sharedContext);

    bool initWithSharedContext();

    EGLSurface createWindowSurface(ANativeWindow *_window);

    EGLSurface createOffscreenSurface(int width, int height);

    bool makeCurrent(EGLSurface eglSurface);

    void doneCurrent();

    bool swapBuffers(EGLSurface eglSurface);

    int querySurface(EGLSurface surface, int what);

    int setPresentationTime(EGLSurface surface, khronos_stime_nanoseconds_t nsecs);

    void releaseSurface(EGLSurface eglSurface);

    void release();

    EGLContext getContext();

    EGLDisplay getDisplay();

    EGLConfig getConfig();

private:
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;

    PFNEGLPRESENTATIONTIMEANDROIDPROC pfneglPresentationTimeANDROID;
};

#endif //VIDEOPLAYER_EGLCORE_H
