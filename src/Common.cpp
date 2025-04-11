#include "Common.h"

#include <QFileInfo>
#include <QTextCodec>
#include <QTime>

QString getQString(QByteArray byteArray)
{
	QTextCodec::ConverterState state;
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QString qstr = codec->toUnicode(byteArray.constData(), byteArray.size(), &state);
	if (state.invalidChars > 0) {
		qstr = QTextCodec::codecForName("GBK")->toUnicode(byteArray);
	}
	else {
		qstr = byteArray;
	}
	return qstr;
}

int getChineseIndex(QString qstr)
{
    int index = 0;
    for(QChar ch: qstr){
        if(ch.unicode() >= 0x4E00 && ch.unicode() <= 0x9FA5){
            //中文
            break;
        }
        index++;
    }
    return index == qstr.length() || index == 0 ? -1:index; //第一个是中文也返回-1;
}

bool isEnglishMusic(QString file_path)
{
    QFileInfo info(file_path);
    QString name = info.baseName();

    return getChineseIndex(name) == -1;
}

QString getTimeQstr(qint64 value)
{
	return QString("%1").arg(QTime::fromMSecsSinceStartOfDay(value).toString("hh:mm:ss"));
}

long long getQstrTime(QString qstr_time)
{
	//QTime time = QTime::fromString(qstr_time, "hh:mm:ss");
	//hh:mm:ss
	if (qstr_time.contains(".")) {
		qstr_time = qstr_time.split(".")[0];
	}
	QStringList qstrList = qstr_time.split(":");
	long long seconds = 0;
	switch (qstrList.size()) {
	case 2:
		seconds = qstrList[0].toUInt() * 60 + qstrList[1].toUInt();
		break;
	case 3:
	default:
		seconds = qstrList[0].toUInt() * 3600 + qstrList[1].toUInt() * 60 + qstrList[2].toUInt();
		break;
	}
	return seconds;
}