//
// Created by weiqianghu on 2019/9/2.
//

#include "DecoderRequestHeader.h"

DecoderRequestHeader::DecoderRequestHeader() {
    uri = nullptr;
    maxAnalyzeDurations = nullptr;
    extraData = nullptr;
}

DecoderRequestHeader::~DecoderRequestHeader() = default;