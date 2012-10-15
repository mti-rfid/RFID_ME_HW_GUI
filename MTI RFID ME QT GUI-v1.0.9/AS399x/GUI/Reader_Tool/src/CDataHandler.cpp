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

#include "CDataHandler.h"

#include <QStringList>

CDataHandler::CDataHandler()
	: QrfeTraceModule("CDataHandler")
{
	m_db = new QrfeDatabaseInterface("QSQLITE", "localhost", "test_db", "root", "ajg90RSA", this);
    if(!m_db->connect())
    	fatal("Could not connect to database...");
	if(!createDatabase())
		fatal("Could not create database...");
}

CDataHandler::~CDataHandler() {
    if(!m_db->disconnect())
    	fatal("Could not close database connection...");
    m_db->deleteLater();
}

bool CDataHandler::isConfigSet(QString epc)
{
	if(m_cache.keys().contains(epc))
		return true;

	QList< QList<QVariant> > values;
	bool ret = m_db->sql_select(
			"tbl_test",
			QStringList() << "TagId",
			"TagId = \"" + epc + "\"",
			values);
	if(!ret)
		return false;
	return values.size() > 0;
}

bool CDataHandler::getConfig(QString epc, TagActionInfo &t)
{
	if(m_cache.keys().contains(epc)){
		t = m_cache.value(epc);
		return true;
	}

	QList< QList<QVariant> > values;
	bool ret = m_db->sql_select(
			"tbl_test",
			QStringList() << "Information" << "PerformAction" << "ForTime" << "ActionType" << "PicPath" << "AppPath" << "AppParams",
			"TagId = \"" + epc + "\"",
			values);

	if(!ret)
		return false;
	if(values.size() != 1)
		return false;

	bool ok = true;
	t.aliasName = values.at(0).at(0).toString();
	t.performAction = values.at(0).at(1).toBool();
	t.time = values.at(0).at(2).toInt(&ok);
	if(!ok)return false;
	t.type = (ActionType)values.at(0).at(3).toInt(&ok);
	if(!ok)return false;
	t.picPath = values.at(0).at(4).toString();
	t.appPath = values.at(0).at(5).toString();
	t.appParams = values.at(0).at(6).toString();

	m_cache.insert(epc, t);

	return true;
}

bool CDataHandler::saveConfig(QString epc, TagActionInfo t)
{
	bool ret;

	m_cache.insert(epc, t);

	ret = m_db->sql_delete(
			"tbl_test",
			"TagId = \"" + epc + "\""
			);

	ret = m_db->sql_insert(
			"tbl_test",
			QStringList() << "TagId" << "Information" << "PerformAction" << "ForTime" << "ActionType" << "PicPath" << "AppPath" << "AppParams",
			QList<QVariant>() << epc << t.aliasName << t.performAction << t.time << t.type << t.picPath << t.appPath << t.appParams);

	return ret;
}

bool CDataHandler::createDatabase()
{
	QString create;
	if(m_db->driverName() == "QMYSQL")
		create = qmysql;
	else if(m_db->driverName() == "QSQLITE")
		create = qsqlite;
	else
		fatal("CDataHandler: Not supported driver type ...");

	return m_db->sql_exec(create);
}

quint64 CDataHandler::timeValToMsecs(quint32 timeVal)
{
	switch(timeVal){
	case  0: return 1000;
	case  1: return 2000;
	case  2: return 3000;
	case  3: return 5000;
	case  4: return 10000;
	case  5: return 20000;
	case  6: return 30000;
	case  7: return 60000;
	case  8: return 120000;
	case  9: return 180000;
	case 10: return 300000;
	case 11: return 600000;
	case 12: return 1200000;
	case 13: return 1800000;
	default: return 1000;
	}
}
