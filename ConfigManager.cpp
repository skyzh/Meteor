//
// Created by Alex Chi on 2019/12/06.
//

#include "ConfigManager.h"

static ConfigManager* cfm = nullptr;

void ConfigManager::set(QString k, QVariant v) {
    _configs[k] = v;
}

QVariant ConfigManager::get(QString k) {
    return _configs[k];
}

ConfigManager *ConfigManager::instance() {
    if (cfm == nullptr) {
        cfm = new ConfigManager;
        cfm->_configs["DATASET_PATH"] = "/Users/skyzh/Work/Qt/dataset_CS241";
    }
    return cfm;
}
