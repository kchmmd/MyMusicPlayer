#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QTimer>
#include <QImage>

#define BILE (1/10)

class PlayerAnimation:public QWidget
{
    Q_OBJECT
public:
    PlayerAnimation();
	void setImage(const QImage &image);
    void play();
    void stop();
protected slots:
    void onTimeout();
protected:
    void paintEvent(QPaintEvent *event);
private:
    QTimer m_timer;
    double m_angle;
    QImage m_image;
};

#endif // PLAYER_H
