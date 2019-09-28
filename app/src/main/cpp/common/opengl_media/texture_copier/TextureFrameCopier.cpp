//
// Created by weiqianghu on 2019/9/19.
//

#include "TextureFrameCopier.h"

TextureFrameCopier::TextureFrameCopier() = default;

TextureFrameCopier::~TextureFrameCopier() = default;

void TextureFrameCopier::destroy() {
    mIsInitialized = false;
    if (mGLProgId) {
        glDeleteProgram(mGLProgId);
        mGLProgId = 0;
    }
}