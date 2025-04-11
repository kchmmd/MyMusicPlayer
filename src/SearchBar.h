#ifndef SEARCHBAR_H
#define SEARCHBAR_H

#include <QWidget>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QEvent>

class SearchBar: public QWidget
{
    Q_OBJECT
public:
    SearchBar(QWidget* parent=nullptr);
protected:
	bool eventFilter(QObject *obj, QEvent *event);
signals:
    void sigSearch(const QString &);
    void sigClear();
protected slots:
    void onTextChanged(const QString & text);
    void onClear();
private:
    QLineEdit* m_lineEdit;
    QPushButton* m_btn_search;
    QPushButton* m_btn_clear;
};

#endif // SEARCHBAR_H
