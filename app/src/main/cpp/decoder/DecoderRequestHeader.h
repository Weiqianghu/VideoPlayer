//
// Created by weiqianghu on 2019/9/2.
//

#ifndef VIDEOPLAYER_DECODERREQUESTHEADER_H
#define VIDEOPLAYER_DECODERREQUESTHEADER_H

#include <map>
#include <string>

class DecoderRequestHeader {
private:
    char *uri;
    int *maxAnalyzeDurations;
    int analyzeCnt;
    int probeSize;
    bool fpsProbeSizeConfigured;
    std::map<std::string, void *> *extraData;
public:
    DecoderRequestHeader();

    ~DecoderRequestHeader();

    void init(char *uriParam) {
        int length = strlen(uriParam);
        uri = new char[length + 1];
        memcpy(uri, uriParam, sizeof(char) * (length + 1));
        extraData = new std::map<std::string, void *>();
        fpsProbeSizeConfigured = false;
    };

    void init(char *uriParam, int *max_analyze_duration, int analyzeCnt, int probeSize,
              bool fpsProbeSizeConfigured) {
        int length = strlen(uriParam);
        uri = new char[length + 1];
        memcpy(uri, uriParam, sizeof(char) * (length + 1));
        maxAnalyzeDurations = new int[analyzeCnt];
        memcpy(maxAnalyzeDurations, max_analyze_duration, sizeof(int) * analyzeCnt);
        this->probeSize = probeSize;
        this->fpsProbeSizeConfigured = fpsProbeSizeConfigured;
        extraData = new std::map<std::string, void *>();
    };

    void destroy() {
        if (nullptr != uri) {
            delete[] uri;
            uri = nullptr;
        }
        if (nullptr != maxAnalyzeDurations) {
            delete[] maxAnalyzeDurations;
            maxAnalyzeDurations = nullptr;
        }
        if (nullptr != extraData) {
            std::map<std::string, void *>::iterator iterator = extraData->begin();
            for (; iterator != extraData->end(); iterator++) {
                void *value = iterator->second;
                delete value;
            }
            extraData->clear();
            delete extraData;
            extraData = nullptr;
        }
    };

    char *getURI() {
        return uri;
    };

    int *getMaxAnalyzeDurations() {
        return maxAnalyzeDurations;
    };

    int getAnalyzeCnt() {
        return analyzeCnt;
    };

    int getProbeSize() {
        return probeSize;
    };

    bool getFPSProbeSizeConfigured() {
        return fpsProbeSizeConfigured;
    };

    void put(std::string key, void *value) {
        extraData->insert(std::pair<std::string, void *>(key, value));
    };

    void *get(std::string key) {
        std::map<std::string, void *>::iterator iterator = extraData->find(key);
        if (iterator != extraData->end()) {
            return iterator->second;
        }
        return nullptr;
    };
};

#endif //VIDEOPLAYER_DECODERREQUESTHEADER_H
