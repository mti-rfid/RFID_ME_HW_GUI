#ifndef QRFESLEEPER_H_
#define QRFESLEEPER_H_

#include <QCoreApplication>
#include <QThread>

class QrfeSleeper : public QThread
{
	Q_OBJECT

public:
	virtual void run();

	static void MSleep ( unsigned long msecs );

	static void MSleepAlive ( unsigned long msecs, QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents );
};


#endif /*QRFESLEEPER_H_*/
