#ifndef LYRICTABLEWIDGET_H
#define LYRICTABLEWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QTimer>
#include <QMediaPlayer>
#include "DataStruct.h"

//歌名和歌手
class LyricTitle: public QWidget
{
    Q_OBJECT
public:
    LyricTitle(QWidget* parent=nullptr):QWidget(parent){
        setObjectName("LyricTitle");
        setAttribute(Qt::WA_TranslucentBackground);
        QVBoxLayout* vbox = new QVBoxLayout;
        m_label_music_name=  new QLabel(this);
        m_label_user_name=  new QLabel(this);
        vbox->addWidget(m_label_music_name);
        vbox->addWidget(m_label_user_name);
        setLayout(vbox);
        QFont ft= m_label_music_name->font();
        ft.setPointSize(15);
        ft.setBold(true);
        m_label_music_name->setFont(ft);
        m_label_music_name->setAlignment(Qt::AlignCenter);
        m_label_user_name->setAlignment(Qt::AlignCenter);
    }
    void setTitle(QString music_name, QString user_name){
        m_label_music_name->setText(music_name);
        m_label_user_name->setText(user_name);
    }
private:
    QLabel* m_label_music_name;
    QLabel* m_label_user_name;
};
//

//歌词列表
class LyricTableWidget:public QWidget
{
    Q_OBJECT
public:
    LyricTableWidget(QWidget* parent=nullptr);
	~LyricTableWidget();
	void clear();
	void setLoading();
    void setLyricData(std::vector<LineLyricData> vec_lyric);
    void setCurrentDuration(qint64 duration);
    void setCurrentLyric(int index);
    void setTitle(QString music_name, QString user_name);
    void player();
    void pause();
    void stop();
    void reStart();
signals:
    void itemDoubleClicked(QTableWidgetItem*);
    void sigCurrentLyricSingle(QString, int);
    void sigCurrentLyricDouble(QString, QString,int);
public slots:
    void onStateChanged(QMediaPlayer::State newState);
protected slots:
    void onTimeout();
    void onItemDoubleClicked(QTableWidgetItem* item);
private:
    int getIndex(qint64 time);
private:
    LyricTitle* m_lyricTitle;
    QTableWidget* m_tableWidget;
    QTableWidgetItem* m_pre_item;
	QWidget* m_widget_loading;
	QLabel* m_label_loading;
    std::vector<LineLyricData> m_vec_lyric;
    QTimer m_timer;
    qint64 m_time;
    QFont m_default_ft;
    QFont m_current_ft;
};

#endif // LYRICTABLEWIDGET_H
