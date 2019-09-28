//
// Created by weiqianghu on 2019/9/10.
//

#ifndef VIDEOPLAYER_TEXTUREFRAME_H
#define VIDEOPLAYER_TEXTUREFRAME_H


#include <GLES2/gl2.h>

class TextureFrame {
protected:
    bool checkGLError(const char *op);

public:
    TextureFrame();

    virtual ~TextureFrame();

    virtual bool createTexture() = 0;

    virtual void updateTexImage() = 0;

    virtual bool bindTexture(GLint *uniformSamplers) = 0;

    virtual void dealloc() = 0;
};

#endif //VIDEOPLAYER_TEXTUREFRAME_H
