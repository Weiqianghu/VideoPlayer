//
// Created by weiqianghu on 2019/9/5.
//

#include "MovieFrame.h"

MovieFrame::MovieFrame() {
    position = 0.0f;
    duration = 0.0f;
}

AudioFrame::AudioFrame() {
    dataUseUp = true;
    samples = nullptr;
    size = 0;
}

void AudioFrame::fillFullData() {
    dataUseUp = false;
}

void AudioFrame::useUpData() {
    dataUseUp = true;
}

bool AudioFrame::isDataUseUp() {
    return dataUseUp;
}

AudioFrame::~AudioFrame() {
    if (nullptr != samples) {
        delete[] samples;
        samples = nullptr;
    }
}

VideoFrame::VideoFrame() {
    luma = nullptr;
    chromaB = nullptr;
    chromaR = nullptr;
    width = 0;
    height = 0;
}

VideoFrame::~VideoFrame() {
    if (nullptr != luma) {
        delete[] luma;
        luma = nullptr;
    }
    if (nullptr != chromaB) {
        delete[] chromaB;
        chromaB = nullptr;
    }
    if (nullptr != chromaR) {
        delete[] chromaR;
        chromaB = nullptr;
    }
}
