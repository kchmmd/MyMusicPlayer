/*******************************************
*SearchBar.cpp
*搜索框
********************************************/
#pragma execution_character_set("utf-8")
#include "SearchBar.h"

#include <QDebug>

SearchBar::SearchBar(QWidget* parent)
    :QWidget(parent)
{
    this->setAttribute(Qt::WA_StyledBackground,true);
    setObjectName("SearchBar");
    QHBoxLayout* hbox = new QHBoxLayout;
    m_lineEdit = new QLineEdit;
    m_btn_search = new QPushButton;
    m_btn_clear = new QPushButton;
    hbox->addWidget(m_btn_search);
    hbox->addWidget(m_lineEdit);
    hbox->addWidget(m_btn_clear);
    this->setLayout(hbox);
    hbox->setContentsMargins(0, 5, 5, 5);
    hbox->setSpacing(0);

    m_btn_clear->setObjectName("SearchBar_clear");
    m_btn_search->setObjectName("SearchBar_search");
    m_btn_clear->setToolTip(tr("Clear"));
    m_btn_search->setToolTip(tr("Search"));
    m_btn_clear->setIcon(QIcon(":/resource/resource/none.png"));
    m_btn_search->setIcon(QIcon(":/resource/resource/none.png"));
    m_lineEdit->setObjectName("SearchBar_lineEdit");

	m_btn_clear->installEventFilter(this);

    connect(m_lineEdit, &QLineEdit::textChanged, this, &SearchBar::sigSearch);
    connect(m_lineEdit, &QLineEdit::textChanged, this, &SearchBar::onTextChanged);
    connect(m_btn_clear, &QPushButton::clicked, this, &SearchBar::onClear);

    m_btn_clear->hide();
}

void SearchBar::onTextChanged(const QString &text)
{
    if(!text.isEmpty()){
        m_btn_clear->show();
    }
}

void SearchBar::onClear()
{
    m_lineEdit->clear();
    m_btn_clear->hide();
}

bool SearchBar::eventFilter(QObject * obj, QEvent * event)
{
	if (m_btn_clear == obj) {
		if (event->type() == QEvent::Enter) {
			setCursor(Qt::PointingHandCursor);
		}
		else if (event->type() == QEvent::Leave) {
			unsetCursor(); // 恢复默认光标
		}
	}
	return QWidget::eventFilter(obj, event);
}