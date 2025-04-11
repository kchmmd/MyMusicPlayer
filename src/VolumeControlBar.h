#ifndef VOLUMECONTROLBAR_H
#define VOLUMECONTROLBAR_H

#include "MyWidget.h"

#include <QLabel>
#include <QSlider>
#include <QLayout>

//自定义QSlider
class MySlider: public QSlider
{
    Q_OBJECT
public:
    MySlider(Qt::Orientation orientation, QWidget *parent = nullptr)
        : QSlider(orientation, parent){
        setObjectName("volume_control");
    }
signals:
   void  sigfocusOut();
protected:
    void focusOutEvent(QFocusEvent *event){
        Q_UNUSED(event);
        emit sigfocusOut();
    }
    void leaveEvent(QEvent* event){
        Q_UNUSED(event);
        emit sigfocusOut();
    }
};
//声音
class MyVolumeControlBar:public MyWidget
{
    Q_OBJECT
public:
    MyVolumeControlBar(QWidget* parent=nullptr);

    void setBMute(bool b_mute){
        m_slider->setValue(b_mute? 0 : m_current_volume);
        m_label->setText(QString("%1%").arg(b_mute? 0 : m_current_volume));
    }

    int value(){return m_current_volume;}
    void setPosition(QPoint point, int btn_volume_width){
        //qDebug()<<width()<<" : "<<height();
        int border = 10;
        move(point.x() + btn_volume_width/2 - width()/2 , point.y() - height() - border);
        show();
    }
    void setVolume(int volume){
        m_slider->setValue(volume);
        m_label->setText(QString("%1%").arg(volume));
    }
    bool getBEnter(){return m_b_enter;}
signals:
    void valueChanged(int);
protected:
    void enterEvent(QEvent* event){
        //qDebug()<<"enterEvent";
        Q_UNUSED(event);
        m_b_enter = true;
    }
    void leaveEvent(QEvent* event){
        //qDebug()<<"leaveEvent";
        Q_UNUSED(event);
        m_b_enter = false;
    }
protected slots:
    void onValueChanged(int volume){
        if(volume != 0){
            m_current_volume = volume;
        }
        m_label->setText(QString("%1%").arg(volume));
    }
private:
    QLabel* m_label;
    MySlider* m_slider;
    int m_current_volume;
    bool m_b_enter;
};
#endif
