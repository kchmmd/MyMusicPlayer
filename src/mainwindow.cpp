/*******************************************
*mainwindow.cpp
*播放器主界面
********************************************/
#pragma execution_character_set("utf-8")

#include "mainwindow.h"
#include "PlayerAnimation.h"
#include "Common.h"

#include <set>
#include <map>
#include <QApplication>
#include <QCoreApplication>
#include <QStyle>
#include <QTime>
#include <QDateTime>
#include <QMediaContent>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QMediaPlaylist>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QRandomGenerator>
#include <QPropertyAnimation>
#include <QDebug>
#include <QDir>
#include <QUrl>
#include <QStandardPaths>
#include <QDirIterator>
#include <QDomDocument>
#include <QDesktopWidget>
#include <QEventLoop>
#include <QApplication>
#include <QRandomGenerator>

#define RESOURCE "/resource/config.xml"
#define LOG_FILE_NAME "log.txt"


MainWindow::MainWindow(QWidget *parent)
    : MyWidget(parent)
{
    setObjectName("mainwindow");
    QVBoxLayout* vbox = new QVBoxLayout;
    m_headerBar = new HeaderBar;
    m_musicList = new MusicList;
    m_controlBar = new ControlBar;
    m_lyricTableWidget = new LyricTableWidget;
    m_myVolumeControlBar = new MyVolumeControlBar;
    m_lyricBar = new LyricBar;
    m_systemTrayIcon = new QSystemTrayIcon;

    vbox->addWidget(m_headerBar);
    vbox->addWidget(m_lyricTableWidget);
    vbox->addWidget(m_controlBar);
    this->setLayout(vbox);

    m_menu_mainwindow = new QMenu;
    m_menu_music_list = new QMenu;
    m_menu_trayIcon = new QMenu;

    m_menu_mainwindow->setWindowFlag(Qt::FramelessWindowHint);
    m_menu_music_list->setWindowFlag(Qt::FramelessWindowHint);
    m_menu_trayIcon->setWindowFlag(Qt::FramelessWindowHint);
    m_menu_mainwindow->setAttribute(Qt::WA_TranslucentBackground);
    m_menu_music_list->setAttribute(Qt::WA_TranslucentBackground);
    m_menu_trayIcon->setAttribute(Qt::WA_TranslucentBackground);
    //在windows下，自带的阴影效果仍然是直角，还需设置去除阴影效果
    m_menu_mainwindow->setWindowFlag(Qt::NoDropShadowWindowHint);
    m_menu_trayIcon->setWindowFlag(Qt::NoDropShadowWindowHint);

    m_systemTrayIcon->setContextMenu(m_menu_trayIcon);
    QAction* act_add = new QAction("添加歌曲");
    QAction* act_scan = new QAction("扫描歌曲");
    QAction* act_remove = new QAction("从列表中删除");
    QAction* act_show = new QAction("显示", m_menu_trayIcon);
    QAction* act_show_reset = new QAction("坐标重置", m_menu_trayIcon);
    QAction* act_quit = new QAction("退出", m_menu_trayIcon);
    act_add->setData(ACTION_ADD);
    act_scan->setData(ACTION_SCAN);
    act_remove->setData(ACTION_REMOVE);
    act_show->setData(ACTION_SHOW);
    act_show_reset->setData(ACTION_SHOW_RESET);
    act_quit->setData(ACTION_QUIT);
    m_menu_mainwindow->addAction(act_scan);
    m_menu_mainwindow->addAction(act_add);
    m_menu_music_list->addAction(act_scan);
    m_menu_music_list->addAction(act_add);
    m_menu_music_list->addAction(act_remove);
    m_menu_trayIcon->addAction(act_show);
    m_menu_trayIcon->addAction(act_show_reset);
    m_menu_trayIcon->addAction(act_quit);
    m_musicList->setMenu(m_menu_music_list);

    QMediaPlaylist* player_list = new QMediaPlaylist;
    m_player = new QMediaPlayer(this);
    m_player->setPlaylist(player_list);

	m_musicListManager = new MusicListManager;
	m_musicListManager_thread = new QThread;
	m_musicListManager->moveToThread(m_musicListManager_thread);
	m_musicListManager_thread->start();

	m_musicLyricManager = new MusicLyricManager;
	m_musicLyricManager_thread = new QThread;
	m_musicLyricManager->moveToThread(m_musicLyricManager_thread);
	m_musicLyricManager_thread->start();

	connect(m_musicListManager, &MusicListManager::sigAddLog, this, &MainWindow::addLog);
	connect(m_musicListManager, &MusicListManager::sigAddMusicData, this, &MainWindow::onAddMusicData);
	connect(this, &MainWindow::sigAddMusicFiles, m_musicListManager, &MusicListManager::anlaysisMusicFiles);
	connect(m_musicList, &MusicList::sigAddUrls, m_musicListManager, &MusicListManager::anlaysisMusicUrls);

	connect(m_musicLyricManager, &MusicLyricManager::sigAddLog, this, &MainWindow::addLog);
	connect(m_musicLyricManager, &MusicLyricManager::sigMusicLyric, this, &MainWindow::onMusicLyric);
	connect(this, &MainWindow::sigAnalysisMusicLyric, m_musicLyricManager, &MusicLyricManager::analysisMusicLyric);

    connect(m_menu_music_list, &QMenu::triggered, this, &MainWindow::onMenu);
    connect(m_menu_trayIcon, &QMenu::triggered, this, &MainWindow::onMenu);
    connect(m_systemTrayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onActivated);

    connect(m_headerBar, &HeaderBar::sigClickedClose, this, &MainWindow::close);
    connect(m_headerBar, &HeaderBar::sigClickedMaxOrNormal, this, &MainWindow::onClickedMaxOrNormal);
    connect(m_headerBar, &HeaderBar::sigClickedMin, this, &MainWindow::showMinimized);

    connect(m_headerBar, &HeaderBar::sigClickedStyle, this, &MainWindow::onSetStyle);

    connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::onDurationChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::onPositionChanged);
    connect(m_player, &QMediaPlayer::stateChanged, m_lyricTableWidget, &LyricTableWidget::onStateChanged);
    connect(m_player, &QMediaPlayer::currentMediaChanged, this, &MainWindow::onCurrentMediaChanged);
    connect(m_player, &QMediaPlayer::stateChanged, m_controlBar, &ControlBar::onStateChanged);
    connect(m_player, &QMediaPlayer::stateChanged, m_lyricBar, &LyricBar::stateChanged);

    connect(m_lyricTableWidget, &LyricTableWidget::itemDoubleClicked, this, &MainWindow::onLyricDoubleClicked);
    connect(m_lyricTableWidget, &LyricTableWidget::sigCurrentLyricSingle, m_lyricBar, &LyricBar::setCurrentLyricSingle);
    connect(m_lyricTableWidget, &LyricTableWidget::sigCurrentLyricDouble, m_lyricBar, &LyricBar::setCurrentLyricDouble);

    connect(m_lyricBar, &LyricBar::sigGoMain, this, &MainWindow::onClickedGoMain);
    connect(m_lyricBar, &LyricBar::sigPre, this, &MainWindow::onClickedPre);
    connect(m_lyricBar, &LyricBar::sigPlayerOrPause, this, &MainWindow::onClickedPlayerOrPause);
    connect(m_lyricBar, &LyricBar::sigNext, this, &MainWindow::onClickedNext);

    connect(m_musicList, &MusicList::itemDoubleClicked, this,&MainWindow::onItemDoubleClicked);

    connect(m_controlBar, &ControlBar::sliderMoved, this, &MainWindow::onSliderMoved);
    connect(m_controlBar, &ControlBar::sliderPressed, this, &MainWindow::onSliderPressed);
    connect(m_controlBar, &ControlBar::sliderReleased, this, &MainWindow::onSliderReleased);
    connect(m_controlBar, &ControlBar::valueChanged, this, &MainWindow::onSliderValueChanged);
    connect(m_controlBar, &ControlBar::sigClickedPre, this, &MainWindow::onClickedPre);
    connect(m_controlBar, &ControlBar::sigClickedPlayerPause, this, &MainWindow::onClickedPlayerOrPause);
    connect(m_controlBar, &ControlBar::sigClickedNext, this, &MainWindow::onClickedNext);
    connect(m_controlBar, &ControlBar::sigClickedMute, this, &MainWindow::onClickedMute);
    connect(m_controlBar, &ControlBar::sigBtnVolumeEnter, this, &MainWindow::onEnterMute);
    connect(m_controlBar, &ControlBar::sigClickedTable, this, &MainWindow::onClickedTable);
    connect(m_controlBar, &ControlBar::sigClickedLyric, this, &MainWindow::onClickedLyric);
    connect(m_controlBar, &ControlBar::sigSetPlayerType, this, &MainWindow::onSetCurrentPlayType);

    connect(m_myVolumeControlBar, &MyVolumeControlBar::valueChanged, this, &MainWindow::onVolumeValueChanged);

	m_musicList->hide();
	m_controlBar->setFocus();
	m_systemTrayIcon->show();

    init();
    initLog();
}

MainWindow::~MainWindow()
{
    setConfig();
    saveConfig();
    saveReCord();
    m_menu_trayIcon->deleteLater();
    m_menu_mainwindow->deleteLater();
    m_menu_music_list->deleteLater();
    m_musicList->deleteLater();
    m_controlBar->deleteLater();
    m_myVolumeControlBar->deleteLater();
    m_lyricBar->deleteLater();
    m_systemTrayIcon->deleteLater();

	m_musicListManager_thread->quit();
	m_musicListManager_thread->wait();
	m_musicListManager_thread->deleteLater(); 
	m_musicListManager->deleteLater();

	m_musicLyricManager_thread->quit();
	m_musicLyricManager_thread->wait();
	m_musicLyricManager_thread->deleteLater();
	m_musicLyricManager->deleteLater();
    quitLog();
}

void MainWindow::init()
{
    m_playType = ListLoop;
    m_slider_press = false;
    initConfig();
    loadConfig();

	m_headerBar->setTitle(m_config.software_name);
	m_headerBar->setTitleFontSize(15);
	m_headerBar->setTitleToolTip(m_config.software_about);
	m_musicLyricManager->init(m_config.lyric_analysis_cmd, m_config.lyric_analysis_exe, m_config.lyric_analysis_model);
    setWindowTitle(m_config.software_name);
    setWindowIcon(QIcon(getRootPath() + "/resource/" + m_config.icon));
    m_systemTrayIcon->setIcon(QIcon(getRootPath() + "/resource/" + m_config.icon));
    m_systemTrayIcon->setToolTip(m_config.software_name);
    m_lyricBar->setVisible(m_config.lyric_show);
    m_lyricBar->setGeometry(m_config.lyric_x, m_config.lyric_y, m_config.lyric_width, m_config.lyric_height);
    this->setGeometry(m_config.widget_x, m_config.widget_y, m_config.widget_width, m_config.widget_height);
    m_controlBar->setCurrentPlayerType(m_config.playType);
    m_playType = m_config.playType;
	m_myVolumeControlBar->setVolume(m_config.volume);
    // 加载QSS文件
    QString style_qss = getCurrentStyle();
    setStyle(style_qss);//加载样式
    loadReCord();//加载记录
}

void MainWindow::showReset()
{
    this->setGeometry(DEFAULT_WIDGET_X, DEFAULT_WIDGET_Y, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT);
}

void MainWindow::initConfig()
{
    m_config.volume = 50;
    m_config.software_name = QString::fromLocal8Bit("k歌之王");
	m_config.software_about = "软件：k歌之王\n作者：kch";
    m_config.icon = "logo.ico";
    m_config.widget_width = DEFAULT_WIDGET_WIDTH;
    m_config.widget_height = DEFAULT_WIDGET_HEIGHT;
    m_config.widget_x = DEFAULT_WIDGET_X;
    m_config.widget_y = DEFAULT_WIDGET_Y;
    m_config.lyric_show = false;
    m_config.lyric_width = DEFAULT_LYRIC_BAR_WIDTH;
    m_config.lyric_height = DEFAULT_LYRIC_BAR_HEIGHT;
    m_config.lyric_x = QApplication::desktop()->width()/2 - m_config.lyric_width/2;
    m_config.lyric_y = 0;
    m_config.playType = m_playType;
    m_config.style_index = 0;
    m_config.set_styles.insert("/resource/style_black.qss");
    m_config.set_styles.insert("/resource/style_white.qss");

	m_config.lyric_analysis_cmd = LYRIC_ANALYSIS_CMD;
	m_config.lyric_analysis_exe = LYRIC_ANALYSIS_EXE;
	m_config.lyric_analysis_model = LYRIC_ANALYSIS_MODEL;

	m_current_index = 0;
}

void MainWindow::setConfig()
{
    m_config.volume = m_myVolumeControlBar->value();
    m_config.software_name = this->windowTitle();
    m_config.lyric_width = m_lyricBar->width();
    m_config.lyric_height = m_lyricBar->height();
    m_config.lyric_x = m_lyricBar->x();
    m_config.lyric_y = m_lyricBar->y();
    m_config.widget_width = this->width();
    m_config.widget_height = this->height();
    m_config.widget_x = this->x();
    m_config.widget_y = this->y();
    m_config.playType = m_playType;
}

void MainWindow::initLog()
{
    QString log_file = getRootPath() + "/" + LOG_FILE_NAME;
    m_log_file.setFileName(log_file);
	if (QFile(log_file).exists()) {
		QString lastTime = QFileInfo(log_file).lastModified().toString("dd");
		QString currentTime = QDateTime::currentDateTime().toString("dd");
		qDebug() << "lastTime:" << lastTime << " currentTime:" << currentTime;
		if (lastTime == currentTime) {
			m_log_file.open(QIODevice::Append | QIODevice::Text);
			return;
		}
	}

    m_log_file.open(QIODevice::WriteOnly | QIODevice::Text);
}

void MainWindow::addLog(const QString &log)
{
    if(m_log_file.isOpen()){

		QDateTime currentDateTime = QDateTime::currentDateTime();
		// 输出当前时间
		QString currentTime = currentDateTime.toString("hh:mm:ss");
        m_log_file.write(("[" + currentTime + "]:" + log + "\n").toLocal8Bit());
        m_log_file.flush();
    }
}

void MainWindow::quitLog()
{
    if(m_log_file.isOpen()){
        m_log_file.close();
    }
}

quint64 MainWindow::getMusicDuration(const QString &music_file)
{
    quint64 duration = 0;
    QMediaPlayer mediaPlayer;
    mediaPlayer.setMedia(QMediaContent(QUrl::fromLocalFile(music_file)));
    mediaPlayer.play();
    mediaPlayer.pause();
    duration = mediaPlayer.metaData(QMediaMetaData::Duration).toULongLong();
    int cnt=1000000;
    while(duration == 0 && cnt-- > 0){
        duration = mediaPlayer.metaData(QMediaMetaData::Duration).toULongLong();
        QCoreApplication::processEvents();
    }
    //time_qstr = QTime::fromMSecsSinceStartOfDay(time).toString("hh:mm:ss");
    //qDebug()<<"isAudioAvailable:"<<mediaPlayer.isAudioAvailable() <<" duration:"<<duration;


    QStringList keyList = mediaPlayer.availableMetaData();
    for(QString key:keyList){
        qDebug()<<"key:"<<key<<" value:"<<mediaPlayer.metaData(key);
        if(key == "Author"){
            qDebug()<<"key:"<<key<<" value:"<<getQString(mediaPlayer.metaData(key).toString().toLocal8Bit());
        }
    }

    return duration;
}

QString MainWindow::getMusicDurationQstrFormat(quint64 duration)
{
    return QTime::fromMSecsSinceStartOfDay(duration).toString("hh:mm:ss");
}

bool MainWindow::getMusicData(MusicData *musicData, const QString &file_path)
{
    quint64 duration = 0;
    QMediaPlayer mediaPlayer;
    mediaPlayer.setMedia(QMediaContent(QUrl::fromLocalFile(file_path)));
    mediaPlayer.play();
    mediaPlayer.pause();
    duration = mediaPlayer.metaData(QMediaMetaData::Duration).toULongLong();
    int cnt=1000000;//超时则该文件不是音频
    while(duration == 0 && cnt-- > 0){
        duration = mediaPlayer.metaData(QMediaMetaData::Duration).toULongLong();
        QCoreApplication::processEvents();
    }
    if(duration == 0){
        return false;
    }

    //time_qstr = QTime::fromMSecsSinceStartOfDay(time).toString("hh:mm:ss");
    //qDebug()<<"isAudioAvailable:"<<mediaPlayer.isAudioAvailable() <<" duration:"<<duration;
    //qDebug()<<"file_path" <<file_path;

    QStringList keyList = mediaPlayer.availableMetaData();
//    qDebug()<<"=====================";
//    for(QString key: keyList){
//        qDebug()<<"key:" << key << " value:"<<mediaPlayer.metaData(key);
//    }
//    qDebug()<<"=====================";
    musicData->File_path = file_path;
    musicData->Duration = duration;//时长
    musicData->Duration_format = getMusicDurationQstrFormat(duration);
    if(keyList.indexOf("Title") != -1){
        musicData->Title = mediaPlayer.metaData("Title").toString();//标题
    }
    if(musicData->Title.isEmpty() || musicData->Title.contains("?")){
        musicData->Title = QFileInfo(file_path).baseName();//默认填充
    }

    if(keyList.indexOf("Author") != -1){
        musicData->Author = mediaPlayer.metaData("Author").toString();//作者
    }
    if(musicData->Author.isEmpty() || musicData->Author.contains("?")){
        musicData->Author = musicData->Title;//默认填充
    }

    if(keyList.indexOf("ThumbnailImage") != -1){
        musicData->ThumbnailImage = mediaPlayer.metaData("ThumbnailImage").value<QImage>();//专辑图
    }else{
        musicData->ThumbnailImage = QImage(getRootPath() + "/resource/" + m_config.icon);//默认填充
    }


    return true;
}

void MainWindow::player()
{
    m_player->play();
    m_player->setVolume(m_myVolumeControlBar->value());
}

void MainWindow::autoPlayerNext()
{
    //qDebug()<<"m_playType:"<<m_playType;
    switch (m_playType) {
        case ShufflePlay://随机播放
            {
                int randomInt = getRandomIndex();
                m_player->playlist()->setCurrentIndex(randomInt);
                player();
            }
            break;
        case OrderPlay:
            break;
        case SingleLoop:
            m_player->playlist()->setCurrentIndex(m_player->playlist()->currentIndex());
            m_lyricTableWidget->reStart();
            break;
        case ListLoop:
        default:
            if(m_player->playlist()->currentIndex() == m_player->playlist()->mediaCount() - 1){
                m_player->playlist()->setCurrentIndex(0);
            }
            break;
    }
}

int MainWindow::getRandomIndex()
{
	return QRandomGenerator::global()->bounded(int(m_musicList->rowCount()));
}

QString MainWindow::getCurrentName()
{
    return QFileInfo(m_player->currentMedia().canonicalUrl().fileName()).baseName();
}

QString MainWindow::getCurrentMusicFile()
{
    return m_player->currentMedia().canonicalUrl().toLocalFile();
}

void MainWindow::add()
{
    QStringList files = QFileDialog::getOpenFileNames(nullptr, QStringLiteral("选择歌曲"),QStringLiteral("/"),QStringLiteral("*"));
	emit sigAddMusicFiles(files);
}

void MainWindow::scan()
{
    QString dir = QFileDialog::getExistingDirectory(nullptr, "选择扫描目录","/");
    if(dir.isEmpty()){
        return;
    }
    QDirIterator it(dir, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
	QStringList files;
    while (it.hasNext()) {
        it.next();
        if (it.fileInfo().isDir()) {
            //qDebug() << "Directory:" << it.filePath();
        } else {
            qDebug()<<"it.filePath():"<<it.filePath();
			files.append(it.filePath());
        }
    }
	emit sigAddMusicFiles(files);
}

void MainWindow::remove()
{
    QMediaPlaylist* playlist = m_player->playlist();
    QList<QTableWidgetItem*> listItem = m_musicList->selectedItems();
    std::map<int, QTableWidgetItem*> map_row_item;
    for(QTableWidgetItem* item:listItem){
        map_row_item[item->row()] = item;
    }
	int current_index = playlist->currentIndex();
    for(auto itor = map_row_item.begin();
        itor != map_row_item.end(); itor++){

		emit sigRemoveMusicFile(m_musicList->getRowMusicPath(itor->second->row()));
        playlist->removeMedia(itor->second->row());
        m_musicList->removeRow(itor->second->row());
    }
	if (map_row_item.find(current_index) != map_row_item.end()) {
		m_lyricTableWidget->clear();
		m_lyricBar->clear();
		m_controlBar->clear();
		playlist->next();
		m_player->play();
	}
}

void MainWindow::loadReCord()
{
    QString music_files_path = getRootPath() + RECORD_FILE;
    QFile file(music_files_path);
    if(!file.exists()){
        return;
    }
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::information(nullptr, "提示", music_files_path+"歌曲记录读取失败！");
        return;
    }
    QStringList files;
    QString qstr_current = file.readLine();
    qstr_current.replace("\n","");
	m_current_index = qstr_current.toInt();
    while(!file.atEnd()){
        QString line_qstr = file.readLine().replace("\n","");
        qDebug()<<"line_qstr:"<<line_qstr;
        files.append(line_qstr);
    }
    file.close();

    //player_list->setCurrentIndex(current_index);
    //m_player->setPosition(current_index);

    //线程加载，加载一个刷一个
	emit sigAddMusicFiles(files);
}

void MainWindow::saveReCord()
{
    QString music_files_path = getRootPath() + RECORD_FILE;
    int current_index = m_player->playlist()->currentIndex();
    int volume = m_player->volume();
    m_musicList->saveReCord(music_files_path, current_index, volume);
}

void MainWindow::loadConfig()
{
    QString config = getRootPath() + RESOURCE;
    QFile file(config);
    if(!file.open(QIODevice::ReadOnly)){
        //QMessageBox::warning(nullptr,"提示","配置文件" + config + "打开失败！");
        return;
    }

    QDomDocument doc;//doc
    QString error;
    int row = 0,column = 0 ;
    if(!doc.setContent(&file,false,&error,&row,&column)){
        //QMessageBox::warning(nullptr,"提示","读取xml格式失败！");
        return;
    }

    QDomElement hRoot = doc.documentElement();//获取根节点
    QDomNode node = hRoot.firstChild();
    while(!node.isNull()){
        QDomElement domElement= node.toElement();
        QString type = domElement.attribute("type");
        if(type == "software"){
            m_config.software_name = domElement.attribute("value");
			m_config.software_about = domElement.attribute("about");
            m_config.icon = domElement.attribute("icon");
            m_config.widget_x = domElement.attribute("x").toUInt();
            m_config.widget_y = domElement.attribute("y").toUInt();
            m_config.widget_width = domElement.attribute("width").toUInt();
            m_config.widget_height = domElement.attribute("height").toUInt();
        }else if(type == "styles"){
            m_config.style_index = domElement.attribute("value").toUInt();
            QDomNode node_child = domElement.firstChild();
            while(!node_child.isNull()){
                QString style = node_child.toElement().attribute("value");
                m_config.set_styles.insert(style);
                node_child = node_child.nextSibling();
            }
        }else if(type == "playType"){
            m_config.playType = (PlayType)domElement.attribute("value").toUInt();
            qDebug()<<"read  m_config.playType:"<< m_config.playType;
        }else if(type == "volume"){
            m_config.volume = domElement.attribute("value").toUInt();
        }else if(type == "lyric_show"){
            m_config.lyric_show = domElement.attribute("value") == "true";
            m_config.lyric_width = domElement.attribute("width").toUInt();
            m_config.lyric_height = domElement.attribute("height").toUInt();
            m_config.lyric_x = domElement.attribute("x").toUInt();
            m_config.lyric_y = domElement.attribute("y").toUInt();
        }else if (type == "lyric_analysis") {
			m_config.lyric_analysis_cmd = domElement.attribute("value");
			m_config.lyric_analysis_exe = domElement.attribute("exe");
			m_config.lyric_analysis_model = domElement.attribute("model");
		}
        node = node.nextSibling();
    }

    file.close();
}

void MainWindow::saveConfig()
{
    QString config = getRootPath() + RESOURCE;
    QFile file(config);
    if(!file.open(QIODevice::WriteOnly)){
        //QMessageBox::warning(nullptr,"提示","文件打开失败！");
        return;
    }
    QDomDocument doc;
    QDomElement domElement = doc.createElement("Node");//创建节点名
    doc.appendChild(domElement);//添加到根节点

    QDomElement domElementChild = doc.createElement("Node");//创建节点名
    domElementChild.setAttribute("type","software");
    domElementChild.setAttribute("value",m_config.software_name);
	domElementChild.setAttribute("about", m_config.software_about);
    domElementChild.setAttribute("icon",m_config.icon);
    domElementChild.setAttribute("x",QString::number(m_config.widget_x));
    domElementChild.setAttribute("y",QString::number(m_config.widget_y));
    domElementChild.setAttribute("width",QString::number(m_config.widget_width));
    domElementChild.setAttribute("height",QString::number(m_config.widget_height));
    domElement.appendChild(domElementChild);//添加到node节点

    domElementChild = doc.createElement("Node");
    domElementChild.setAttribute("type","styles");
    domElementChild.setAttribute("value",QString::number(m_config.style_index));
    for(auto itor = m_config.set_styles.begin();
        itor != m_config.set_styles.end(); itor++){
         QDomElement domElementChildChild = doc.createElement("Node");
         domElementChildChild.setAttribute("type","style");
         domElementChildChild.setAttribute("value",*itor);
         domElementChild.appendChild(domElementChildChild );
    }
    domElement.appendChild(domElementChild);

    domElementChild = doc.createElement("Node");
    domElementChild.setAttribute("type","volume");
    domElementChild.setAttribute("value",m_config.volume);
    domElement.appendChild(domElementChild);

    domElementChild = doc.createElement("Node");
    domElementChild.setAttribute("type","playType");
    domElementChild.setAttribute("value",QString::number(m_config.playType));
    domElement.appendChild(domElementChild);

    domElementChild = doc.createElement("Node");
    domElementChild.setAttribute("type","lyric_show");
    domElementChild.setAttribute("value",m_config.lyric_show ? "true":"false");
    domElementChild.setAttribute("width",QString::number(m_config.lyric_width));
    domElementChild.setAttribute("height",QString::number(m_config.lyric_height));
    domElementChild.setAttribute("x",QString::number(m_config.lyric_x));
    domElementChild.setAttribute("y",QString::number(m_config.lyric_y));
    domElement.appendChild(domElementChild);

	domElementChild = doc.createElement("Node");
	domElementChild.setAttribute("type", "lyric_analysis");
	domElementChild.setAttribute("value", m_config.lyric_analysis_cmd);
	domElementChild.setAttribute("exe", m_config.lyric_analysis_exe);
	domElementChild.setAttribute("model", m_config.lyric_analysis_model);
	domElement.appendChild(domElementChild);

    QTextStream stream(&file);
    doc.save(stream, 4);//保存到文件
    file.close();
}

int MainWindow::getIndex(qint64 duration)
{
    for(LineLyricData data: m_vec_lyric){
        if(duration >= data.start_time && duration < data.end_time){
            //qDebug()<<"duration:"<<duration << " data.start_time:"<<data.start_time;
            return data.index;
        }
    }
    return -1;
}

void MainWindow::updateTableSize()
{
    m_musicList->setGeometry(this->x() + width() - TABLE_WIDTH, this->y() + TOP_BORDER, TABLE_WIDTH- 5,
                               height()  - TOP_BORDER- BOTTOM_BORDER - m_controlBar->height());

}

void MainWindow::setStyle(QString style)
{
    if(!QFile(style).exists()){
        style = getRootPath() + style;
    }
    QFile file(style);
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet); // 或者使用button.setStyleSheet(styleSheet); 只应用于按钮
}

QString MainWindow::getCurrentStyle()
{
     QString style_qss;
    unsigned int index = 0;
    for(QString qstr:m_config.set_styles){
        if(index == m_config.style_index){
            style_qss = qstr;
            break;
        }
        index++;
    }
    return style_qss;
}

QString MainWindow::getRootPath()
{
    return QCoreApplication::applicationDirPath();
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    m_menu_mainwindow->exec(event->globalPos());
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    m_musicList->hide();
	setMoveEnableRange(0, 0, width(), 70);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //qDebug()<<"closeEvent";
    if(m_player->state() == QMediaPlayer::PlayingState){
        //最小化
        showMinimized();
        hide();
        event->ignore();
    }else{
        event->accept();
    }
}

void MainWindow::focusOutEvent(QFocusEvent *event)
{
    m_musicList->hide();
    return MyWidget::focusOutEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    m_musicList->hide();
    return MyWidget::mousePressEvent(event);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    onClickedMaxOrNormal();
    return MyWidget::mouseDoubleClickEvent(event);
}

void MainWindow::onSetStyle()
{
    m_config.style_index++;
    if(m_config.style_index >= m_config.set_styles.size()){
        m_config.style_index = 0;
    }
    // 加载QSS文件
    QString style_qss = getCurrentStyle();
    setStyle(style_qss);
}

void MainWindow::onClickedMaxOrNormal()
{
    if(this->isMaximized()){
        this->showNormal();
        m_headerBar->setMaxOrNormalTooltip(QString::fromLocal8Bit("最大化"));
    }else{
        this->showMaximized();
        m_headerBar->setMaxOrNormalTooltip(QString::fromLocal8Bit("还原"));
    }
}

void MainWindow::onClickedGoMain()
{
    showNormal();
    setFocus();
}

void MainWindow::onClickedPre()
{
    //qDebug()<<"OnClickedPre";
    QMediaPlaylist* playlist = m_player->playlist();
    playlist->previous();
    m_player->play();
}
void MainWindow::onClickedNext()
{
    //qDebug()<<"PlayerNext";
    QMediaPlaylist* playlist = m_player->playlist();
    playlist->next();
    m_player->play();
}
void MainWindow::onClickedSet()
{

}
void MainWindow::onClickedPlayerOrPause()
{
    if(m_player->state() == QMediaPlayer::PlayingState){
        m_player->pause();
    }else{
        m_player->play();
    }
}
void MainWindow::onClickedMute()
{
    bool b_Muted = m_player->isMuted();
    m_player->setMuted(!b_Muted);
    m_controlBar->setBMute(!b_Muted);
    m_myVolumeControlBar->setBMute(!b_Muted);
}

void MainWindow::onClickedTable()
{
    m_musicList->setBShow(!m_musicList->isVisible());
    m_musicList->setVisible(!m_musicList->isVisible());
    m_musicList->setFocus();

    QRect rect_start = QRect(this->x() + width(),
                             this->y() + TOP_BORDER,
                             0,
                             height()  - TOP_BORDER- BOTTOM_BORDER - m_controlBar->height());

    QRect rect_end = QRect(this->x() + width() - TABLE_WIDTH,
                           this->y() + TOP_BORDER,
                           TABLE_WIDTH- 5,
                           height()  - TOP_BORDER- BOTTOM_BORDER - m_controlBar->height());

    m_musicList->setGeometry(rect_end);
//    QPropertyAnimation animation(m_musicList, "geometry");
//    animation.setEasingCurve(QEasingCurve::OutQuad);
//    animation.setDuration(100);
//    animation.setStartValue(rect_start);
//    animation.setEndValue(rect_end);  // 向左滑出
//    animation.start();

//    QEventLoop loop;
//    connect(&animation, &QPropertyAnimation::finished, &loop, &QEventLoop::quit);
//    loop.exec();
}

void MainWindow::onClickedLyric()
{
    m_lyricBar->setVisible(!m_lyricBar->isVisible());
    m_config.lyric_show = m_lyricBar->isVisible();
}

void MainWindow::onSetCurrentPlayType(PlayType playType)
{
   m_playType = playType;
   //switch (m_playType) {
   //    case ShufflePlay://随机播放
   //        break;
   //    case OrderPlay:
   //        break;
   //    case SingleLoop:
   //        break;
   //    case ListLoop:
   //    default:
   //        break;
   //}
}

void MainWindow::onEnterMute()
{
    //qDebug()<<"OnEnterMute:"<<m_controlBar->getBtnMutePoint() <<" "<<mapToGlobal(m_controlBar->getBtnMutePoint());
    m_myVolumeControlBar->setPosition(m_controlBar->getBtnMutePoint(), m_controlBar->getBtnVolumeWidth());
    //延时隐藏1000ms
    QTime dieTime = QTime::currentTime().addMSecs(1000);
    while( QTime::currentTime() < dieTime ){
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    if(!m_myVolumeControlBar->getBEnter()){
        m_myVolumeControlBar->hide();
    }
}

void MainWindow::onMenu(QAction *act)
{
    switch (act->data().toInt()) {
    case ACTION_ADD:
        add();
        break;
    case ACTION_SCAN:
        scan();
        break;
    case ACTION_REMOVE:
        remove();
        break;
    case ACTION_SHOW:
        showNormal();
        break;
    case ACTION_SHOW_RESET:
        showReset();
        break;
    case ACTION_QUIT:
        m_player->stop();
        close();
        break;
    default:
        break;
    }
}

void MainWindow::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    Q_UNUSED(reason);
    onClickedGoMain();
}

void MainWindow::onDurationChanged(qint64 duration)
{
    //qDebug()<<"duration:"<<duration;
    m_player->setVolume(m_myVolumeControlBar->value());
    m_controlBar->setRange(0, duration);
    m_duration = duration;
}

void MainWindow::onPositionChanged(qint64 duration)
{
    if(m_slider_press){
        return;
    }
    m_controlBar->setValue(duration);

    //qDebug()<<duration<<"/"<<m_duration;
    if(0 != duration && duration == m_duration){
		qDebug()<<"m_duration:"<< m_duration;
		m_lyricTableWidget->setCurrentDuration(0);//方式单曲循环，故放完重置
        autoPlayerNext();
    }
}

void MainWindow::onSliderMoved()
{
    //qDebug()<<"OnSliderMoved";
    //m_slider_press = true;
}

void MainWindow::onSliderPressed()
{
    //qDebug()<<"OnSliderPressed";
    m_slider_press = true;
}

void MainWindow::onSliderReleased()
{
    //qDebug()<<"OnSliderReleased";
    m_slider_press = false;
    addLog("m_player->duration():" +QString::number(m_player->duration()));
    if(m_player->duration() == 0){
        return;
    }
    m_player->setPosition(m_controlBar->getValue());
    m_lyricTableWidget->setCurrentDuration(m_controlBar->getValue());
}

void MainWindow::onSliderValueChanged(int value)
{
    //qDebug()<<"OnSliderValueChanged:"<<value;
}

void MainWindow::onVolumeValueChanged(int value)
{
    //qDebug()<<"onVolumeValueChanged:"<<value;
    m_player->setVolume(value);
    m_controlBar->setBMute(value == 0);
}

void MainWindow::onCurrentMediaChanged(const QMediaContent &media)
{
    addLog("onCurrentMediaChanged m_player->playlist()->currentIndex():" + QString::number(m_player->playlist()->currentIndex()));
    if(m_player->playlist()->currentIndex() < 0){
        return;
    }
    QString music_file = media.canonicalUrl().toString();
    QFileInfo info(music_file);
    QString title, author;
	QImage image;
    m_musicList->selectRow(m_player->playlist()->currentIndex());
    m_musicList->getMusicInfo(m_player->playlist()->currentIndex(), title, author, image);
	m_lyricBar->clear();
	m_lyricTableWidget->clear();
	m_lyricTableWidget->setTitle(title, author);
	m_controlBar->setCurrentTitle(title);
	m_controlBar->setImage(image);
    //加载歌词
	emit sigAnalysisMusicLyric(music_file);
}

void MainWindow::onAddMusicData(MusicData musicData)
{
	QMediaPlaylist* playlist = m_player->playlist();
	m_musicList->addItem(musicData);
	playlist->addMedia(QMediaContent(QUrl(musicData.File_path)));

	if (m_current_index + 1 == playlist->mediaCount()) {
		addLog("默认播放第一个");
		m_player->playlist()->setCurrentIndex(m_current_index);
		m_player->play();//默认播放第一个
		m_player->pause();
	}
}

void MainWindow::onMusicLyric(const QString& music_file, const std::vector<LineLyricData>& vec_lyric)
{
	QString current_music_file = m_player->currentMedia().canonicalUrl().toString();
	if (music_file == current_music_file) {
		m_vec_lyric = vec_lyric;
		m_lyricTableWidget->setLyricData(vec_lyric);
	}
}

void MainWindow::onItemDoubleClicked(QTableWidgetItem *item)
{
    m_player->playlist()->setCurrentIndex(item->row());
    player();
}

void MainWindow::onLyricDoubleClicked(QTableWidgetItem *item)
{
    qint64 start_time = item->data(Qt::UserRole).toLongLong();
    m_player->setPosition(start_time*1000);
}
