#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "HeaderBar.h"
#include "MyWidget.h"
#include "LyricTableWidget.h"
#include "MusicList.h"
#include "DataStruct.h"
#include "ControlBar.h"
#include "LyricBar.h"
#include "MusicList.h"
#include "VolumeControlBar.h"


#include <QMainWindow>
#include <QMediaPlayer>
#include <QProgressBar>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QSlider>
#include <QTableWidget>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QLineEdit>
#include <QMenu>
#include <QAction>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QShowEvent>
#include <QHideEvent>
#include <QEvent>
#include <QFocusEvent>
#include <QCloseEvent>
#include <QLabel>
#include <QSystemTrayIcon>
#include <QFile>
#include <QMediaPlaylist>
#include <QThread>
#include <QDomDocument>
#include <QDebug>

//主界面
class MainWindow : public MyWidget
{
    Q_OBJECT

public:
    MainWindow(Config config, QWidget *parent = nullptr);
    ~MainWindow();

    void init();
	void addLog(const QString &log);
	static void initConfig(Config& config);//首次默认配置
	static void loadConfig(Config& config);//配置
private:
    void showReset();
    void setConfig();

    void initLog();
    void quitLog();

    quint64 getMusicDuration(const QString & music_file);
    QString getMusicDurationQstrFormat(quint64 duration);
    bool getMusicData(MusicData *musicData, const QString &music_file);

    void player();
    void autoPlayerNext();
    int getRandomIndex();

    QString getCurrentName();
    QString getCurrentMusicFile();

    void add();//添加
    void scan();//扫描歌曲
    void remove();//删除

    void loadReCord();//加载记录
    void saveReCord();//保存记录
	static void saveConfig(const Config& config);

    int getIndex(qint64 duration);

    void updateTableSize();
    void setLanguage(QString language);
	void setStyle(QString style);
    QString getCurrentStyle();
	QString getCurrentLanguage();
    static QString getRootPath();
protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void resizeEvent(QResizeEvent* event);
    void closeEvent(QCloseEvent* event);
    void focusOutEvent(QFocusEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
signals:
	void sigAddMusicFiles(const QStringList &music_file);
	void sigRemoveMusicFile(const QString &music_file);

	void sigAnalysisMusicLyric(const QString &music_file);
private slots:
    void onSetStyle();

    void onClickedMaxOrNormal();
	void onClickedLanguage();

    void onClickedGoMain();
    void onClickedSet();
    void onClickedPre();
    void onClickedPlayerOrPause();
    void onClickedNext();
    void onClickedMute();
    void onClickedTable();
    void onClickedLyric();
    void onSetCurrentPlayType(PlayType);
    void onEnterMute();
	void onLeaveMute();

    void onMenu(QAction* act);
    void onActivated(QSystemTrayIcon::ActivationReason);

    void onDurationChanged(qint64 duration);
    void onPositionChanged(qint64 duration);

    void onSliderMoved();
    void onSliderPressed();
    void onSliderReleased();
    void onSliderValueChanged(int value);

    void onVolumeValueChanged(int value);

    void onCurrentMediaChanged(const QMediaContent &media);

	void onAddMusicData(MusicData musicData);//加载音乐列表
	void onMusicLyric(const QString&, const std::vector<LineLyricData>&);//加载歌词

    void onItemDoubleClicked(QTableWidgetItem* item);
    void onLyricDoubleClicked(QTableWidgetItem* item);
private:
	QTranslator *m_translator;
	QThread* m_musicListManager_thread;
	MusicListManager* m_musicListManager;
	QThread* m_musicLyricManager_thread;
	MusicLyricManager* m_musicLyricManager;
    LyricTableWidget* m_lyricTableWidget;
    ControlBar* m_controlBar;
    MyVolumeControlBar* m_myVolumeControlBar;
    LyricBar* m_lyricBar;
    QMediaPlayer* m_player;
    MusicList* m_musicList;
    HeaderBar* m_headerBar;
    PlayType m_playType;
    QSystemTrayIcon* m_systemTrayIcon;//托盘图标
    std::vector<LineLyricData> m_vec_lyric;
    std::vector<int> m_vec_playList;
    QMenu* m_menu_mainwindow;
    QMenu* m_menu_music_list;
    QMenu* m_menu_trayIcon;
    Config m_config;
    QFile m_log_file;
    int m_duration;
	int m_current_index;
    bool m_slider_press;
};
#endif // MAINWINDOW_H
