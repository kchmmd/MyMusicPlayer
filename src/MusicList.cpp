/*******************************************
*MusicList.cpp
*歌曲列表
********************************************/
#pragma execution_character_set("utf-8")

#include "MusicList.h"
#include "SearchBar.h"
#include "Common.h"

#include <QHeaderView>
#include <QCoreApplication>
#include <QScrollBar>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QTime>
#include <QFileInfo>
#include <QMetaObject>

Q_DECLARE_METATYPE(MusicData);
Q_DECLARE_METATYPE(LineLyricData);
Q_DECLARE_METATYPE(std::vector<LineLyricData>);
MusicListManager::MusicListManager()
{
	qRegisterMetaType<MusicData>();
}
void MusicListManager::removeMusicFile(const QString & music_file)
{
	m_music_files.removeOne(music_file);
}
void MusicListManager::anlaysisMusicFiles(const QStringList &music_files)
{
	for (const QString &music_file : music_files) {
		// 将URL转换为本地文件路径
		if (-1 != m_music_files.indexOf(music_file)) {
			continue;//去重
		}
		qDebug() << "anlaysisMusicFilePaths file_path:" << music_file;
		analysisMusicFile(music_file);
	}
}
void MusicListManager::anlaysisMusicUrls(const QList<QUrl> &urls)
{
	for(const QUrl &url :  urls) {
		// 将URL转换为本地文件路径
		QString music_file = url.toLocalFile();
		if (-1 != m_music_files.indexOf(music_file)) {
			continue;//去重
		}
		qDebug() << "anlaysisMusicUrls music_file:" << music_file;
		analysisMusicFile(music_file);
	}
}
void MusicListManager::analysisMusicFile(const QString &music_file)
{
	MusicData musicData;
	bool re = getMusicData(&musicData, music_file);
	if (!re) {
		addLog(music_file + " getMusicData error.");
		return;
	}
	qDebug() << "File_path:" << musicData.File_path
		<< "Title:" << musicData.Title.toLocal8Bit()
		<< " Author:" << musicData.Author
		<< "musicData.File_path:" << musicData.File_path;

	m_music_files.append(music_file);
	emit sigAddMusicData(musicData);

}
bool MusicListManager::getMusicData(MusicData * musicData, const QString & music_file)
{
	quint64 duration = 0;
	QMediaPlayer mediaPlayer;
	mediaPlayer.setMedia(QMediaContent(QUrl::fromLocalFile(music_file)));
	mediaPlayer.play();
	mediaPlayer.pause();
	duration = mediaPlayer.metaData(QMediaMetaData::Duration).toULongLong();
	int cnt = 1000000;//超时则该文件不是音频
	while (duration == 0 && cnt-- > 0) {
		duration = mediaPlayer.metaData(QMediaMetaData::Duration).toULongLong();
		QCoreApplication::processEvents();
	}
	if (duration == 0) {
		return false;
	}

	QStringList keyList = mediaPlayer.availableMetaData();
	musicData->File_path = music_file;
	musicData->Duration = duration;//时长
	musicData->Duration_format = QTime::fromMSecsSinceStartOfDay(duration).toString("hh:mm:ss");
	if (keyList.indexOf("Title") != -1) {
		musicData->Title = mediaPlayer.metaData("Title").toString();//标题
	}
	if (musicData->Title.isEmpty() || musicData->Title.contains("?")) {
		musicData->Title = QFileInfo(music_file).baseName();//默认填充
	}

	if (keyList.indexOf("Author") != -1) {
		musicData->Author = mediaPlayer.metaData("Author").toString();//作者
	}
	if (musicData->Author.isEmpty() || musicData->Author.contains("?")) {
		musicData->Author = musicData->Title;//默认填充
	}

	if (keyList.indexOf("ThumbnailImage") != -1) {
		musicData->ThumbnailImage = mediaPlayer.metaData("ThumbnailImage").value<QImage>();//专辑图
	}
	else {
		musicData->ThumbnailImage = QImage(QCoreApplication::applicationDirPath() + "/resource/" + MUSIC_ICON);//默认填充
	}

	return true;
}
void MusicListManager::addLog(const QString &log)
{
	emit sigAddLog(log);
}

MusicLyricManager::MusicLyricManager()
{
	qRegisterMetaType<LineLyricData>();
	qRegisterMetaType<std::vector<LineLyricData>>();
	qRegisterMetaType<QProcess::ExitStatus>();
}

MusicLyricManager::~MusicLyricManager()
{
	for (auto itor = m_map_process_file.begin();
		itor != m_map_process_file.end(); itor++) {
		itor->first->deleteLater();
	}
}
void MusicLyricManager::init(const QString & lyric_analysis_cmd, const QString & lyric_analysis_exe, const QString & lyric_analysis_model)
{
	m_lyric_analysis_cmd = lyric_analysis_cmd;
	m_lyric_analysis_exe = lyric_analysis_exe;
	m_lyric_analysis_model = lyric_analysis_model;
	QString root_path = QCoreApplication::applicationDirPath();
	if (!QFile(m_lyric_analysis_exe).exists()) {
		m_lyric_analysis_exe = root_path + m_lyric_analysis_exe;
	}
	if (!QFile(m_lyric_analysis_model).exists()) {
		m_lyric_analysis_model = root_path + m_lyric_analysis_model;
	}
}
void MusicLyricManager::analysisMusicLyric(const QString &music_file)
{ 
	QFileInfo info(music_file);
	m_music_lyric = info.absolutePath() + "/" + info.baseName() + LYRIC_SUFFIX;

	std::vector<LineLyricData> vec_lyric;
	int i_re = getMusicLyricData(music_file, vec_lyric);
	if (vec_lyric.size() != 0) {
		emit sigMusicLyric(music_file, vec_lyric);
		return;
	}
	i_re = getFileLyricData(m_music_lyric, vec_lyric);
	if (vec_lyric.size() != 0) {
		emit sigMusicLyric(music_file, vec_lyric);
		return;
	}

	analysisMusicLyricData(music_file);
	return;
}
int MusicLyricManager::getMusicLyricData(const QString &music_file, std::vector<LineLyricData>& vec_lyric)
{
	quint64 duration = 0;
	QMediaPlayer mediaPlayer;
	mediaPlayer.setMedia(QMediaContent(QUrl::fromLocalFile(music_file)));
	mediaPlayer.play();
	mediaPlayer.pause();
	duration = mediaPlayer.metaData(QMediaMetaData::Duration).toULongLong();
	int cnt = 1000000;//超时则该文件不是音频
	while (duration == 0 && cnt-- > 0) {
		duration = mediaPlayer.metaData(QMediaMetaData::Duration).toULongLong();
		QCoreApplication::processEvents();
	}
	if (duration == 0) {
		return -1;
	}

	bool isEnglish = isEnglishMusic(music_file);
	QString lyrics = mediaPlayer.metaData("Lyrics").toString();
	if (lyrics.isEmpty()) {
		return -1;
	}
	QStringList qstrList = lyrics.split("[00:00.00]");
	if (qstrList.size() != 2) {
		addLog(music_file + ":" + lyrics);
		return -1;
	}
	QStringList lyric_list = qstrList[1].split("[");
	if (qstrList.size() != 2) {
		addLog(music_file + ":" + lyrics);
		return -1;
	}
	qint64 start_time = 0, end_time;
	int index = 0;
	//特殊处理开始和结尾
	QString qstr_lyric = lyric_list[0];
	qstr_lyric.replace("\n", "");
	for (int i = 1; i < lyric_list.length(); i++) {

		QString lyric = lyric_list[i];
		lyric.replace("\n", "");
		QStringList lyric_list_tmp = lyric.split("]");
		QString qstr_time = lyric_list_tmp[0];
		end_time = getQstrTime(qstr_time);
		if (isEnglish) {
			updateLineLyric(qstr_lyric);
		}
		//qDebug()<<"qstr_lyric:"<<qstr_lyric;
		LineLyricData data = { index, start_time, end_time, qstr_lyric };
		if (!qstr_lyric.isEmpty() || !qstr_lyric.replace(" ", "").isEmpty()) {
			vec_lyric.push_back(data);
			index++;
		}
		qstr_lyric = lyric_list_tmp[1].trimmed();
		start_time = end_time;
	}

	return 0;
}
int MusicLyricManager::getFileLyricData(const QString &lyric_file, std::vector<LineLyricData>& vec_lyric)
{
	QFile file(lyric_file);
	if (!file.exists()) {
		addLog(lyric_file + "不存在." );
		return -1;
	}
	if (!file.open(QIODevice::ReadOnly)) {
		addLog(lyric_file + "歌词打开失败.");
		return -1;
	}
	int index = 0;
	while (!file.atEnd()) {
		QString qstr_line = file.readLine();

		qstr_line.replace("\n", "");
		qstr_line.replace("[", "");

		QStringList qstrList = qstr_line.split("]");
		if (qstrList.length() != 2) {
			file.close();
			addLog(lyric_file + "歌词格式解析错误.");
			return -1;
		}

		QString qstr_time = qstrList[0].replace(" ", "");
		QString qstr_lyric = qstrList[1].trimmed();

		QStringList qstrList_time = qstr_time.split("-->");
		if (qstrList.length() != 2) {
			file.close();
			addLog(lyric_file + "歌词格式解析错误.");
			return -1;
		}
		qint64 start_time = getQstrTime(qstrList_time[0].split(".").at(0));
		qint64 end_time = getQstrTime(qstrList_time[1].split(".").at(0));
		LineLyricData data = { index, start_time, end_time, qstr_lyric };
		vec_lyric.push_back(data);
		index++;
	}
	file.close();

	return 0;
}
void MusicLyricManager::analysisMusicLyricData(const QString & music_file)
{
	if (!QFile(m_lyric_analysis_exe).exists()) {
		addLog(m_lyric_analysis_exe + "不存在,无法解析歌词.");
		return;
	}
	QString root_path = QCoreApplication::applicationDirPath();
	QString cmd = m_lyric_analysis_cmd;
	cmd = cmd.replace("analysis_exe", m_lyric_analysis_exe);
	cmd = cmd.replace("analysis_model", m_lyric_analysis_model);
	cmd = cmd.replace("analysis_music_file", music_file);
	cmd = cmd.replace("analysis_lyric_file", m_music_lyric);

	QProcess* process = new QProcess;
	connect(process, &QProcess::readAllStandardOutput, this, &MusicLyricManager::onReadAllStandardOutput);
	connect(process, &QProcess::readAllStandardError, this, &MusicLyricManager::onReadAllStandardError);
	connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &MusicLyricManager::onFinished);

	process->start(cmd);
	m_map_process_file[process] = music_file;
	addLog(cmd);

	return;
}
void MusicLyricManager::onFinished(int exit_code, QProcess::ExitStatus)
{
	addLog("onFinished exit_code:" + QString::number(exit_code));
	QProcess* process = static_cast<QProcess*>(sender());
	QString music_file = m_map_process_file[process];
	if (!QFile(m_music_lyric).exists()) {
		addLog(m_music_lyric + "不存在.");
		return;
	}
	std::vector<LineLyricData> vec_lyric;
	int i_re = getFileLyricData(m_music_lyric, vec_lyric);
	if (vec_lyric.size() != 0) {
		emit sigMusicLyric(music_file, vec_lyric);
		return;
	}
}
void MusicLyricManager::onReadAllStandardOutput()
{
	QProcess* process = static_cast<QProcess*>(sender());
	addLog(process->readAllStandardOutput());
}
void MusicLyricManager::onReadAllStandardError()
{
	QProcess* process = static_cast<QProcess*>(sender());
	addLog(process->readAllStandardError());
}
void MusicLyricManager::updateLineLyric(QString &lyric)
{
	int index = -1;
	index = getChineseIndex(lyric);
	if (index != -1) {
		lyric.insert(index, '\n');
	}
}
void MusicLyricManager::addLog(const QString & log)
{
	emit sigAddLog(log);
}

MusicList::MusicList(QWidget *parent)
    :MyWidget(parent)
{
    setObjectName("MusicList");
    setMouseTracking(true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setMoveEnable(false);
    setChangeSizeEnable(false);

    QVBoxLayout* vbox_main = new QVBoxLayout;
    SearchBar* searchBar = new SearchBar;
    m_table_music = new MyTableWidget;
    vbox_main->addWidget(searchBar);
    vbox_main->addWidget(m_table_music);
    this->setLayout(vbox_main);

    m_table_music->setObjectName("table_music_list");

    QStringList qstr_list;
    qstr_list << QString::fromLocal8Bit("歌名");//<< "歌手" <<"时长";
    m_table_music->setColumnCount(qstr_list.length());
    m_table_music->setHorizontalHeaderLabels(qstr_list);
    m_table_music->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table_music->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table_music->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table_music->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    m_table_music->setFrameStyle(QFrame::NoFrame);
    m_table_music->horizontalHeader()->setVisible(false);
    m_table_music->verticalHeader()->setVisible(false);
    m_table_music->verticalScrollBar()->setObjectName("table_music_scrollBar");
    m_table_music->setAttribute(Qt::WA_TranslucentBackground);

    connect(m_table_music, &MyTableWidget::sigAddUrls, this, &MusicList::sigAddUrls);
    connect(m_table_music, &MyTableWidget::itemDoubleClicked, this, &MusicList::itemDoubleClicked);
    connect(searchBar, &SearchBar::sigSearch, this, &MusicList::onSearch);
}

MusicList::~MusicList()
{
}

int MusicList::rowCount()
{
    return m_table_music->rowCount();
}

void MusicList::removeRow(int row)
{
    m_table_music->removeRow(row);
}

QList<QTableWidgetItem *> MusicList::selectedItems()
{
    return m_table_music->selectedItems();
}

void MusicList::selectRow(int row)
{
    m_table_music->selectRow(row);
}

void MusicList::addItem(const MusicData& musicData)
{
    m_table_music->setRowCount(m_table_music->rowCount() + 1);
    QTableWidgetItem* item_name = new QTableWidgetItem(musicData.Title);
    QString tip = QString::fromLocal8Bit("歌名: %1\n歌手: %2\n时长: %3")
            .arg(musicData.Title)
            .arg(musicData.Author)
            .arg(musicData.Duration_format);
    item_name->setToolTip(tip);
    item_name->setIcon(QPixmap::fromImage(musicData.ThumbnailImage));
    m_table_music->setItem(m_table_music->rowCount() - 1, 0, item_name);
    item_name->setData(Qt::UserRole, musicData.File_path);
    item_name->setData(Qt::UserRole + 1, musicData.Duration);
    item_name->setData(Qt::UserRole + 2, musicData.Title);
    item_name->setData(Qt::UserRole + 3, musicData.Author);
	item_name->setData(Qt::UserRole + 4, musicData.ThumbnailImage);
}

void MusicList::getMusicInfo(int row, QString &title, QString &author, QImage& image)
{
    title = m_table_music->item(row, 0)->data(Qt::UserRole + 2).toString();
    author = m_table_music->item(row, 0)->data(Qt::UserRole + 3).toString();
	image = m_table_music->item(row, 0)->data(Qt::UserRole + 4).value<QImage>();
}

QString MusicList::getRowMusicPath(int row)
{
	return m_table_music->item(row, 0)->data(Qt::UserRole).toString();;
}

void MusicList::onSearch(const QString &text)
{
    int rowCount = m_table_music->rowCount();
    if(text.isEmpty()){
        for(int i=0; i<rowCount; i++){
            m_table_music->setRowHidden(i, false);
        }
    }else{
        //找到符合条件的索引 是通过你输入的和表格里面所有内容进行比对
        QList <QTableWidgetItem *> list_item = m_table_music->findItems(text, Qt::MatchContains);
        for(int i=0; i<rowCount; i++){
            m_table_music->setRowHidden(i, true);//先隐藏所有
        }

        if(!list_item.isEmpty()){
            for(QTableWidgetItem* item: list_item){
                m_table_music->setRowHidden(item->row(), false);//符合条件的显示
            }
        }
    }
}

void MusicList::saveReCord(QString music_files_path, int current_index, int volume)
{
    //QString music_files_path = QCoreApplication::applicationDirPath() + RECORD_FILE;
    QFile file(music_files_path);
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::information(nullptr, "提示", music_files_path+"歌曲记录保存失败！");
        return;
    }
    if(m_table_music->rowCount() == 0){
        file.close();
        file.remove();
        return;
    }
    //int current_index = m_player->playlist()->currentIndex();
    //int volume = m_player->volume();
	file.write(QString("%1\n").arg(current_index).toLocal8Bit());
    for(int i=0; i<m_table_music->rowCount(); i++){
        QString path = m_table_music->item(i, 0)->data(Qt::UserRole).toString();
        QString qstr = QString("%1\n").arg(path);
        //qDebug()<<"path:"<< path;
		file.write(qstr.toLocal8Bit());
    }

    file.close();
}
