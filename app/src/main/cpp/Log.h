//
// Created by weiqianghu on 2019/9/22.
//

#ifndef VIDEOPLAYER_LOG_H
#define VIDEOPLAYER_LOG_H

#include <android/log.h>

#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, "VideoPlayer",__VA_ARGS__)
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN, "VideoPlayer",__VA_ARGS__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, "VideoPlayer",__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, "VideoPlayer",__VA_ARGS__)

class Log {

};


#endif //VIDEOPLAYER_LOG_H
