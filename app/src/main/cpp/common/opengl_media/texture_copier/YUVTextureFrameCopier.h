//
// Created by weiqianghu on 2019/9/19.
//

#ifndef VIDEOPLAYER_YUVTEXTUREFRAMECOPIER_H
#define VIDEOPLAYER_YUVTEXTUREFRAMECOPIER_H

#include "../texture/TextureFrame.h"
#include "TextureFrameCopier.h"

static char *YUV_FRAME_FRAGMENT_SHADER =
        const_cast<char *>("varying highp vec2 yuvTexCoords;\n"
                           "uniform sampler2D s_texture_y;\n"
                           "uniform sampler2D s_texture_u;\n"
                           "uniform sampler2D s_texture_v;\n"
                           "void main(void)\n"
                           "{\n"
                           "highp float y = texture2D(s_texture_y, yuvTexCoords).r;\n"
                           "highp float u = texture2D(s_texture_u, yuvTexCoords).r - 0.5;\n"
                           "highp float v = texture2D(s_texture_v, yuvTexCoords).r - 0.5;\n"
                           "\n"
                           "highp float r = y + 1.402 * v;\n"
                           "highp float g = y - 0.344 * u - 0.714 * v;\n"
                           "highp float b = y + 1.772 * u;\n"
                           "gl_FragColor = vec4(r,g,b,1.0);\n"
                           "}\n");

class YUVTextureFrameCopier : public TextureFrameCopier {
public:
    YUVTextureFrameCopier();

    virtual ~YUVTextureFrameCopier();

    virtual bool init();

    virtual void renderWithCoords(TextureFrame *textureFrame, GLuint texId, GLfloat *vertexCoords,
                                  GLfloat *textureCoords);

protected:
    GLint _uniformSamplers[3];
};


#endif //VIDEOPLAYER_YUVTEXTUREFRAMECOPIER_H
