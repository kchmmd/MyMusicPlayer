#ifndef COMMON_H
#define COMMON_H

#include <QString>

QString getQString(QByteArray byteArray);

int getChineseIndex(QString qstr);
bool isEnglishMusic(QString file_path);
QString getTimeQstr(qint64 value);
long long getQstrTime(QString qstr_time);
#endif
