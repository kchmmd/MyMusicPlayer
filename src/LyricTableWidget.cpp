/*******************************************
*LyricTableWidget.cpp
*歌词列表
********************************************/
#pragma execution_character_set("utf-8")
#include "LyricTableWidget.h"
#include "Common.h"

LyricTableWidget::LyricTableWidget(QWidget *parent):QWidget(parent)
{
    setObjectName("LyricTableWidget");
    setAttribute(Qt::WA_TranslucentBackground);
    QVBoxLayout* vbox_main = new QVBoxLayout;
	QVBoxLayout* vbox = new QVBoxLayout;
    m_lyricTitle = new LyricTitle;
    m_tableWidget = new QTableWidget;
	m_widget_loading = new QWidget;
	m_label_loading = new QLabel(m_widget_loading);
	vbox->addStretch();
	vbox->addWidget(m_label_loading);
	vbox->addStretch();
	m_widget_loading->setLayout(vbox);
    vbox_main->addWidget(m_lyricTitle);
	vbox_main->addWidget(m_tableWidget);
	vbox_main->addWidget(m_widget_loading);
    setLayout(vbox_main);
    m_lyricTitle->setFixedHeight(LYRIC_TITLE_HEIGHT);

    m_tableWidget->setGridStyle(Qt::NoPen);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setFrameStyle(QAbstractItemView::NoFrame);
    m_tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_tableWidget->horizontalHeader()->setVisible(false);
    m_tableWidget->verticalHeader()->setVisible(false);
    m_tableWidget->setColumnCount(1);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableWidget->setShowGrid(false);
    m_tableWidget->setObjectName("table_lyric");

    m_default_ft.setBold(false);
    m_default_ft.setPointSize(DEFAULT_LYRIC_FONT_SIZE);
    m_current_ft.setBold(true);
    m_current_ft.setPointSize(CURRETN_LYRIC_FONT_SIZE);

	m_widget_loading->setObjectName("widget_loading");
	m_label_loading->setObjectName("label_loading");
	m_label_loading->setAlignment(Qt::AlignCenter);
	QFont ft;
	ft.setPointSize(20);
	m_label_loading->setAttribute(Qt::WA_StyledBackground, true);
	m_label_loading->setFont(ft);

	m_timer.setInterval(1000);

    connect(&m_timer, &QTimer::timeout, this, &LyricTableWidget::onTimeout);
    connect(m_tableWidget, &QTableWidget::itemDoubleClicked, this, &LyricTableWidget::itemDoubleClicked);
    connect(m_tableWidget, &QTableWidget::itemDoubleClicked, this, &LyricTableWidget::onItemDoubleClicked);
}

LyricTableWidget::~LyricTableWidget()
{
	m_lyricTitle->deleteLater();
	m_tableWidget->deleteLater();
	m_widget_loading->deleteLater();
	m_label_loading->deleteLater();
}

void LyricTableWidget::clear()
{
	setTitle("","");
	setLyricData(std::vector<LineLyricData>());
	setLoading();
	m_time = 0;
}

void LyricTableWidget::setLoading()
{
	m_tableWidget->hide();
	m_label_loading->setText(tr("Lyric loading..."));
	m_widget_loading->show();
}

void LyricTableWidget::setLyricData(std::vector<LineLyricData> vec_lyric)
{
	if (vec_lyric.size() > 0) {
		m_widget_loading->hide();
		m_tableWidget->show();
	}
    m_pre_item = nullptr;
    m_vec_lyric = vec_lyric;
    m_tableWidget->clear();
    m_tableWidget->setRowCount(vec_lyric.size());
    int i=0;
    for(LineLyricData lyricData:vec_lyric){
        QTableWidgetItem* item = new QTableWidgetItem;
        item->setFont(m_default_ft);
        item->setTextAlignment(Qt::AlignCenter);
        item->setText(lyricData.lyric);
        item->setData(Qt::UserRole, lyricData.start_time);
        item->setToolTip(QString("%1-%2").arg(lyricData.start_time).arg(lyricData.end_time));
        m_tableWidget->setRowHeight(i, lyricData.lyric.contains("\n") ? LYRIC_DOUBLE_HEIGHT : LYRIC_SINGLE_HEIGHT);
        m_tableWidget->setItem(i, 0, item);
        i++;
    }
}

void LyricTableWidget::setCurrentDuration(qint64 duration)
{
    m_time = duration/1000;
    int index = getIndex(m_time);
    if(index != -1){
        this->setCurrentLyric(index);
    }
}

void LyricTableWidget::setCurrentLyric(int index)
{
    if(m_pre_item && index == m_pre_item->row()){
        return;
    }
    if(m_pre_item){
        m_pre_item->setFont(m_default_ft);
    }
    m_tableWidget->item(index, 0)->setFont(m_current_ft);
    m_tableWidget->selectRow(index);
    m_pre_item = m_tableWidget->item(index, 0);
   // qDebug()<<"m_pre_item->text():"<<m_pre_item->text();
    QString lyric = m_pre_item->text();
    if(lyric.contains("\n")){
        QStringList lyric_list = lyric.split("\n");
        emit sigCurrentLyricDouble(lyric_list[0], lyric_list[1], m_vec_lyric[index].end_time - m_vec_lyric[index].start_time);
    }else{
        emit sigCurrentLyricSingle(lyric, m_vec_lyric[index].end_time - m_vec_lyric[index].start_time);
    }
}

void LyricTableWidget::setTitle(QString music_name, QString user_name)
{
    m_lyricTitle->setTitle(music_name, user_name);
   // m_tableWidget->verticalHeader()->set(-1 == getChineseIndex(music_name) ? 60 : 30);
}

void LyricTableWidget::player()
{
    m_timer.start();
}

void LyricTableWidget::pause()
{
    m_timer.stop();
}

void LyricTableWidget::stop()
{
    m_timer.stop();
    m_time = 0;
}

void LyricTableWidget::reStart()
{
    m_time = 0;
}

void LyricTableWidget::onStateChanged(QMediaPlayer::State newState)
{
    if(newState == QMediaPlayer::PlayingState){
        player();
    }else if(newState == QMediaPlayer::PausedState){
        pause();
    }else if(newState == QMediaPlayer::StoppedState){
        stop();
    }
}

void LyricTableWidget::onTimeout()
{
    m_time++;
    int index = getIndex(m_time);
    if(index != -1){
        this->setCurrentLyric(index);
    }
}

void LyricTableWidget::onItemDoubleClicked(QTableWidgetItem *item)
{
    m_time = item->data(Qt::UserRole).toLongLong();
    int index = getIndex(m_time);
    if(index != -1){
        this->setCurrentLyric(index);
    }
}

int LyricTableWidget::getIndex(qint64 time)
{
    for(LineLyricData data: m_vec_lyric){
        if(time >= data.start_time && time < data.end_time){
            return data.index;
        }
    }
    return -1;
}
