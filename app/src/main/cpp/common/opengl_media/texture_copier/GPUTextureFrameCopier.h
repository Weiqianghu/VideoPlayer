//
// Created by weiqianghu on 2019/9/22.
//

#ifndef VIDEOPLAYER_GPUTEXTUREFRAMECOPIER_H
#define VIDEOPLAYER_GPUTEXTUREFRAMECOPIER_H

#include "TextureFrameCopier.h"

static char *GPU_FRAME_VERTEX_SHADER =
        const_cast<char *>("attribute vec4 vPosition;\n"
                           "attribute vec4 vTexCords;\n"
                           "varying vec2 yuvTexCoords;\n"
                           "uniform highp mat4 trans; \n"
                           "void main() {\n"
                           "  yuvTexCoords = vTexCords.xy;\n"
                           "  gl_Position = trans * vPosition;\n"
                           "}\n");

static char *GPU_FRAME_FRAGMENT_SHADER =
        const_cast<char *>("#extension GL_OES_EGL_image_external : require\n"
                           "precision mediump float;\n"
                           "uniform samplerExternalOES yuvTexSampler;\n"
                           "varying vec2 yuvTexCoords;\n"
                           "void main() {\n"
                           "  gl_FragColor = texture2D(yuvTexSampler, yuvTexCoords);\n"
                           "}\n");

class GPUTextureFrameCopier : public TextureFrameCopier {
public:
    GPUTextureFrameCopier();

    virtual ~GPUTextureFrameCopier();

    virtual bool init();

    virtual void renderWithCoords(TextureFrame *textureFrame, GLuint texId, GLfloat *vertexCoords,
                                  GLfloat *textureCoords);

protected:
    GLint mGLUniformTexture;
};


#endif //VIDEOPLAYER_GPUTEXTUREFRAMECOPIER_H
