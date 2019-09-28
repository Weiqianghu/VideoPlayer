//
// Created by weiqianghu on 2019/9/19.
//

#include "YUVTextureFrameCopier.h"
#include "../../egl_core/GLTools.h"
#include "matrix.h"
#include "../../Matrix.h"

YUVTextureFrameCopier::YUVTextureFrameCopier() {
    mVertexShader = NO_FILTER_VERTEX_SHADER;
    mFragmentShader = YUV_FRAME_FRAGMENT_SHADER;
}

YUVTextureFrameCopier::~YUVTextureFrameCopier() = default;

bool YUVTextureFrameCopier::init() {
    mGLProgId = loadProgram(mVertexShader, mFragmentShader);
    if (!mGLProgId) {
        LOGE("could not create program");
        return false;
    }
    mGLVertexCoords = static_cast<GLuint>(glGetAttribLocation(mGLProgId, "vPosition"));
    checkGLError("glGetAttribLocation vPosition");
    mGLTextureCoords = static_cast<GLuint>(glGetAttribLocation(mGLProgId, "vTexCords"));
    checkGLError("glGetAttribLocation vTexCords");
    glUseProgram(mGLProgId);

    _uniformSamplers[0] = glGetUniformLocation(mGLProgId, "s_texture_y");
    _uniformSamplers[1] = glGetUniformLocation(mGLProgId, "s_texture_u");
    _uniformSamplers[2] = glGetUniformLocation(mGLProgId, "s_texture_v");

    mUniformTransforms = glGetUniformLocation(mGLProgId, "trans");
    checkGLError("glGetUniformLocation trans");

    mUniformTexMatrix = glGetUniformLocation(mGLProgId, "texMatrix");
    checkGLError("glGetUniformLocation texMatrix");

    mIsInitialized = true;
    return true;
}

void YUVTextureFrameCopier::renderWithCoords(TextureFrame *textureFrame, GLuint texId,
                                             GLfloat *vertexCoords, GLfloat *textureCoords) {
    glBindTexture(GL_TEXTURE_2D, texId);
    checkGLError("glBindTexture");

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
    checkGLError("glFramebufferTexture2D");

    glUseProgram(mGLProgId);
    if (!mIsInitialized) {
        return;
    }
    glVertexAttribPointer(mGLVertexCoords, 2, GL_FLOAT, GL_FALSE, 0, vertexCoords);
    glEnableVertexAttribArray(mGLVertexCoords);
    glVertexAttribPointer(mGLTextureCoords, 2, GL_FLOAT, GL_FALSE, 0, textureCoords);
    glEnableVertexAttribArray(mGLTextureCoords);
    textureFrame->bindTexture(_uniformSamplers);

    float texTransMatrix[4 * 4];
    matrixSetIdentityM(texTransMatrix);
    glUniformMatrix4fv(mUniformTexMatrix, 1, GL_FALSE, texTransMatrix);

    float rotateMatrix[4 * 4];
    matrixSetIdentityM(rotateMatrix);
    glUniformMatrix4fv(mUniformTransforms, 1, GL_FALSE, rotateMatrix);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(mGLVertexCoords);
    glDisableVertexAttribArray(mGLTextureCoords);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
}