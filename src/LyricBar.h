#ifndef LYRICBAR_H
#define LYRICBAR_H

#include <QWidget>
#include <QDialog>
#include <QLayout>
#include <QPushButton>
#include <QPaintEvent>
#include <QPainter>
#include <QMediaPlayer>
#include <QTimer>
#include <QMenu>
#include <QContextMenuEvent>

//歌词控制
class LyricBarControl:public QWidget
{
    Q_OBJECT
public:
    LyricBarControl(QWidget* parent=nullptr);
    void showBtns();
    void hiddenBtns();
    void cancelFiexd();
public slots:
    void onStateChanged(QMediaPlayer::State newState);
protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
private slots:
    void onClickedFiexd();
signals:
    void sigFiexd(bool);
    void sigGoMain();
    void sigBack();
    void sigForward();
    void sigPre();
    void sigNext();
    void sigPlayerOrPause();
    void sigSet();
    void sigClose();
private:
    void setBtnsVisible(bool bl);
private:
    QPushButton* m_btn_player_pause;
    QPushButton* m_btn_show_main;
    QPushButton* m_btn_back;
    QPushButton* m_btn_forward;
    QPushButton* m_btn_pre;
    QPushButton* m_btn_next;
    QPushButton* m_btn_set;
    QPushButton* m_btn_fixed;
    QPushButton* m_btn_close;
    bool m_b_fixed;
    bool m_b_enter;
};

//歌词
class LyricPlayer:public QWidget
{
    Q_OBJECT
public:
    LyricPlayer(QWidget* parent=nullptr);
	void clear();
    void setCurrentLyricSingle(QString lyric, int time);
    void setCurrentLyricDouble(QString lyric_one, QString lyric_two, int time);
    void addTimeout();//+50ms
    void subTimeout();//-50ms
signals:
    void sigEnter();
public slots:
    void onStateChanged(QMediaPlayer::State newState);
protected:
    void paintEvent(QPaintEvent* event);
    void enterEvent(QEvent *event);
protected slots:
    void onTimeout();
private:
    int getFontSize(const QString &lyric,  int &width, int &height, const bool &b_single=true);
    int updateFontSize(QString lyric, const int &width, const int &height, int &width_change, int &height_change);
    void getLyricInfo(QString &lyric_one, QString &lyric_two,
                      int &font_size_one, int &font_size_two,
                      int &width_one, int &width_two,
                      int &height_one, int &height_two);

    void paintSingle(QPainter& painter);
    void paintDouble(QPainter& painter);
private:
    QTimer m_timer;
    QString m_lyric_one;//中文、英文
    QString m_lyric_two;

    int m_time;
    int m_current_time;
    int m_every_time;
    int m_time_out;
    QColor m_text_color;
    QColor m_player_color;
};

//歌词主界面
class LyricBar:public QDialog
{
    Q_OBJECT
public:
    enum MOUSE_POSITION{
        Bottom, Right,RightBottom
    };
    LyricBar(QWidget* parent=nullptr);
    ~LyricBar();
	void clear();
    void setCurrentLyricSingle(QString lyric, int time);
    void setCurrentLyricDouble(QString lyric_one, QString lyric_two, int time);
signals:
    void sigLeave();
    void sigGoMain();
    void sigBack();
    void sigForward();
    void sigPre();
    void sigNext();
    void sigPlayerOrPause();
    void sigSet();
    void stateChanged(QMediaPlayer::State newState);
public slots:
    void onEnter();
    void onLeave();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void leaveEvent(QEvent *event);
protected slots:
    void onFiexd(bool bl);
private:
    LyricBarControl* m_lyricBarControl;
    LyricPlayer* m_lyricPlayer;
    QPoint m_point_start;

    QPoint pressPoint;
    MOUSE_POSITION mousePosition;
    bool  mouseLeftPressChangeSize;   //鼠标左键按下
    bool  mouseLeftPressMove;   //鼠标左键按下

    bool m_b_enter;
    bool m_b_fixed;
};

#endif // LYRICBAR_H
