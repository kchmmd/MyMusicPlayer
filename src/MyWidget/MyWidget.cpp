#include "MyWidget.h"
#include <QLayout>
#include <QGraphicsDropShadowEffect>
#include <QPainterPath>
#include <QDebug>

const int  SHADOWN_BORDER = 5;
MyWidget::MyWidget(QWidget *parent)
    :QWidget(parent)
{
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);

    m_resizeEdge = None;
    m_b_moveEnable = true;
    m_b_changeSizeEnable = true;
	m_b_set_moveRange = false;
}

void MyWidget::setMoveEnable(bool bl)
{
    m_b_moveEnable = bl;
}

void MyWidget::setChangeSizeEnable(bool bl)
{
    m_b_changeSizeEnable = bl;
}

void MyWidget::setMoveEnableRange(int x, int y, int width, int height)
{
	m_b_set_moveRange = true;
	m_moveGeometry = QRect(x, y, width, height);
}

void MyWidget::mousePressEvent(QMouseEvent *event)
{
    m_pressPos = event->globalPos();
    if (event->button() == Qt::LeftButton) {
        m_dragPos = event->globalPos();       // 记录起始坐标
        m_oldGeometry = geometry();           // 保存当前窗口尺寸‌:ml-citation{ref="4" data="citationList"}
        m_resizeEdge = getResizeEdge(event->pos());  // 判断当前操作区域‌:ml-citation{ref="5" data="citationList"}
    }
}

void MyWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        if(isMoveEnable(event->pos())){
            move(pos() + event->globalPos() - m_pressPos);
            m_pressPos = event->globalPos();
        }
    }

    if(!m_b_changeSizeEnable){
        return;
    }
    // 边缘检测（5px敏感区域）
   const int borderWidth = 5;
   QPoint pos = event->pos();
   QRect rect = this->rect();

   // 设置光标形状
   if (pos.x() < borderWidth && pos.y() < borderWidth) {
       setCursor(Qt::SizeFDiagCursor);       // 左上角
   } else if (pos.x() > rect.width()-borderWidth && pos.y() > rect.height()-borderWidth) {
       setCursor(Qt::SizeFDiagCursor);       // 右下角‌:ml-citation{ref="6" data="citationList"}
   } else if (pos.x() < borderWidth || pos.x() > rect.width()-borderWidth) {
       setCursor(Qt::SizeHorCursor);         // 左右边缘‌:ml-citation{ref="2" data="citationList"}
   } else if (pos.y() < borderWidth || pos.y() > rect.height()-borderWidth) {
       setCursor(Qt::SizeVerCursor);         // 上下边缘‌:ml-citation{ref="4" data="citationList"}
   } else {
       setCursor(Qt::ArrowCursor);
   }

   // 处理缩放
   if (event->buttons() & Qt::LeftButton && m_resizeEdge != None) {
       QRect newGeo = m_oldGeometry;
       QPoint delta = event->globalPos() - m_dragPos;
       switch (m_resizeEdge) {
       case Left:
           newGeo.setLeft(newGeo.left() + delta.x());
           break;
       case Right:
           newGeo.setRight(newGeo.right() + delta.x());
           break;
       case Top:
           newGeo.setTop(newGeo.top() + delta.y());
           break;
       case Bottom:
           newGeo.setBottom(newGeo.bottom() + delta.y());
           break;
       case BottomRight:
           newGeo.setBottom(newGeo.bottom() + delta.y());
           newGeo.setRight(newGeo.right() + delta.x());
           break;
       default:
           break;
       }
       setGeometry(newGeo);                  // 应用新尺寸‌:ml-citation{ref="1" data="citationList"}
   }
}

void MyWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QColor bk_color = this->palette().background().color();// qss样式配置背景，这里获取

    if(this->isMaximized()){
        painter.setBrush(bk_color);
        painter.drawRect(0,0,width(),height());
        return;
    }
    // 绘制多层渐变圆角矩形模拟阴影
    QColor color(0, 0, 0, 50);
    for (int i = 0; i < SHADOWN_BORDER; i++) {
        QPainterPath path;
        QRectF rect(SHADOWN_BORDER-i, SHADOWN_BORDER-i, width()-(SHADOWN_BORDER-i)*2, height()-(SHADOWN_BORDER-i)*2);
        path.addRoundedRect(rect, SHADOWN_BORDER, SHADOWN_BORDER);
        color.setAlpha(50 - i*10);
        painter.fillPath(path, color);
    }

    QPainterPath path;
    QRectF rect(SHADOWN_BORDER, SHADOWN_BORDER, width()-SHADOWN_BORDER*2, height()-SHADOWN_BORDER*2);
    path.addRoundedRect(rect, SHADOWN_BORDER, SHADOWN_BORDER);
    // 绘制实际内容区域
    painter.fillPath(path, bk_color);
}

ResizeEdge MyWidget::getResizeEdge(const QPoint &pos)
{
    const int borderWidth = 5;
    QRect rect = this->rect();

    if (pos.x() > rect.width()-borderWidth && pos.y() > rect.height()-borderWidth)
        return BottomRight;
    if (pos.x() < borderWidth) return Left;
    if (pos.x() > rect.width()-borderWidth) return Right;
    if (pos.y() < borderWidth) return Top;
    if (pos.y() > rect.height()-borderWidth) return Bottom;
    return None;
}

bool MyWidget::isMoveEnable(QPoint point)
{
	if (!m_b_moveEnable) {
		return false;
	}
	if (m_b_set_moveRange) {
		int x = point.x();
		int y = point.y();
		//qDebug() << "point:" << point;
		//qDebug() << "m_moveGeometry:" << m_moveGeometry;
		//qDebug() << "this x:" << this->x()<< " this y:" << this->y();
		if (y > m_moveGeometry.height()) {
			return false;
		}
	}
	return true;
}
