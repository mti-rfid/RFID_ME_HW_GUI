/*
 *****************************************************************************
 * Copyright @ 2009 by austriamicrosystems AG                                *
 * All rights are reserved.                                                  *
 *                                                                           *
 * IMPORTANT - PLEASE READ CAREFULLY BEFORE COPYING, INSTALLING OR USING     *
 * THE SOFTWARE.                                                             *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       * 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         *
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS         *
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT  *
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT          *
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
 *****************************************************************************
 */
/*
 * CDatabaseInterface.cpp
 *
 *  Created on: 07.10.2008
 *      Author: stefan.detter
 */

#include "../inc/QrfeDatabaseInterface.h"

#include <QStringList>
#include <QSqlRecord>

QrfeDatabaseInterface::QrfeDatabaseInterface(QString driver, QString hostName, QString dbName, QString userName, QString passwd, QObject* parent)
	: QObject(parent)
#ifdef QrfeDATABASEINTERFACE_DEBUG
	, QrfeTraceModule("QrfeDatabaseInterface")
#endif
{
#ifdef QrfeDATABASEINTERFACE_DEBUG
	trc(9, "Creating...");
	trc(10, "Available drivers: " + QSqlDatabase::drivers().join(", "));
#endif

	m_db = QSqlDatabase::addDatabase(driver);
	m_db.setHostName(hostName);
	m_db.setDatabaseName(m_db_name = dbName);
	m_db.setUserName(userName);
	m_db.setPassword(passwd);

	if(driver == "QSQLITE")
		m_query_db_name = "";
	else
		m_query_db_name = "`" + m_db_name + "`.";
}

QrfeDatabaseInterface::~QrfeDatabaseInterface()
{
	if(m_db.isOpen())
		disconnect();
	m_db = QSqlDatabase();
}

bool QrfeDatabaseInterface::connect ()
{
	if(!m_db.open()){
#ifdef QrfeDATABASEINTERFACE_DEBUG
		trc(1, getLastErrorText(m_db));
		fatal("Could not open the database...");
#endif
	}

	return m_db.isOpen();
}

bool QrfeDatabaseInterface::disconnect ()
{
	m_db.close();
	if(m_db.isOpen()){
#ifdef QrfeDATABASEINTERFACE_DEBUG
		trc(1, getLastErrorText(m_db));
		fatal("Could not open the database...");
#endif
		return false;
	}
	return true;
}

bool QrfeDatabaseInterface::reconnectAndTryAgain (QSqlQuery &query)
{
#ifdef QrfeDATABASEINTERFACE_DEBUG
	trc(10, "-> reconnectAndTryAgain");
#endif
	m_db.close();
	m_db.open();
	if(!m_db.isOpen()){
#ifdef QrfeDATABASEINTERFACE_DEBUG
		trc(1, "Could not open database: " + m_db.connectionName());
#endif
		return false;
	}

	return query.exec();
}

bool QrfeDatabaseInterface::sql_exec (QSqlQuery &query)
{
#ifdef QrfeDATABASEINTERFACE_DEBUG
	trc(10, "-> sql_exec (QSqlQuery &query)");
#endif
	m_mutex.lock();
#ifdef QrfeDATABASEINTERFACE_DEBUG
	trc(10, "Mutex locked...");
#endif

	if(!query.exec()){
		if(!reconnectAndTryAgain(query)){
#ifdef QrfeDATABASEINTERFACE_DEBUG
			trc(1, "Could not exec the query \"" + query.executedQuery() + "\" ");
			trc(1, getLastErrorText(m_db));
			trc(1, getLastErrorText(query));
#endif

			m_mutex.unlock();
#ifdef QrfeDATABASEINTERFACE_DEBUG
			trc(10, "Mutex unlocked...");
#endif
			return false;
		}
	}

#ifdef QrfeDATABASEINTERFACE_DEBUG
	trc(9, "Executed query: " + query.executedQuery());
#endif

	m_mutex.unlock();
#ifdef QrfeDATABASEINTERFACE_DEBUG
	trc(10, "Mutex unlocked...");
#endif
	return true;
}


bool QrfeDatabaseInterface::sql_exec (QString queryStr)
{
#ifdef QrfeDATABASEINTERFACE_DEBUG
	trc(10, "-> sql_exec (QString queryStr)");
#endif

	QStringList queryList = queryStr.split(";", QString::SkipEmptyParts);
#ifdef QrfeDATABASEINTERFACE_DEBUG
	trc(9, "Executing " + QString::number(queryList.size()) + " querys.");
#endif

	foreach(QString singleQuery, queryList){
		QSqlQuery query(m_db);
		query.prepare(singleQuery);
		if(!sql_exec(query)){
#ifdef QrfeDATABASEINTERFACE_DEBUG
			trc(1, "ERROR on query: " + singleQuery);
#endif
			return false;
		}
	}

	return true;
}

bool QrfeDatabaseInterface::sql_exec (QString queryStr, QList<QVariant> values)
{
#ifdef QrfeDATABASEINTERFACE_DEBUG
	trc(10, "-> sql_exec (QString queryStr, QList<QVariant> values)");
#endif

	QSqlQuery query(m_db);
	query.prepare(queryStr);

	for(int i = 0; i < values.size(); i++){
		query.addBindValue(values.at(i));
#ifdef QrfeDATABASEINTERFACE_DEBUG
		trc(9, QString::number(i) + ":  " + values.at(i).toString());
#endif
	}

	if(!sql_exec(query)){
		return false;
	}

	return true;
}

bool QrfeDatabaseInterface::sql_insert	(QString tableName, QStringList elements, QList<QVariant> values)
{
#ifdef QrfeDATABASEINTERFACE_DEBUG
	trc(10, "-> sql_insert (QString tableName, QStringList elements, QList<QVariant> values)");
#endif

	if(elements.size() != values.size()){
#ifdef QrfeDATABASEINTERFACE_DEBUG
		trc(1, "Size of elements does not match size of values");
#endif
		return false;
	}

	QString bindList;
	for(int i = 0; i < elements.size(); i++){
		bindList += "?";
		if(i < elements.size()-1)
			bindList += ", ";
	}

	QString q = "INSERT INTO "  + m_query_db_name + "`" + tableName + "` (`" + elements.join("`, `") + "`) VALUES (" + bindList + ");";
	QSqlQuery query(m_db);
	query.prepare(q);
	for(int i = 0; i < values.size(); i++){
		query.addBindValue(values.at(i));
#ifdef QrfeDATABASEINTERFACE_DEBUG
		trc(9, QString::number(i) + ":  " + elements.at(i) + ": " + values.at(i).toString());
#endif
	}

	if(!sql_exec(query)){
		return false;
	}

	return true;

}

bool QrfeDatabaseInterface::sql_select	(QString tableName, QStringList elements, QList< QList<QVariant> > &values)
{
#ifdef QrfeDATABASEINTERFACE_DEBUG
	trc(10, "-> sql_select (QString tableName, QStringList elements, QList< QList<QVariant> > &values)");
#endif

	QString q = "SELECT `" + elements.join("`, `") +  "` FROM " + m_query_db_name + "`" + tableName + "`;";
	QSqlQuery query(m_db);
	query.prepare(q);
	if(!sql_exec(query)){
		return false;
	}

	while (query.next()) {
	    QList<QVariant> row;
	    for(int i=0; i < elements.size(); i++){
	    	row.append(query.value(query.record().indexOf(elements.at(i))));
	    }
	    values.append(row);
	}

	return true;
}

bool QrfeDatabaseInterface::sql_select	(QString tableName, QStringList elements, QString rules, QList< QList<QVariant> > &values)
{
#ifdef QrfeDATABASEINTERFACE_DEBUG
	trc(10, "-> sql_select (QString tableName, QStringList elements, QString rules, QList< QList<QVariant> > &values)");
#endif

	QString q = "SELECT `" + elements.join("`, `") +  "` FROM " + m_query_db_name + "`" + tableName + "` WHERE " + rules + ";";
	QSqlQuery query(m_db);
	query.prepare(q);
	if(!sql_exec(query)){
		return false;
	}

	while (query.next()) {
	    QList<QVariant> row;
	    for(int i=0; i < elements.size(); i++){
	    	row.append(query.value(query.record().indexOf(elements.at(i))));
	    }
	    values.append(row);
	}

	return true;
}

bool QrfeDatabaseInterface::sql_update	(QString tableName, QMap<QString, QVariant> assignments, QString rules)
{
#ifdef QrfeDATABASEINTERFACE_DEBUG
	trc(10, "-> sql_update (QString tableName, QMap<QString, QVariant> assignments, QString rules)");
#endif

	QStringList fields = assignments.keys();
	QStringList assignmentString;
	for(int i=0; i < fields.size(); i++)
		assignmentString << "`" + fields.at(i) + "`= ?";

	QString q = "UPDATE " + m_query_db_name + "`" + tableName + "` SET " + assignmentString.join(", ") + " WHERE " + rules + ";";
	QSqlQuery query(m_db);
	query.prepare(q);

	for(int i = 0; i < fields.size(); i++){
		query.addBindValue(assignments.value(fields.at(i)));
#ifdef QrfeDATABASEINTERFACE_DEBUG
		trc(9, QString::number(i) + ":  " + fields.at(i) + ": " + assignments.value(fields.at(i)).toString());
#endif
	}

	if(!sql_exec(query)){
		return false;
	}

	return true;
}

bool QrfeDatabaseInterface::sql_delete	(QString tableName, QString rules)
{
#ifdef QrfeDATABASEINTERFACE_DEBUG
	trc(10, "-> sql_delete (QString tableName, QString rules)");
#endif

	QString q = "DELETE FROM " + m_query_db_name + "`" + tableName + "` WHERE " + rules + ";";
	QSqlQuery query(m_db);
	query.prepare(q);

	if(!sql_exec(query)){
		return false;
	}

	return true;
}

