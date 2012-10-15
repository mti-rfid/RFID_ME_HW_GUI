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
 * @file	CReaderTool.cpp
 * @brief	Main class of the app
 */

#include "CReaderTool.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QSplashScreen>
#include <QBitmap>
#include "qextserialport.h"
#include "enumser.h" 
//Del by yingwei tseng for hiding AMS code, 2010/03/10
//#include "atlstr.h"
//End by yingwei tseng for hiding AMS code, 2010/03/10

extern const char* FW_VERSION;
extern const char* APPLICATION_NAME;
/*
 * @brief 	Constructor of the main class
 * The Constructor initializes all needed variables and one instance of every used dialog.
 */
CReaderTool::CReaderTool(QWidget *parent)
    : QMainWindow(parent)
	, QrfeTraceModule("Reader_Tool")
{
	m_updateThread = NULL;

	int alignment = Qt::AlignHCenter | Qt::AlignBottom;

	//Mod by yingwei tseng for hiding AMS code, 2010/03/10
	#if 0
	QPixmap logo(":/ams logos/amsStartupLogo");
	#endif
	QPixmap logo(":/ams logos/mtiRfidLogo");
	//End by yingwei tseng for hiding AMS code, 2010/03/10
	
	//clark 2011.12.15
	//logo.setMask(logo.createHeuristicMask());
	QSplashScreen *splash = new QSplashScreen(logo);
	splash->setMask(logo.mask());
	splash->setWindowOpacity(0);
	splash->show();
	for(double i = 0; i < 1; i += 0.05)
	{
		splash->setWindowOpacity(i);
		Sleep(50);
	}

	/* Init variables */
	m_scanActive = false;

	splash->showMessage(tr("Initialize GUI..."), alignment);
	/* Setup the ui */
	ui.setupUi(this);

	this->setWindowTitle(QString(APPLICATION_NAME));

	splash->showMessage(tr("Create Handlers..."), alignment);

	/* Create Action Handler */
	m_actionHandler = new CActionHandler(&m_dataHandler, this);

	/* Create Read Rate Calculator */
	m_readRateCalc = new CReadRateCalc(this);

	/* Create Tag Manager */
	m_tagManager = new CTagManager(this);

	m_amsComWrapper = new USBHIDWrapper(0x0);

	splash->showMessage(tr("Create Dialogs..."), alignment);

	/* Create Dialogs */
	m_aboutDialog = new QrfeAboutDialog(this);
	m_keyDialog = new QrfeKeyWindow(this);
	m_tagListDialog = new CTagListView(m_tagManager, this);
	m_settingsDialog = new CSettingsDialog(this);
	m_tagSettingsDialog = new CTagSettingsDialog(&m_dataHandler, this);
	m_gen2SettingsDialog = new CGen2TagDialog(this);

       
	/* Create the view manager */
	m_tagViewManager = new CTagViewManager(ui.readerTreeWidget, &m_dataHandler, m_readRateCalc, this);
	m_tagViewManager->setUp(	m_settingsDialog->showAlias(),
								m_settingsDialog->useTtl(),
								m_settingsDialog->msecsToShowInactive(),
								m_settingsDialog->msecsToShowOutOfRange(),
								m_settingsDialog->msecsToDelete());
	QObject::connect(m_tagViewManager, SIGNAL(requestTagSettings(QString)), this, SLOT(requestTagSettingsDialog(QString)));
	QObject::connect(m_tagViewManager, SIGNAL(requestTagAdvancedSettings(QString, QString)), this, SLOT(requestTagAdvancedSettingsDialog(QString, QString)));
	QObject::connect(m_tagViewManager, SIGNAL(requestReaderAdvancedSettings(QString)), this, SLOT(requestReaderAdvancedSettingsDialog(QString)));
	QObject::connect(m_tagViewManager, SIGNAL(requestReaderRegisterMap(QString)), this, SLOT(requestReaderRegisterMap(QString)));
	QObject::connect(m_tagViewManager, SIGNAL(newTagCount(int)), ui.tagCountNumber, SLOT(display(int)));
	QObject::connect(m_tagViewManager, SIGNAL(newDifferentTagCount(int)), ui.differentTagCountNumber, SLOT(display(int)));
	QObject::connect(m_tagViewManager, SIGNAL(newOverallDifferentTagCount(int)), ui.overallDifferentTagCountNumber, SLOT(display(int)));
	QObject::connect(m_tagViewManager, SIGNAL(oldTagEntryRemoved(QString,QString)), m_tagManager, SLOT(oldTagEntryRemoved(QString,QString)));
	QObject::connect(m_tagViewManager, SIGNAL(currentReaderChanged(QString)), this, SLOT(currentReaderChanged(QString)));

	splash->showMessage(tr("Connect..."), alignment);

	/* Connect the signals of the gui to the right slots */
	QObject::connect(QrfeTrace::getInstance(), SIGNAL(traceSignal(QString)), 	ui.traceBrowser, SLOT(append(QString)));
	QObject::connect(ui.actionAboutReaderTool, SIGNAL(triggered (bool)), 		m_aboutDialog, SLOT(exec()));
	QObject::connect(ui.actionShow_TagList, SIGNAL(triggered (bool)), 			m_tagListDialog, SLOT(exec()));

	QObject::connect(ui.readerTabWidget, SIGNAL(currentChanged(int)), 			this, SLOT(selectReader(int)));
	QObject::connect(ui.startScanButton, SIGNAL(toggled (bool)), 				this, SLOT(startScan(bool)));
	QObject::connect(ui.handleActionPushButton, SIGNAL(toggled(bool)), 			this, SLOT(handleActionsToggled(bool)));
	QObject::connect(ui.actionAdd_Serial_Reader, SIGNAL(triggered(bool)), 		this, SLOT(addSerialReader()));
	//Del by yingwei tseng for hiding AMS code, 2010/03/10
	//QObject::connect(ui.actionAdd_Tcp_Reader, SIGNAL(triggered(bool)), 			this, SLOT(addTcpReader()));
	//End by yingwei tseng for hiding AMS code, 2010/03/10	
	QObject::connect(ui.actionHandle_Actions, SIGNAL(triggered(bool)), 			this, SLOT(handleActionsToggled(bool)));
	QObject::connect(ui.actionShow_Alias_Names, SIGNAL(triggered ( bool)), 		this, SLOT(showAliasNames(bool)));
	QObject::connect(ui.actionUse_Time_To_Live, SIGNAL(triggered ( bool)), 		this, SLOT(useTimeToLive(bool)));
	QObject::connect(ui.actionPreferences, SIGNAL(triggered ( bool)), 			this, SLOT(showSettings()));
	QObject::connect(ui.actionOpen_Register_Map, SIGNAL(triggered ( bool)), 	this, SLOT(showRegisterMap()));
	QObject::connect(ui.clearButton, SIGNAL(clicked()), 						m_readRateCalc, SLOT(clearResults()));
	QObject::connect(ui.actionClear_Tags, SIGNAL(triggered (bool)), 			m_readRateCalc, SLOT(clearResults()));
	QObject::connect(ui.clearButton, SIGNAL(clicked()), 						m_tagViewManager, SLOT(clearTags()));
	QObject::connect(ui.actionClear_Tags, SIGNAL(triggered (bool)), 			m_tagViewManager, SLOT(clearTags()));
	QObject::connect(ui.clearOfflineReaderButton, SIGNAL(clicked()), 			m_tagViewManager, SLOT(clearOfflineReader()));
	QObject::connect(ui.clearOfflineReaderButton, SIGNAL(clicked()), 			this, SLOT(clearOfflineReader()));
	QObject::connect(ui.actionClear_Offline_Reader, SIGNAL(triggered(bool)), 	m_tagViewManager, SLOT(clearOfflineReader()));

	QObject::connect(m_gen2SettingsDialog,    SIGNAL(easterKeyUnlocked()),      this, SLOT(easterKeyUnlocked()));

	/* Create the scan timer to get the end of the scan */
	m_scanTimer = new QTimer(this);
	m_scanTimer->setSingleShot(true);
	QObject::connect(m_scanTimer, SIGNAL(timeout()), this, SLOT(stopScan()));

	/* Create timer for the scan progress bar */
	m_scanProgressTimer = new QTimer(this);
	m_scanProgressTimer->setSingleShot(false);
	m_scanProgressTimer->setInterval(1000);

	m_regMapWindow = NULL;

	/* Connect to the Reader Manager */
	QObject::connect(this, SIGNAL(currentReaderChanged(QrfeReaderInterface*)), m_amsComWrapper, SLOT(gotReader(QrfeReaderInterface*)));
	QObject::connect(&m_readerManager, SIGNAL(lostReader(QrfeReaderInterface*)), m_amsComWrapper, SLOT(lostReader(QrfeReaderInterface*)));
	QObject::connect(&m_readerManager, SIGNAL(gotReader(QrfeReaderInterface*)), this, SLOT(gotReader(QrfeReaderInterface*)));
	QObject::connect(&m_readerManager, SIGNAL(lostReader(QrfeReaderInterface*)), this, SLOT(lostReader(QrfeReaderInterface*)));

    //Add by yingwei tseng for using bar to set power, 2010/07/09
    QObject::connect(ui.powerSlider, SIGNAL(valueChanged(int)), this, SLOT(powerSliderChange(int)));
	//End by yingwei tseng for using bar to set power, 2010/07/09

	/* Create the timer for the multiplexer control */
	m_multiplexTimer = new QTimer(this);
	m_multiplexTimer->setSingleShot(true);
	m_multiplexTimer->setInterval(m_settingsDialog->multiplexTime());
	QObject::connect(m_multiplexTimer, SIGNAL(timeout()), this, SLOT(multiplexISR()));

	/* Finally set up the gui */
	ui.traceDockWidget->setVisible(false);
	ui.informationBox->setVisible(false);
	ui.actionShow_Alias_Names->setChecked(m_settingsDialog->showAlias());
	ui.actionUse_Time_To_Live->setChecked(m_settingsDialog->useTtl());

	splash->showMessage(tr("Starting up..."), alignment);
	ActivateSettings();
	Sleep(1000);
	splash->close();

	//Add by yingwei tseng for hiding AMS code, 2010/03/10
	ui.handleActionPushButton->hide();
	ui.clearOfflineReaderButton->hide();
	//ui.actionAdd_Serial_Reader->setVisible(false);
	ui.actionAdd_USB->setVisible(false);
	ui.actionAdd_Tcp_Reader->setVisible(false);
	ui.actionHandle_Actions->setVisible(false);
	ui.actionClear_Offline_Reader->setVisible(false);
	//End by yingwei tseng for hiding AMS code, 2010/03/10

	//Add by yingwei tseng for hiding items, 2010/12/08
    ui.actionShow_Alias_Names->setVisible(false);
	ui.actionUse_Time_To_Live->setVisible(false);
	ui.actionShow_Trace_Browser->setVisible(false);
	ui.actionShow_TagList->setVisible(false);
	//End by yingwei tseng for hiding items, 2010/12/08
	ui.groupBox_3->hide();
}

/*!
 * @brief 	Destructor
 * Destroys the object and frees all used memory.
 */
CReaderTool::~CReaderTool()
{
	/* Stop action handler */
	m_actionHandler->stopActionHandling();

	/* Stop and delete timer */
	m_scanTimer->stop();
	m_scanTimer->deleteLater();
	m_scanProgressTimer->stop();
	m_scanProgressTimer->deleteLater();
	m_multiplexTimer->stop();
	m_multiplexTimer->deleteLater();

	/* Delete view manager */
	m_tagViewManager->deleteLater();

	/* Delete dialogs */
	m_settingsDialog->deleteLater();
	m_tagSettingsDialog->deleteLater();
}

void CReaderTool::closeEvent(QCloseEvent *event)
{
	if(m_regMapWindow != NULL)	
		m_regMapWindow->close();

	event->accept();
}

void CReaderTool::addSerialReader()
{
	int i;
	bool ok;
	QList<UINT> ports;
	QStringList sFriendlyNames;
	QStringList sl;
	QString s;
	if (CEnumerateSerial::UsingSetupAPI1(ports, sFriendlyNames))
	{
		for (i=0; i<ports.size(); i++)
		{
			sl.append(QString("COM%1 (%2)").arg(ports[i]).arg(sFriendlyNames[i]));			
		}
	}

    //Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
    #if 0
	foreach (QString text, sl)
	{
QMessageBox::information(0, "Serial Test",
	text, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	}
	#endif
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 

	s = QInputDialog::getItem(this,"Choose COM port","Port",sl,0,false,&ok);
	//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
	#if 0
QMessageBox::information(0, "Serial Test2",
	s, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	#endif
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	if ( ok )
	{
		QString msg;
		m_readerManager.serialReaderAttached(QString("\\\\.\\")+s.split(" ").at(0),msg);
		if (msg.size()!=0) QMessageBox::critical(this,"Serial reader failed",msg);
	}
}

/*!
 * @brief	Slot that is connected to the Reader Manager
 * This slot is connected to the Reader Manager. It is called when the reader manager detects a new reader.
 * @param	reader 		Pointer to the new reader implementation
 */
void CReaderTool::gotReader(QrfeReaderInterface* reader)
{
	if (m_reader.size() == 0)
		emit currentReaderChanged(reader);

	/* Get the hardware and software revision of the reader */
	QString hardware, software;

	/* Check if it is a passive reader */
	bool passive = false;
	if(reader->readerType() == QrfeReaderInterface::TYPE_PASSIVE)
		passive = true;

	/* Add and connect the reader to the tag view */
	m_tagViewManager->clearOfflineReader();
	m_tagViewManager->addReader(reader->readerId(), hardware, software, reader->currentActionString(), reader->currentStateString(), passive, true,reader->getRssiChildren().size(),reader->getRssiChildren());
	m_tagManager->addReader(reader->readerId());
	QObject::connect(reader, SIGNAL(cyclicInventory(QString, QString)), m_tagManager, 		SLOT(cyclicInventoryResult(QString, QString))/*, Qt::DirectConnection*/);
	QObject::connect(reader, SIGNAL(cyclicInventory(QString, QString)), m_readRateCalc, 	SLOT(cyclicInventoryResult(QString, QString))/*, Qt::DirectConnection*/);
	QObject::connect(reader, SIGNAL(cyclicInventory(QString, QString)), m_tagViewManager, 	SLOT(cyclicInventroyResult(QString, QString))/*, Qt::DirectConnection*/);
	QObject::connect(reader, SIGNAL(cyclicRSSIInformation(QString,QString,QByteArray)), m_tagViewManager, SLOT(cyclicInventoryRSSIResult(QString,QString,QByteArray))/*, Qt::DirectConnection*/);
    //Del by yingwei tseng for modifying GUI style, 2010/08/30
	//QObject::connect(reader, SIGNAL(heartBeat(QString)), m_tagViewManager, SLOT(heartBeat(QString))/*, Qt::DirectConnection*/);
    //End by yingwei tseng for modifying GUI style, 2010/08/30
	QObject::connect(reader, SIGNAL(changedRssiEnable(QString,bool,uchar,QStringList) ), m_tagViewManager, SLOT( readerSetRSSI(QString,bool,uchar,QStringList)));

	/* Connect the reader to this object */
	QObject::connect(reader, SIGNAL(changedState(QString)), this, SLOT(readerChangedState(QString)));
	QObject::connect(reader, SIGNAL(changedAction(QString)), this, SLOT(readerChangedAction(QString)));

	/* Save the ID and the pointer to the protocol handler in a map */
	m_reader.insert(reader->readerId(), reader);

	/* Add the tab of the reader to the tab view */
	ui.readerTabWidget->addTab(new QWidget(ui.readerTabWidget), reader->readerId());

	/* Get the appropriate picture for the reader */
	if(passive)
		ui.readerTabWidget->setTabIcon(ui.readerTabWidget->count() -1, QIcon(":/tree widget icons/passiveReader"));
	else
		ui.readerTabWidget->setTabIcon(ui.readerTabWidget->count() -1, QIcon(":/tree widget icons/activeReader"));

	/* Reselect the current reader */
	selectReader(ui.readerTabWidget->currentIndex());

	/* Update the count of reader */
	ui.readerCountNumber->display(m_reader.size());

		if(	reader->getHardwareRevision(hardware) != QrfeReaderInterface::OK ||
		reader->getSoftwareRevision(software) != QrfeReaderInterface::OK ){
		hardware = "Unknown";
		software = "Unknown";
	}
	m_tagViewManager->changeHardFirm(reader->readerId(),hardware,software);


	/* If the scan is active in the moment, start the reader to scan */
	if(m_scanActive){
		if(m_settingsDialog->useMultiplex())
			m_multiplexTimer->start(m_settingsDialog->multiplexTime());
		else
			reader->startCyclicInventory();
	}
}

/*!
 * @brief 	Slot that is connected to the Reader Manager
 * The slot is called if a reader was either plugged off or is no more responding.
 * @param	reader	Pointer to the reader object that should be destroyed.
 */
void CReaderTool::lostReader(QrfeReaderInterface* reader)
{
	m_amsLEOSettingsDialogs.remove(reader);
	if(!m_reader.contains(reader->readerId()))
		return;

	/* Delete the reader from the map */
	m_reader.remove(reader->readerId());
	m_activeReader.removeAll(reader->readerId());

	/* Remove tab from the tab view */
	for(int i = 1; i < ui.readerTabWidget->count(); i++){
		if(ui.readerTabWidget->tabText(i) == reader->readerId())
			ui.readerTabWidget->removeTab(i);
	}

	/* Display current reader count*/
	ui.readerCountNumber->display(m_reader.size());
}

/*!
 * @brief	Slot that is connected to every used reader object
 * Slot that is called, if a reader changed his state.
 * @param	state	String that contains the new state.
 */
void CReaderTool::readerChangedState(QString state)
{
	/* get the reader */
	QrfeReaderInterface* ph = qobject_cast<QrfeReaderInterface*>(sender());

	if (ph == NULL)
	{
		trc(2, "The reader changed state and was null");
		return;
	}

	trc(2, "The reader " + ph->readerId() + " changed state to " + ph->currentStateString());
	/* notify the view manager to change the viewn state */
	m_tagViewManager->readerChangedState(ph->readerId(), state);
}

/*!
 * @brief	Slot that is connected to every used reader object
 * Slot that is called, if a reader changed his action.
 * @param	action	String that contains the new action.
 */
void CReaderTool::readerChangedAction(QString action)
{
	/* get the reader */
	QrfeReaderInterface* ph = qobject_cast<QrfeReaderInterface*>(sender());

	if (ph == NULL)
	{
		trc(2, "The reader changed action and was null");
		return;
	}



	trc(2, "The reader " + ph->readerId() + " changed action to " + ph->currentActionString());
	/* notify the view manager to change the viewn action */
	m_tagViewManager->readerChangedAction(ph->readerId(), action);
}

/*!
 * @brief	Slot that is connected to the tab bar
 * This slot is called if a tab of the tab bar is activated.
 * @param 	readerIndex		Index of the activated tab
 */
void CReaderTool::selectReader(int readerIndex)
{
	/* If tab is "All Reader" */
	if(readerIndex == 0){
		/* Reset read rate calculation */
		m_readRateCalc->reset();

		/* If scan active and multiplex is not activated, start every reader with scan */
		if(m_scanActive && !m_settingsDialog->useMultiplex()){
			foreach(QString readerId, m_reader.keys()){
				m_reader.value(readerId)->startCyclicInventory();
				m_readRateCalc->readerStartedInventory(readerId);
				m_tagViewManager->setActive(readerId);
			}
		}

		/* Show all reader in the view */
		m_tagViewManager->showAllReader();
		/* Now all reader are active */
		m_activeReader = m_reader.keys();

	}
	else{
		/* Get reader ID */
		QString readerId = ui.readerTabWidget->tabText(readerIndex);

		/* Reset read rate calculation */
		m_readRateCalc->reset();

		/* If scan active and multiplex is not activated, stop every reader from scan */
		if(m_scanActive && !m_settingsDialog->useMultiplex()){
			foreach(QString rID, m_activeReader){
				m_reader.value(rID)->stopCyclicInventory();
				m_tagViewManager->setUnactive(rID);
			}
		}

		/* Only show the selected reader in the view */
		m_tagViewManager->selectSingleReader(readerId);
		/* The only active reader is the selected one */
		m_activeReader = QStringList() << readerId;

		/* If scan is running, and not multiplexed, start the scan */
		if(m_scanActive && !m_settingsDialog->useMultiplex()){
			m_reader.value(readerId)->startCyclicInventory();
			m_readRateCalc->readerStartedInventory(readerId);
			m_tagViewManager->setActive(readerId);
		}
	}
}


/*!
 * @brief	Slot that is connected to the Reader-Tag-View
 * It is called if the user requests the tag settings.
 * @param 	tagId		ID of the tag
 */
void CReaderTool::requestTagSettingsDialog(QString tagId)
{
	/* open the tag settings dialog */
	m_tagSettingsDialog->exec(tagId);
}

/*!
 * @brief	Slot that is connected to the Reader-Tag-View
 * It is called if the user requests the advanced tag settings.
 * @param 	readerId	ID of the reader
 * @param 	tagId		ID of the tag
 */
void CReaderTool::requestTagAdvancedSettingsDialog(QString readerId, QString tagId)
{
	/* If scan is running it is not possible */
	if(m_scanActive)
	{
		QMessageBox::critical(this, "Advanced Tag Settings", "You must stop the scan before you can change the settings of a tag.");
		return;
	}

	/* Get the reader object */
	QrfeReaderInterface* reader = m_reader.value(readerId);
	if(reader == 0)
		return;

	/* Select the appropriate dialog */
	if(reader->readerType() == QrfeReaderInterface::TYPE_PASSIVE)
	{
		if(reader->tagType() == QrfeReaderInterface::TAG_GEN2)
		{
			m_gen2SettingsDialog->exec(reader, tagId);
			return;
		}
		else if(reader->tagType() == QrfeReaderInterface::TAG_ISO6B)
		{
			QMessageBox::information(this, "Advanced Tag Settings", "The advanced tag settings for ISO6B tags are not available at present.");
			return;
		}
	}
	else if(reader->readerType() == QrfeReaderInterface::TYPE_ACTIVE){
		QMessageBox::information(this, "Advanced Tag Settings", "The advanced tag settings for active tags are not available at present.");
		return;
	}

	QMessageBox::information(this, "Advanced Tag Settings", "The advanced tag settings are not available at present.");

}

/*!
 * @brief	Slot that is connected to the Reader-Tag-View
 * It is called if the user requests the advanced reader settings.
 * @param 	readerId	ID of the reader
 */
void CReaderTool::requestReaderRegisterMap(QString readerId)
{
	/* If scan is running it is not possible */
	if(m_scanActive)
	{
		QMessageBox::critical(this, "Register Map", "You must stop the scan before you open the register map for a reader.");
		return;
	}

	/* Get the reader object */
	QrfeReaderInterface* reader = m_reader.value(readerId);
	if(reader == 0)
		return;

	/* Select the appropriate dialog */
	if(reader->readerType() == QrfeReaderInterface::TYPE_PASSIVE){
		if(reader->readerVersion() == QrfeReaderInterface::VERSION_AMS_LEO){
			if (m_regMapWindow!=NULL) delete m_regMapWindow;
			m_regMapWindow = NULL;
			try
			{

				QByteArray vals;
                                m_regFileAvailable = true;
				if (reader->getParam(0x13,vals) != QrfeReaderInterface::OK )
					return;
				if ((vals.at(0) & 0x30) == 0x30)
				{
					m_regMapWindow = new RegisterMap(NULL,m_amsComWrapper,"register_map_AS3991.xml");
				}
				else
				{
					m_regMapWindow = new RegisterMap(NULL,m_amsComWrapper,"register_map_AS3992.xml");
				}
				m_regMapWindow->resize(QSize(740,1024));
				m_regMapWindow->setWindowTitle("Register Map "+readerId);
				showRegisterMap();
				m_regMapWindow->readOnce();
				return;
			}
			catch(QString msg)
			{
				if (m_regMapWindow!=NULL) delete m_regMapWindow;
				m_regMapWindow = NULL;
                                m_regFileAvailable = false;
				//handle here the exception in case no xml file available 
				QMessageBox::warning(this, tr("Register Map"), msg);
				return;
			}

		}
	}
	else if(reader->readerType() == QrfeReaderInterface::TYPE_ACTIVE){
		QMessageBox::information(this, "Register Map", "The advanced reader settings for active reader are not available at present.");
		return;
	}

	QMessageBox::information(this, "Register Map", "The Register Map for this type of reader is not available at present.");
}

/*!
 * @brief	Slot that is connected to the Reader-Tag-View
 * It is called if the user requests the advanced reader settings.
 * @param 	readerId	ID of the reader
 */
void CReaderTool::requestReaderAdvancedSettingsDialog(QString readerId)
{
	/* If scan is running it is not possible */
	if(m_scanActive)
	{
		QMessageBox::critical(this, "Advanced Reader Settings", "You must stop the scan before you can change the settings of a reader.");
		return;
	}

	/* Get the reader object */
	QrfeReaderInterface* reader = m_reader.value(readerId);
	if(reader == 0)
		return;

	if (! m_amsLEOSettingsDialogs.contains(reader))
	{
		//Add by yingwei tseng for printer application, 2010/05/27	
		//m_amsLEOSettingsDialogs.insert(reader,new CAmsLeoDialog(this));
		//m_amsLEOSettingsDialogs.insert(reader,new CAmsLeoDialog(this, m_tagManager, m_tagViewManager));
		m_amsLEOSettingsDialogs.insert(reader,new CAmsLeoDialog(this, m_tagManager, m_tagViewManager, m_readRateCalc));
		//End by yingwei tseng for printer application, 2010/05/27
	}

	/* Select the appropriate dialog */
	if(reader->readerType() == QrfeReaderInterface::TYPE_PASSIVE){
		if(reader->readerVersion() == QrfeReaderInterface::VERSION_AMS_LEO){
			//Mod by yingwei tseng for modifying GUI style, 2010/08/30
			//m_amsLEOSettingsDialogs.value(reader)->setWindowTitle("Advanced Reader Settings "+readerId);
			m_amsLEOSettingsDialogs.value(reader)->setWindowTitle("Reader Settings "+readerId);
			//End by yingwei tseng for modifying GUI style, 2010/08/30
			m_amsLEOSettingsDialogs.value(reader)->exec(reader);
			return;
		}
	}
	else if(reader->readerType() == QrfeReaderInterface::TYPE_ACTIVE){
		QMessageBox::information(this, "Advanced Reader Settings", "The advanced reader settings for active reader are not available at present.");
		return;
	}

	QMessageBox::information(this, "Advanced Reader Settings", "The advanced reader settings for this type of reader are not available at present.");
}


/*!
 * @brief 	Slot that is connected to an action
 * This slot is called by an action. It changes the display of the tags to the stored alias names.
 * @param 	show	Says if to use alias names or not
 */
void CReaderTool::showAliasNames(bool show)
{
	/* Save the setting */
	m_settingsDialog->setShowAlias(show);
	/* Notify the view manager to change the settings */
	m_tagViewManager->setUp(	m_settingsDialog->showAlias(),
								m_settingsDialog->useTtl(),
								m_settingsDialog->msecsToShowInactive(),
								m_settingsDialog->msecsToShowOutOfRange(),
								m_settingsDialog->msecsToDelete());
}

/*!
 * @brief 	Slot that is connected to an action
 * This slot is called by an action. It changes the behavior of the display of the tags.
 * @param 	use		Says if to use time to live or not
 */
void CReaderTool::useTimeToLive(bool use)
{
	/* Save the setting */
	m_settingsDialog->setUseTtl(use);
	/* Notify the view manager to change the settings */
	m_tagViewManager->setUp(	m_settingsDialog->showAlias(),
								m_settingsDialog->useTtl(),
								m_settingsDialog->msecsToShowInactive(),
								m_settingsDialog->msecsToShowOutOfRange(),
								m_settingsDialog->msecsToDelete());
}

/*!
 * @brief 	Slot that is connected to an action
 * This slot is called by an action. It activates the handling of the configured actions (Show picture/Start app).
 * @param 	checked		Says if to handle actions or not
 */
void CReaderTool::handleActionsToggled(bool checked)
{
	/* Set the buttons state */
	ui.actionHandle_Actions->setChecked(checked);
	ui.handleActionPushButton->setChecked(checked);

	if(checked){
		/* Connect every reader to the action handler */
		foreach(QString readerId, m_reader.keys()){
			QrfeReaderInterface* reader = m_reader.value(readerId);
			QObject::connect(reader, SIGNAL(cyclicInventory(QString, QString)), m_actionHandler, SLOT(cyclicInventroyResult(QString, QString)));
		}
	}
	else{
		/* Disconnect every reader to the action handler */
		foreach(QString readerId, m_reader.keys()){
			QrfeReaderInterface* reader = m_reader.value(readerId);
			QObject::disconnect(reader, SIGNAL(cyclicInventory(QString, QString)), m_actionHandler, SLOT(cyclicInventroyResult(QString, QString)));
		}
	}


	/* If a scan is already active, start the action handler immediately */
	if(m_scanActive){
		if(checked)
			m_actionHandler->startActionHandling();
		if(!checked)
			m_actionHandler->stopActionHandling();
	}
}

/*!
 * @brief	Slot is connected to the gui
 * This slot is called if a scan should be started OR stopped.
 * @param	start		Varaible that identifies whether to start or stop scanning
 */
void CReaderTool::startScan(bool start)
{
	/* Start the scan */
	if(start)
	{
		/* Set the variable that scan is running */
		m_scanActive = true;

		/* Change button to stop */
		ui.startScanButton->setText("Stop Scan");
		ui.actionStart_Scan->setText("Stop Scan");

		/* Clear trace browser, because of lack of memory */
		ui.traceBrowser->clear();

		/* disable the time out box */
		ui.scanTimeoutBox->setDisabled(true);

		/* Calculate the scan time out */
		m_scanTimeout = ui.scanTimeoutBox->value() * 1000;

		/* Save the start time of the scan */
		m_scanStart = QTime::currentTime();

		/* If a scan timeout is set*/
		if(m_scanTimeout > 0)
		{
			/* Disable the time out box */
			ui.scanTimeoutBox->setDisabled(true);

			/* Prepare progress bar */
			ui.scanProgressBar->setValue(0);
			ui.scanProgressBar->setMaximum(m_scanTimeout);
			ui.scanProgressLabel->setText(QString::number(0) + " s");

			/* Start the timer for the given time out */
			m_scanTimer->start(m_scanTimeout);

			/* Start the helper timer, to increment the progress bar */
			QObject::connect(m_scanProgressTimer, SIGNAL(timeout()), this, SLOT(incrementScanProgress()));
			m_scanProgressTimer->start();

		}
		/* If time out is set to infinite */
		else
		{
			/* Prepare progress bar */
			ui.scanProgressBar->setDisabled(true);
			ui.scanProgressLabel->setText("Infinite");

			/* Start the helper timer, to only show the time the scan is running */
			QObject::connect(m_scanProgressTimer, SIGNAL(timeout()), this, SLOT(showScanTime()));
			m_scanProgressTimer->start();
		}

		this->repaint();

		/* Reset read rate calculation */
		m_readRateCalc->reset();
		/* Notify tag manager that a scan is started */
		m_tagManager->startScan();
		/* Clear all reader and tag infos in the view */
		m_tagViewManager->clearAllReaderTagInfos();

		/* If it is configured to use multiplex */
		if(m_settingsDialog->useMultiplex())
		{
			m_multiplexCurrentReader = 0;
			m_multiplexReaderOn = false;

			/* Start the first reader of the active reader list */
			for(m_multiplexCurrentReader = 0; m_multiplexCurrentReader < m_activeReader.size(); m_multiplexCurrentReader++)
			{
				if(m_reader.contains(m_activeReader.at(m_multiplexCurrentReader))){
					if(m_reader.value(m_activeReader.at(m_multiplexCurrentReader))->startCyclicInventory() == QrfeReaderInterface::OK){
						/* Notify the read rate calculator that a new round started */
						m_readRateCalc->readerStartedInventory(m_activeReader.at(m_multiplexCurrentReader));
						/* Set the reader active in the view */
						m_tagViewManager->setActive(m_activeReader.at(m_multiplexCurrentReader));
						/* Store that there is a reader running */
						m_multiplexReaderOn = true;
						break;
					}
				}
			}

			/* Start the multiplex timer */
			m_multiplexTimer->start(m_settingsDialog->multiplexTime());
		}
		else
		{
			/* Start every reader */
			foreach(QString id, m_activeReader)
			{
				if(m_reader.contains(id)){
					m_reader.value(id)->startCyclicInventory();
					m_readRateCalc->readerStartedInventory(id);
				}
			}

			/* Set all reader active in the view */
			m_tagViewManager->setActive();
		}


		m_tagViewManager->startGuiUpdate();

		/* start the action handler, if needed */
		if(ui.handleActionPushButton->isChecked())
			m_actionHandler->startActionHandling();

	}

	/* Stop the scan */
	else
	{
		/* Change the button back to start */
		ui.startScanButton->setText("Start Scan");
		ui.actionStart_Scan->setText("Start Scan");

		/* Re-enable the time out box */
		ui.scanTimeoutBox->setEnabled(true);

		/* If a scan timeout was configured */
		if(m_scanTimeout > 0)
		{
			/* Stop the scan timer */
			m_scanTimer->stop();

			/* Stop the helper timer */
			QObject::disconnect(m_scanProgressTimer, SIGNAL(timeout()), this, SLOT(incrementScanProgress()));
			m_scanProgressTimer->stop();

			/* Set progress bar to maximum */
			ui.scanProgressBar->setValue(m_scanTimeout);
			ui.scanProgressLabel->setText(QString::number(m_scanTimeout/1000) + " s");

			trc(2, "Scan stopped after " + QTime().addSecs(qAbs(m_scanStart.secsTo(QTime::currentTime()))).toString("h:mm:ss"));
		}
		/* If the scan was infinite */
		else
		{
			/* Stop the scan timer */
			m_scanTimer->stop();

			/* Stop the helper timer */
			QObject::disconnect(m_scanProgressTimer, SIGNAL(timeout()), this, SLOT(showScanTime()));
			m_scanProgressTimer->stop();

			/* Prepare progress bar */
			ui.scanProgressBar->setEnabled(true);

			trc(2, "Scan stopped after " + QTime().addSecs(qAbs(m_scanStart.secsTo(QTime::currentTime()))).toString("h:mm:ss"));
		}

		this->repaint();

		/* Stop each reader */
		foreach(QString id, m_activeReader)
		{
			if(m_reader.contains(id)){
				m_reader.value(id)->stopCyclicInventory();
				m_readRateCalc->readerStoppedInventory(id);
			}
		}

		/* Stop the view from updating */
		m_tagViewManager->stopGuiUpdate();

		/* Stop handling actions */
		m_actionHandler->stopActionHandling();

		/* Store that scanning is done */
		m_scanActive = false;

		/* Notify the reader manager to now clean up the plugged off reader */
		m_readerManager.cleanUp();
	}
}

/*!
 * @brief 	Slot that is connected to the scan timer.
 * If the scan timer runs into a timeout this slot is called. It sets the scan button to unchecked and thus calls indirectly the startScan
 * method with param false.
 */
void CReaderTool::stopScan()
{
	ui.startScanButton->setChecked(false);
}


/*!
 * @brief Slot that is called from the progressTimer to increment the progress bar.
 */
void CReaderTool::incrementScanProgress()
{
	ui.scanProgressBar->setValue(ui.scanProgressBar->value() + 1000);
	ui.scanProgressLabel->setText(QString::number(ui.scanProgressBar->value()/1000) + " s");
}

/*!
 * @brief Slot that prints out the current scan time.
 */
void CReaderTool::showScanTime()
{
	QTime t(0,0);
	QString time = t.addSecs(qAbs(m_scanStart.secsTo(QTime::currentTime()))).toString("h:mm:ss");
	ui.scanProgressLabel->setText(time);
}

/*!
 * @brief 	Slot that is called from the multiplex timer.
 * This slot is called from the multiplex timer. It handles the whole multiplex logic.
 */
void CReaderTool::multiplexISR()
{
	/* If reader is scanning, stop it */
	if(m_multiplexReaderOn == true){
		/* Check index */
		if(m_multiplexCurrentReader < m_activeReader.size())
		{
			/* Try to stop the active reader */
			if(m_reader.value(m_activeReader.at(m_multiplexCurrentReader))->stopCyclicInventory() == QrfeReaderInterface::OK)
			{
				/* Notify the read rate calculator that the reader is paused*/
				m_readRateCalc->readerPausedInventory(m_activeReader.at(m_multiplexCurrentReader));
				/* Increment reader index */
				m_multiplexCurrentReader++;
				m_multiplexReaderOn = false;

			}
			/* If it was not successful to stop the reader, try again immediately. */
			else{
				m_multiplexTimer->start(1);
				return;
			}
		}
	}

	/* If no reader is scanning, but scan is still running, start the next */
	if(m_multiplexReaderOn == false && m_scanActive)
	{
		/* Check index */
		if(m_multiplexCurrentReader >= m_activeReader.size())
			m_multiplexCurrentReader = 0;

		/* Check index */
		if(m_multiplexCurrentReader < m_activeReader.size())
		{
			/* Try to start the next reader */
			if(m_reader.value(m_activeReader.at(m_multiplexCurrentReader))->startCyclicInventory() == QrfeReaderInterface::OK)
			{
				m_multiplexReaderOn = true;

				/* Notify the read rate calculator that a new round started */
				m_readRateCalc->readerResumedInventory(m_activeReader.at(m_multiplexCurrentReader));
				/* Set the reader active in the view */
				m_tagViewManager->setActive(m_activeReader.at(m_multiplexCurrentReader));

				/* Restart the multiplex timer */
				m_multiplexTimer->start(m_settingsDialog->multiplexTime());
			}
			/* If it was not successful to start the reader, try again immediately. */
			else
			{
				m_multiplexTimer->start(1);
				return;
			}
		}
	}
}
/*!
 * @brief 	Function to activate the current settings.
 */

void CReaderTool::ActivateSettings()
{

	/* Notify the view manager with the new settings */
	m_tagViewManager->setUp(	m_settingsDialog->showAlias(),
								m_settingsDialog->useTtl(),
								m_settingsDialog->msecsToShowInactive(),
								m_settingsDialog->msecsToShowOutOfRange(),
								m_settingsDialog->msecsToDelete());

	/* Change the ui */
	ui.actionShow_Alias_Names->setChecked(m_settingsDialog->showAlias());
	ui.actionUse_Time_To_Live->setChecked(m_settingsDialog->useTtl());

	/* Activate trace if needed */
	if(m_settingsDialog->useTrace())
		QrfeTrace::setTrcLevel(m_settingsDialog->traceLevel());
	else
		QrfeTrace::setTrcLevel(-1);
	/* Set the multiplex time out */
	if(m_settingsDialog->useMultiplex())
		m_multiplexTimer->setInterval(m_settingsDialog->multiplexTime());
	
	autoUpdate(m_settingsDialog->useRegisterUpdate());
}
/*!
 * @brief 	Slot that is called from the gui
 * This slot is called if the settings window is requested.
 */
void CReaderTool::showSettings()
{
	/* Show the settings dialog with the current settings */
	if(m_settingsDialog->exec() != QDialog::Accepted)
		return;
	ActivateSettings();
}


/*!
 * @brief 	Slot that is called from the special gui
 * This slot is called if the easter key is unlocked.
 */
void CReaderTool::easterKeyUnlocked()
{
    if(QMessageBox::Yes == QMessageBox::question(this, "The secret key", "The key was detected. \nYou wanna play?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
        m_keyDialog->exec();
}


/*!
 * @brief	Member function to get a reader interface
 * This member function returns the reader interface of the given readerId
 * @param 	readerId	String with the reader ID
 * @return	Returns a pointer to the reader interface. If there is no reader interface to the given reader id a zero pointer is returned.
 */
QrfeReaderInterface* CReaderTool::getInterface(QString readerId)
{
	return m_reader.value(readerId);
}

void CReaderTool::clearOfflineReader()
{
	m_readerManager.cleanUp();
}

void CReaderTool::showRegisterMap()
{
	if(m_regMapWindow != NULL && m_regFileAvailable)
	{
		m_regMapWindow->show();
		m_regMapWindow->setWindowTitle("Register Map " + m_tagViewManager->getCurrentReader());
	}
}

void CReaderTool::autoUpdate(bool update)
{
	if (m_regMapWindow == NULL) return;
    if (m_updateThread == NULL)
	{
		m_updateThread = new AMSAutoUpdateThread(this->m_amsComWrapper, m_regMapWindow, m_settingsDialog->registerUpdateTime());
	}

	if (update)
	{
		m_updateThread->start();
	}
	else
	{
		m_updateThread->terminate();
	}
}

void CReaderTool::currentReaderChanged(QString readerID)
{
	bool isVisible = false;
	QrfeReaderInterface* ph = qobject_cast<QrfeReaderInterface*>(m_reader.value(readerID));
	if(ph == NULL)
		return;
	emit currentReaderChanged(ph);
	if (m_regMapWindow) 
	{
		isVisible = m_regMapWindow->isVisible();
		m_regMapWindow->close();
		delete m_regMapWindow;
	}

	try
        {
			QByteArray vals;
			ph->getParam(0x13,vals);
                        m_regFileAvailable = true;
			if (vals.size()>0 && (vals.at(0) & 0x30) == 0x30)
			{
				m_regMapWindow = new RegisterMap(NULL,m_amsComWrapper,"register_map_AS3991.xml");
			}
			else
			{
				m_regMapWindow = new RegisterMap(NULL,m_amsComWrapper,"register_map_AS3992.xml");
			}
			m_regMapWindow->resize(QSize(740,1024));
        }
        catch(QString msg)
        {
            //handle here the exception in case no xml file available 
            m_regFileAvailable = false;
            QMessageBox::warning(this, tr("Register Map"), msg);
			return;
        }

	m_regMapWindow->setWindowTitle("Register Map "+readerID);
	if(isVisible)
	{
		showRegisterMap();
		m_regMapWindow->readOnce();
	}
}

void CReaderTool::show(void)
{
	QMainWindow::show();
	// search for reader might trigger modal dialog, do that after the main window is visible=accesible via window list
	m_readerManager.searchForReader();
}

//Add by yingwei tseng for using bar to set power, 2010/07/09
void CReaderTool::powerSliderChange(int val)
{
    QrfeAmsReader* m_ph = NULL;
	
    this->ui.powerLText->setText(QString("%1 dBm").arg(val));

	//signed char power = this->ui.powerSlider->value();

	/* Start every reader */
    foreach(QString id, m_activeReader)
	{
	    if(m_reader.contains(id)){
		    m_ph = qobject_cast<QrfeAmsReader*>(m_reader.value(id));
			QrfeReaderInterface::Result resPowerState = m_ph->AntennaPortSetPowerLevel(val);	
			if (resPowerState != QrfeReaderInterface::OK){
				QMessageBox::critical(this, "Error", QString("Could not set power level to %1 dBm").arg(val));
				break;
			}
		}	
	}	
}
//End by yingwei tseng for using bar to set power, 2010/07/09

