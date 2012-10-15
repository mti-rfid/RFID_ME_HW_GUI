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
 * CTagManager.h
 *
 *  Created on: 28.01.2009
 *      Author: stefan.detter
 */

#ifndef CTAGVIEWMANAGER_H_
#define CTAGVIEWMANAGER_H_

#include <QObject>
#include <QTreeWidget>
#include <QTimer>
#include <QMenu>
#include <QTime>

#include <QrfeTrace.h>
#include <QCoreApplication>    //Add by yingwei tseng for print tagid and read rate to readerInfo.txt, 2010/06/21 

#include "CDataHandler.h"
#include "CReadRateCalc.h"
#include "CTreeWidget.h"

typedef enum { STATE_ACTIVE, STATE_INACTIVE , STATE_OUTOFRANGE, STATE_DELETE } TagState;

typedef struct{
	TagState			tagState;
	quint64				readCounter;
	QByteArray			rssi;
	QTime				lastTimeSeen;
	QTreeWidgetItem* 	widgetEntry;
} STagInfo;

typedef struct{
	bool 				passive;
	bool				showRSSI;
	quint8				rssiChildCount;
	QStringList 		rssiChildNames;
	quint64				readCounter;
} SReaderInfo;

class CTagViewManager : public QObject
	, QrfeTraceModule
{
	Q_OBJECT
public:
	CTagViewManager(CTreeWidget* tree, CDataHandler* dataHandler, CReadRateCalc* readRateCalc, QObject* parent = 0);
	virtual ~CTagViewManager();

public:
	void setUp(bool showAlias, bool useTimeToLive, uint msecsToShowInactive, uint msecsToShowOutOfRange, uint msecsToDelete);

	void	addReader(QString readerId, QString hardwareRev, QString softwareRev, QString action, QString state, bool passive, bool showRSSI = false, uchar rssiChildCount = 0, QStringList rssiChildNames = QStringList());
	void	changeHardFirm(QString readerId, QString hardwareRev, QString softwareRev);
	void	removeReader(QString readerId);
	QString getCurrentReader();

public slots:
	void readerChangedState(QString readerId, QString state);
	void readerChangedAction(QString readerId, QString action);
	void readerSetRSSI(QString readerId, bool on, uchar rssiChildCount, QStringList rssiChildNames);
	void currentItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous );

public:
	void resetReaderInfo();
	void resetReaderInfo(QString readerId);
	void resetTagInfo(QString readerId);
	void resetTagInfoTime(QString readerId);
	void clearTagsOfReader(QString readerId);
	void clearAllReaderTagInfos();

	void selectSingleReader(QString readerId);
	void showAllReader();

public slots:
	void clearTags();
	void clearOfflineReader();

public:
	void setActive(QString readerId = QString());
	void setUnactive(QString readerId);

public slots:
	void heartBeat(QString readerId);
	void cyclicInventroyResult(QString readerId, QString tagId);
	void cyclicInventoryRSSIResult(QString readerId, QString tagId, QByteArray rssi);

public:
	void startGuiUpdate();
	void stopGuiUpdate();

public slots:
	void cyclicGuiUpdate ( );

private:
	QTreeWidgetItem* createTagEntry (QString readerId, QString tagId);
	void removeTagEntry (QTreeWidgetItem* tagItem);

	void addRSSIInfo(QTreeWidgetItem* tagItem, uchar rssiChildCount, QStringList rssiChildNames);
	void removeRSSIInfo(QTreeWidgetItem* tagItem);

	void updateReaderInfo(QString readerId);
	void updateTagInfo(QString readerId, QString tagId);

	void checkForOld(QString readerId);

	void changeTagState(QString readerId, QString tagId, TagState tagState);
	void showAllTagsActive(QString readerId);

	void changeTagNames( );

	void readerInfoWrite(int position, const QString msg);    //Add by yingwei tseng for print tagid and read rate to readerInfo.txt, 2010/06/21 	

public slots:
	void itemDoubleClicked ( QTreeWidgetItem * item );
	void showPopup(const QPoint & iPoint);
	void contextMenuClicked();

signals:
	void requestTagSettings(QString tagId);
	void requestTagAdvancedSettings(QString readerId, QString tagId);
	void requestReaderAdvancedSettings(QString readerId);
	void requestReaderRegisterMap(QString readerId);

	void newTagCount(int count);
	void newDifferentTagCount(int count);
	void newOverallDifferentTagCount(int count);

	void oldTagEntryRemoved(QString readerId, QString tagId);
	void currentReaderChanged(QString);

private:
	CTreeWidget* 		m_treeWidget;
	CDataHandler* 		m_dataHandler;
	CReadRateCalc* 		m_readRateCalc;

	QMenu* 		m_tagContext;
	QAction* 	m_tagContext_Settings;
	QAction* 	m_tagContext_AdvancedSettings;
	QMenu* 		m_readerContext;
	QAction* 	m_readerContext_AdvancedSettings;
	QAction* 	m_readerContext_RegMap;
	QTreeWidgetItem* m_contextItem;

    QMap<QString, QTreeWidgetItem*> 			m_readerEntries;
    QMap<QString, SReaderInfo>					m_readerInfo;

    QMap<QTreeWidgetItem*, QString>				m_tagEntries;			// MAP= TreeWidgetItem:TagID
    QMap<QString, QMap<QString, STagInfo> > 	m_readerTagsInfo;		// MAP= ReaderID:(TagID:Info)

    QMap<QString, QTreeWidgetItem*> 			m_readerEntriesToDelete;

    QTimer* 	m_guiUpdateTimer;

    QStringList	m_activeReader;

    QSet<QString>	m_overallDifferentTags;

	QFont 		m_readerIdFont;
	QFont 		m_readerVersionFont;
	QFont 		m_readerStateFont;
	QFont 		m_readerInfoFont;
	QFont 		m_readerInfoDataFont;
	QFont 		m_tagIdFont;
	QFont 		m_tagInfoDataFont;
	QFile*      m_readerInfoFile;    	//Add by yingwei tseng for print tagid and read rate to readerInfo.txt, 2010/06/21     

    bool 		m_showAlias;
    bool 		m_useTimeToLive;
    uint		m_msecsToShowInactive;
    uint		m_msecsToShowOutOfRange;
    uint		m_msecsToDelete;

    static const int	m_ReaderColumn;
    static const int	m_HardwareColumn;
    static const int	m_SoftwareColumn;
    static const int	m_ActionColumn;
    static const int	m_HeartBeatColumn;
    static const int	m_StateColumn;
};


#endif /* CTAGVIEWMANAGER_H_ */
