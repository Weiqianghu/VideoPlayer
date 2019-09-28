//
// Created by weiqianghu on 2019/9/8.
//

#ifndef VIDEOPLAYER_EGLSHARECONTEXT_H
#define VIDEOPLAYER_EGLSHARECONTEXT_H

#include <EGL/egl.h>

class EglShareContext {
public:
    ~EglShareContext() = default;

    static EGLContext getShaderContext() {
        if (instance_->sharedDisplay == EGL_NO_DISPLAY) {
            instance_->init();
        }
        return instance_->sharedContext;
    }

protected:
    EglShareContext();

    void init();


private:
    static EglShareContext *instance_;
    EGLContext sharedContext;
    EGLDisplay sharedDisplay;
};


#endif //VIDEOPLAYER_EGLSHARECONTEXT_H
