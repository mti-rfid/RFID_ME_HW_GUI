/*
 * CActionHandler.h
 *
 *  Created on: 02.02.2009
 *      Author: stefan.detter
 */

#ifndef CACTIONHANDLER_H_
#define CACTIONHANDLER_H_

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QTimer>

#ifdef QrfeDATABASEINTERFACE_DEBUG
#include <QrfeTrace.h>
#endif

#include "../CDataHandler.h"
#include "CApplicationStarter.h"
#include "CShowPictureWidget.h"

class CActionHandler : public QObject
	, QrfeTraceModule
{
	Q_OBJECT
public:
	CActionHandler(CDataHandler* dataHandler, QObject* parent = 0);
	virtual ~CActionHandler();

public slots:
	void startActionHandling();
	void stopActionHandling();

	void deleteShownPicture();
	void stoppApplication();

	void stopAll();

	void cyclicInventroyResult(QString readerId, QString epc);

private:

	CDataHandler* m_dataHandler;

    QStringList m_handledTags;
    QMap<QTimer*, QWidget*>					m_openPicture;
    QMap<QTimer*, CApplicationStarter*>		m_openApps;

    bool 	m_doActionHandling;

};

#endif /* CACTIONHANDLER_H_ */
