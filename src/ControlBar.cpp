/*******************************************
*ControBar.cpp
*播放器控制
********************************************/
#pragma execution_character_set("utf-8")
#include "ControlBar.h"
#include "DataStruct.h"
#include "Common.h"

#define CONTROLBAR_WIDTH 90

ControlBar::ControlBar(QWidget *parent)
    :QWidget(parent)
{
    setObjectName("ControlBar");
    setAttribute(Qt::WA_TranslucentBackground);
    QVBoxLayout* vbox = new QVBoxLayout;
    QHBoxLayout* hbox = new QHBoxLayout;
    m_widget_left = new QWidget;
    m_widget_right = new QWidget;//为了居中
    QVBoxLayout* vbox_left = new QVBoxLayout;
    QHBoxLayout* hbox_left = new QHBoxLayout;
    QHBoxLayout* hbox_right = new QHBoxLayout;

    m_playerAnimation = new PlayerAnimation;
    m_slider_time = new MyTimeSlider(Qt::Horizontal,this);
    //m_btn_love = new QPushButton(this);
    m_btn_pre = new QPushButton(this);
    m_btn_player_pause = new QPushButton(this);
    m_btn_next = new QPushButton(this);
    m_btn_player_model = new QPushButton(this);
    m_btn_lyric = new QPushButton(this);
    m_btn_volume = new MyButton(this);
    m_btn_table = new QPushButton(this);

    m_label_current_name = new QLabel(this);
    m_label_time = new QLabel(this);

    vbox_left->setMargin(0);
    hbox_left->setContentsMargins(10, 0, 0,0);
    vbox_left->addStretch();
    vbox_left->addWidget(m_label_current_name);
    vbox_left->addWidget(m_label_time);
    vbox_left->addStretch();
    hbox_left->addWidget(m_playerAnimation);
    hbox_left->addLayout(vbox_left);
    hbox_left->addStretch();
    m_widget_left->setLayout(hbox_left);

    hbox->setSpacing(20);
    hbox->addWidget(m_widget_left);
    hbox->addStretch();
    //hbox->addWidget(m_btn_love);
    hbox->addWidget(m_btn_pre);
    hbox->addWidget(m_btn_player_pause);
    hbox->addWidget(m_btn_next);
    hbox->addStretch();
    hbox->addWidget(m_widget_right);

    hbox_right->addStretch();
    hbox_right->addWidget(m_btn_player_model);
    hbox_right->addWidget(m_btn_lyric);
    hbox_right->addWidget(m_btn_volume);
    hbox_right->addWidget(m_btn_table);
    m_widget_right->setLayout(hbox_right);

    vbox->addWidget(m_slider_time);
    vbox->addLayout(hbox);
    this->setLayout(vbox);
    vbox->setMargin(0);
    vbox->setSpacing(0);

    m_widget_left->setFixedWidth(200);
    m_widget_right->setFixedWidth(200);
    m_playerAnimation->setFixedSize(60,60);
    this->setFixedHeight(CONTROLBAR_WIDTH);

    //m_btn_love->setToolTip("喜欢");
    m_btn_pre->setToolTip(QString::fromLocal8Bit("上一首"));
    m_btn_player_pause->setToolTip(QString::fromLocal8Bit("播放"));
    m_btn_next->setToolTip(QString::fromLocal8Bit("下一首"));
    m_btn_player_model->setToolTip(QString::fromLocal8Bit("顺序播放"));
    m_btn_lyric->setToolTip(QString::fromLocal8Bit("歌词"));
    m_btn_volume->setToolTip(QString::fromLocal8Bit("静音"));
    m_btn_table->setToolTip(QString::fromLocal8Bit("播放列表"));

    //m_btn_love->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_pre->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_player_pause->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_next->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_player_model->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_lyric->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_volume->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_table->setIcon(QIcon(":/resource/resource/none.png"));//设置空白图标，qss设置的尺寸才有效

    //m_btn_love->setObjectName("love");
    m_btn_pre->setObjectName("pre");
    m_btn_player_pause->setObjectName("player_pause");
    m_btn_next->setObjectName("next");
    m_btn_player_model->setObjectName("player_model");
    m_btn_lyric->setObjectName("lyric");
    m_btn_volume->setObjectName("volume");
    m_btn_table->setObjectName("music_list");

    m_slider_time->setObjectName("music_time");

	m_list_object << m_btn_pre << m_btn_player_pause << m_btn_next << m_btn_player_model << m_btn_lyric << m_btn_volume << m_btn_table << m_slider_time;
	for (QObject* obj : m_list_object) {
		obj->installEventFilter(this);
	}

	m_playType = OrderPlay;

	m_btn_player_pause->setProperty("status", "pause");
	m_btn_player_model->setProperty("status", "order_play");
	m_btn_volume->setProperty("status", "volume");

    //connect(m_btn_love, &QPushButton::clicked, this, &ControlBar::sigClickedLove);
    connect(m_btn_pre, &QPushButton::clicked, this, &ControlBar::sigClickedPre);
    connect(m_btn_player_pause, &QPushButton::clicked, this, &ControlBar::sigClickedPlayerPause);
    connect(m_btn_next, &QPushButton::clicked, this, &ControlBar::sigClickedNext);
    connect(m_btn_player_model, &QPushButton::clicked, this, &ControlBar::onClickedPlayerType);
    connect(m_btn_lyric, &QPushButton::clicked, this, &ControlBar::sigClickedLyric);
    connect(m_btn_volume, &QPushButton::clicked, this, &ControlBar::sigClickedMute);
    connect(m_btn_volume, &MyButton::sigEnter, this, &ControlBar::sigBtnVolumeEnter);
    connect(m_btn_table, &QPushButton::clicked, this, &ControlBar::sigClickedTable);

    connect(m_slider_time, &QSlider::sliderMoved, this, &ControlBar::sliderMoved);
    connect(m_slider_time, &QSlider::sliderPressed, this, &ControlBar::sliderPressed);
    connect(m_slider_time, &QSlider::sliderReleased, this, &ControlBar::sliderReleased);
    connect(m_slider_time, &QSlider::valueChanged, this, &ControlBar::valueChanged);
}

void ControlBar::clear()
{
	setCurrentTitle("");
	setRange(0, 0);
}

void ControlBar::setImage(const QImage & image)
{
	m_playerAnimation->setImage(image);
}

void ControlBar::setCurrentTitle(QString qstr)
{
    m_label_current_name->setText(qstr);
}
void ControlBar::setRange(int min, int max)
{
    m_slider_time->setRange(min,max);
	QString current_time = getTimeQstr(0);
	QString all_time = getTimeQstr(max);
	m_label_time->setText(current_time + "/" + all_time);
	m_slider_time->setToolTip(current_time + "/" + all_time);
}
void ControlBar::setValue(int value)
{
    m_slider_time->setValue(value);
	QString current_time = getTimeQstr(value);
	QString all_time = getTimeQstr(m_slider_time->maximum());
	m_label_time->setText(current_time + "/" + all_time);
	m_slider_time->setToolTip(current_time + "/" + all_time);
}

void ControlBar::setBMute(bool b_mute)
{
    m_btn_volume->style()->unpolish(m_btn_volume);
    if(b_mute){
        m_btn_volume->setProperty("status", "mute");
    }else{
        m_btn_volume->setProperty("status", "volume");
    }
    m_btn_volume->style()->polish(m_btn_volume);
}

void ControlBar::setCurrentPlayerType(PlayType playType)
{
    //qDebug()<<"setCurrentPlayerType:"<<playType;
    m_btn_player_model->style()->unpolish(m_btn_player_model);
    switch (playType) {
        case ShufflePlay://随机播放
            m_btn_player_model->setToolTip(QString::fromLocal8Bit("随机播放"));
            m_btn_player_model->setProperty("status","random_play");
            break;
        case OrderPlay:
            m_btn_player_model->setToolTip(QString::fromLocal8Bit("顺序播放"));
            m_btn_player_model->setProperty("status","order_play");
            break;
        case SingleLoop:
            m_btn_player_model->setToolTip(QString::fromLocal8Bit("单曲循环"));
            m_btn_player_model->setProperty("status","single_loop");
            break;
        case ListLoop:
        default:
            m_btn_player_model->setToolTip(QString::fromLocal8Bit("列表循环"));
            m_btn_player_model->setProperty("status","list_loop");
            break;
    }
    m_playType = playType;
    m_btn_player_model->style()->polish(m_btn_player_model);
}

bool ControlBar::eventFilter(QObject * obj, QEvent * event)
{
	if (m_list_object.indexOf(obj) != -1) {
		if (event->type() == QEvent::Enter) {
			setCursor(Qt::PointingHandCursor);
		}
		else if (event->type() == QEvent::Leave) {
			unsetCursor(); // 恢复默认光标
		}
	}
	return QWidget::eventFilter(obj, event);
}

void ControlBar::onStateChanged(QMediaPlayer::State newState)
{
    m_btn_player_pause->style()->unpolish(m_btn_player_pause);
    if(newState == QMediaPlayer::PlayingState){
        m_btn_player_pause->setProperty("status", "play");
        m_playerAnimation->play();
    }else if(newState == QMediaPlayer::PausedState){
        m_btn_player_pause->setProperty("status", "pause");
        m_playerAnimation->stop();
    }else if(newState == QMediaPlayer::StoppedState){
        m_btn_player_pause->setProperty("status", "pause");
        m_playerAnimation->stop();
    }
    m_btn_player_pause->style()->polish(m_btn_player_pause);
}

void ControlBar::onClickedPlayerType()
{
    switch (m_playType) {
        case ShufflePlay://随机播放
            m_playType = OrderPlay;
            break;
        case OrderPlay:
            m_playType = SingleLoop;
            break;
        case SingleLoop:
            m_playType = ListLoop;
            break;
        case ListLoop:
            m_playType = ShufflePlay;
            break;
        default:
            break;
    }
    setCurrentPlayerType(m_playType);
    emit sigSetPlayerType(m_playType);
}
