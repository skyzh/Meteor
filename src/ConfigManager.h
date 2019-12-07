//
// Created by Alex Chi on 2019/12/06.
//

#ifndef METRO_CONFIGMANAGER_H
#define METRO_CONFIGMANAGER_H

#include <QMap>
#include <QVariant>

class ConfigManager {
private:
    QMap <QString, QVariant> _configs;
public:
    void set(QString k, QVariant v);
    QVariant get(QString k);

    static ConfigManager *instance();
};


#endif //METRO_CONFIGMANAGER_H
