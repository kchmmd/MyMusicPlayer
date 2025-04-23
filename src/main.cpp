#pragma execution_character_set("utf-8")
#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QFile>
#include <QLocale>

QString getLanguage(Config &config)
{
	QString config_xml = QCoreApplication::applicationDirPath() + RESOURCE;
	QString current_language;
	QLocale locale = QLocale::system();
	if (config.language_index == -1) {
		current_language = "/resource/" + locale.name() + ".qm";
		config.set_language.insert(current_language);

		int index = 0;
		for (QString qstr : config.set_language) {
			if (current_language == qstr) {
				config.language_index = index;
				break;
			}
			index++;
		}
	}
	else {
		int index = 0;
		for (QString qstr : config.set_language) {
			if (index == config.language_index) {
				current_language = qstr;
				break;
			}
			index++;
		}
	}

	if (!QFile(current_language).exists()) {
		current_language = QCoreApplication::applicationDirPath() + current_language;
	}

	return current_language;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("gbk");
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
#endif
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
#endif
    //qDebug() <<QString::fromUtf8("中文测试");

	Config config;
	MainWindow::initConfig(config);
	MainWindow::loadConfig(config);
	QString language = getLanguage(config);
	QTranslator translator;
	if (translator.load(language)) {  // 尝试加载翻译文件
		a.installTranslator(&translator);
	}
    MainWindow w(config);
    w.show();
	w.addLog("current_language:" + language);
    return a.exec();
}
