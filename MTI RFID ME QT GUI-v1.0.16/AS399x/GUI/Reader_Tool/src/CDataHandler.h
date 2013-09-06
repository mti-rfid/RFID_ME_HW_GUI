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

#ifndef CDATAHANDLER_H_
#define CDATAHANDLER_H_

#include <QrfeTrace.h>

#include <QrfeDatabaseInterface.h>

enum ActionType{ShowPicture = 0, StartApp = 1};

struct TagActionInfo{
	QString aliasName;
	bool performAction;
	int time;
	ActionType type;
	QString picPath;
	QString appPath;
	QString appParams;
};

class CDataHandler : public QObject
	, QrfeTraceModule
{
	Q_OBJECT

public:
	CDataHandler();
	virtual ~CDataHandler();

	bool isConfigSet(QString epc);

	bool getConfig(QString epc, TagActionInfo &t);
	bool saveConfig(QString epc, TagActionInfo t);

	quint64 timeValToMsecs(quint32 timeVal);

private:
	bool createDatabase();

	QMap<QString, TagActionInfo> m_cache;

	QrfeDatabaseInterface* m_db;
};


static const QString qmysql =
	"CREATE TABLE IF NOT EXISTS `tbl_test` ("
	"  `TagId` varchar(30) unsigned NOT NULL,"
	"  `Information` varchar(45) NOT NULL,"
	"  `PerformAction` bool NOT NULL,"
	"  `ForTime` int(10) unsigned,"
	"  `ActionType` int(10) unsigned,"
	"  `PicPath` varchar(100),"
	"  `AppPath` varchar(100),"
	"  `AppParams` varchar(100)"
	"  PRIMARY KEY  (`Id`)"
	") ENGINE=InnoDB DEFAULT CHARSET=latin1;";


static const QString qsqlite =
	"CREATE TABLE IF NOT EXISTS `tbl_test` ("
	" `TagId` text NOT NULL PRIMARY KEY,"
	" `Information` text NOT NULL,"
	" `PerformAction` integer NOT NULL,"
	" `ForTime` integer,"
	" `ActionType` integer,"
	" `PicPath` text,"
	" `AppPath` text,"
	" `AppParams` text"
	");";


#endif /* CDATAHANDLER_H_ */
