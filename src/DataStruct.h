#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <QString>
#include <map>
#include <set>
#include <QImage>

#define VOLUME_INIT 50


#define ACTION_ADD  0
#define ACTION_SCAN 1
#define ACTION_REMOVE 2
#define ACTION_SHOW 3
#define ACTION_QUIT 4
#define ACTION_SHOW_RESET 5

#define TABLE_WIDTH 280
#define TOP_BORDER 50
#define BOTTOM_BORDER 20

#define LYRIC_TITLE_HEIGHT 60
#define DEFAULT_LYRIC_FONT_SIZE 10
#define CURRETN_LYRIC_FONT_SIZE 15

#define LYRIC_BORDER 10
#define PAINTER_LYRIC_BORDER 5
#define LYRIC_SINGLE_HEIGHT 30
#define LYRIC_DOUBLE_HEIGHT 60

#define DEFAULT_WIDGET_X (QApplication::desktop()->width() / 2 - m_config.widget_width / 2)
#define DEFAULT_WIDGET_Y (QApplication::desktop()->height() / 2 - m_config.widget_height / 2)

#define DEFAULT_WIDGET_WIDTH 760
#define DEFAULT_WIDGET_HEIGHT 450

#define DEFAULT_LYRIC_BAR_WIDTH 1000
#define DEFAULT_LYRIC_BAR_HEIGHT 120

#define MUSIC_ICON "logo.png"
#define PLAYERANIMATION_IMAGE "/resource/logo.png"

#define RECORD_FILE "/resource/current_playlist.txt"
#define LYRIC_SUFFIX ".txt"

#define LYRIC_ANALYSIS_CMD "analysis_exe -m analysis_model -l zh -f analysis_music_file -otxt analysis_lyric_file"
#define LYRIC_ANALYSIS_EXE "/cli/main.exe"
#define LYRIC_ANALYSIS_MODEL "/cli/ggml-large-v1.bin"


enum PlayType{
    ShufflePlay=0,//随机播放
    OrderPlay,//顺序播放
    SingleLoop,//单曲循环
    ListLoop//列表循环
};

struct MusicData
{
    QString File_path;//音频路径
    QString Title;//标题
    QString Author;//作者
    QString Duration_format;//时长格式化
    quint64  Duration;//时长
    QImage ThumbnailImage;//专辑图
};

struct LineLyricData
{
  int index;
  long long start_time;
  long long end_time;
  QString lyric;
};

struct Config
{
    std::map<QString, QString> map_name_file;
    std::set<QString> set_styles;
    QString software_name;
	QString software_about;
    QString icon;
    PlayType playType;
	QString lyric_analysis_cmd;
	QString lyric_analysis_exe;
	QString lyric_analysis_model;
    unsigned int style_index;
    int volume;
    int widget_x;
    int widget_y;
    int widget_width;
    int widget_height;
    int lyric_x;
    int lyric_y;
    int lyric_width;
    int lyric_height;
    bool lyric_show;
};

#endif // DATASTRUCT_H
