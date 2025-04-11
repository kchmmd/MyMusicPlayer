#ifndef CONTROLBAR_H
#define CONTROLBAR_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QLayout>
#include <QApplication>
#include <QStyle>
#include <QSlider>
#include <QEvent>
#include <QMediaPlayer>
#include <QEnterEvent>
#include "PlayerAnimation.h"
#include "DataStruct.h"

class MyTimeSlider : public QSlider
{
	Q_OBJECT
public:
	MyTimeSlider(Qt::Orientation orientation, QWidget *parent = nullptr)
	:QSlider(orientation, parent) {}
protected:
	void mouseReleaseEvent(QMouseEvent *event) {
		qDebug() << "moveRelease";
		// 1. 计算点击位置对应的值
		int value = QStyle::sliderValueFromPosition(
			minimum(), maximum(),
			event->pos().x(), width(),  // 水平滑块用x坐标，垂直滑块用y
			false  // 是否精确对齐刻度（如启用tickmarks）
		);
		setValue(value);
		emit sliderReleased();
	}
};

class MyButton: public QPushButton
{
    Q_OBJECT
public:
    MyButton(QWidget* parent=nullptr):QPushButton(parent){
        setObjectName("MyButton");
        setMouseTracking(true);
    }
signals:
     void sigEnter();
protected:
    void enterEvent(QEvent* event){
        Q_UNUSED(event);
        emit sigEnter();
    }
};

class ControlBar: public QWidget
{
    Q_OBJECT
public:
    ControlBar(QWidget* parent=nullptr);

	void clear();
	void setImage(const QImage& image);
    void setCurrentTitle(QString qstr);
    void setRange(int min, int max);
    void setValue(int value);
    int getValue(){return m_slider_time->value();}
    int maximum(){return m_slider_time->maximum();}
    void setBMute(bool b_mute);
    QPoint getBtnMutePoint(){return  m_widget_right->mapToGlobal(m_btn_volume->pos());}
    int getBtnVolumeWidth(){return m_btn_volume->width();}
    void setCurrentPlayerType(PlayType);
protected:
	bool eventFilter(QObject *obj, QEvent *event);
public slots:
    void onStateChanged(QMediaPlayer::State newState);
    void onClickedPlayerType();
signals:
    void sigClickedLove();
    void sigClickedPre();
    void sigClickedPlayerPause();
    void sigClickedNext();
    void sigClickedLyric();
    void sigClickedMute();
    void sigClickedTable();

    void sigSetPlayerType(PlayType);

    void sliderMoved(int position);
    void sliderPressed();
    void sliderReleased();
    void valueChanged(int value);

    void sigBtnVolumeEnter();
private:
    QSlider* m_slider_time;
    //QPushButton* m_btn_love;
    QPushButton* m_btn_pre;
    QPushButton* m_btn_player_pause;
    QPushButton* m_btn_next;
    QPushButton* m_btn_player_model;
    QPushButton* m_btn_lyric;
    MyButton* m_btn_volume;
    QPushButton* m_btn_table;
    QWidget* m_widget_left;
    QWidget* m_widget_right;//为了居中
    PlayerAnimation* m_playerAnimation;
    PlayType m_playType;
	QList<QObject*> m_list_object;
    QLabel* m_label_current_name;
    QLabel* m_label_time;
};

#endif // CONTROLBAR_H
