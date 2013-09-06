/*
 * CApplicationStarter.h
 *
 *  Created on: 28.10.2008
 *      Author: stefan.detter
 */

#ifndef CAPPLICATIONSTARTER_H_
#define CAPPLICATIONSTARTER_H_

#include <QObject>
#include <QProcess>
#include <QTimer>

#include <QrfeTrace.h>

class CApplicationStarter : public QObject 
	,QrfeTraceModule
{
	Q_OBJECT
public:
	CApplicationStarter(QString epc, QString appPath, QString appParams, QTimer* timer);
	virtual ~CApplicationStarter();

    QString epc(){return m_epc;}

public slots:
	void finished ( int exitCode, QProcess::ExitStatus exitStatus );

private:
	QString m_epc;
	QTimer* m_timer;

	QProcess* m_process;

};

#endif /* CAPPLICATIONSTARTER_H_ */
