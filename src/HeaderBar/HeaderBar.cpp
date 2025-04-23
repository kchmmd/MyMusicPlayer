#include "HeaderBar.h"
#include <QLayout>
#include <QApplication>
#include <QStyle>
#include <QDebug>

HeaderBar::HeaderBar(QWidget *parent)
    :QWidget(parent)
{
    setObjectName("HeaderBar");
    QHBoxLayout* hbox_main = new QHBoxLayout;
    m_lable_title = new QLabel(this);
    m_btn_title = new QPushButton(this);
    m_btn_set= new QPushButton(this);
	m_btn_language = new QPushButton(this);
    m_btn_style= new QPushButton(this);
    m_btn_min= new QPushButton(this);
    m_btn_maxOrNormal= new QPushButton(this);
    m_btn_close= new QPushButton(this);
    hbox_main->addWidget(m_btn_title);
    hbox_main->addWidget(m_lable_title);
    hbox_main->addStretch();
    hbox_main->addWidget(m_btn_set);
    hbox_main->addWidget(m_btn_style);
	hbox_main->addWidget(m_btn_language);
    hbox_main->addWidget(m_btn_min);
    hbox_main->addWidget(m_btn_maxOrNormal);
    hbox_main->addWidget(m_btn_close);
    setLayout(hbox_main);

    m_btn_close->setObjectName("close");
    m_btn_maxOrNormal->setObjectName("maxOrNormal");
    m_btn_min->setObjectName("min");
	m_btn_set->setObjectName("set");
	m_btn_language->setObjectName("language");
    m_btn_style->setObjectName("style");
    m_btn_maxOrNormal->setProperty("status","normal");
    m_lable_title->setObjectName("title");
    m_btn_title->setObjectName("title");

    m_btn_title->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_set->setIcon(QIcon(":/resource/resource/none.png"));
	m_btn_language->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_style->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_min->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_maxOrNormal->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_close->setIcon(QIcon(":/resource/resource/none.png"));

	m_list_object << m_btn_set << m_btn_language << m_btn_style << m_btn_min << m_btn_maxOrNormal << m_btn_close;
	for (QObject* obj : m_list_object) {
		obj->installEventFilter(this);
	}

    connect(m_btn_title, &QPushButton::clicked, this, &HeaderBar::sigClickedTitle);
	connect(m_btn_language, &QPushButton::clicked, this, &HeaderBar::sigClickedLanguage);
    connect(m_btn_set, &QPushButton::clicked, this, &HeaderBar::sigClickedSet);
    connect(m_btn_style, &QPushButton::clicked, this, &HeaderBar::sigClickedStyle);
    connect(m_btn_min, &QPushButton::clicked, this, &HeaderBar::sigClickedMin);
    connect(m_btn_maxOrNormal, &QPushButton::clicked, this, &HeaderBar::sigClickedMaxOrNormal);
    connect(m_btn_close, &QPushButton::clicked, this, &HeaderBar::sigClickedClose);
    connect(m_btn_maxOrNormal, &QPushButton::clicked, this, &HeaderBar::onClickedMaxOrNormal);

    setSetTooltip(tr("Set"));
	setLanguageTooltip(tr("Language"));
    setStyleTooltip(tr("Style"));
    setMinTooltip(tr("Min"));
    setMaxOrNormalTooltip(tr("Max"));
    setCloseTooltip(tr("Close"));

    m_btn_set->hide();
}

void HeaderBar::setTitle(QString title)
{
    m_lable_title->setText(title);
}
void HeaderBar::setTitleIcon(QIcon icon)
{
    m_btn_title->setIcon(icon);
}

void HeaderBar::setTitleFontSize(int fontSize)
{
    QFont ft;
    ft.setPointSize(fontSize);
    m_lable_title->setFont(ft);
}
void HeaderBar::setTitleToolTip(QString qstr)
{
    m_lable_title->setToolTip(qstr);
    m_btn_title->setToolTip(qstr);
}

void HeaderBar::setSetTooltip(QString qstr)
{
    m_btn_set->setToolTip(qstr);
}
void HeaderBar::setLanguageTooltip(QString qstr)
{
	m_btn_language->setToolTip(qstr);
}
void HeaderBar::setStyleTooltip(QString qstr)
{
    m_btn_style->setToolTip(qstr);
}
void HeaderBar::setMinTooltip(QString qstr)
{
    m_btn_min->setToolTip(qstr);
}
void HeaderBar::setMaxOrNormalTooltip(QString qstr)
{
    m_btn_maxOrNormal->setToolTip(qstr);
}
void HeaderBar::setCloseTooltip(QString qstr)
{
    m_btn_close->setToolTip(qstr);
}
void HeaderBar::onClickedMaxOrNormal()
{
    qDebug()<<"status:"<<m_btn_maxOrNormal->property("status");
    m_btn_maxOrNormal->style()->unpolish(m_btn_maxOrNormal);
    if(m_btn_maxOrNormal->property("status").toString() == "normal"){
        m_btn_maxOrNormal->setProperty("status","max");
		m_btn_maxOrNormal->setToolTip(tr("Restore"));
    }else{
        m_btn_maxOrNormal->setProperty("status","normal");
		m_btn_maxOrNormal->setToolTip(tr("Max"));
    }
    m_btn_maxOrNormal->style()->polish(m_btn_maxOrNormal);
}

bool HeaderBar::eventFilter(QObject * obj, QEvent * event)
{
	if (m_list_object.indexOf(obj) != -1) {
		if (event->type() == QEvent::Enter) {
			setCursor(Qt::PointingHandCursor);
		}
		else if (event->type() == QEvent::Leave) {
			unsetCursor(); // 恢复默认光标
		}
		else if (event->type() == QEvent::MouseMove) {
			return true;
		}
	}
	return QWidget::eventFilter(obj, event);
}
