#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>


enum ResizeEdge { None, Left, Right, Top, Bottom, BottomRight };
class MyWidget:public QWidget
{
    Q_OBJECT
public:
    MyWidget(QWidget* parent=nullptr);
    void setMoveEnable(bool bl);
    void setChangeSizeEnable(bool bl);
	void setMoveEnableRange(int x, int y, int width, int height);
protected:
	void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent* event);

private:
    ResizeEdge getResizeEdge(const QPoint &pos);
	bool isMoveEnable(QPoint point);
private:
    QPoint m_pressPos;
    QPoint m_dragPos;
    QRect m_oldGeometry;
	QRect m_moveGeometry;
    ResizeEdge m_resizeEdge;
    bool m_b_moveEnable;
    bool m_b_changeSizeEnable;
	bool m_b_set_moveRange;
};

#endif // MYWIDGET_H
