/*******************************************
*LyricBar.cpp
*桌面歌词播放
********************************************/
#pragma execution_character_set("utf-8")
#include "LyricBar.h"
#include "DataStruct.h"
#include <QApplication>
#include <QStyle>
#include <QTime>
#include <QPainterPath>
#include "ControlBar.h"
#include "Common.h"

#include <QDebug>

#define RADIUS 10
#define MIN_HEIGHT 50
#define MIN_WIDTH 50

#define ACTION_CANCEL_FIXED 0

LyricBar::LyricBar(QWidget *parent)
    :QDialog(parent)
{
    setObjectName("LyricBar");
    setMouseTracking(true);
    resize(600, 100);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Tool);  // 去除窗口边框  置顶
    setAttribute(Qt::WA_TranslucentBackground);  // 设置背景透明‌:ml-citation{ref="1,6" data="citationList"}
    QVBoxLayout* vbox_main = new QVBoxLayout;
    m_lyricBarControl = new LyricBarControl;
    m_lyricPlayer = new LyricPlayer;
    vbox_main->addWidget(m_lyricBarControl);
    vbox_main->addWidget(m_lyricPlayer);
    vbox_main->setMargin(0);
    vbox_main->setSpacing(0);
    setLayout(vbox_main);

	m_b_enter = true;
	m_b_fixed = false;

	mouseLeftPressChangeSize = false;
	mouseLeftPressMove = false;

	m_lyricBarControl->setFixedHeight(30);
	m_lyricBarControl->hiddenBtns();

    connect(m_lyricBarControl, &LyricBarControl::sigGoMain, this, &LyricBar::sigGoMain);
    connect(m_lyricBarControl, &LyricBarControl::sigBack, m_lyricPlayer, &LyricPlayer::subTimeout);
    connect(m_lyricBarControl, &LyricBarControl::sigForward, m_lyricPlayer, &LyricPlayer::addTimeout);
    connect(m_lyricBarControl, &LyricBarControl::sigPre, this, &LyricBar::sigPre);
    connect(m_lyricBarControl, &LyricBarControl::sigNext, this, &LyricBar::sigNext);
    connect(m_lyricBarControl, &LyricBarControl::sigPlayerOrPause, this, &LyricBar::sigPlayerOrPause);
    connect(m_lyricBarControl, &LyricBarControl::sigSet, this, &LyricBar::sigSet);
    connect(m_lyricBarControl, &LyricBarControl::sigFiexd, this, &LyricBar::onFiexd);
    connect(m_lyricBarControl, &LyricBarControl::sigClose, this, &LyricBar::close);

    connect(m_lyricPlayer, &LyricPlayer::sigEnter, this, &LyricBar::onEnter);
    connect(m_lyricPlayer, &LyricPlayer::sigEnter, m_lyricBarControl, &LyricBarControl::showBtns);

    connect(this, &LyricBar::sigLeave, this, &LyricBar::onLeave);
    connect(this, &LyricBar::sigLeave, m_lyricBarControl, &LyricBarControl::hiddenBtns);
    connect(this, &LyricBar::stateChanged, m_lyricBarControl, &LyricBarControl::onStateChanged);
    connect(this, &LyricBar::stateChanged, m_lyricPlayer, &LyricPlayer::onStateChanged);

    onLeave();
}

LyricBar::~LyricBar()
{
	m_lyricBarControl->deleteLater();
	m_lyricPlayer->deleteLater();
}

void LyricBar::setCurrentLyricSingle(QString lyric, int time)
{
    m_lyricPlayer->setCurrentLyricSingle(lyric, time);
}

void LyricBar::setCurrentLyricDouble(QString lyric_one, QString lyric_two, int time)
{
    m_lyricPlayer->setCurrentLyricDouble(lyric_one, lyric_two, time);
}

void LyricBar::clear()
{
    m_lyricPlayer->clear();
}

void LyricBar::onEnter()
{
    m_b_enter = true;
    update();
}

void LyricBar::onLeave()
{
    m_b_enter = false;
    update();
}

void LyricBar::mousePressEvent(QMouseEvent *event)
{
    if(m_b_fixed){
        return;
    }
    QRect rectThis = this->geometry(); //界面Rect
    QPoint currentPoint = QCursor::pos();//获取鼠标的绝对位置

    int m_border = 20;//焦点范围
    int Temp_FlagRight = 0,Temp_FlagBotton = 0;

    if (event->buttons() == Qt::LeftButton) {
        //下
        if( qAbs( currentPoint.y() - rectThis.y() - this->height() ) < m_border ){
            Temp_FlagBotton = 1;
            mouseLeftPressChangeSize = true;
        }
        //右
        if( qAbs( currentPoint.x() - rectThis.x() - this->width() ) < m_border ){
            Temp_FlagRight = 1;
            mouseLeftPressChangeSize = true;
        }
        //右下
        if( Temp_FlagBotton && Temp_FlagRight ){
            mouseLeftPressChangeSize = true;
        }

        if(!mouseLeftPressChangeSize &&
                currentPoint.x() > this->x() && currentPoint.x() < this->x() + width() &&
                currentPoint.y() > this->y() && currentPoint.y() < this->y() + height()){
            mouseLeftPressMove = true;
            pressPoint = event->globalPos() - pos();
        }
    }
}

void LyricBar::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mouseLeftPressMove = false;
    mouseLeftPressChangeSize = false;
}

void LyricBar::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug()<<"mouseMoveEvent:";
    if(m_b_fixed){
        return;
    }
    Q_UNUSED(event);
    QRect rectThis = this->geometry();
    QPoint currentPoint = QCursor::pos();//获取鼠标的绝对位置
    int Temp_x = currentPoint.x() - rectThis.x();
    int Temp_y = currentPoint.y() - rectThis.y();
    if( mouseLeftPressChangeSize == false ){
        int m_border = 20;
        this->setCursor(Qt::ArrowCursor);
        int Temp_FlagRight = 0,Temp_FlagBotton = 0;

        //下
        if( qAbs( Temp_y - this->height() ) < m_border ){
            Temp_FlagBotton = 1;
            mousePosition = Bottom;
            this->setCursor(Qt::SizeVerCursor);
        }
        //右
        if( qAbs( Temp_x - this->width() ) < m_border ){
            Temp_FlagRight = 1;
            mousePosition = Right;
            this->setCursor(Qt::SizeHorCursor);
        }
        //右下
        if( Temp_FlagBotton && Temp_FlagRight ){
            this->setCursor(Qt::SizeFDiagCursor);
            mousePosition = RightBottom;
        }
    }
    else{
        if ((event->buttons() == Qt::LeftButton) )
        {
            if(Temp_x < MIN_WIDTH || Temp_y < MIN_WIDTH)
               return;
            switch (mousePosition) {
            case Bottom://下
                move(rectThis.x(),rectThis.y());
                this->setFixedHeight( Temp_y );
                break;
            case Right://右
                move(rectThis.x(),rectThis.y());
                this->setFixedWidth( Temp_x );
                break;
            case RightBottom://右下
                move(rectThis.x(),rectThis.y());
                this->setFixedSize( Temp_x ,Temp_y );
                break;
            default:
                break;
            }
        }
    }

    if (mouseLeftPressMove && (event->buttons() == Qt::LeftButton))
    {
        move(event->globalPos() - pressPoint);
    }
}

void LyricBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if(!m_b_enter || m_b_fixed){
        return;
    }
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);//文字抗锯齿
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0,0,0,100));
    painter.drawRoundedRect(0, 0, width(),height(),RADIUS,RADIUS);
    return QWidget::paintEvent(event);
}

void LyricBar::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    //qDebug()<<"leaveEvent";
    emit sigLeave();
}

void LyricBar::onFiexd(bool bl)
{
    m_b_fixed = bl;
    update();
}

LyricBarControl::LyricBarControl(QWidget *parent)
    :QWidget(parent)
{
    setObjectName("LyricBarControl");
    setAttribute(Qt::WA_StyledBackground,true);
    setAttribute(Qt::WA_TranslucentBackground);  // 设置背景透明‌:ml-citation{ref="1,6" data="citationList"}
    QHBoxLayout* hbox_main = new QHBoxLayout;
    m_btn_show_main = new QPushButton;
    m_btn_back = new QPushButton;
    m_btn_forward = new QPushButton;
    m_btn_pre = new QPushButton;
    m_btn_player_pause = new QPushButton;
    m_btn_next= new QPushButton;
    m_btn_set= new QPushButton;
    m_btn_fixed= new QPushButton;
    m_btn_close= new QPushButton;

    hbox_main->addStretch();
    hbox_main->addWidget(m_btn_show_main);
    hbox_main->addWidget(m_btn_back);
    hbox_main->addWidget(m_btn_forward);
    hbox_main->addWidget(m_btn_pre);
    hbox_main->addWidget(m_btn_player_pause);
    hbox_main->addWidget(m_btn_next);
    hbox_main->addWidget(m_btn_set);
    hbox_main->addWidget(m_btn_fixed);
    hbox_main->addWidget(m_btn_close);
    hbox_main->addStretch();
    setLayout(hbox_main);
    hbox_main->setMargin(0);
    hbox_main->setSpacing(10);

    m_btn_back->setText("-50ms");
    m_btn_forward->setText("+50ms");

    m_btn_show_main->setIcon(QIcon(":/resource/resource/none.png"));
    //m_btn_back->setIcon(QIcon(":/resource/resource/none.png"));
    //m_btn_forward->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_pre->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_player_pause->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_next->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_set->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_fixed->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_close->setIcon(QIcon(":/resource/resource/none.png"));

    m_btn_show_main->setObjectName("lyric_show_main");
    m_btn_back->setObjectName("lyric_back");
    m_btn_forward->setObjectName("lyric_forward");
    m_btn_pre->setObjectName("lyric_pre");
    m_btn_player_pause->setObjectName("lyric_player_pause");
    m_btn_next->setObjectName("lyric_next");
    m_btn_set->setObjectName("lyric_set");
    m_btn_fixed->setObjectName("lyric_fixed");
    m_btn_close->setObjectName("lyric_close");

    m_btn_show_main->setToolTip(tr("Show mainwindow"));
    m_btn_back->setToolTip(tr("Lyric play slow 50ms"));
    m_btn_forward->setToolTip(tr("Lyric play fast 50ms"));
    m_btn_pre->setToolTip(tr("Previous"));
    m_btn_player_pause->setToolTip(tr("Play"));
    m_btn_next->setToolTip(tr("Next"));
    m_btn_set->setToolTip(tr("Set"));
    m_btn_fixed->setToolTip(tr("Lock lyric"));
    m_btn_close->setToolTip(tr("Unlock lyric"));

    connect(m_btn_show_main, &QPushButton::clicked, this, &LyricBarControl::sigGoMain);
    connect(m_btn_back, &QPushButton::clicked, this, &LyricBarControl::sigBack);
    connect(m_btn_forward, &QPushButton::clicked, this, &LyricBarControl::sigForward);
    connect(m_btn_pre, &QPushButton::clicked, this, &LyricBarControl::sigPre);
    connect(m_btn_player_pause, &QPushButton::clicked, this, &LyricBarControl::sigPlayerOrPause);
    connect(m_btn_next, &QPushButton::clicked, this, &LyricBarControl::sigNext);
    connect(m_btn_set, &QPushButton::clicked, this, &LyricBarControl::sigSet);
    connect(m_btn_fixed, &QPushButton::clicked, this, &LyricBarControl::onClickedFiexd);
    connect(m_btn_close, &QPushButton::clicked, this, &LyricBarControl::sigClose);
    m_b_fixed = false;

    m_btn_player_pause->setProperty("status","pause");
    m_btn_fixed->setProperty("status","unlock");
}

void LyricBarControl::showBtns()
{
    m_btn_fixed->setVisible(true);
    if(!m_b_fixed){
        setBtnsVisible(true);
    }
}

void LyricBarControl::hiddenBtns()
{
    if(m_b_fixed){
        //延时隐藏500ms
        QTime dieTime = QTime::currentTime().addMSecs(500);
        while( QTime::currentTime() < dieTime ){
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
    }
    if(m_b_enter){
        return;
    }
    m_btn_fixed->setVisible(false);
    if(!m_b_fixed){
        setBtnsVisible(false);
    }
}

void LyricBarControl::setBtnsVisible(bool bl)
{
    m_btn_player_pause->setVisible(bl);
    m_btn_show_main->setVisible(bl);
    m_btn_back->setVisible(bl);
    m_btn_forward->setVisible(bl);
    m_btn_pre->setVisible(bl);
    m_btn_next->setVisible(bl);
    m_btn_set->setVisible(bl);
    m_btn_close->setVisible(bl);
    m_btn_fixed->setToolTip(tr(bl ? "Lock lyric":"Unlock lyric"));
}

void LyricBarControl::onStateChanged(QMediaPlayer::State newState)
{
    m_btn_player_pause->style()->unpolish(m_btn_player_pause);
    if(newState == QMediaPlayer::PlayingState){
        m_btn_player_pause->setProperty("status", "play");
    }else if(newState == QMediaPlayer::PausedState){
        m_btn_player_pause->setProperty("status", "pause");
    }else if(newState == QMediaPlayer::StoppedState){
        m_btn_player_pause->setProperty("status", "pause");
    }
    m_btn_player_pause->style()->polish(m_btn_player_pause);
}

void LyricBarControl::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_b_enter = true;
}

void LyricBarControl::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_b_enter = false;
}

void LyricBarControl::onClickedFiexd()
{
    m_b_fixed = !m_b_fixed;
    setBtnsVisible(!m_b_fixed);
    emit sigFiexd(m_b_fixed);

    m_btn_fixed->style()->unpolish(m_btn_fixed);
    if(m_b_fixed){
        m_btn_fixed->setProperty("status", "lock");
    }else{
        m_btn_fixed->setProperty("status", "unlock");
    }
    m_btn_fixed->style()->polish(m_btn_fixed);
}

LyricPlayer::LyricPlayer(QWidget *parent)
    :QWidget(parent),
      m_lyric_one(""),
      m_lyric_two(""),
      m_every_time(100),
      m_time_out(100),
      m_text_color(Qt::white),
      m_player_color(Qt::red)
{
    setObjectName("LyricPlayer");
    setAttribute(Qt::WA_StyledBackground,true);
    setAttribute(Qt::WA_TranslucentBackground);  // 设置背景透明‌:ml-citation{ref="1,6" data="citationList"}
    setMouseTracking(true);
    connect(&m_timer, &QTimer::timeout, this, &LyricPlayer::onTimeout);
    m_timer.setInterval(m_time_out);//100ms
}

void LyricPlayer::setCurrentLyricSingle(QString lyric, int time)
{
    m_current_time = 0;
    m_time = time*1000;//转换为ms
    m_lyric_one = lyric;
    update();
}

void LyricPlayer::setCurrentLyricDouble(QString lyric_one, QString lyric_two, int time)
{
    m_current_time = 0;
    m_lyric_one = lyric_one;
    m_lyric_two = lyric_two;
    m_time = time*1000;//转换为ms
    update();
}

void LyricPlayer::addTimeout()
{
    m_every_time +=50;
    //qDebug()<<"m_every_time:"<<m_every_time;
}

void LyricPlayer::subTimeout()
{
    m_every_time -=50;
    if(m_every_time<=0){
        m_every_time = 50;
    }
    //qDebug()<<"m_every_time:"<<m_every_time;
}

void LyricPlayer::clear()
{
    m_lyric_one="";
    m_lyric_two="";
	update();
}

void LyricPlayer::onStateChanged(QMediaPlayer::State newState)
{
    if(newState == QMediaPlayer::PlayingState){
        m_timer.start();
    }else if(newState == QMediaPlayer::PausedState){
        m_timer.stop();
    }else if(newState == QMediaPlayer::StoppedState){
        m_timer.stop();
    }
}

void LyricPlayer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if(m_lyric_one.isEmpty()){
        m_lyric_one = tr("No found lyric");
    }
    //qDebug()<<"m_lyric_one:"<<m_lyric_one;
    //qDebug()<<"m_lyric_two:"<<m_lyric_two;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);//文字抗锯齿

    if(m_lyric_two.isEmpty()){
        //single
        paintSingle(painter);
    }else{
        paintDouble(painter);
    }

    return QWidget::paintEvent(event);
}

void LyricPlayer::paintSingle(QPainter &painter)
{
    QFont ft = painter.font();
    int width, height;
    int fontSize = getFontSize(m_lyric_one, width, height);
    ft.setPointSize(fontSize);
    painter.setFont(ft);
    painter.setPen(m_text_color);
    painter.drawText(this->width()/2 - width/2, height, m_lyric_one);

    QPainterPath path;
    path.addText(this->width()/2 - width/2, height, ft, m_lyric_one);  // 指定起始坐标和文本
    painter.setPen(Qt::gray);
    painter.drawPath(path);  // 绘制路径
    painter.setClipPath(path);

	if (m_time <= 0) {
		return;
	}
    painter.setBrush(m_player_color);
    painter.drawRect(this->width()/2 - width/2,0,width*m_current_time/m_time, this->height());
}

void LyricPlayer::paintDouble(QPainter &painter)
{
    QFont ft = painter.font();
    QString lyric_one, lyric_two;
    int font_size_one, font_size_two, width_one, width_two,height_one, height_two;
    getLyricInfo(lyric_one,lyric_two,
                font_size_one, font_size_two,
                width_one, width_two,
                height_one, height_two);
    int width = width_one > width_two ? width_one: width_two;

    ft.setPointSize(font_size_one);
    painter.setFont(ft);
    painter.setPen(m_text_color);
    painter.drawText(this->width()/2 - width_one/2, height_one, lyric_one);
    ft.setPointSize(font_size_two);
    painter.setFont(ft);
    painter.drawText(this->width()/2 - width_two/2, height_one + height_two + PAINTER_LYRIC_BORDER, lyric_two);

    QPainterPath path;
    //第一行歌词
    ft.setPointSize(font_size_one);
    painter.setFont(ft);
    path.addText(this->width()/2 - width_one/2, height_one, ft, lyric_one);  // 指定起始坐标和文本
    painter.setPen(Qt::gray);
    painter.drawPath(path);  // 绘制路径
    //painter.setClipPath(path);
    ft.setPointSize(font_size_two);
    painter.setFont(ft);
    path.addText(this->width()/2 - width_two/2, height_one + height_two + PAINTER_LYRIC_BORDER, ft, lyric_two);  // 指定起始坐标和文本
    painter.setPen(Qt::gray);
    painter.drawPath(path);  // 绘制路径
    painter.setClipPath(path);

	if (m_time <= 0) {
		return;
	}
    //绘制动态着色
    painter.setBrush(m_player_color);
    painter.drawRect(this->width()/2 - width/2, 0, width*m_current_time/m_time, height());
}

void LyricPlayer::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    //qDebug()<<"enterEvent";
    emit sigEnter();
}

void LyricPlayer::onTimeout()
{
    m_current_time+=m_every_time;
    update();
}

int LyricPlayer::getFontSize(const QString &lyric, int &width, int &height, const bool &b_single)
{
    int max_height = b_single ? this->height() - LYRIC_BORDER*2 : this->height()*2/3 - LYRIC_BORDER*2;
    int fontSize = 0;
    QFont ft;
    do {
        fontSize++;
        ft.setPointSize(fontSize);
        QFontMetrics metrics(ft); // 获取当前字体的度量信息
        height = metrics.height(); // 计算文本宽度
        width = metrics.horizontalAdvance(lyric);
    }while (height < max_height && width < this->width() - LYRIC_BORDER*2);

    return fontSize;
}

int LyricPlayer::updateFontSize(QString lyric, const int &width, const int &height, int &width_change, int &height_change)
{
    int fontSize = 0;
    QFont ft;
    do {
        fontSize++;
        ft.setPointSize(fontSize);
		QFontMetrics metrics(ft); // 获取当前字体的度量信息
        width_change = metrics.horizontalAdvance(lyric);// 获取当前字体的度量信息
		height_change = metrics.height();
    }while(width_change < width && height_change < height);

    return fontSize;
}

void LyricPlayer::getLyricInfo(QString &lyric_one, QString &lyric_two,
                               int &font_size_one, int &font_size_two,
                               int &width_one, int &width_two,
                               int &height_one, int &height_two)
{
    //中文第一行
    bool isEnglish_one = isEnglishMusic(m_lyric_one);
    if(isEnglish_one){
        lyric_one = m_lyric_two;
        lyric_two = m_lyric_one;
    }else{
        lyric_one = m_lyric_one;
        lyric_two = m_lyric_two;
    }
    font_size_one = getFontSize(lyric_one, width_one, height_one, false);
    //根据第一个改变第二个尺寸
    font_size_two = updateFontSize(lyric_two, width_one, height_one, width_two, height_two);
}
