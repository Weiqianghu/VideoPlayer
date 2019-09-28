//
// Created by weiqianghu on 2019/9/10.
//

#include "TextureFrame.h"
#include "../../../Log.h"

TextureFrame::TextureFrame() = default;

TextureFrame::~TextureFrame() = default;

bool TextureFrame::checkGLError(const char *op) {
    GLint error;
    for (error = glGetError(); error; error = glGetError()) {
        LOGI("error::after %s() glError (0x%x)\n", op, error);
        return true;
    }
    return false;
}