//
// Created by weiqianghu on 2019/9/22.
//

#include "GPUTextureFrameCopier.h"
#include "../../egl_core/GLTools.h"
#include "../../Matrix.h"

GPUTextureFrameCopier::GPUTextureFrameCopier() {
    mVertexShader = NO_FILTER_VERTEX_SHADER;
    mFragmentShader = GPU_FRAME_FRAGMENT_SHADER;
}

GPUTextureFrameCopier::~GPUTextureFrameCopier() = default;

bool GPUTextureFrameCopier::init() {
    mGLProgId = loadProgram(mVertexShader, mFragmentShader);
    if (!mGLProgId) {
        LOGE("Could not create program.");
        return false;
    }
    mGLVertexCoords = static_cast<GLuint>(glGetAttribLocation(mGLProgId, "vPosition"));
    checkGLError("glGetAttribLocation vPosition");
    mGLTextureCoords = static_cast<GLuint>(glGetAttribLocation(mGLProgId, "vTexCords"));
    checkGLError("glGetAttribLocation vTexCords");
    mGLUniformTexture = glGetUniformLocation(mGLProgId, "yuvTexSampler");
    checkGLError("glGetAttribLocation yuvTexSampler");

    mUniformTexMatrix = glGetUniformLocation(mGLProgId, "texMatrix");
    checkGLError("glGetUniformLocation mUniformTexMatrix");

    mUniformTransforms = glGetUniformLocation(mGLProgId, "trans");
    checkGLError("glGetUniformLocation mUniformTransforms");

    mIsInitialized = true;
    return true;
}

void GPUTextureFrameCopier::renderWithCoords(TextureFrame *textureFrame, GLuint texId,
                                             GLfloat *vertexCoords, GLfloat *textureCoords) {
    glBindTexture(GL_TEXTURE_2D, texId);
    checkGLError("glBindTexture");

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           texId, 0);
    checkGLError("glFramebufferTexture2D");

    glUseProgram(mGLProgId);
    if (!mIsInitialized) {
        return;
    }

    glVertexAttribPointer(mGLVertexCoords, 2, GL_FLOAT, GL_FALSE, 0, vertexCoords);
    glEnableVertexAttribArray (mGLVertexCoords);
    glVertexAttribPointer(mGLTextureCoords, 2, GL_FLOAT, GL_FALSE, 0, textureCoords);
    glEnableVertexAttribArray (mGLTextureCoords);
    /* Binding the input texture */
    textureFrame->bindTexture(&mGLUniformTexture);

    float texTransMatrix[4 * 4];
    matrixSetIdentityM(texTransMatrix);
    glUniformMatrix4fv(mUniformTexMatrix, 1, GL_FALSE, (GLfloat *) texTransMatrix);

    float rotateMatrix[4 * 4];
    matrixSetIdentityM(rotateMatrix);
    glUniformMatrix4fv(mUniformTransforms, 1, GL_FALSE, (GLfloat *) rotateMatrix);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(mGLVertexCoords);
    glDisableVertexAttribArray(mGLTextureCoords);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
}