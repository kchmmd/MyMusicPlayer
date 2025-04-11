/*******************************************
*VolumeControlBar.cpp
*声音控制
********************************************/
#pragma execution_character_set("utf-8")
#include "VolumeControlBar.h"
#include "DataStruct.h"

MyVolumeControlBar::MyVolumeControlBar(QWidget* parent):MyWidget(parent)
{
	setObjectName("MyVolumeControlBar");
	setFixedSize(44, 150);
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::SubWindow);
    setMoveEnable(false);
    setChangeSizeEnable(false);
	QVBoxLayout* vbox_main = new QVBoxLayout;
	m_label =  new QLabel(this);
    m_slider = new MySlider(Qt::Vertical);
	vbox_main->addWidget(m_slider);
	vbox_main->addWidget(m_label);
    setLayout(vbox_main);

    m_slider->setObjectName("volume_control");
    m_slider->setRange(0, 100);

	connect(m_slider, &QSlider::valueChanged, this, &MyVolumeControlBar::valueChanged);
	connect(m_slider, &QSlider::valueChanged, this, &MyVolumeControlBar::onValueChanged);
	connect(m_slider, &MySlider::sigfocusOut, this, &MyVolumeControlBar::hide);

	setVolume(VOLUME_INIT);
    m_b_enter = false;

}
