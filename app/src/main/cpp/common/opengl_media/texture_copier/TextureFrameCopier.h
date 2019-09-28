//
// Created by weiqianghu on 2019/9/19.
//

#ifndef VIDEOPLAYER_TEXTUREFRAMECOPIER_H
#define VIDEOPLAYER_TEXTUREFRAMECOPIER_H

#include "../texture/TextureFrame.h"

static char *NO_FILTER_VERTEX_SHADER =
        const_cast<char *>("attribute vec4 vPosition;\n"
                           "attribute vec4 vTexCords;\n"
                           "varying vec2 yuvTexCoords;\n"
                           "uniform highp mat4 texMatrix;\n"
                           "uniform highp mat4 trans; \n"
                           "void main() {\n"
                           "  yuvTexCoords = (texMatrix*vTexCords).xy;\n"
                           "  gl_Position = trans * vPosition;\n"
                           "}\n");

static char *NO_FILTER_FRAGMENT_SHADER =
        const_cast<char *>("varying vec2 yuvTexCoords;\n"
                           "uniform sampler2D yuvTexSampler;\n"
                           "void main() {\n"
                           "  gl_FragColor = texture2D(yuvTexSampler, yuvTexCoords);\n"
                           "}\n");

class TextureFrameCopier {
public:
    TextureFrameCopier();

    virtual ~TextureFrameCopier();

    virtual bool init() = 0;

    virtual void renderWithCoords(TextureFrame *textureFrame, GLuint texId, GLfloat *vertexCoords,
                                  GLfloat *textureCoords) = 0;

    virtual void destroy();

protected:
    char *mVertexShader;
    char *mFragmentShader;

    bool mIsInitialized;
    GLuint mGLProgId;
    GLuint mGLVertexCoords;
    GLuint mGLTextureCoords;

    GLint mUniformTexMatrix;
    GLint mUniformTransforms;
};


#endif //VIDEOPLAYER_TEXTUREFRAMECOPIER_H
