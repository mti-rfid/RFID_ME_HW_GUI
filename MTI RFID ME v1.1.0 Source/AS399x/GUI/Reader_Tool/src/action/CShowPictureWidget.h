#ifndef CSHOWPICTUREWIDGET_H
#define CSHOWPICTUREWIDGET_H

#include <QtGui/QWidget>
#include "../../build/tmp/ui/ui_CShowPictureWidget.h"

#include <QString>

class CShowPictureWidget : public QWidget
{
    Q_OBJECT

public:
    CShowPictureWidget(QWidget *parent, QString epc, QString picturePath, QString info, QString readerId);
    ~CShowPictureWidget();

    QString epc(){return m_epc;}
private:
	QString m_epc;

    Ui::CShowPictureWidgetClass ui;
};

#endif // CSHOWPICTUREWIDGET_H
