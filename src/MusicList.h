#ifndef MUSICLIST_H
#define MUSICLIST_H

#include <QWidget>
#include <QTableWidget>
#include <QDropEvent>
#include <QMimeData>
#include <QMenu>
#include <QLayout>
#include <QTabWidget>
#include <QMessageBox>
#include <QProcess>
#include <QDebug>
#include "MyWidget.h"
#include "DataStruct.h"

class MusicListManager : public QObject 
{
	Q_OBJECT
public:
	MusicListManager();
	void removeMusicFile(const QString &music_file);
	void anlaysisMusicFiles(const QStringList &music_files);
	void anlaysisMusicUrls(const QList<QUrl> &urls);
signals:
	void sigAddLog(QString);
	void sigAddMusicData(MusicData musicData);
private:
	void analysisMusicFile(const QString &music_file);
	bool getMusicData(MusicData *musicData, const QString &music_file);
	void addLog(const QString &log);
private:
	QStringList m_music_files;
};

class MusicLyricManager : public QObject
{
	Q_OBJECT
public:
	MusicLyricManager();
	~MusicLyricManager();
	void init(const QString &lyric_analysis_cmd,
		const QString &lyric_analysis_exe,
		const QString &lyric_analysis_model);
	void analysisMusicLyric(const QString &music_file);
signals:
	void sigAddLog(const QString&);
	void sigMusicLyric(const QString&, const std::vector<LineLyricData>&);
protected slots:
	void onFinished(int, QProcess::ExitStatus);
	void onReadAllStandardOutput();
	void onReadAllStandardError();
private:
	int getMusicLyricData(const QString &music_file, std::vector<LineLyricData>& vec_lyric);
	int getFileLyricData(const QString &lyric_file, std::vector<LineLyricData>& vec_lyric);
	void analysisMusicLyricData(const QString &music_file);
	void updateLineLyric(QString &lyric);
	void addLog(const QString &log);
private:
	QString m_music_lyric;
	std::map<QProcess*, QString> m_map_process_file;
	QString m_lyric_analysis_cmd;
	QString m_lyric_analysis_exe;
	QString m_lyric_analysis_model;
};

//歌曲列表
class MyTableWidget: public QTableWidget
{
    Q_OBJECT
public:
    MyTableWidget(QWidget* parent=nullptr):QTableWidget(parent){
        setObjectName("MyTableWidget");
		setAcceptDrops(true);
        m_menu =nullptr;
    }
    void setMenu(QMenu* menu){
        m_menu = menu;
    }
protected:
    void dragEnterEvent(QDragEnterEvent *event) override {
      if (event->mimeData()->hasUrls()) {
          event->acceptProposedAction(); // 接受拖拽动作
      }
    }

    void dragMoveEvent(QDragMoveEvent *event) override {
      if (event->mimeData()->hasUrls()) {
          event->acceptProposedAction(); // 接受拖拽动作
      }
    }
    void dropEvent(QDropEvent *event) override {
        if (event->mimeData()->hasUrls()) {
            event->setDropAction(Qt::CopyAction);
            event->accept();
            emit sigAddUrls(event->mimeData()->urls());
        }
    }
    void contextMenuEvent(QContextMenuEvent *event){
        Q_UNUSED(event);
        if(m_menu){
            m_menu->exec(event->globalPos());
        }
    }
signals:
    void sigAddUrls(QList<QUrl>);
private:
    bool m_b_show;
    QMenu* m_menu;
};

class MusicList: public MyWidget
{
    Q_OBJECT
public:
    MusicList(QWidget* parent=nullptr);
    ~MusicList();

    void setMenu(QMenu* menu){m_table_music->setMenu(menu);}
    int rowCount();
    void removeRow(int row);
    QList<QTableWidgetItem*> selectedItems();
    void saveReCord(QString file, int current_index, int volume);
    void setBShow(bool b_show){m_b_show=b_show;}
    bool getBShow(){return m_b_show;}
    void selectRow(int row);
    void addItem(const MusicData& musicData);
    void getMusicInfo(int row, QString &title, QString &author, QImage& image);
	QString getRowMusicPath(int row);
signals:
    void sigAddUrls(QList<QUrl>);
    void itemDoubleClicked(QTableWidgetItem *item);
protected slots:
    void onSearch(const QString &);
private:
    MyTableWidget *m_table_music;
    bool m_b_show;
};

#endif // MUSICLIST_H
