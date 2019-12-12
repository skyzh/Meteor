//
// Created by Alex Chi on 2019/12/06.
//

#include "ConfigManager.h"
#include <QDebug>

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
    }
    return cfm;
}
