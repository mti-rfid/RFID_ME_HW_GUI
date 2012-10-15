/*
 * QrfeDatabaseInterface.h
 *
 *  Created on: 07.10.2008
 *      Author: stefan.detter
 */

#ifndef QRFEDATABASEINTERFACE_H_
#define QRFEDATABASEINTERFACE_H_

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QMutex>
#include <QList>

#ifdef QrfeDATABASEINTERFACE_DEBUG
#include <QrfeTrace.h>
#endif


class QrfeDatabaseInterface : public QObject
#ifdef QrfeDATABASEINTERFACE_DEBUG
	, QrfeTraceModule
#endif
{
	Q_OBJECT

public:
	QrfeDatabaseInterface(QString driver, QString hostName, QString dbName, QString userName, QString passwd, QObject* parent = 0);
	virtual ~QrfeDatabaseInterface();

	bool connect ();
	bool disconnect ();

	static QString getLastErrorText(QSqlDatabase db) 	{ return "Error: " + db.lastError().text() + " - " + db.lastError().driverText(); }
	static QString getLastErrorText(QSqlQuery qu) 		{ return "Error: " + qu.lastError().text() + " - " + qu.lastError().driverText(); }


	QString driverName () {return m_db.driverName();}

	bool sql_exec (QSqlQuery &query);
	bool sql_exec (QString query);

	bool sql_exec (QString query, QList<QVariant> values);
	bool sql_exec_multi (QString query);

	bool sql_insert	(QString tableName, QStringList elements, QList<QVariant> values);
	bool sql_select	(QString tableName, QStringList elements, QList< QList<QVariant> > &values);
	bool sql_select	(QString tableName, QStringList elements, QString rules, QList< QList<QVariant> > &values);
	bool sql_update	(QString tableName, QMap<QString, QVariant> assignements, QString rules);
	bool sql_delete	(QString tableName, QString rules);


private:

	bool reconnectAndTryAgain (QSqlQuery &query);

	QMutex 			m_mutex;

	QSqlDatabase 	m_db;
	QString 		m_db_name;
	QString 		m_query_db_name;
};

#endif /* QRFEDATABASEINTERFACE_H_ */
