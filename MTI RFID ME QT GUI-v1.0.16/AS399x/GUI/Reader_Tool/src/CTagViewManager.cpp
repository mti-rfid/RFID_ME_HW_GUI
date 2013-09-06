/*
 * CTagViewManager.cpp
 *
 *  Created on: 28.01.2009
 *      Author: stefan.detter
 */

#include "CTagViewManager.h"

#include <QLabel>
#include <QProgressBar>


const int	CTagViewManager::m_ReaderColumn = 0;
const int	CTagViewManager::m_HardwareColumn = 1;
const int	CTagViewManager::m_SoftwareColumn = 2;
const int	CTagViewManager::m_ActionColumn = 3;
//Mod by yingwei tseng for modifying GUI style, 2010/08/30
#if 0
const int	CTagViewManager::m_HeartBeatColumn = 4;
const int	CTagViewManager::m_StateColumn = 5;
#endif
const int	CTagViewManager::m_StateColumn = 4;
//End by yingwei tseng for modifying GUI style, 2010/08/30
/*!
 * @brief 	Constructor of the view manager
 * @param	treeWidget		Pointer to tree widget, where the data should be written in
 * @param	dataHandler		Pointer to the data handler that enables access to the database
 * @param	readRateCalc	Pointer to the object that calculates the read rate
 * @param	parent			Pointer top the parent object
 */
CTagViewManager::CTagViewManager(CTreeWidget* treeWidget, CDataHandler* dataHandler, CReadRateCalc* readRateCalc, QObject* parent)
	: QObject(parent)
	, QrfeTraceModule("CTagViewManager")
{
	/* Store data handler */
	m_dataHandler = dataHandler;

	/* Store read rate calc */
	m_readRateCalc = readRateCalc;

	/* Store tree widget and connect to the needed signals */
	m_treeWidget = treeWidget;
	m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	QObject::connect(m_treeWidget, SIGNAL(customContextMenuRequested ( const QPoint& )), this, SLOT(showPopup(const QPoint &)));
	QObject::connect(m_treeWidget, SIGNAL(itemDoubleClicked (QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked ( QTreeWidgetItem *)));
	QObject::connect(m_treeWidget, SIGNAL(currentItemChanged (QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

	/* Set up tree widget */
	//Mod by yingwei tseng for modifying GUI style, 2010/08/30
#if 0
	m_treeWidget->setColumnCount(6);
	m_treeWidget->setHeaderLabels(QStringList() << "Reader" << "Hardware" << "Software" << "Action" << "HB" << "State" );
#endif
	m_treeWidget->setColumnCount(5);
	m_treeWidget->setHeaderLabels(QStringList() << "Reader" << "Hardware" << "Software" << "Action" << "State" );
	//End by yingwei tseng for modifying GUI style, 2010/08/30
	m_treeWidget->setColumnPercentWidth(m_ReaderColumn, 35);
	m_treeWidget->setColumnPercentWidth(m_HardwareColumn, 19);
	m_treeWidget->setColumnPercentWidth(m_SoftwareColumn, 19);
	m_treeWidget->setColumnPercentWidth(m_ActionColumn, 10);
	//Del by yingwei tseng for modifying GUI style, 2010/08/30
#if 0	
	m_treeWidget->setColumnPercentWidth(m_HeartBeatColumn, 3);
#endif
	//End by yingwei tseng for modifying GUI style, 2010/08/30
	m_treeWidget->setColumnPercentWidth(m_StateColumn, 14);
	m_treeWidget->setIconSize(QSize(30,30));

	/* Set up the used fonts */
	m_readerIdFont.setFamily("Tahoma");
	m_readerIdFont.setPointSize(11);
	m_readerIdFont.setBold(true);

	m_readerVersionFont.setFamily("Tahoma");
	m_readerVersionFont.setBold(true);
	m_readerVersionFont.setPointSize(8);

	m_readerStateFont.setFamily("Tahoma");
	m_readerStateFont.setBold(true);
	m_readerStateFont.setPointSize(10);

	m_readerInfoFont.setFamily("Tahoma");
	m_readerInfoFont.setBold(true);
	m_readerInfoFont.setPointSize(10);

	m_readerInfoDataFont.setFamily("Tahoma");
	m_readerInfoDataFont.setBold(false);
	m_readerInfoDataFont.setPointSize(8);

	m_tagIdFont.setFamily("Tahoma");
	m_tagIdFont.setBold(true);
	m_tagIdFont.setPointSize(9);

	m_tagInfoDataFont.setFamily("Tahoma");
	m_tagInfoDataFont.setBold(false);
	m_tagInfoDataFont.setPointSize(8);

	/* Create context menus and actions */
	m_tagContext = new QMenu("Tag Context Menu", m_treeWidget);
	//Del by yingwei tseng for hiding AMS code, 2010/03/10
	#if 0
	m_tagContext->addAction(m_tagContext_Settings =	new QAction(QIcon(QString::fromUtf8(":/button icons/settingsIcon")), "Tag Associations", m_tagContext));
	QObject::connect(m_tagContext_Settings, SIGNAL(triggered (bool)), this, SLOT(contextMenuClicked()));
	#endif
	//End by yingwei tseng for hiding AMS code, 2010/03/10
	m_tagContext->addAction(m_tagContext_AdvancedSettings = new QAction(QIcon(QString::fromUtf8(":/button icons/advancedSettingsIcon")), "Advanced Tag Settings", m_tagContext));
	QObject::connect(m_tagContext_AdvancedSettings, SIGNAL(triggered (bool)), this, SLOT(contextMenuClicked()));

	m_readerContext = new QMenu("Reader Context Menu", m_treeWidget);
	//Mod by yingwei tseng for hiding AMS code, 2010/03/10
	//m_readerContext->addAction(m_readerContext_AdvancedSettings = new QAction(QIcon(QString::fromUtf8(":/button icons/advancedSettingsIcon")), "Reader Settings and Diagnostics", m_readerContext));
	m_readerContext->addAction(m_readerContext_AdvancedSettings = new QAction(QIcon(QString::fromUtf8(":/button icons/advancedSettingsIcon")), "Reader Settings", m_readerContext));
	//End by yingwei tseng for hiding AMS code, 2010/03/10
	QObject::connect(m_readerContext_AdvancedSettings, SIGNAL(triggered (bool)), this, SLOT(contextMenuClicked()));
	//Mod by yingwei tseng for hiding AMS code, 2010/03/10
	#if 0
	m_readerContext->addAction(m_readerContext_RegMap = new QAction(QIcon(QString::fromUtf8(":/button icons/advancedSettingsIcon")), "Register Map", m_readerContext));
	QObject::connect(m_readerContext_RegMap, SIGNAL(triggered (bool)), this, SLOT(contextMenuClicked()));
	#endif
	//End by yingwei tseng for hiding AMS code, 2010/03/10

	/* Create timer for cyclic checking for old tag entries */
	m_guiUpdateTimer = new QTimer();
	m_guiUpdateTimer->setInterval(250);
	m_guiUpdateTimer->setSingleShot(false);
	QObject::connect(m_guiUpdateTimer, SIGNAL(timeout()), this, SLOT(cyclicGuiUpdate()));

	m_contextItem = 0;

	//Add by yingwei tseng for print tagid and read rate to readerInfo.txt, 2010/06/21
//	m_readerInfoFile = new QFile(QString(QCoreApplication::applicationDirPath()
//    						+ "/readerInfo.txt"));	
//    m_readerInfoFile->open(QIODevice::WriteOnly);
	//End by yingwei tseng for print tagid and read rate to readerInfo.txt, 2010/06/21	
}

/*!
 * @brief	Destructor
 */
CTagViewManager::~CTagViewManager()
{
	/* Stop and delete the timer */
	m_guiUpdateTimer->stop();
	m_guiUpdateTimer->deleteLater();
//	m_readerInfoFile->close();//Add by yingwei tseng for print tagid and read rate to readerInfo.txt, 2010/06/21 
}

/*!
 * @brief 	Function to set up the behavior of the visualization
 */
void CTagViewManager::setUp(bool showAlias, bool useTimeToLive,  uint msecsToShowInactive, uint msecsToShowOutOfRange, uint msecsToDelete)
{
	/* If time to live is switched off, show all tags active */
	if(m_useTimeToLive && !useTimeToLive){
		foreach(QString readerId, m_readerEntries.keys()){
			showAllTagsActive(readerId);
		}
	}
	m_useTimeToLive = useTimeToLive;


	/* Save the given settings */
	m_showAlias = showAlias;
	m_msecsToShowInactive = msecsToShowInactive;
	m_msecsToShowOutOfRange = msecsToShowOutOfRange;
	m_msecsToDelete = msecsToDelete;

	changeTagNames();
}

/*!
 * @brief 	Function to add a new reader to the list
 */
void CTagViewManager::addReader(QString readerId, QString hardwareRev, QString softwareRev, QString action, QString state, bool passive, bool showRSSI, uchar rssiChildCount, QStringList rssiChildNames)
{
	trc(2, "The reader >>" + readerId + "<< was added");
	/* If the reader was delete but we have still the entry in the treewidget, take this */
	if(m_readerEntriesToDelete.contains(readerId)){

		/* Move the reader to the active map */
		m_readerEntries.insert(readerId, m_readerEntriesToDelete.value(readerId));

		/* Save the information of this reader */
		SReaderInfo info;
		info.passive = passive;
		info.showRSSI = showRSSI;
		info.rssiChildCount = rssiChildCount;
		info.rssiChildNames = rssiChildNames;
		info.readCounter = 0;
		m_readerInfo.insert(readerId, info);

		/* Set the new state of the reader */
		readerChangedState(readerId, state);

		/* Remove the reader from the inactive map */
		m_readerEntriesToDelete.remove(readerId);

		return;
	}

	/* Create new item for the reader */
	//Mod by yingwei tseng for modifying GUI style, 2010/08/30
	//QTreeWidgetItem* readerItem = new QTreeWidgetItem(QStringList() << readerId << hardwareRev << softwareRev << action << "" << state );
	QTreeWidgetItem* readerItem = new QTreeWidgetItem(QStringList() << readerId << hardwareRev << softwareRev << action << state );
    //End by yingwei tseng for modifying GUI style, 2010/08/30
	m_treeWidget->addTopLevelItem(readerItem);

	/* Set up font of the columns */
	readerItem->setFont(m_ReaderColumn, m_readerIdFont);
	readerItem->setFont(m_HardwareColumn, m_readerVersionFont);
	readerItem->setFont(m_SoftwareColumn, m_readerVersionFont);
	readerItem->setFont(m_ActionColumn, m_readerVersionFont);
    //Del by yingwei tseng for modifying GUI style, 2010/08/30	
#if 0  	
	readerItem->setFont(m_HeartBeatColumn, m_readerVersionFont);
#endif
    //End by yingwei tseng for modifying GUI style, 2010/08/30	
	readerItem->setFont(m_StateColumn, m_readerStateFont);

	/* Set up foreground of the columns */
	readerItem->setForeground(m_ReaderColumn, QBrush( QColor( Qt::white ) ));
	readerItem->setForeground(m_HardwareColumn, QBrush( QColor( Qt::white ) ));
	readerItem->setForeground(m_SoftwareColumn, QBrush( QColor( Qt::white ) ));
	readerItem->setForeground(m_ActionColumn, QBrush( QColor( Qt::white ) ));
    //Del by yingwei tseng for modifying GUI style, 2010/08/30	
#if 0    
	readerItem->setForeground(m_HeartBeatColumn, QBrush( QColor( Qt::white ) ));
#endif
	//End by yingwei tseng for modifying GUI style, 2010/08/30
	if(state == "Online")
		readerItem->setForeground(m_StateColumn, QBrush( QColor( Qt::green ) ));
	else
		readerItem->setForeground(m_StateColumn, QBrush( QColor( Qt::red ) ));

	/* Set up foreground of the columns */
	readerItem->setBackground(m_ReaderColumn, QBrush( QColor( Qt::darkGray ) ));
	readerItem->setBackground(m_HardwareColumn, QBrush( QColor( Qt::darkGray ) ));
	readerItem->setBackground(m_SoftwareColumn, QBrush( QColor( Qt::darkGray ) ));
	readerItem->setBackground(m_ActionColumn, QBrush( QColor( Qt::darkGray ) ));
//Del by yingwei tseng for modifying GUI style, 2010/08/30	
#if 0    	
	readerItem->setBackground(m_HeartBeatColumn, QBrush( QColor( Qt::darkGray ) ));
#endif
//End by yingwei tseng for modifying GUI style, 2010/08/30  
	readerItem->setBackground(m_StateColumn, QBrush( QColor( Qt::darkGray ) ));

	/* Set the flags for the new item */
	readerItem->setFlags(Qt::ItemIsEnabled);

	/* Get the right picture for the reader */
	if(passive)
		readerItem->setIcon(m_ReaderColumn, QIcon(":/tree widget icons/passiveReader"));
	else
		readerItem->setIcon(m_ReaderColumn, QIcon(":/tree widget icons/activeReader"));


	/* Insert the new reader object into the map */
	m_readerEntries.insert(readerId, readerItem);

	if(m_readerEntries.count() == 1) // first reader in the list
		m_treeWidget->setCurrentItem(readerItem);
	/* Save the information of this reader */
	SReaderInfo info;
	info.passive = passive;
	info.showRSSI = showRSSI;
	info.rssiChildCount = rssiChildCount;
	info.rssiChildNames = rssiChildNames;
	info.readCounter = 0;
	m_readerInfo.insert(readerId, info);

	/* Create reader information */
	QTreeWidgetItem* readerInfoItem = new QTreeWidgetItem(QStringList() << "Reader Information");
	readerInfoItem->setIcon(0, QIcon(":/tree widget icons/information"));
	readerInfoItem->setFirstColumnSpanned(true);
	readerInfoItem->setFont(0, m_readerInfoFont);
	readerInfoItem->setFlags(Qt::ItemIsEnabled);
	readerItem->addChild(readerInfoItem);

	QTreeWidgetItem* differentTagItem = new QTreeWidgetItem(QStringList() << "Different Tags" << "0");
	differentTagItem->setFont(0, m_readerInfoDataFont);
	differentTagItem->setFont(1, m_readerInfoDataFont);
	readerInfoItem->addChild(differentTagItem);

	QTreeWidgetItem* readerCountItem = new QTreeWidgetItem(QStringList() << "Read Count" << QString::number(info.readCounter));
	readerCountItem->setFont(0, m_readerInfoDataFont);
	readerCountItem->setFont(1, m_readerInfoDataFont);
	readerInfoItem->addChild(readerCountItem);

	QTreeWidgetItem* readerSpeedItem = new QTreeWidgetItem(QStringList() << "Reads per Second" << "0");
	readerSpeedItem->setFont(0, m_readerInfoDataFont);
	readerSpeedItem->setFont(1, m_readerInfoDataFont);
	readerInfoItem->addChild(readerSpeedItem);

	readerItem->setExpanded(true);
}

/*!
 * @brief	Function sets the reader to removed. The reader stays in the list, but is no more used. The reader entry is deleted on clearOfflineReader.
 */
void CTagViewManager::removeReader(QString readerId)
{
	if( !m_readerInfo.contains(readerId) || !m_readerEntries.contains(readerId) ){
		return;
	}

	if(m_activeReader.contains(readerId))
		m_activeReader.removeOne(readerId);

	/* Remove all tags of this reader */
	clearTagsOfReader(readerId);
	m_readerTagsInfo.remove(readerId);

	/* Remove the entry out of the map with the active reader */
	QTreeWidgetItem* item = m_readerEntries.value(readerId);
	item->setExpanded(false);
	m_readerEntries.remove(readerId);
	m_readerInfo.remove(readerId);

	/* Put the reader into the map with the inactive reader */
	m_readerEntriesToDelete.insert(readerId, item);
}

/*!
 * @brief	Function is called if reader changes state
 */
void CTagViewManager::readerChangedState(QString readerId, QString state)
{
	if( !m_readerInfo.contains(readerId) || !m_readerEntries.contains(readerId) ){
		//fatal("Got readerId that does not exist: " + readerId);
		return;
	}

	/* Get the treewidget entry */
	QTreeWidgetItem* item = m_readerEntries.value(readerId);
	if(item == 0)
	{
		trc(1, "Do not know this reader ID");
		return;
	}

	/* Set the new text in the right color */
	item->setText(m_StateColumn, state);
	if(state == "Online")
		item->setForeground(m_StateColumn, QBrush( QColor( Qt::green ) ));
	else if(state == "Offline"){
		item->setForeground(m_StateColumn, QBrush( QColor( Qt::red ) ));
		clearTagsOfReader(readerId);
		removeReader(readerId);
	}
	else{
		item->setForeground(m_StateColumn, QBrush( QColor( Qt::red ) ));
		clearTagsOfReader(readerId);
	}

}
void CTagViewManager::changeHardFirm(QString readerId, QString hardwareRev, QString softwareRev)
{
	if( !m_readerEntries.contains(readerId) ){
		return;
	}

	/* Get the treewidget entry */
	QTreeWidgetItem* item = m_readerEntries.value(readerId);

	item->setText(1,hardwareRev);
	item->setText(2,softwareRev);
}

/*!
 * @brief	Function is called if reader changes action
 */
void CTagViewManager::readerChangedAction(QString readerId, QString action)
{
	if( !m_readerInfo.contains(readerId) || !m_readerEntries.contains(readerId) ){
		return;
	}

	/* Get the treewidget entry */
	QTreeWidgetItem* item = m_readerEntries.value(readerId);
	if(item == 0)
	{
		trc(1, "Do not know this reader ID");
		return;
	}

	/* Set the new text */
	item->setText(m_ActionColumn, action);
}

/*!
 * @brief	Function is called if reader changes action
 */
void CTagViewManager::readerSetRSSI(QString readerId, bool on, uchar rssiChildCount, QStringList rssiChildNames)
{
	if( !m_readerInfo.contains(readerId) || !m_readerEntries.contains(readerId) ){
		fatal("Got readerId that does not exist: " + readerId);
		return;
	}

	/* Get the reader info of this reader */
	SReaderInfo& readerInfo = m_readerInfo[readerId];

	/* If rssi is already turned on, skip */
	if(readerInfo.showRSSI == on)
		return;

	/* Store the rssi description */
	readerInfo.showRSSI = on;
	readerInfo.rssiChildCount = rssiChildCount;
	readerInfo.rssiChildNames = rssiChildNames;

	/* Add or remove the rssi entry from each tag that is already in the list */
	QMap<QString, STagInfo> &tagsOfReader = m_readerTagsInfo[readerId];
	foreach(QString tagId, tagsOfReader.keys())
	{
		STagInfo &s = tagsOfReader[tagId];
		if(s.widgetEntry == 0)
		{
			trc(1, "Have no pointer to the widgetEntry");
			continue;
		}
		if(on)
			addRSSIInfo(s.widgetEntry, rssiChildCount, rssiChildNames);
		else
			removeRSSIInfo(s.widgetEntry);
	}

}

/*!
 * @brief	Function removes all entries of offline reader
 */
void CTagViewManager::clearOfflineReader()
{
	/* Remove and delete every inactive reader from the treewidget */
	foreach(QString readerId, m_readerEntriesToDelete.keys())
	{
		QTreeWidgetItem* item = m_readerEntriesToDelete.value(readerId);
		m_readerEntriesToDelete.remove(readerId);
		if(item == 0)
			continue;
		int idxTopLevel = m_treeWidget->indexOfTopLevelItem(item);
		if (idxTopLevel != -1)
			m_treeWidget->takeTopLevelItem(idxTopLevel);
		delete item;
	}

	/* Clear the map with the inactive reader */
	m_readerEntriesToDelete.clear();
}

/*!
 * @brief	Function to reset the reader info of each reader
 */
void CTagViewManager::resetReaderInfo()
{
	foreach(QString readerId, m_readerEntries.keys()){
		resetReaderInfo(readerId);
	}
}

/*!
 * @brief	Function to reset the reader info of the given reader
 */
void CTagViewManager::resetReaderInfo(QString readerId)
{
	if( !m_readerInfo.contains(readerId) || !m_readerEntries.contains(readerId) ){
		return;
	}

	/* Reset the reader info */
	SReaderInfo& readerInfo = m_readerInfo[readerId];
	readerInfo.readCounter = 0;

	/* Update the visualization of the reader info */
	updateReaderInfo(readerId);
}

/*!
 * @brief	Function to reset the tag info of the tags of the given reader
 */
void CTagViewManager::resetTagInfo(QString readerId)
{
	if( !m_readerInfo.contains(readerId) || !m_readerEntries.contains(readerId) ){
		return;
	}

	/* Get the tags of the reader */
	QMap<QString, STagInfo> &tagsOfReader = m_readerTagsInfo[readerId];
	foreach(QString tagId, tagsOfReader.keys())
	{
		STagInfo &s = tagsOfReader[tagId];

		/* Reset read counter */
		s.readCounter = 0;
		s.rssi.clear();

		/* Update the visualization of the tag info */
		updateTagInfo(readerId, tagId);
	}
}

/*!
 * @brief	Function to clear all tags of each reader
 */
void CTagViewManager::clearTags()
{
	/* Remove and Delete every tag of every reader from the treewidget */
	foreach(QString readerId, m_readerTagsInfo.keys())
	{
		clearTagsOfReader(readerId);
		resetReaderInfo(readerId);
	}

	/* Clear the map with the tag entries */
	m_tagEntries.clear();

	m_overallDifferentTags.clear();

	/* Emit new tag counts */
	emit newTagCount(m_tagEntries.size());
	emit newDifferentTagCount(QSet<QString>::fromList(m_tagEntries.values()).size());
	emit newOverallDifferentTagCount(m_overallDifferentTags.size());
}


/*!
 * @brief	Function to clear the tags of the specified reader
 */
void CTagViewManager::clearTagsOfReader(QString readerId)
{
	if( !m_readerInfo.contains(readerId) || !m_readerEntries.contains(readerId) ){
		return;
	}

	/* Remove the tags */
	m_readerTagsInfo.remove(readerId);
	QTreeWidgetItem* readerItem = m_readerEntries.value(readerId);
	if(readerItem == 0)
	{
		trc(1, "Do not know this reader ID");
		return;
	}

	int count = readerItem->childCount();
	for(int i = 1; i < count; i++)
	{
		removeTagEntry(readerItem->child(1));
	}

	/* Emit new tag count */
	emit newTagCount(m_tagEntries.size());
	emit newDifferentTagCount(QSet<QString>::fromList(m_tagEntries.values()).size());

}

/*!
 * @brief	Function to clear all information of every reader
 */
void CTagViewManager::clearAllReaderTagInfos()
{
	foreach(QString readerId, m_readerEntries.keys())
	{
		resetReaderInfo(readerId);
		resetTagInfo(readerId);
	}
}

/*!
 * @brief 	Function to select only one single reader and only show this reader in the view
 */
void CTagViewManager::selectSingleReader(QString readerId)
{
	foreach(QString rId, m_readerEntries.keys())
	{
		QTreeWidgetItem* item = m_readerEntries.value(rId);
		if(rId != readerId)
			item->setHidden(true);
		else
			item->setHidden(false);
	}
	foreach(QString rId, m_readerEntriesToDelete.keys())
	{
		QTreeWidgetItem* item = m_readerEntriesToDelete.value(rId);
		item->setHidden(true);
	}

}

/*!
 * @brief 	Function to show all reader in the view
 */
void CTagViewManager::showAllReader()
{
	foreach(QString rId, m_readerEntries.keys())
	{
		QTreeWidgetItem* item = m_readerEntries.value(rId);
		item->setHidden(false);
	}
	foreach(QString rId, m_readerEntriesToDelete.keys())
	{
		QTreeWidgetItem* item = m_readerEntriesToDelete.value(rId);
		item->setHidden(false);
	}

}

/*!
 * @brief 	Function to set reader as active, only active reader are checked for old tags
 */
void CTagViewManager::setActive(QString readerId)
{
	m_activeReader.clear();

	if(readerId.isNull()){
		foreach(readerId, m_readerEntries.keys()){
			m_activeReader << readerId;
		}
	}
	else
	{
		if( !m_readerInfo.contains(readerId) || !m_readerEntries.contains(readerId) ){
			return;
		}
		m_activeReader << readerId;
	}
}

/*!
 * @brief 	Function to remove reader from the active list
 */
void CTagViewManager::setUnactive(QString readerId)
{
	m_activeReader.removeAll(readerId);
}




/*******************************************************************************************************************************/
// Interrupts from Reader


/*!
 * @brief 	Heart beat interrupt from the reader
 */
void CTagViewManager::heartBeat(QString readerId)
{
//Del by yingwei tseng for modifying GUI style, 2010/08/30
#if 0
	if( !m_readerInfo.contains(readerId) || !m_readerEntries.contains(readerId) ){
		fatal("Got readerId that does not exist: " + readerId);
		return;
	}

	/* Get view entry */
	QTreeWidgetItem* readerItem = m_readerEntries.value(readerId);
	if(readerItem == 0)
		return;

	/* Set heart beat sign */
	QString heart;
	if(readerItem->text(m_HeartBeatColumn) == " | ")
		heart = " / ";
	else if(readerItem->text(m_HeartBeatColumn) == " / ")
		heart = " - ";
	else if(readerItem->text(m_HeartBeatColumn) == " - ")
		heart = " \\ ";
	else
		heart = " | ";
	readerItem->setText(m_HeartBeatColumn, heart);
#endif	
//End by yingwei tseng for modifying GUI style, 2010/08/30
}

/*!
 * @brief 	Cyclic inventory result interrupt from the reader
 */
void CTagViewManager::cyclicInventroyResult(QString readerId, QString tagId)
{
	trc(3, "Got from the reader: >>" + readerId + "<< the EPC: " + tagId);

	if( !m_readerInfo.contains(readerId) || !m_readerEntries.contains(readerId) ){
		fatal("Got readerId that does not exist: " + readerId);
		return;
	}

	/* Get the item of the reader */
	QTreeWidgetItem* readerItem = m_readerEntries.value(readerId);
	if(readerItem == 0)
	{
		trc(1, "Do not know this reader ID");
		return;
	}

	/* Get the current tags of the reader */
	QMap<QString, STagInfo> &tagsOfReader = m_readerTagsInfo[readerId];

	STagInfo tagInfo;
	tagInfo.readCounter = 0;

	QTreeWidgetItem* tagItem;

	/* If the tag was already read, get the information and the treewidget entry */
	if(tagsOfReader.contains(tagId)){
		tagInfo = tagsOfReader.value(tagId);
		tagItem = tagsOfReader.value(tagId).widgetEntry;
	}
	/* If the tag was not read before */
	else
	{
		tagItem = createTagEntry(readerId, tagId);
		if(tagItem == 0)
			return;

		m_overallDifferentTags.insert(tagId);
		emit newTagCount(m_tagEntries.size());
		emit newDifferentTagCount(QSet<QString>::fromList(m_tagEntries.values()).size());
		emit newOverallDifferentTagCount(m_overallDifferentTags.size());

	}


	/* Change and show the state of the tag to ACTIVE */
	changeTagState(readerId, tagId, STATE_ACTIVE);

	SReaderInfo &readerInfo = m_readerInfo[readerId];
	readerInfo.readCounter++;
	emit countTotalTags(readerId);
	
	updateReaderInfo(readerId);

	/* Save the tag information */
	tagInfo.readCounter = tagInfo.readCounter + 1;
	tagInfo.lastTimeSeen = QTime::currentTime();
	tagInfo.widgetEntry = tagItem;

	/* Overwrite the entry of the tag */
	tagsOfReader.insert(tagId, tagInfo);
	updateTagInfo(readerId, tagId);

	readerItem->setExpanded(true);
}

/*!
 * @brief 	Cyclic inventory result interrupt containing the rssi value of the tag from the reader
 */
void CTagViewManager::cyclicInventoryRSSIResult(QString readerId, QString tagId, QByteArray rssi)
{
	if( !m_readerInfo.contains(readerId) || !m_readerEntries.contains(readerId) ){
		fatal("Got readerId that does not exist: " + readerId);
		return;
	}

	/* Get the current tags of the reader */
	QMap<QString, STagInfo> &tagsOfReader = m_readerTagsInfo[readerId];

	if(!tagsOfReader.contains(tagId)){
		fatal("Got tagId that does not exist: " + tagId);
		return;
	}

	/* Store the rssi value in the tag info and update view */
	STagInfo &tagInfo = tagsOfReader[tagId];
	tagInfo.rssi = rssi;
	updateTagInfo(readerId, tagId);
}

/*!
 * @brief	Function that creates a new tag entry
 */
QTreeWidgetItem* CTagViewManager::createTagEntry (QString readerId, QString tagId)
{
	if( !m_readerInfo.contains(readerId) || !m_readerEntries.contains(readerId) ){
		fatal("Got readerId that does not exist: " + readerId);
		return 0;
	}
	if (tagId.size() == 0) tagId = QString("<zero>");

	// get the item of the reader
	QTreeWidgetItem* readerItem = m_readerEntries.value(readerId);
	if(readerItem == 0)
	{
		trc(1, "Do not know this reader ID");
		return 0;
	}

	// create new treewidget item
	QTreeWidgetItem* tagItem = new QTreeWidgetItem();
	readerItem->addChild(tagItem);
	if(m_readerInfo.value(readerId).passive)
		tagItem->setIcon(0, QIcon(":/tree widget icons/passiveTag"));
	else
		tagItem->setIcon(0, QIcon(":/tree widget icons/activeTag"));
	tagItem->setFlags(Qt::ItemIsEnabled);

	QTreeWidgetItem* readCountItem = new QTreeWidgetItem();
	tagItem->addChild(readCountItem);
	readCountItem->setText(0, "Read Count:");
	readCountItem->setText(2, ".");

	tagItem->setExpanded(true);

	// set up font of the entry
	tagItem->setFont(0, m_tagIdFont);

	readCountItem->setFont(0, m_tagInfoDataFont);
	readCountItem->setFont(1, m_tagInfoDataFont);
	readCountItem->setFont(2, m_tagInfoDataFont);


	if(m_readerInfo.value(readerId).showRSSI)
		addRSSIInfo(tagItem, m_readerInfo.value(readerId).rssiChildCount, m_readerInfo.value(readerId).rssiChildNames);


	// save the new treewidget entry
	m_tagEntries.insert(tagItem, tagId);


	// set tag name
	TagActionInfo t;
	// if the sowAlias setting is activated get the config
	if(m_showAlias && m_dataHandler->getConfig(tagId, t)){
		// insert the alias name and some more information
		tagItem->setFirstColumnSpanned(false);
		tagItem->setText(0, t.aliasName);
		if(t.performAction){
			if(t.type == StartApp)
				tagItem->setText(1, "Starts the Application: " + t.appPath);
			else
				tagItem->setText(1, "Shows the Picture: " + t.picPath);
		}
	}
	else{
		// insert the epc of the tag
		tagItem->setFirstColumnSpanned(true);
		tagItem->setText(0, tagId);
	}


	return tagItem;
}

/*!
 * @brief	Function that removes a tag entry
 */
void CTagViewManager::removeTagEntry(QTreeWidgetItem* tagItem)
{
	// delete progress bar of rssi value
	if(tagItem->child(1) != 0){
		removeRSSIInfo(tagItem);
	}

	while(tagItem->childCount() != 0)
		delete tagItem->takeChild(0);

	QTreeWidgetItem* readerItem = tagItem->parent();
	if(readerItem != 0)
	{
		trc(1, "tag removed");
		readerItem->removeChild(tagItem);
	}

	m_tagEntries.remove(tagItem);

	delete tagItem;

}

/*!
 * @brief	Function that adds a rssi informartion to a tag entry
 */
void CTagViewManager::addRSSIInfo(QTreeWidgetItem* tagItem, uchar rssiChildCount, QStringList rssiChildNames)
{
	QTreeWidgetItem* rssiItem = new QTreeWidgetItem();
	tagItem->addChild(rssiItem);
	rssiItem->setText(0, "RSSI:");
	QProgressBar* pb = new QProgressBar(m_treeWidget);
	m_treeWidget->setItemWidget(rssiItem, 1, pb);
	pb->setValue(0);
	pb->setMaximum(100);
	pb->setMaximumHeight(15);
	rssiItem->setFont(0, m_tagInfoDataFont);

	rssiItem->setExpanded(false);

	for(int i = 0; i < rssiChildCount; i++){
		QTreeWidgetItem* rssi_Child_Item = new QTreeWidgetItem();
		rssiItem->addChild(rssi_Child_Item);
		rssi_Child_Item->setText(0, rssiChildNames.at(i));
		QProgressBar* pbI = new QProgressBar(m_treeWidget);
		m_treeWidget->setItemWidget(rssi_Child_Item, 1, pbI);
		pbI->setValue(0);
		pbI->setMaximum(32);
		pbI->setMaximumHeight(15);
		pbI->setFormat("%v dB");
		rssi_Child_Item->setFont(0, m_tagInfoDataFont);
	}
}

/*!
 * @brief	Function that removes the rssi informartion from a tag entry
 */
void CTagViewManager::removeRSSIInfo(QTreeWidgetItem* tagItem)
{
	if(tagItem->childCount() < 2)
		return;

	// delete progress bar of rssi value
	if(tagItem->child(1) != 0){
		QProgressBar* pb = qobject_cast<QProgressBar*>(m_treeWidget->itemWidget(tagItem->child(1), 1));
		if(pb != 0)
			delete pb;

		for(int i = 0; i < tagItem->child(1)->childCount(); i++)
		{
			QProgressBar* pbI = qobject_cast<QProgressBar*>(m_treeWidget->itemWidget(tagItem->child(1)->child(i), 1));
			if(pbI != 0)
				delete pbI;
		}
	}

	while(tagItem->child(1)->childCount() != 0)
		delete tagItem->child(1)->takeChild(0);
	delete tagItem->takeChild(1);

	return;
}


void CTagViewManager::readerInfoWrite() {
	static QMutex mutex;
	mutex.lock();

	m_readerInfoFile = new QFile(QString(QCoreApplication::applicationDirPath() + "/readerInfo.txt"));	
	m_readerInfoFile->open(QIODevice::WriteOnly);

	foreach(QString rId, m_readerEntries.keys())
	{
		int eachCount =0;
		int totalCount = 0;

		QMap<QString, STagInfo> tagsOfReader = m_readerTagsInfo.value(rId);
		foreach(QString tagId, tagsOfReader.keys())
		{
			eachCount = tagsOfReader.value(tagId).readCounter;
			totalCount += eachCount;
			m_readerInfoFile->write(tagId.toAscii() + "\t\t" + QByteArray::number(eachCount, 10) + "\r\n");
		}
		m_readerInfoFile->write("\r\nTag Numbers = " + QByteArray::number(tagsOfReader.size(), 10) + "\r\n");
		m_readerInfoFile->write("Total Counts = " + QByteArray::number(totalCount, 10) + "\r\n");
		m_readerInfoFile->write("Reading Rate = " + QByteArray::number(m_readRateCalc->getReadRate(rId), 'g', 6) + "\r\n");
		emit countTotalTags(rId);
	}

	m_readerInfoFile->close();

	mutex.unlock();
}


/*!
 * @brief	Function that updates the tag info in the view according to the stored data
 */
void CTagViewManager::updateTagInfo(QString readerId, QString tagId)
{
	if( !m_readerInfo.contains(readerId) || !m_readerEntries.contains(readerId) ){
		fatal("Got readerId that does not exist: " + readerId);
		return;
	}

	// get the current tags of the reader
	QMap<QString, STagInfo> &tagsOfReader = m_readerTagsInfo[readerId];

	if(!tagsOfReader.contains(tagId)){
		fatal("Got tagId that does not exist: " + tagId);
		return;
	}

	STagInfo &tagInfo = tagsOfReader[tagId];

	QTreeWidgetItem* tagItem = tagInfo.widgetEntry;

	// Update read count
	QTreeWidgetItem* readCountItem = tagItem->child(0);
	if(readCountItem == 0){
		fatal("Invalid pointer to readCountItem of tag: " + tagId);
		return;
	}

	readCountItem->setText(1, QString::number(tagInfo.readCounter));

	QString runner;
	for(int i = 0; i < tagInfo.readCounter%10; i++)
		runner += " ";
	runner += ".";
	if(runner.size() >= 10)
		runner = ".";
	readCountItem->setText(2, runner);


	if(m_readerInfo.value(readerId).showRSSI)
	{
		// Update rssi value
		QTreeWidgetItem* rssiItem = tagItem->child(1);
		if(rssiItem == 0 && m_readerInfo.value(readerId).showRSSI){
			fatal("Invalid pointer to readCountItem of tag: " + tagId);
			return;
		}

		uchar val = (tagInfo.rssi.size() < 1) ? 0 : tagInfo.rssi.at(0);
		QProgressBar* pb = qobject_cast<QProgressBar*>(m_treeWidget->itemWidget(rssiItem, 1));
		if(pb == 0)
			return;
		pb->setValue( (val > pb->maximum()) ? pb->maximum() : val);

		for(int i = 0; i < rssiItem->childCount(); i++){
			if(tagItem->child(1)->child(i) == 0)
				continue;
			QProgressBar* pbI = qobject_cast<QProgressBar*>(m_treeWidget->itemWidget(tagItem->child(1)->child(i), 1));

			uchar val = (tagInfo.rssi.size() < i+2) ? 0 : tagInfo.rssi.at(i+1);
			pbI->setValue( (val > pbI->maximum()) ? pbI->maximum() : val);
		}
	}

}

/*!
 * @brief	Function that updates the reader info in the view according to the stored data
 */
void CTagViewManager::updateReaderInfo(QString readerId)
{
	if( !m_readerInfo.contains(readerId) || !m_readerEntries.contains(readerId) ){
		fatal("Got readerId that does not exist: " + readerId);
		return;
	}

	SReaderInfo& readerInfo = m_readerInfo[readerId];

	QTreeWidgetItem* readerItem = m_readerEntries.value(readerId);
	QTreeWidgetItem* readerInfoItem = readerItem->child(0);

	QTreeWidgetItem* differentTagItem = readerInfoItem->child(0);
	differentTagItem->setText(1, QString::number(m_readerTagsInfo.value(readerId).size()));

	QTreeWidgetItem* readerCountItem = readerInfoItem->child(1);
	readerCountItem->setText(1, QString::number(readerInfo.readCounter));

	QTreeWidgetItem* readerSpeedItem = readerInfoItem->child(2);
	readerSpeedItem->setText(1, QString::number(m_readRateCalc->getReadRate(readerId)));
	//trc(0, "speed" + QString::number(m_readRateCalc->getReadRate(readerId)));	
}


/**********************************************************************************************************************/
// Cyclic gui value refresh

/*!
 * @brief	Function that changes the visualization of the tag names. Either the epc or the alias name is shown.
 */
void CTagViewManager::changeTagNames (  )
{
	// For every reader
	foreach(QString readerId, m_readerTagsInfo.keys())
	{
		QMap<QString, STagInfo> tagsOfReader = m_readerTagsInfo.value(readerId);
		// and every tag of the reader
		foreach(QString tagId, tagsOfReader.keys())
		{
			// get the information of this tag
			STagInfo s = tagsOfReader.value(tagId);
			if(s.widgetEntry == 0)
			{
				trc(1, "Have no pointer to the widgetEntry");
				continue;
			}

			// insert the text for this tag
			TagActionInfo t;
			if(m_showAlias && m_dataHandler->getConfig(tagId, t)){
				s.widgetEntry->setFirstColumnSpanned(false);
				s.widgetEntry->setText(0, t.aliasName);
				if(t.performAction){
					if(t.type == StartApp)
						s.widgetEntry->setText(1, "Starts the Application: " + t.appPath);
					else
						s.widgetEntry->setText(1, "Shows the Picture: " + t.picPath);
				}
			}
			else{
				s.widgetEntry->setFirstColumnSpanned(true);
				s.widgetEntry->setText(0, tagId);
			}
		}
	}
}

/*!
 * @brief	Function that starts the cyclic gui update
 */
void CTagViewManager::startGuiUpdate()
{
	if(!m_guiUpdateTimer->isActive())
		m_guiUpdateTimer->start();
}

/*!
 * @brief	Function to stop the gui update
 */
void CTagViewManager::stopGuiUpdate()
{
	m_guiUpdateTimer->stop();
	readerInfoWrite();
}

/*!
 * @brief	Slot is cyclic called to update the gui
 */
void CTagViewManager::cyclicGuiUpdate ( )
{
	foreach(QString readerId, m_activeReader)
	{
		if( !m_readerInfo.contains(readerId) || !m_readerEntries.contains(readerId) ){
			fatal("Got readerId that does not exist: " + readerId);
			continue;
		}

		updateReaderInfo(readerId);
		if(m_useTimeToLive)
			checkForOld(readerId);
	}

	m_treeWidget->repaint();

}


/*!
 * @brief	Function to check for old tag id entries
 */
void CTagViewManager::checkForOld(QString readerId)
{
	if( !m_readerInfo.contains(readerId) || !m_readerEntries.contains(readerId) ){
		fatal("Got readerId that does not exist: " + readerId);
		return;
	}

	QMap<QString, STagInfo> &tagsOfReader = m_readerTagsInfo[readerId];
	// and every tag of the reader
	foreach(QString tagId, tagsOfReader.keys())
	{
		// get the information of this tag
		STagInfo &s = tagsOfReader[tagId];

		// check how long the tag was not read with the settings
		if((uint)s.lastTimeSeen.msecsTo(QTime::currentTime()) > m_msecsToDelete){
			changeTagState(readerId, tagId, STATE_DELETE);
			emit oldTagEntryRemoved(readerId, tagId);
			continue;
		}
		if((uint)s.lastTimeSeen.msecsTo(QTime::currentTime()) > m_msecsToShowOutOfRange){
			changeTagState(readerId, tagId, STATE_OUTOFRANGE);
			continue;
		}
		else if((uint)s.lastTimeSeen.msecsTo(QTime::currentTime()) > m_msecsToShowInactive){
			changeTagState(readerId, tagId, STATE_INACTIVE);
			continue;
		}
		else{
			changeTagState(readerId, tagId, STATE_ACTIVE);
			continue;
		}
	}
}


/*!
 * @brief	Function to set the state of a tag and to visualize this at the view
 */
void CTagViewManager::changeTagState(QString readerId, QString tagId, TagState tagState)
{
	QMap<QString, STagInfo> &tagsOfReader = m_readerTagsInfo[readerId];

	if(!tagsOfReader.contains(tagId))
		return;

	STagInfo &s = tagsOfReader[tagId];
	if(s.widgetEntry == 0)
	{
		trc(1, "Have no pointer to the widgetEntry");
		return;
	}

	if(tagState != s.tagState)
	{
		switch(tagState){
		case STATE_ACTIVE:
			for(int i = 0; i < s.widgetEntry->columnCount(); i++){
				s.widgetEntry->setForeground(i, QBrush( QColor( Qt::black ) ));
				s.widgetEntry->setBackground(i, QBrush( QColor( Qt::transparent ) ));
			}

			s.tagState = tagState;

			return;

		case STATE_INACTIVE:
			for(int i = 0; i < s.widgetEntry->columnCount(); i++){
				s.widgetEntry->setBackground(i, QBrush( QColor( Qt::lightGray ) ));
			}

			s.tagState = tagState;
			s.rssi.clear();
			updateTagInfo(readerId, tagId);

			return;

		case STATE_OUTOFRANGE:
			for(int i = 0; i < s.widgetEntry->columnCount(); i++){
				s.widgetEntry->setForeground(i, QBrush( QColor( Qt::darkGray ) ));
			}

			s.tagState = tagState;

			return;

		case STATE_DELETE:
			removeTagEntry(s.widgetEntry);
			tagsOfReader.remove(tagId);

			newTagCount(m_tagEntries.size());
			newDifferentTagCount(QSet<QString>::fromList(m_tagEntries.values()).size());

			return;
		}

	}
}

/*!
 * @brief	Function that sets the state of each tag of the specified reader to active
 */
void CTagViewManager::showAllTagsActive(QString readerId)
{
	if(!m_readerTagsInfo.contains(readerId))
		return;

	QMap<QString, STagInfo> tagsOfReader = m_readerTagsInfo.value(readerId);
	foreach(QString tagId, tagsOfReader.keys())
	{
		changeTagState(readerId, tagId, STATE_ACTIVE);
	}
}

/*******************************************************************************************************************************
 * Context menus
 */

void CTagViewManager::showPopup(const QPoint & iPoint)
{
	// save the item, on which was clicked in a member
	m_contextItem = m_treeWidget->itemAt(iPoint);
	if ( m_contextItem == 0 )
		return;

	if(!m_tagEntries.keys().contains(m_contextItem) && !m_readerEntries.values().contains(m_contextItem))
	{
		m_contextItem = 0;
		return;
	}

	// if the item is a reader item, show the context menu for reader
	if(m_contextItem->parent() == 0)
		m_readerContext->exec(QPoint(m_treeWidget->mapToGlobal(iPoint).x(), m_treeWidget->mapToGlobal(iPoint).y()+20));
	// else show the context menu of the tags
	else
		m_tagContext->exec(QPoint(m_treeWidget->mapToGlobal(iPoint).x(), m_treeWidget->mapToGlobal(iPoint).y()+20));
}

void CTagViewManager::contextMenuClicked()
{
	// if there was no item saved, return
	if(m_contextItem == 0)
		return;

	// if the saved item is a reader item
	if(m_contextItem->parent() == 0)
	{
		// get the reader id
		QString readerId = m_readerEntries.key(m_contextItem);
		if(readerId.isNull())
			return;

		// emit the signal, that the advanced settings dialog is requested
		if((QAction*)sender() == m_readerContext_AdvancedSettings)
		{
			emit requestReaderAdvancedSettings(readerId);
		}
		// emit the signal, that the advanced settings dialog is requested
		if((QAction*)sender() == m_readerContext_RegMap)
		{
			emit requestReaderRegisterMap(readerId);
		}

	}

	// if the saved item is a tag item
	else if(m_tagEntries.contains(m_contextItem))
	{
		// get the epc of the tag
		QString epc = m_tagEntries.value(m_contextItem);
		if(epc.isNull())
			return;

		QString readerId = m_readerEntries.key(m_contextItem->parent());
		if(readerId.isNull())
			return;

		// emit the right signal, that a dialog is requested
		//Add by yingwei tseng for hiding AMS code, 2010/03/10
		#if 0
		if((QAction*)sender() == m_tagContext_Settings)
		{
			emit requestTagSettings(epc);
		}
		else 
		#endif
		//End by yingwei tseng for hiding AMS code, 2010/03/10		
		if((QAction*)sender() == m_tagContext_AdvancedSettings)
		{
			emit requestTagAdvancedSettings(readerId, epc);
		}
	}
}

void CTagViewManager::itemDoubleClicked ( QTreeWidgetItem * item )
{
	// if a tag item was double clicked, emit the signal that the normal settings dialog is requested
	if(item->parent() == 0)
		trc(1, "TopLevel Item pressed.");
	else 
	if(m_tagEntries.contains(m_contextItem))
	{
		QString epc = m_tagEntries.value(item);
		if(epc.isNull())
			return;
		emit requestTagSettings(epc);
	}
}

QString CTagViewManager::getCurrentReader()
{
	QTreeWidgetItem* currentItem = m_treeWidget->currentItem();

	if(currentItem == 0)
		return "";
	
	// if the saved item is a reader item
	if(currentItem->parent() == 0)
	{
		// get the reader id
		return m_readerEntries.key(currentItem);
	}

	// if the saved item is a tag item
	else if(m_tagEntries.contains(currentItem))
	{
		return m_readerEntries.key(currentItem->parent());
	}
	return "";
}

void CTagViewManager::currentItemChanged ( QTreeWidgetItem * currentItem, QTreeWidgetItem * previous )
{
	if(currentItem && currentItem->parent() != 0)
	{
		return;
	}
	emit currentReaderChanged(getCurrentReader());
}
