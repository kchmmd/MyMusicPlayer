/*******************************************
*PlayerAnimation.cpp
*播放动画
********************************************/
#pragma execution_character_set("utf-8")
#include "DataStruct.h"
#include "PlayerAnimation.h"
#include <QPainter>
#include <QBrush>
#include <QTimer>
#include <QPainterPath>
#include <QFile>
#include <QCoreApplication>
#include <QDebug>

#define TIMEOUT 10

PlayerAnimation::PlayerAnimation()
{
    setObjectName("PlayerAnimation");
    m_angle = 1;
    connect(&m_timer,&QTimer::timeout, this, &PlayerAnimation::onTimeout);
    m_timer.setInterval(TIMEOUT);
	QString root_path = QCoreApplication::applicationDirPath();
	m_image = QImage(root_path + PLAYERANIMATION_IMAGE);
}

void PlayerAnimation::setImage(const QImage &image)
{
	if (image.isNull()) {
		QString root_path = QCoreApplication::applicationDirPath();
		m_image = QImage(root_path + PLAYERANIMATION_IMAGE);
	}
	else {
		m_image = image;
	}
}

void PlayerAnimation::play()
{
    m_timer.start();
}

void PlayerAnimation::stop()
{
    m_timer.stop();
}

void PlayerAnimation::onTimeout()
{
    m_angle = m_angle + 0.2;
    if(m_angle>=360){
        m_angle = 1;
    }
    update();
}

void PlayerAnimation::paintEvent(QPaintEvent *event)
{
    int center_radius = width() < height() ?  width()/2 : height()/2;
    //int top_radius = center_radius/10;
    //int top_radius_1 = top_radius*2/3;
    //int top_radius_2 = top_radius_1/3;
    //qDebug()<<"center_radius:"<<center_radius<<" top_radius:"<<top_radius;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);// 获取部件的中心作为渐变的中心


    QLinearGradient gradient_1(0, 0, 0, height());// 创建一个线性渐变对象，起点为(0, 0)，终点为(0, height())
    gradient_1.setColorAt(0, QColor(60,60,60));// 设置渐变的起始颜色
    gradient_1.setColorAt(1, QColor(9,9,9));// 设置渐变的结束颜色
    //painter.fillRect(rect(), gradient_1);// 使用渐变作为画刷填充矩形

    //绘制半透明圆形背景
    painter.setOpacity(1);
    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient_1);
    painter.drawEllipse(QPointF(center_radius, height()-center_radius),
                        center_radius,
                        center_radius);

    //绘制圆形黑色
//    int border_spacing=20,spacing = 10;
//    painter.setOpacity(0.05);
//    painter.setBrush(Qt::black);
//    painter.drawEllipse(QPointF(center_radius, height()-center_radius),
//                        center_radius - spacing,
//                        center_radius - spacing);

//    QPen pen = painter.pen();
//    pen.setWidth(4);
//    pen.setStyle(Qt::SolidLine);
//    pen.setColor(Qt::black);
//    painter.setPen(Qt::NoPen);
//    painter.setBrush(QColor(48,48,48));
//    painter.setPen(pen);
//    painter.setOpacity(1);
//    painter.drawEllipse(QPointF(center_radius, height()-center_radius),
//                        center_radius - border_spacing,
//                        center_radius - border_spacing);

//    pen.setColor(QColor(50,50,50));
//    painter.setPen(pen);
//    painter.setBrush(Qt::NoBrush);
//    for(int i=border_spacing + spacing; i<center_radius - border_spacing-4; i+=spacing){

//        painter.drawEllipse(QPointF(center_radius, height()-center_radius),
//                            i,
//                            i);

//    }

    //上面放音柄
//    pen.setWidth(20);
//    pen.setColor(Qt::white);
//    painter.setPen(pen);
//    // 创建一个路径对象
//    QPainterPath path;
//    path.moveTo(center_radius - top_radius, top_radius);// 设置起始点
//    // 绘制三次贝塞尔曲线，参数依次为第一个控制点、第二个控制点、终点
//    path.cubicTo(center_radius*5/4, height() - center_radius*2,
//                 center_radius*5/4, height() - center_radius*2,
//                 center_radius*7/4, height() - center_radius*2 + border_spacing);

//    painter.drawPath(path);//绘制路径

//    pen.setWidth(10);
//    pen.setColor(QColor(235,235,235));
//    painter.setPen(pen);
//    painter.setBrush(Qt::white);
//    painter.drawEllipse(QPointF(center_radius*7/4, height() - center_radius*2 + border_spacing), border_spacing, border_spacing);


//    painter.setOpacity(0.1);
//    painter.setPen(Qt::NoPen);
//    painter.setBrush(Qt::white);
//    painter.drawEllipse(QPointF(center_radius - top_radius,top_radius),
//                        top_radius,
//                        top_radius);

//    painter.setOpacity(1);
//    painter.drawEllipse(QPointF(center_radius - top_radius,top_radius),
//                        top_radius_1,
//                        top_radius_1);


//    painter.setOpacity(0.1);
//    painter.setBrush(Qt::gray);
//    painter.drawEllipse(QPointF(center_radius - top_radius,top_radius),
//                        top_radius_2,
//                        top_radius_2);

    //转盘
    int pic_radius = center_radius*2/3;
    painter.translate(center_radius, height()-center_radius);// 将坐标系原点移动到窗口中心
    painter.rotate(m_angle);// 旋转坐标系

//    painter.setPen(Qt::NoPen);
//    painter.setOpacity(1);
//    QBrush brush(QImage(m_qstr_image).scaled(QSize(480,480), Qt::KeepAspectRatio, Qt::FastTransformation));
//    painter.setBrush(brush);
//    painter.drawEllipse(QPointF(0,0),
//                        pic_radius,
//                        pic_radius);

    QPainterPath path;
    path.addRoundedRect(QRect(-pic_radius, -pic_radius,pic_radius*2, pic_radius*2), pic_radius, pic_radius);  // 指定起始坐标和文本
    painter.setPen(Qt::NoPen);
    painter.drawPath(path);  // 绘制路径
    painter.setClipPath(path);

    QRect target(-pic_radius, -pic_radius, pic_radius*2, pic_radius*2);
    QPixmap pixmap = QPixmap::fromImage(m_image);
    // 自适应
    painter.drawPixmap(target, pixmap.scaled(QSize(480,480), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
