#ifndef HEADERBAR_H
#define HEADERBAR_H

#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QEvent>

class HeaderBar:public QWidget
{
    Q_OBJECT
public:
    HeaderBar(QWidget* parent=nullptr);
    void setTitle(QString title);
    void setTitleIcon(QIcon icon);
    void setTitleFontSize(int fontSize);
    void setTitleToolTip(QString qstr);

    void setSetTooltip(QString qstr);
    void setStyleTooltip(QString qstr);
    void setMinTooltip(QString qstr);
    void setMaxOrNormalTooltip(QString qstr);
    void setCloseTooltip(QString qstr);
protected:
	bool eventFilter(QObject *obj, QEvent *event);
signals:
    void sigClickedTitle();
    void sigClickedSet();
    void sigClickedStyle();
    void sigClickedMin();
    void sigClickedMaxOrNormal();
    void sigClickedClose();
protected slots:
    void onClickedMaxOrNormal();
private:
    QLabel* m_lable_title;
    QPushButton* m_btn_title;
    QPushButton* m_btn_set;
    QPushButton* m_btn_style;
    QPushButton* m_btn_min;
    QPushButton* m_btn_maxOrNormal;
    QPushButton* m_btn_close;
	QList<QObject*> m_list_object;
};

#endif // HEADERBAR_H
