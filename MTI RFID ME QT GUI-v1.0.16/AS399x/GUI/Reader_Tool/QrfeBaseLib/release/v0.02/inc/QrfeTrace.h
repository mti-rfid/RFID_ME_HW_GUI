#ifndef QRFETRACE_H_
#define QRFETRACE_H_

#include <QChar>
#include <QFile>
#include <QTime>
#include <QThread>

#define TRC_FILE_COUNT 4
#define TRC_FILE_SIZE 1048576 // 1MB

class QrfeTrace: public QObject
{
Q_OBJECT

private:
	QrfeTrace()	;
public	:
	~QrfeTrace()
	{	delete m_trcFile;}

	static QrfeTrace* getInstance();

	static void init( quint8 trcLevel, quint32 mode = Trace2Stdout );

	static quint8 trcLevel ( );
	static void setTrcLevel( quint8 trcLevel );

	static void trc (const int trcLevel, const QString msg );

	static QString makeHTML ( const QString trc );

	static quint32 Trace2Stdout;
	static quint32 Trace2File;
	static quint32 Trace2Signal;
	static quint32 Trace2HTMLSignal;
private:
	static QrfeTrace m_theInstance;

	qint8 m_trcLevel;
	bool m_trcFileInit;
	QFile* m_trcFile;
	quint8 m_trcFileNr;
	quint32 m_trcMode;

public slots:
	void trace ( const int trcLevel, const QString msg );

	signals:
	void traceSignal( QString str );
	void traceSignalHTML( QString str );
};

#define MAX_MODULE_NAME_SIZE 25

class QrfeTraceModule
{
private:
	QString traceModuleName;
	static uint maxModuleNameLength;

protected:
	void warning(QString msg);
	void error(QString msg);
	void fatal(QString msg);

	void trc(const int trcLevel, const QString msg);
	void trcBytes(const int trcLevel, const QString msg,
			const QByteArray bytes, const QString pattern = "0x%1 ");

public:
	static void strc(const int trcLevel, const QString msg);
	static void strcBytes(const int trcLevel, const QString msg,
			const QByteArray bytes, const QString pattern = "0x%1 ");

	static void strc(const QString moduleName, const int trcLevel,
			const QString msg);
	static void strcBytes(const QString moduleName, const int trcLevel,
			const QString msg, const QByteArray bytes, const QString pattern =
					"0x%1 ");

public:
	QrfeTraceModule(QString name);
	virtual ~QrfeTraceModule();
};

#endif /*QRFETRACE_H_*/
