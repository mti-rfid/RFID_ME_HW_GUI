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

#include "CAmsLeoDialog.h"

#include "../helper/QrfeProgressBar.h"

#include "qwt_plot.hxx"
#include "qwt_plot_curve.h"
#include "qwt_legend.hxx"
#include "CAmsLeoReflectedPower.h"

#include <QMessageBox>
#include <string>

//add by yo chen, 2010/11/01
#include <Qfile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
//end by yo chen, 2010/11/01
//Mod by yingwei tseng for printer application, 2010/05/27
//CAmsLeoDialog::CAmsLeoDialog(QWidget *parent)
//CAmsLeoDialog::CAmsLeoDialog(QWidget *parent, CTagManager *tagManager, CTagViewManager *tagViewManager)
CAmsLeoDialog::CAmsLeoDialog(QWidget *parent, CTagManager *tagManager, CTagViewManager *tagViewManager, CReadRateCalc *readRateCalc)
//End by yingwei tseng for printer application, 2010/05/27
    : QDialog(parent)
{
	ui.setupUi(this);
	//Add by yingwei tseng for printer application, 2010/05/27
	m_tagManager = tagManager;    
	m_tagViewManager = tagViewManager;
	m_readRateCalc = readRateCalc;
	//End by yingwei tseng for printer application, 2010/05/27
	QObject::connect(ui.buttonBox, 						SIGNAL(rejected()), 		this, SLOT(tryAccept()));
	QObject::connect(ui.setInventoryIntervalButton, 	SIGNAL(clicked()), 			this, SLOT(setInventoryInterval()));
	QObject::connect(ui.setFrequencyButton, 			SIGNAL(clicked()), 			this, SLOT(setFrequency()));
	QObject::connect(ui.setTagModeButton,	 			SIGNAL(clicked()), 			this, SLOT(setTagMode()));
	QObject::connect(ui.setGen2Configuration,			SIGNAL(clicked()), 			this, SLOT(setGen2Configuration()));
	QObject::connect(ui.levelButton,					SIGNAL(clicked()), 			this, SLOT(setLevel()));
	QObject::connect(ui.levelSlider,					SIGNAL(valueChanged(int)), 	this, SLOT(levelSliderChanged(int)));
	QObject::connect(ui.sensSlider,						SIGNAL(valueChanged(int)), 	this, SLOT(sensSliderChanged(int)));
	QObject::connect(ui.sweepButton,		 			SIGNAL(clicked()), 			this, SLOT(sweepFrequencys()));
	QObject::connect(ui.commandButton,		 			SIGNAL(clicked()), 			this, SLOT(sendCommand()));


	QObject::connect(ui.startFrequencyBox,	 			SIGNAL(valueChanged(double)),this, SLOT(oneFreqChanged(double)));
	QObject::connect(ui.endFrequencyBox,	 			SIGNAL(valueChanged(double)),this, SLOT(oneFreqChanged(double)));
	QObject::connect(ui.incrementBox,	 				SIGNAL(valueChanged(double)),this, SLOT(oneFreqChanged(double)));
	QObject::connect(ui.listenTimeBox,					SIGNAL(valueChanged(double)),this, SLOT(oneFreqChanged(double)));
	QObject::connect(ui.idleTimeBox,					SIGNAL(valueChanged(double)),this, SLOT(oneFreqChanged(double)));
	QObject::connect(ui.allocationTimeBox,				SIGNAL(valueChanged(double)),this, SLOT(oneFreqChanged(double)));
	QObject::connect(ui.rssiThresholdBox,				SIGNAL(valueChanged(int)),this, SLOT(oneFreqChanged(int)));
	
	QObject::connect(ui.hoppingBox,			 			SIGNAL(currentIndexChanged( const QString &)),this, SLOT(setHopper( const QString &)));
	QObject::connect(ui.openRFPButton,					SIGNAL(clicked()),			this, SLOT(openRFPDialog()));
	QObject::connect(ui.continuousSendButton,			SIGNAL(clicked()),			this, SLOT(continuousSend()));
	QObject::connect(ui.sensButton,						SIGNAL(clicked()),          this, SLOT(setSensitivity()));
	//Add by yingwei tseng for region, 2009/12/03
	QObject::connect(ui.regionBox,						SIGNAL(currentIndexChanged(int)), 	this, SLOT(loadRegion()) );
	//End by yingwei tseng for region, 2009/12/03
	//Add by yingwei tseng for submit, 2009/12/03
	QObject::connect(ui.setAdvReaderButton,		 		SIGNAL(clicked()), 			 this, SLOT(setAdvancedReader()));  
	QObject::connect(ui.closeButton,		 			SIGNAL(clicked()), 			 this, SLOT(close())); 
	//End by yingwei tseng for submit, 2009/12/03
	//Add by yingwei tseng for frequency, 2010/01/04
	QObject::connect(ui.freqFixedButton,		        SIGNAL(clicked()), 	         this, SLOT(loadFreqType()));
    QObject::connect(ui.freqHoppingButton,		        SIGNAL(clicked()), 	         this, SLOT(loadFreqType()));
	QObject::connect(ui.fixedPowerOffButton,		    SIGNAL(clicked()), 	         this, SLOT(setFixedPowerOff()));
	//End by yingwei tseng for frequency, 2010/01/04
	//Add by yingwei tseng for modulation tseting, 2010/05/10
	QObject::connect(ui.modulationBox,		            SIGNAL(currentIndexChanged(int)), 	this, SLOT(loadModulation()));
	//End by yingwei tseng for modulation tseting, 2010/05/10
	//Add by yingwei tseng for printer application, 2010/05/27
	QObject::connect(ui.setPrinterButton,		 		SIGNAL(clicked()), 			 this, SLOT(setPrinter())); 
	QObject::connect(ui.tabWidget,                      SIGNAL(currentChanged(int)), this, SLOT(selectTabWidget(int)));
	//QObject::connect(ui.tabWidget,                      SIGNAL(currentChanged(int)), this, SLOT(selectTabWidget(int, QrfeAmsReader)));//rick
	//QObject::connect(ui.tabWidget,                      SIGNAL(currentChanged(int)), this, SLOT(tabWidgetToGetRegion(QrfeReaderInterface* r_clsReaderInterface)));
	QObject::connect(ui.lengthBox,			            SIGNAL(valueChanged(int)),	 this, SLOT(changeLength(int)));
	QObject::connect(ui.closePrinterButton,		 			SIGNAL(clicked()), 			 this, SLOT(close())); 
	//End by yingwei tseng for printer application, 2010/05/27

	//ui.tagModeBox->setCurrentIndex(1);//
	QObject::connect(ui.setAdvReaderButton,		 		SIGNAL(clicked()), 			 this, SLOT(tabWidgetToGetRegion()));//rick¡õ
    ui.tagModeBox->setCurrentIndex(0);
	ui.tabWidget->setCurrentIndex(0);
	ui.label_45->hide();
	ui.groupBox_6->hide();//rick¡ô
	ui.setCommunicationPortButton->hide();
	QObject::connect(ui.setCommunicationPortButton,     SIGNAL(clicked()),     this,SLOT(setCommunicationPort()));
	//rick¡ô2014-04-02

	//Add by yingwei tseng for hiding AMS code, 2010/03/10
	ui.label_3->hide();
	ui.inventoryIntervalBox->hide();
	ui.label_4->hide();
	ui.setInventoryIntervalButton->hide();
	ui.label_22->hide();
	ui.levelSlider->hide();
	ui.levelLabel->hide();
	ui.levelButton->hide();
	ui.label_6->hide();
	ui.hoppingBox->hide();
	ui.label_7->hide();
	ui.startFrequencyBox->hide();
	ui.label_16->hide();
	ui.endFrequencyBox->hide();
	ui.label_17->hide();
	ui.setFrequencyButton->hide();
	ui.incrementBox->hide();
	//Mod by yingwei tseng for printer application, 2010/05/27
	ui.tabWidget->removeTab(1);    
	//End by yingwei tseng for printer application, 2010/05/27
	//Add by yingwei tseng for printer application, 2010/05/27
	ui.label_9->hide();
	ui.sweepStartSpinBox->hide();	
	ui.label_13->hide();	
	ui.label_10->hide();
	ui.sweepStopSpinBox->hide();
	ui.label_14->hide();
	ui.label_12->hide();
	ui.sweepModeBox->hide();
	ui.label_11->hide();
	ui.sweepStepSpinBox->hide();
	ui.label_15->hide();
	ui.sweepButton->hide();
	ui.groupBox_4->hide();
	ui.groupBox_5->hide();
	//End by yingwei tseng for printer application, 2010/05/27
	ui.setTagModeButton->hide();
	ui.sensButton->hide();
	ui.setGen2Configuration->hide();
	ui.pilotToneBox->hide();    //Add by yingwei tseng for modfying gen2 setting pilot tone, 2010/07/23
	//End by yingwei tseng for hiding AMS code, 2010/03/10
    ui.groupBox_2->hide();    //Add by yingwei tseng for hiding items, 2010/12/08

	//Add by yingwei tseng for modulation tseting, 2010/05/10
	m_transPatternTimer = new QTimer(this);
	m_transPatternTimer->setSingleShot(true);
	QObject::connect(m_transPatternTimer, SIGNAL(timeout()), this, SLOT(stopTransPattern()));
	
	/* Create timer for the scan progress bar */
	m_transPatternProgressTimer = new QTimer(this);
	m_transPatternProgressTimer->setSingleShot(false);
	m_transPatternProgressTimer->setInterval(1000);
	//End by yingwei tseng for modulation tseting, 2010/05/10
	readProfiles();
}

CAmsLeoDialog::~CAmsLeoDialog()
{
	//Add by yingwei tseng for modulation tseting, 2010/05/10
	m_transPatternTimer->stop();
	m_transPatternTimer->deleteLater();	
	m_transPatternProgressTimer->stop();
	m_transPatternProgressTimer->deleteLater();
	//End by yingwei tseng for modulation tseting, 2010/05/10
}

QrfeAmsReader* tab_ph; bool tabRegionFlag=false;
int CAmsLeoDialog::exec(QrfeReaderInterface* r_clsReaderInterface)
{
	bool success = true;
	int  iIndex  = 0;



	m_ph = qobject_cast<QrfeAmsReader*>(r_clsReaderInterface);
	if( m_ph == 0 )
		return 0;

	tab_ph=m_ph;//Add by rick for tabwidget
    //tabRegionFlag=true;
	clearInfo();

	QrfeProgressBar* pb = new QrfeProgressBar("Reading data from reader...", 3, qobject_cast<QWidget*>(parent()));
	pb->show();

	pb->increasePorgressBar();

	quint32 interval = m_ph->intervalTime();

	pb->increasePorgressBar();

	ui.inventoryIntervalBox->setValue(interval);

	enabledAdvancedItems();    //Add by yingwei tseng for modulation testing bug, 2010/07/22

	//Add by yingwei tseng for frequency, 2010/01/04

	do
	{
		int        iRegionSupport = 0;
		int        totalChannel;
		int		   iIndex = 0;
		int		   iRegionIndex = 0;
		double     freq, stepChannel;
		QByteArray btRegion, btAddr, btData;
		/*QString currentRegion[]={"United States / Canada", "Europe", "Taiwan", 
			"China", "South Korea", "Australia / New Zealand", "Europe2", "Brazil", 
			"Hong Kong", "Malaysia", "Singapore", "Thailand", "Israel", "Russia Federation", 
			"India", "Saudi Arabia", "Jordan", "Mexico", "Indonesia"};
			*/

		//load the region from file and add to the regionBox
		QFile   regionCurrent(QDir::currentPath()+"/region");
		//QString regionLineCurrent[50];           //scan the file line by line	
		QString currentRegion[50];

		//Open region file
		if( regionCurrent.open(QIODevice::ReadOnly) == false)
		{
			QMessageBox::information( 0, 
									  "Get region file",
									  "Get region file fail", 
									  QMessageBox::Ok );
			break;
		}
		QTextStream scanRegionCurrent(&regionCurrent);

		for(iIndex=0; !scanRegionCurrent.atEnd(); iIndex++)
		{
			//regionLineCurrent[iIndex] = scanRegionCurrent.readLine();
			currentRegion[iIndex]=scanRegionCurrent.readLine();
		}
		regionCurrent.close();
		






//Region===================================================
		//Get Region
		QrfeReaderInterface::Result result = m_ph->getRegion(btRegion, btData);	

		if (result != QrfeReaderInterface::OK)
		{
			QMessageBox::information( 0, 
									  "Get Region",
									  "Get Region Fail", 
									  QMessageBox::Ok );
			break;
		}
		else
		{
			ui.lineEdit_3->setText(currentRegion[btData[0]]);
		}

		//Get Region Support
		btAddr = QByteArray::fromHex("0084");
		for(iIndex = 0; iIndex<4; iIndex++)
		{
			if ( QrfeReaderInterface::OK != m_ph->getMacFirmwareOemCfg(btAddr, btData) )
			{

				QMessageBox::information( 0, 
										  "Get Region Support",
										  "Get Region Support Fail", 
										  QMessageBox::Ok );

			
				break;
			}	

			btAddr[1] = btAddr[1] + 1;
			iRegionSupport |= btData[0] << (24 - iIndex*8);
		}		
		
		//Add by yingwei tseng for adding 869.85(EU) from OEMCfg, 2010/08/02
		iRegionIndex = (unsigned char)btRegion.at(0);
		ui.regionBox->clear();

		//load the region from file and add to the regionBox
		QFile   region(QDir::currentPath()+"/region");
		QString regionLine[50];           //scan the file line by line	
		bool    bRegionMatch = false;

		//Open region file
		if( region.open(QIODevice::ReadOnly) == false)
		{
			QMessageBox::information( 0, 
									  "Get region file",
									  "Get region file fail", 
									  QMessageBox::Ok );

			break;
		}
		QTextStream scanRegion(&region);
	
		
		//Push support region to region comboBox
		for(iIndex=0; !scanRegion.atEnd(); iIndex++)
		{			
			regionLine[iIndex] = scanRegion.readLine();

			//Only add support region
			if( iRegionSupport & (0x01 << iIndex) )
			{
				ui.regionBox->addItem( regionLine[iIndex], QVariant(iIndex) );

				//Set region combox index
				if( iIndex == iRegionIndex)
				{
					ui.regionBox->setCurrentIndex(  ui.regionBox->findData( QVariant(iRegionIndex) ) ); 
					bRegionMatch = true;
				}
			}
		}		
		region.close();
		

		//Check whether region sel and support region match or not.
		if( bRegionMatch == false )
		{		
			//If can't match, show "UNKNOW"
			ui.regionBox->addItem( "UNKOWN", QVariant(iIndex) );
			ui.regionBox->setCurrentIndex( ui.regionBox->findData( QVariant(iIndex) ) ); 

			QMessageBox::information( 0, 
									  "Region doesn't match",
									  "Region doesn't match support region", 
									  QMessageBox::Ok );

			break;
		}



//Region Data==================================================		
		
		//load the freq data from file to match the region in regionBox
		QFile regionFile(QDir::currentPath()+"/regionData");
		QTextStream scanFile(&regionFile);
		QString scanLine;           //scan the file line by line and compare with [compLine]
		QString compLine = ui.regionBox->currentText();//compare with [scanLine], to find the match region String ex. EU, CN
		QString line[5];            //for storing 'region' 'freq' 'totalChannel' 'stepChannel', and the final space line
		
		regionFile.open(QIODevice::ReadOnly);
		do
		{
			scanLine = scanFile.readLine();
			
			if((QString::compare(scanLine, compLine, Qt::CaseInsensitive)) == 0)
			{
				line[0] = scanLine;//add by yo chen, filter some channels for Brazil, 2011/06/09				
				//use .isEmpty to determine the space line, then stop scanning
				for(int i=1; !scanLine.isEmpty(); i++)
				{					
					scanLine = scanFile.readLine();					
					line[i] = scanLine;
				}
				break;
			}
		}while(!scanFile.atEnd());
		regionFile.close();
		


		//declare the parameters for converting [QString] to [int]
		QByteArray freqBA, totalChannelBA, stepChannelBA;    
		char *freqChar, *totalChannelChar, *stepChannelChar;
		//1. QString to QByteArray then to char
		freqBA = line[1].toLocal8Bit();
		totalChannelBA = line[2].toLocal8Bit();
		stepChannelBA = line[3].toLocal8Bit();
		//2. QByteArray to char
		freqChar = freqBA.data();
		totalChannelChar = totalChannelBA.data();
		stepChannelChar = stepChannelBA.data();
		//3. char to int or double
		freq = atof(freqChar);
		totalChannel = atoi(totalChannelChar);
		stepChannel = atof(stepChannelChar);
		//end by yo chen for using readfile to read the region data, 2010/11/02

		//Add by yingwei tseng for get frequency error in EU1, 2010/12/15
        ui.setFrequencyBox->clear();
		ui.setFrequencyBox->addItem("None");	
		//End by yingwei tseng for get frequency error in EU1, 2010/12/15
		for (int k = 0; k < totalChannel; k++ )
		{
			//mod by yo chen, filter some channel for Brazil, 2011/06/09
			if(line[0] == "Brazil")
			{
				if(freq <= 907.25 || 915.25 <= freq)
				{
					ui.setFrequencyBox->addItem(QString::number(freq));
				}
				freq = freq + stepChannel;				
			}
			else
			{
				ui.setFrequencyBox->addItem(QString::number(freq));
				freq = freq + stepChannel;	
			}
		}
		//End by yingwei tseng for add TW/CN/KR/JP region, 2010/04/29

	}while(0);

//=============Model name====================================
	
	QByteArray btAddr, btData, OemData;

	btAddr = QByteArray::fromHex("0030");
	for(int i = 0; i<16; i++)//Byte Length of Model Name
	{
		if( QrfeReaderInterface::OK != m_ph->getMacFirmwareOemCfg(btAddr, btData) )
		{
			ui.lineEdit->setText(QString("-- Could not get Model Name --"));
	        showNOK();			
			break;
		}
		else
		{
			OemData=OemData.append(btData[0]);
		}
		btAddr[1] = btAddr[1] + 1;
	}
	ui.lineEdit->setText(OemData);
	//1.QByteArray to QString
	QString moduleNameForCommunicationPort = QString(OemData);
	moduleNameForCommunicationPort = moduleNameForCommunicationPort.left(6);
	if(moduleNameForCommunicationPort == "RU-888")
	{		
		ui.CommunicationUARTradioButton->setEnabled(true);
		ui.CommunicationUSBradioButton->setEnabled(true);

	}
	else
	{
		//ui.setFrequencyBox->setEnabled(false);
		ui.CommunicationUARTradioButton->setEnabled(false);
		ui.CommunicationUSBradioButton->setEnabled(false);
		ui.setCommunicationPortButton->setEnabled(false);
	}
	OemData.clear();

//=============Serial Number==================================

	btAddr = QByteArray::fromHex("0050");
	for(int i = 0; i<15; i++)
	{
		if( QrfeReaderInterface::OK != m_ph->getMacFirmwareOemCfg(btAddr, btData) )
		{
			ui.lineEdit_2->setText(QString("-- Could not get Serial Number --"));
	        showNOK();			
			break;
		}
		else
		{
			//ModelNameData[i]=btData[0];
			OemData=OemData.append(btData[0]);
		}
		btAddr[1] = btAddr[1] + 1;
	}
	ui.lineEdit_2->setText(OemData);
	OemData.clear();

//=============Firmware version================================
	QString firmwareVersionNum;
	if( QrfeReaderInterface::OK != m_ph->getSoftwareRevision(firmwareVersionNum) )
	{
		ui.lineEdit_4->setText(QString("-- Could not get Firmware Version --"));
	    showNOK();			
	}
	else
	{
		ui.lineEdit_4->setText(firmwareVersionNum.right(5));
	}

//=============OEMCfgManufactureVersionNumber===================
	QString manuVersionNum;
	if( QrfeReaderInterface::OK != m_ph->getOEMCfgMVN(manuVersionNum) )
	{
		ui.lineEdit_5->setText(QString("-- Could not get OEMCfg Manufacture Version Number --"));
	    showNOK();		
	}
	else
	{
	    ui.lineEdit_5->setText(manuVersionNum.right(4));
	}

//=============OEMCfgUpdatePackInformationCode===================
    QString updatePackInformationCode;
	if( QrfeReaderInterface::OK != m_ph->getOEMCfgUPIC(updatePackInformationCode) )
	{
	    ui.lineEdit_6->setText(QString("-- Could not get OEMCfg Update Pack Information Code --"));
	    showNOK();
	}
	else
	{
		ui.lineEdit_6->setText(updatePackInformationCode.mid(26,2));
	}


//=============Power level====================================
	
	//clark 2011.12.15 Set Power level range in the different device.
	int iMax = 0;

	//clark 2012.1.12 Clear Power level comboBox
	ui.powerBox->clear();

	switch( r_clsReaderInterface->devType() )		
    {
		case QrfeReaderInterface::ENUM_PRODUCT::USB_MODULE:
			ui.proDongle->hide();

			iIndex = (int)QrfeReaderInterface::ENUM_POWER::USB_MODULE_MIN;
			iMax   = (int)QrfeReaderInterface::ENUM_POWER::USB_MODULE_MAX;

			for ( ; iIndex <=  iMax; iIndex++)
			    ui.powerBox->addItem(QString::number(iIndex));	

			break;

	    case QrfeReaderInterface::ENUM_PRODUCT::USB_DONGLE:
			ui.proModoule->hide();

			iIndex = (int)QrfeReaderInterface::ENUM_POWER::USB_DONGLE_MIN;
			iMax   = (int)QrfeReaderInterface::ENUM_POWER::USB_DONGLE_MAX;
			
			for ( ; iIndex <=  iMax; iIndex++)
			    ui.powerBox->addItem(QString::number(iIndex));	

			break;

	    case QrfeReaderInterface::ENUM_PRODUCT::RU_865P:
			ui.proModoule->hide();

			iIndex = (int)QrfeReaderInterface::ENUM_POWER::RU_865P_MIN;
			iMax   = (int)QrfeReaderInterface::ENUM_POWER::RU_865P_MAX;
			
			for ( ; iIndex <=  iMax; iIndex++)
			    ui.powerBox->addItem(QString::number(iIndex));	

			break;

		default:		
			break;
    }


    //Add by yingwei tseng for module and USB dongle power range, 2010/07/05
	if (ui.freqFixedButton->isChecked()){
		ui.setFrequencyBox->setEnabled(true);
		ui.modulationBox->setEnabled(true);
	}	
    //End by yingwei tseng for module and USB dongle power range, 2010/07/05

    
	if (ui.freqHoppingButton->isChecked()){
		ui.setFrequencyBox->setEnabled(false);
		ui.modulationBox->setEnabled(false);
	}
	ui.fixedPowerOffButton->setEnabled(false);
	//End by yingwei tseng for frequency, 2010/01/04
	//Del by yingwei tseng for Gen2 setting, 2010/03/23
	#if 0
	success = success && getGen2Config();	
	#endif
	//End by yingwei tseng for Gen2 setting, 2010/03/23
	//success = success && getOutputLevel();
	success = success && getSensLevel();
	//success = success && getFreqs();    //Add by yingwei tseng for USB/UART offline problem, 2010/11/11
	success = success && getPowerLevel();//Add by yingwei tseng for get power level, 2010/08/02
	if (success)
	{
	    //Mod by yingwei tseng for modifying GUI style, 2010/08/30
		//ui.infoEdit->setText("-- Successfully read information from chip --");
		ui.infoEdit->setText("-- Reader successfully configured --");
		//End by yingwei tseng for modifying GUI style, 2010/08/30
		showOK();
	}
	 //Add by yingwei tseng for modulation tseting, 2010/05/10
	loadModulation();
	if (ui.modulationBox->currentIndex() == 0)
	{
		ui.transPatternTimeBox->setEnabled(false);
		ui.transPatternProgressBar->setEnabled(false);
	}
	else
	{
		ui.transPatternTimeBox->setEnabled(true);
		ui.transPatternProgressBar->setEnabled(true);
	}
	//End by yingwei tseng for modulation tseting, 2010/05/10	
	//Add by yingwei tseng for printer application, 2010/05/27
	m_reader.insert(r_clsReaderInterface->readerId(), r_clsReaderInterface);
	m_activeReader = m_reader.keys();
	clearTagInfo();
	//End by yingwei tseng for printer application, 2010/05/27

	setAntennaPortTransmitTime();    //Add by yingwei tseng for pulse modulation tseting, 2010/10/12

    //Del by yingwei tseng for show Gen2 items, 2011/03/25
    //Add by yingwei tseng for hiding items, 2010/12/08
	//ui.linkBox->setEnabled(false);
	//ui.millerBox->setEnabled(false);
	//End by yingwei tseng for hiding items, 2010/12/08
    //End by yingwei tseng for show Gen2 items, 2011/03/25

	pb->increasePorgressBar();
	pb->hide();
	delete pb;

	return QDialog::exec();
}

void CAmsLeoDialog::tryAccept()
{
	QDialog::accept();
}

void CAmsLeoDialog::setInventoryInterval()
{
	clearInfo();
	m_ph->setIntervalTime(ui.inventoryIntervalBox->value());
	ui.infoEdit->setText("-- Inventory Interval Time set to " + QString::number(m_ph->intervalTime()) + " --");
	showOK();
}


void CAmsLeoDialog::setFrequency()
{
//Del by yingwei tseng for frequency, 2010/01/04
#if 0
	QByteArray ba;
	QList<ulong> freqs;
	ulong start,end,increment,freq;
	uchar mode = 8;// first clear it

	if(ui.startFrequencyBox->value() > ui.endFrequencyBox->value())
	{
		ui.endFrequencyBox->setValue(ui.startFrequencyBox->value());
	}
	start = (ulong) 1000 * ui.startFrequencyBox->value();
	end = (ulong) 1000 * ui.endFrequencyBox->value();
	increment = (ulong) 1000 * ui.incrementBox->value();

	ui.infoEdit->setText("");

	srand(QTime::currentTime().msec());
	for ( freq = start; freq <= end; freq += increment)
	{ /* add to the frequency list */
		int sz = freqs.size()+1;	
		freqs.insert(rand()%sz,freq);
	}
	
	foreach( freq, freqs )
	{	
		if(m_ph->setFrequency(freq,mode, ui.rssiThresholdBox->value(),ui.hoppingBox->currentIndex(),ba) == QrfeReaderInterface::OK 
			&& ba.length()>=2 
			&& ba.at(1)==(char)0xff)
		{
			ui.infoEdit->append("-- Added Frequency " + QString("%1").arg(freq) + " --");
			showOK();
		}
		else{
			ui.infoEdit->append("-- Could not add Frequency " + QString("%1").arg(freq) + " --");
			showNOK();
		}
		mode=4; // we already cleared, now add the frequencies
	}
	setAllocationParams();	
#endif
//End by yingwei tseng for frequency, 2010/01/04
}
bool CAmsLeoDialog::getGen2Config()
{
	bool success = true;
	int lf_khz = 40;
	int coding = 2;
	int session = 0;
	bool longPilot = true;
	int qbegin = 4;
	int index;
	QrfeReaderInterface::Result res = QrfeReaderInterface::ERROR;

	res = m_ph->setGen2Config(false,&lf_khz,false, &coding,false, &session,false, &longPilot,false, &qbegin);

	if ( res != QrfeReaderInterface::OK)
	{
		ui.infoEdit->append(QString("-- Error getting gen2Config --"));
		success = false;
		showNOK();
	}

	index = ui.linkBox->findText(QString("%1 kHz").arg(lf_khz));
	if (index == -1) success = false;
	ui.linkBox->setCurrentIndex(index);

	if (coding == 0) index = ui.millerBox->findText("FM0");
	else index = ui.millerBox->findText(QString("Miller %1").arg(coding));
	if (index == -1) success = false;
	ui.millerBox->setCurrentIndex(index);

	ui.qbegin->setValue(qbegin);

	if (session == 4) index = ui.sessionBox->findText("SL");
	else index = ui.sessionBox->findText(QString("S%1").arg(session));
	if (index == -1) success = false;
	ui.sessionBox->setCurrentIndex(index);
	
	ui.pilotToneBox->setChecked(longPilot);

	return success;
}

bool CAmsLeoDialog::getSensLevel()
{
	bool success = false;
	signed char sens;
	QrfeReaderInterface::Result res = QrfeReaderInterface::ERROR;
	res = m_ph->setGetSensitivity(&sens, false);
	ui.sensSlider->setValue(sens);

	if ( res != QrfeReaderInterface::OK)
	{
		ui.infoEdit->setText(QString("-- Could not get sensitivity to --"));
		showNOK();
	}
	else
	{
		success = true;
	}
	return success;
}

bool CAmsLeoDialog::getOutputLevel()
{
	bool success = false;
	signed int coarse, fine;
	QrfeReaderInterface::Result res = QrfeReaderInterface::ERROR;
	QByteArray in;

	res = m_ph->getParam(0x15, in);

	if (res != QrfeReaderInterface::OK)
	{
		ui.infoEdit->setText("-- Could not read output level --");
		showNOK();
		success = false;
		return success;
	}
	success = true;
	coarse = (in[0]&0x18) >> 3;
	fine = in[0] & 0x07;

	ui.levelSlider->setValue(-coarse*6-fine);
	ui.levelLabel->setText(QString("%1-%2=%3").arg(-coarse*6).arg(fine).arg(-coarse*6-fine));

	return success;
}

bool CAmsLeoDialog::getFreqs()
{
	ulong start, stop;
	ushort idleTime, listenTime, allocationTime;
	signed char rssi;
	uchar num_freqs, profile;

	QrfeReaderInterface::Result res = QrfeReaderInterface::ERROR;
	res = m_ph->getFrequencies( &start, &stop, &num_freqs, &idleTime,  &listenTime,  &allocationTime, &rssi, &profile);
	if ( res )
	{
		ui.infoEdit->setText("-- Could not read frequencies --");
		showNOK();
		return false;
	}
	if(profile != 0)
	{
		ui.hoppingBox->setCurrentIndex(profile);
	}
	else
	{
		ui.startFrequencyBox->setValue(start / 1000.0);
		ui.endFrequencyBox->setValue(stop / 1000.0);
		ui.idleTimeBox->setValue(idleTime);
		ui.listenTimeBox->setValue(listenTime);
		ui.allocationTimeBox->setValue(allocationTime);
		ui.rssiThresholdBox->setValue(rssi);
		if (num_freqs > 1)
		{
			ui.incrementBox->setValue((ui.endFrequencyBox->value() - ui.startFrequencyBox->value())/(num_freqs-1));
		}
	}

	return true;
}

//Add by yingwei tseng for get power level, 2010/08/02
bool CAmsLeoDialog::getPowerLevel()
{
    int pl;
    QrfeReaderInterface::Result res = QrfeReaderInterface::ERROR;
	res = m_ph->AntennaPortGetPowerLevel(pl);
	if (res != QrfeReaderInterface::OK)
	{
		ui.infoEdit->setText("-- Could not read power level --");
		showNOK();
		return false;
	}

    pl = pl - ui.powerBox->itemText(0).toInt();
	
    ui.powerBox->setCurrentIndex(pl);

	return true;
}
//End by yingwei tseng for get power level, 2010/08/02
void CAmsLeoDialog::setGen2Configuration()
{
	bool ok;
	int lf_khz = 40;
	int coding = 2;
	int session = 0;
	bool longPilot = true;
	int qbegin = 4;
	QrfeReaderInterface::Result res = QrfeReaderInterface::ERROR;

	lf_khz = ui.linkBox->currentText().split(" ").at(0).toInt();
	coding = ui.millerBox->currentText().right(1).toInt(&ok);
	if (!ok) coding = 2;
	qbegin = ui.qbegin->value();

	session = ui.sessionBox->currentText().right(1).toInt(&ok);
	if (!ok) session = 4;

	longPilot = ui.pilotToneBox->isChecked();

	res = m_ph->setGen2Config(true,&lf_khz,true, &coding,true, &session,true, &longPilot,true, &qbegin);

	if ( res == QrfeReaderInterface::OK)
	{
	    //Mod by yingwei tseng for modifying GUI style, 2010/08/30
		//ui.infoEdit->setText(QString("-- Set specified configuration : lf=%1 cod=%2 session=%3 pilot=%4 qbegin=%5 --").arg(lf_khz).arg(coding).arg(session).arg(longPilot).arg(qbegin));
		ui.infoEdit->append(QString("-- Set specified configuration : lf=%1 cod=%2 session=%3 pilot=%4 qbegin=%5 --").arg(lf_khz).arg(coding).arg(session).arg(longPilot).arg(qbegin));
        //End by yingwei tseng for modifying GUI style, 2010/08/30
		showOK();
	}
	else
	{
	    //Mod by yingwei tseng for modifying GUI style, 2010/08/30
		//ui.infoEdit->setText(QString("-- Could not specified configuration : lf=%1 cod=%2 session=%3 pilot=%4 qbegin=%5 --").arg(lf_khz).arg(coding).arg(session).arg(longPilot).arg(qbegin));
		ui.infoEdit->append(QString("-- Could not specified configuration : lf=%1 cod=%2 session=%3 pilot=%4 qbegin=%5 --").arg(lf_khz).arg(coding).arg(session).arg(longPilot).arg(qbegin));
        //End by yingwei tseng for modifying GUI style, 2010/08/30 
		showNOK();
	}
	return;
}

void CAmsLeoDialog::setTagMode()
{
//Del by yingwei tseng for tag mod, 2010/03/19
#if 0
	QrfeReaderInterface::Result res = QrfeReaderInterface::ERROR;
	if(ui.tagModeBox->currentIndex() == 0){
		res = m_ph->setTagType(QrfeReaderInterface::TAG_GEN2);
		m_ph->setRssiEnabled(false);
	}
	else if(ui.tagModeBox->currentIndex() == 1){
		res = m_ph->setTagType(QrfeReaderInterface::TAG_GEN2);
		m_ph->setRssiEnabled(true);
	}
	else if(ui.tagModeBox->currentIndex() == 2)
	{
		res = m_ph->setTagType(QrfeReaderInterface::TAG_ISO6B);
		m_ph->setRssiEnabled(false);
	}
	else
		return;

	if(res == QrfeReaderInterface::OK)
	{
		ui.infoEdit->setText("-- Set TagType to " + ui.tagModeBox->currentText() + " --");
		showOK();
		return;
	}
	else
	{
		ui.infoEdit->setText("-- Could not set TagType to " + ui.tagModeBox->currentText() + " --");
		showNOK();
		return;
	}
#endif
//End by yingwei tseng for tag mod, 2010/03/19
}
void CAmsLeoDialog::sendCommand()
{
	QrfeReaderInterface::Result res;
	QString s = ui.commandBox->currentText();
	int cmd = s.mid(s.length()-2,2).toInt(0,16);

	res = m_ph->setParam(cmd,QByteArray(3,0));

	if (res != QrfeReaderInterface::OK)
	{
		ui.infoEdit->setText(QString("-- Could not send command 0x%1 --").arg(cmd,0,16));
		showNOK();
		return;
	}
	else
	{
		ui.infoEdit->setText(QString("-- Sent command 0x%1 --").arg(cmd,0,16));
		showOK();
		return;
	}
}

void CAmsLeoDialog::setLevel()
{
	QrfeReaderInterface::Result res;
	QByteArray in,out;

	res = m_ph->getParam(0x15, in);

	if (res != QrfeReaderInterface::OK)
	{
		ui.infoEdit->setText("-- Could not read 0x15 register --");
		showNOK();
		return;
	}
	out = in;
	out[0] = out[0] & 0xe0;
	out[0] = out[0] |  m_tx_lev_coarse<<3 | m_tx_lev_fine;

	res = m_ph->setParam(0x15, out);

	if (res != QrfeReaderInterface::OK)
	{
		ui.infoEdit->setText("-- Could not set 0x15 register --");
		showNOK();
		return;
	}


	ui.infoEdit->setText("-- Set 0x15 register from " + 
		in.mid(2,1).toHex() +
		in.mid(1,1).toHex() +
		in.mid(0,1).toHex() +
		" to " + 
		out.mid(2,1).toHex() +
		out.mid(1,1).toHex() +
		out.mid(0,1).toHex() +
		" --");
	showOK();
	

}
void CAmsLeoDialog::levelSliderChanged(int val)
{
	QString text;
	int v=val;

	m_tx_lev_coarse = 0;

	if (val <= -12)
	{
		text.append("-12");
		v += 12;
		m_tx_lev_coarse = 2;
	}
	else if (val <= -6)
	{
		text.append("-6");
		v += 6;
		m_tx_lev_coarse = 1;
	}
	m_tx_lev_fine = -v;
	if(v==0) text.append("+");
	text.append(""+QString::number(v));
	text.append("="+QString::number(val));

	ui.levelLabel->setText(text);
}

void CAmsLeoDialog::sensSliderChanged(int val)
{
	this->ui.sensLText->setText(QString("%1 dBm").arg(val));
}

void CAmsLeoDialog::setAllocationParams()
{
	ushort idleTime = ui.idleTimeBox->value();
	ushort listenTime = ui.listenTimeBox->value();
	ushort allocationTime = ui.allocationTimeBox->value();

	QrfeReaderInterface::Result res = m_ph->setAllocationParams(listenTime,allocationTime,idleTime);

	ui.infoEdit->append(QString("Set idle=%1 ms listen=%2 ms sending=%3 ms").arg(idleTime).arg(listenTime).arg(allocationTime));
	if (res == QrfeReaderInterface::OK)
	{
		showOK();
	}
	else
	{
		showNOK();
	}
}
void CAmsLeoDialog::displayPlot(double* ivals, double *qvals, double *svals, double *freqs, int size)
{
	QwtPlot *plot  = new QwtPlot();
	QwtPlotCurve icurve("I"),qcurve("Q"),scurve("dBm");
	QDialog plotDiag(this);

	QBoxLayout layout(QBoxLayout::LeftToRight);
	icurve.setData(freqs,ivals,size); icurve.setPen(QPen(Qt::DotLine));
	qcurve.setData(freqs,qvals,size); qcurve.setPen(QPen(Qt::DashLine));
	scurve.setData(freqs,svals,size); scurve.setPen(QPen("red"));
	
	icurve.attach(plot); qcurve.attach(plot); scurve.attach(plot);
	layout.addWidget(plot);
	plotDiag.setLayout(&layout);

	plot->insertLegend(new QwtLegend(NULL), QwtPlot::RightLegend);
	plot->replot();
	plotDiag.exec();
	icurve.detach(); qcurve.detach(); scurve.detach();
	delete plot; 
}

void CAmsLeoDialog::sweepFrequencys()
{
//Del by yingwei tseng for frequency, 2010/01/04
#if 0
	ulong startFreq = ui.sweepStartSpinBox->value() * 1000;
	ulong stopFreq = ui.sweepStopSpinBox->value() * 1000;
	ulong steps = ui.sweepStepSpinBox->value() * 1000;
	ulong size = (stopFreq-startFreq)/steps + 1;
	double *ivals = new double[size];
	double *qvals = new double[size];
	double *svals = new double[size];
	double *freqs = new double[size];
	double G = m_ph->getG_rfp();
	int i; ulong freq;
	bool rssi = ui.sweepModeBox->currentIndex() == 0;
	QString mode = (rssi)?"rssi":"reflected power";

	if(stopFreq < startFreq)
	{
		QMessageBox::critical(this, "Error", "The start value for the sweep must be lower than the stop value!");
		goto exit;
	}

	ui.infoEdit->setText("-- Starting sweep with " + mode + " --");
	for(i=0, freq = startFreq
		; freq <= stopFreq
		; freq += steps,i++)
	{
		QByteArray result;
		QrfeReaderInterface::Result res = QrfeReaderInterface::ERROR;
		res = m_ph->setFrequency(freq, rssi?1:2, 0, 0, result);
		if(res != QrfeReaderInterface::OK){
			ui.infoEdit->append(QString::number(freq)+ "; ");
			continue;
		}
		ivals[i] = result.at(0);
		qvals[i] = result.at(1);
		svals[i] = result.at(2);

		if (!rssi)
		{
			svals[i] = sqrt(ivals[i]*ivals[i]+qvals[i]*qvals[i]);
			if (svals[i] == 0) svals[i] = 0.5;
			svals[i] = 20*log10(svals[i]/G);
		}

		freqs[i] = freq;
		if (rssi)
			ui.infoEdit->append(QString::number(freq)+ "; " + QString("i=%1 q=%2, total %3 dBm")
								.arg((uchar)result.at(0))
								.arg((uchar)result.at(1))
			                    .arg((int)((char)result.at(2)))
								);
		else
			ui.infoEdit->append(QString::number(freq)+ "; " + QString("%1; %2").arg((int)result.at(0)).arg((int)result.at(1)));

		qApp->processEvents();
	}
	showOK();
	displayPlot(ivals,qvals,svals,freqs,size);
exit:
	delete ivals; delete qvals; delete svals;
#endif
//End by yingwei tseng for frequency, 2010/01/04
}

void CAmsLeoDialog::openRFPDialog(void)
{
	CAmsLeoReflectedPower diag;
	diag.exec(m_ph,ui.frequencyBox->value());
}

void CAmsLeoDialog::setSensitivity(void)
{
	signed char sens = this->ui.sensSlider->value();
	signed char sens_old = sens;
	QrfeReaderInterface::Result res = QrfeReaderInterface::ERROR;
	res = m_ph->setGetSensitivity(&sens, true);

	if ( res == QrfeReaderInterface::OK)
	{
		ui.infoEdit->setText(QString("-- Desired sensitivity %1 dBm resulted in %2  --").arg(sens_old).arg(sens));
		showOK();
		this->ui.sensSlider->setValue(sens);
	}
	else
	{
		ui.infoEdit->setText(QString("-- Could not set sensitivity to %1 --").arg(sens_old));
		showNOK();
	}
}

void CAmsLeoDialog::continuousSend(void)
{
	ushort timeout_ms = ui.timeoutBox->value();
	double freq = ui.frequencyBox->value();

	QrfeReaderInterface::Result res = m_ph->continuousSend(freq*1000,timeout_ms);

	if (timeout_ms == 0)
	{
		ui.infoEdit->append(QString("Modulating continuously until next USB command on freq %2")
			.arg(freq));
	}
	else
	{
		ui.infoEdit->append(QString("Modulating continuously for %1 ms on freq %2")
			.arg(timeout_ms).arg(freq));
	}
	if (res == QrfeReaderInterface::OK)
	{
		showOK();
	}
	else
	{
		showNOK();
	}

}

void CAmsLeoDialog::setHopper( const QString & text)
{
	QStringList sl = profiles.value(text);
	if (sl.empty()) return;
	QList<double> freqs;
	foreach(QString s, sl)
	{
		s.toDouble();
		freqs.append(s.toDouble());
	}
	for ( int i = freqs.length(); i<7; i++)
	{
		freqs.append(55.5);
	}

	ui.startFrequencyBox->blockSignals(true);
	ui.endFrequencyBox->blockSignals(true);
	ui.incrementBox->blockSignals(true);
	ui.rssiThresholdBox->blockSignals(true);
	ui.listenTimeBox->blockSignals(true);
	ui.idleTimeBox->blockSignals(true);
	ui.allocationTimeBox->blockSignals(true);

	ui.startFrequencyBox->setValue(freqs.takeFirst());
	ui.endFrequencyBox->setValue(freqs.takeFirst());
	ui.incrementBox->setValue(freqs.takeFirst());
	ui.rssiThresholdBox->setValue(freqs.takeFirst());
	ui.listenTimeBox->setValue(freqs.takeFirst());
	ui.idleTimeBox->setValue(freqs.takeFirst());
	ui.allocationTimeBox->setValue(freqs.takeFirst());

	ui.startFrequencyBox->blockSignals(false);
	ui.endFrequencyBox->blockSignals(false);
	ui.incrementBox->blockSignals(false);
	ui.rssiThresholdBox->blockSignals(false);
	ui.listenTimeBox->blockSignals(false);
	ui.idleTimeBox->blockSignals(false);
	ui.allocationTimeBox->blockSignals(false);

}
void CAmsLeoDialog::oneFreqChanged(double)
{
	ui.hoppingBox->setCurrentIndex(0);
}

void CAmsLeoDialog::oneFreqChanged(int)
{
	ui.hoppingBox->setCurrentIndex(0);
}


void CAmsLeoDialog::clearInfo()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::white));
	ui.infoEdit->setPalette(pal);
	ui.infoEdit->clear();
}

void CAmsLeoDialog::showOK()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::green).lighter());
	ui.infoEdit->setPalette(pal);
}

void CAmsLeoDialog::showNOK()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::red).lighter());
	ui.infoEdit->setPalette(pal);
}

void CAmsLeoDialog::readProfiles()
{
	QFile qf("profiles.csv");
	QString line;
	QStringList sl;
	if (!qf.open(QIODevice::ReadOnly)) return;
	if(!qf.atEnd()) qf.readLine(); // skip heading row
	while(!qf.atEnd())
	{
		line = qf.readLine();
		sl = line.split(",");
		ui.hoppingBox->addItem(sl.first());
		profiles.insert(sl.takeFirst(),sl);
	}
	ui.hoppingBox->setCurrentIndex(1);  
}
//Add by yingwei tseng for frequency, 2010/01/04
void CAmsLeoDialog::loadRegion()
{	
	//Mod by yingwei tseng for add TW/CN/KR/JP region, 2010/04/29
	int totalChannel;
	double freq, stepChannel;
		
	//add by yo chen for using readfile to read the region data, 2010/11/02
	QFile regionFile(QDir::currentPath()+"/regionData");
	QTextStream scanFile(&regionFile);
	QString scanLine;           //scan the file line by line and compare with [compLine]
	QString compLine = ui.regionBox->currentText();//compare with [scanLine], to find the match region String ex. EU, CN
	QString line[5];            //for storing 'region' 'freq' 'totalChannel' 'stepChannel', and the final space line
    regionFile.open(QIODevice::ReadOnly);
	do{
		scanLine = scanFile.readLine();
		if((QString::compare(scanLine, compLine, Qt::CaseInsensitive)) == 0){
			line[0] = scanLine;//add by yo chen, filter some channels for Brazil, 2011/06/09				
			//use .isEmpty to determine the space line, then stop scanning
			for(int i=1; !scanLine.isEmpty(); i++){					
				scanLine = scanFile.readLine();					
				line[i] = scanLine;
			}
			break;
		}
	}while(!scanFile.atEnd());
    regionFile.close();
	
    //declare the parameters for converting [QString] to [int]
    QByteArray freqBA, totalChannelBA, stepChannelBA;    
	char *freqChar, *totalChannelChar, *stepChannelChar;
    //1. QString to QByteArray then to char
    freqBA = line[1].toLocal8Bit();
    totalChannelBA = line[2].toLocal8Bit();
    stepChannelBA = line[3].toLocal8Bit();
	//2. QByteArray to char
    freqChar = freqBA.data();
    totalChannelChar = totalChannelBA.data();
    stepChannelChar = stepChannelBA.data();
    //3. char to int or double
    freq = atof(freqChar);
    totalChannel = atoi(totalChannelChar);
    stepChannel = atof(stepChannelChar);
	//end by yo chen for using readfile to read the region data, 2010/11/02

    //Add by yingwei tseng for get frequency error in EU1, 2010/12/15
	ui.setFrequencyBox->clear();
	ui.setFrequencyBox->addItem("None");
	//End by yingwei tseng for get frequency error in EU1, 2010/12/15
	for (int k = 0; k < totalChannel; k++ ){
		//mod by yo chen, filter some channel for Brazil, 2011/06/09
		if(line[0] == "Brazil"){
			if(freq <= 907.25 || 915.25 <= freq){
				ui.setFrequencyBox->addItem(QString::number(freq));
			}
			freq = freq + stepChannel;				
		}
		else{
			ui.setFrequencyBox->addItem(QString::number(freq));
			freq = freq + stepChannel;	
		}
	}
	//End by yingwei tseng for add TW/CN/KR/JP region, 2010/04/29

    setAntennaPortTransmitTime();    //Add by yingwei tseng for pulse modulation tseting, 2010/10/12
}
void CAmsLeoDialog::loadFreqType()
{
	loadRegion();    //Add by yingwei tseng for adding 869.85(EU) from OEMCfg, 2010/08/02
	if (ui.freqFixedButton->isChecked()){
		ui.setFrequencyBox->setEnabled(true);
		ui.modulationBox->setEnabled(true);
		//Add by yingwei tseng for modulation tseting, 2010/05/10
		if (ui.modulationBox->currentIndex() == 0){
			ui.transPatternTimeBox->setEnabled(false);
			ui.transPatternProgressBar->setEnabled(false);
		}else{
			ui.transPatternTimeBox->setEnabled(true);
			ui.transPatternProgressBar->setEnabled(true);
		}
		//End by yingwei tseng for modulation tseting, 2010/05/10
	}	

	if (ui.freqHoppingButton->isChecked()){
		ui.setFrequencyBox->setEnabled(false);
		ui.modulationBox->setEnabled(false);
		//Add by yingwei tseng for modulation tseting, 2010/05/10
		ui.transPatternTimeBox->setEnabled(false);
		ui.transPatternProgressBar->setEnabled(false);
		//End by yingwei tseng for modulation tseting, 2010/05/10
	}

	ui.fixedPowerOffButton->setEnabled(false);
}
void CAmsLeoDialog::setFixedPowerOff()
{
	QrfeReaderInterface::Result resPowerSetState = QrfeReaderInterface::OK;
	resPowerSetState = m_ph->setAntennaPortSetPowerState(false);	
	if (resPowerSetState == QrfeReaderInterface::OK){
		ui.infoEdit->append("-- Power off successfully. --");
		showOK();
	}else{
		ui.infoEdit->append("-- Power on unsuccessfully. --");
		showNOK();
	}
	ui.fixedPowerOffButton->setEnabled(false);
	enabledAdvancedItems();    //Add by yingwei tseng for modulation tseting, 2010/05/10

}	
//End by yingwei tseng for frequency, 2010/01/04
//Del by yingwei tseng for dongle region default setting, 2010/12/09
#if 0
//Add by yingwei tseng for module and USB dongle power range, 2010/07/05
void CAmsLeoDialog::loadProductPower()
{
	int k = 0;

    //Add by yingwei tseng for selecting product module/dngle, 2010/07/30
	QByteArray selProductAddr = QByteArray::fromHex("0080");
	QByteArray selProductValue;
	QrfeReaderInterface::Result resSelProduct = QrfeReaderInterface::ERROR;
	resSelProduct = m_ph->getMacFirmwareOemCfg(selProductAddr, selProductValue);		
	if (resSelProduct != QrfeReaderInterface::OK)
		ui.infoEdit->setText("-- Unsuccessfully get product(module/dongle) from OEM config --");

    switch(selProductValue.at(1))
    {
	    case USB_MODULE:
            ui.proModButton->setChecked(true); 	
			ui.proModButton->setEnabled(true);
			ui.proDongleButton->setEnabled(false);			
			break;
	    case USB_DONGLE:	
            ui.proDongleButton->setChecked(true); 
			ui.proModButton->setEnabled(false);
			ui.proDongleButton->setEnabled(true);
			break;
		default:
			ui.proModButton->setChecked(true);	
			ui.proModButton->setEnabled(true);
			ui.proDongleButton->setEnabled(false);				
			break;
    }			
	//End by yingwei tseng for selecting product module/dngle, 2010/07/30
	
	for (k = 0; k < MAXPOWER - 3; k++)
		ui.powerBox->removeItem(0);

	k = 0;
	ui.powerBox->addItem("None");
    if (ui.proModButton->isChecked()){
		for (k = 5; k <= MAXPOWER; k++)
			ui.powerBox->addItem(QString::number(k));		
    }

	if (ui.proDongleButton->isChecked()){
		for (k = 5; k <= MAXPOWER - 6; k++)
			ui.powerBox->addItem(QString::number(k));				
	}	
}
//End by yingwei tseng for module and USB dongle power range, 2010/07/05
#endif
//End by yingwei tseng for dongle region default setting, 2010/12/09
//Add by yingwei tseng for submit, 2009/12/03
void CAmsLeoDialog::setAdvancedReader()
{
	bool    check[10];    
	int     totalNum	     = 0;
	int     regionIdx		 = ui.regionBox->itemData( ui.regionBox->currentIndex() ).toInt();    //Add by yingwei tseng for region, 2009/12/03 
	int     freqIdx		     = ui.setFrequencyBox->currentIndex();    
	bool    freqTypeFixedIdx = ui.freqFixedButton->isChecked();
	int     powerIdx		 = ui.powerBox->currentIndex(); 
	QString powerLevel       = ui.powerBox->itemText(powerIdx);
	ushort  transPatternTime = ui.transPatternTimeBox->value(); 


	
	QString regionStr = QString( "%1" ).arg( regionIdx, 2, 16 );
	QByteArray pRegionDataConv = QByteArray::fromHex(regionStr.toAscii());		
	QrfeReaderInterface::Result resRegion = m_ph->setRegion(pRegionDataConv);
	if (resRegion == QrfeReaderInterface::OK)
	{
		ui.infoEdit->setText("-- Set region to " + ui.regionBox->currentText() + " --");
		//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
		//showOK();
		check[0] = true;  
		totalNum++;;
		//End by yingwei tseng for checking OK/NOK, 2010/12/08
	}
	else
	{
		ui.infoEdit->setText("-- Could not set region to " + ui.regionBox->currentText() + " --");
        //Mod by yingwei tseng for checking OK/NOK, 2010/12/08
		//showNOK();
		check[0] = false;
		totalNum++;;
		//End by yingwei tseng for checking OK/NOK, 2010/12/08
	}
	
	
	#if 0
	QString pRegionData = NULL;
	QByteArray pRegionDataConv = 0;
	if (regionIdx == 0){//US
		pRegionData = "00";
		pRegionDataConv = QByteArray::fromHex(pRegionData.toAscii());	
		QrfeReaderInterface::Result resRegion = m_ph->setRegion(pRegionDataConv);
		if (resRegion == QrfeReaderInterface::OK){
			ui.infoEdit->setText("-- Set region to " + ui.regionBox->currentText() + " --");
			showOK();
		}else{
			ui.infoEdit->setText("-- Could not set region to " + ui.regionBox->currentText() + " --");
			showNOK();
		}
	}else{//EU
		pRegionData = "01";
		pRegionDataConv = QByteArray::fromHex(pRegionData.toAscii());	
		QrfeReaderInterface::Result resRegion = m_ph->setRegion(pRegionDataConv);
		if (resRegion == QrfeReaderInterface::OK){
			ui.infoEdit->setText("-- Set region to " + ui.regionBox->currentText() + " --");
			showOK();
		}else{
			ui.infoEdit->setText("-- Could not set region to " + ui.regionBox->currentText() + " --");
			showNOK();
		}
	}
	#endif
	//End by yingwei tseng for region, 2009/12/03 
    //Add by yingwei tseng for antenna power, 2010/01/04
	//Set antenna power level
	//if (powerIdx != 0){    //Del by yingwei tseng for 5 dBm power level could not set, 2010/10/22	 	
	QrfeReaderInterface::Result resPowerState = QrfeReaderInterface::OK;
	//Mod by yingwei tseng for get power level, 2010/08/02
	//resPowerState = m_ph->setAntennaPortSetPowerLevel(powerLevel.toInt());
	resPowerState = m_ph->AntennaPortSetPowerLevel(powerLevel.toInt());	
	//End by yingwei tseng for get power level, 2010/08/02
	if (resPowerState == QrfeReaderInterface::OK)
	{
	    ui.infoEdit->append("-- Set power level to " + powerLevel + " --");
		//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
		//showOK();
		check[1] = true;
		totalNum++;
		//End by yingwei tseng for checking OK/NOK, 2010/12/08
	}
	else
	{
	    ui.infoEdit->append("-- Could not set power level to " + powerLevel + " --");
		//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
		//showNOK();
		check[1] = false;  
		totalNum++;
		//End by yingwei tseng for checking OK/NOK, 2010/12/08
	}	
	//}    //Del by yingwei tseng for 5 dBm power level could not set, 2010/10/22	 
	//Add by yingwei tseng for tag mod, 2010/03/19
	QrfeReaderInterface::Result res = QrfeReaderInterface::ERROR;
	if(ui.tagModeBox->currentIndex() == 0)
	{
		res = m_ph->setTagType(QrfeReaderInterface::TAG_GEN2);
		m_ph->setRssiEnabled(false);
	}
	else if(ui.tagModeBox->currentIndex() == 1)
	{
		res = m_ph->setTagType(QrfeReaderInterface::TAG_GEN2);
		m_ph->setRssiEnabled(true);
	}
	else if(ui.tagModeBox->currentIndex() == 2)
	{
		res = m_ph->setTagType(QrfeReaderInterface::TAG_ISO6B);
		m_ph->setRssiEnabled(false);
	}

	if(res == QrfeReaderInterface::OK)
	{
		ui.infoEdit->append("-- Set TagType to " + ui.tagModeBox->currentText() + " --");
		//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
		//showOK();
		check[2] = true;
		totalNum++;
		//End by yingwei tseng for checking OK/NOK, 2010/12/08
	}
	else
	{
		ui.infoEdit->append("-- Could not set TagType to " + ui.tagModeBox->currentText() + " --");
		//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
		//showNOK();
		check[2] = false; 
		totalNum++;
		//End by yingwei tseng for checking OK/NOK, 2010/12/08
	}
	//End by yingwei tseng for tag mod, 2010/03/19
		
	//add by rick for CommunicationPort ¡õ,2014-04-02
	QrfeReaderInterface::Result resCommPort = QrfeReaderInterface::ERROR;
	QByteArray CPAddr, CPData;
	CPAddr = QByteArray::fromHex("0081");
	if(ui.CommunicationUSBradioButton->isChecked())
	{//USB haz been chosen		
		CPData = QByteArray::fromHex("00");
		resCommPort = m_ph->setMacFirmwareOemCfg(CPAddr, CPData);
		if(QrfeReaderInterface::OK == resCommPort )
		{
			ui.infoEdit->append("-- Set Communication Port to USB --");
		}
		else
		{
			ui.infoEdit->append("-- Could not set Communication Port to USB --");
		}
	}
	else
	{//UART haz been chosen
		CPData = QByteArray::fromHex("01");
		resCommPort = m_ph->setMacFirmwareOemCfg(CPAddr, CPData);
		if(QrfeReaderInterface::OK == resCommPort )
		{
			ui.infoEdit->append("-- Set Communication Port to UART --");
		}
		else
		{
			ui.infoEdit->append("-- Could not set Communication Port to UART --");
		}
	}
	


	//add by rick for CommunicationPort ¡ô,2014-04-02

	signed char sens = this->ui.sensSlider->value();
	signed char sens_old = sens;
	QrfeReaderInterface::Result resSen = QrfeReaderInterface::ERROR;
	resSen = m_ph->setGetSensitivity(&sens, true);
	setGen2Configuration();

	if ( resSen == QrfeReaderInterface::OK)
	{
		ui.infoEdit->append(QString("-- Desired sensitivity %1 dBm resulted in %2  --").arg(sens_old).arg(sens));
        //Mod by yingwei tseng for checking OK/NOK, 2010/12/08
		//showOK();
		check[3] = true; 
		totalNum++;
		//End by yingwei tseng for checking OK/NOK, 2010/12/08
		this->ui.sensSlider->setValue(sens);
		
	}
	else
	{
		ui.infoEdit->append(QString("-- Could not set sensitivity to %1 --").arg(sens_old));
		//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
		//showNOK();
		check[3] = false; 
		totalNum++;
		//End by yingwei tseng for checking OK/NOK, 2010/12/08
	}	
	
	//Add by yingwei tseng for frequency, 2010/01/04
	if (freqTypeFixedIdx == true){		
		if (freqIdx != 0){		
			ulong freq = ui.setFrequencyBox->currentText().toDouble() * 1000;
			//Initial Frequency, Register : 4107 043E F50D 00
			if(m_ph->setFrequency(freq, ui.rssiThresholdBox->value()) == QrfeReaderInterface::OK){
				ui.infoEdit->append("-- Set frequency to " + ui.setFrequencyBox->currentText() + " --");
				//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
				//showOK();
				check[4] = true; 
				totalNum++;
				//End by yingwei tseng for checking OK/NOK, 2010/12/08
			}else{
				ui.infoEdit->append("-- Could not set frequency to " + ui.setFrequencyBox->currentText() + " --");
				//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
				//showNOK();
				check[4] = false; 
				totalNum++;
				//End by yingwei tseng for checking OK/NOK, 2010/12/08
			}
		}
		//Add by yingwei tseng for modulation tseting, 2010/05/10
		disabledAdvancedItems();
		//Mod by yingwei tseng for pulse modulation tseting, 2010/10/12
		//if (ui.modulationBox->currentIndex() == 1){
		if (ui.modulationBox->currentIndex() != 0){	
		//End by yingwei tseng for pulse modulation tseting, 2010/10/12	
			ui.fixedPowerOffButton->setEnabled(false);
			
			m_transPatternTimeout = ui.transPatternTimeBox->value()* 1000;

			/* Save the start time of the transmit pattern */
			m_transPatternStart = QTime::currentTime();

			if ( m_transPatternTimeout > 0){			
				/* Prepare progress bar */
				ui.transPatternProgressBar->setValue(0);
				ui.transPatternProgressBar->setMaximum(m_transPatternTimeout);			
				ui.transPatternProgressLabel->setText(QString::number(0) + " s");

				/* Start the timer for the given time out */
				m_transPatternTimer->start(m_transPatternTimeout);

				/* Start the helper timer, to increment the progress bar */
				QObject::connect(m_transPatternProgressTimer, SIGNAL(timeout()), this, SLOT(incrementTransPatternProgress()));
				m_transPatternProgressTimer->start();
			}else{
				ui.transPatternProgressBar->setDisabled(true);
				ui.transPatternProgressLabel->setText("Infinite");

				/* Start the helper timer, to only show the time the scan is running */
				QObject::connect(m_transPatternProgressTimer, SIGNAL(timeout()), this, SLOT(showTransPatternTime()));
				m_transPatternProgressTimer->start();		
			}
		}else{
			ui.fixedPowerOffButton->setEnabled(true);
		}
		//End by yingwei tseng for modulation tseting, 2010/05/10

//        setGen2Configuration();    //Add by yingwei tseng for Gen2 setting, 2010/07/22	 
		
		//Set modulation on/off
		QrfeReaderInterface::Result resPowerSetState = QrfeReaderInterface::OK;
		if (ui.modulationBox->currentIndex() == 0){
			//Mod by yingwei tseng for modulation tseting, 2010/05/10
			//Mod by yingwei for antenna-port operation mode from ox00 to 0x01, 2010/07/21
			//resPowerSetState = m_ph->setAntennaPortSetState(false, false);
			resPowerSetState = m_ph->setAntennaPortSetState(false, true);
			//End by yingwei for antenna-port operation mode from ox00 to 0x01, 2010/07/21
			//resPowerSetState = m_ph->setAntennaPortSetState(false);
			//End by yingwei tseng for modulation tseting, 2010/05/10
			if (resPowerSetState == QrfeReaderInterface::OK){
				ui.infoEdit->append("-- CW on successfully. --");
				//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
				//showOK();
				check[5] = true;
				totalNum++;
				//End by yingwei tseng for checking OK/NOK, 2010/12/08
			}else{
				ui.infoEdit->append("-- CW on unsuccessfully. --");
				//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
				//showNOK();
				check[5] = false;  
				totalNum++;
				//End by yingwei tseng for checking OK/NOK, 2010/12/08
			}
			//Add by yingwei tseng for modulation tseting, 2010/05/10
			//Set antenna power on/off
			QrfeReaderInterface::Result resPowerState = QrfeReaderInterface::OK;
			resPowerState = m_ph->setAntennaPortSetPowerState(true);	
			if (resPowerState == QrfeReaderInterface::OK){
				ui.infoEdit->append("-- Power on successfully. --");
				//Add by yingwei tseng for modifying GUI style, 2010/08/30
                ui.infoEdit->append("<font color=\"red\">-- If you would like to power off, you must click power Set button again. --</font>"); 
				//End by yingwei tseng for modifying GUI style, 2010/08/30	
				//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
				//showOK();
				check[6] = true;
				totalNum++;
				//End by yingwei tseng for checking OK/NOK, 2010/12/08
			}else{
				ui.infoEdit->append("-- Power on unsuccessfully. --");
				//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
				//showNOK();
				check[6] = false;  
				totalNum++;
				//End by yingwei tseng for checking OK/NOK, 2010/12/08
			}	
			//End by yingwei tseng for modulation tseting, 2010/05/10
		//Add by yingwei tseng for pulse modulation tseting, 2010/10/12		
		//}else{
		}else if(ui.modulationBox->currentIndex() == 1){
		 //End by yingwei tseng for pulse modulation tseting, 2010/10/12		
		 	
			//Mod by yingwei tseng for modulation tseting, 2010/05/10
			//Mod by yingwei for antenna-port operation mode from ox00 to 0x01, 2010/07/21
			//resPowerSetState = m_ph->setAntennaPortSetState(true, false);
			resPowerSetState = m_ph->setAntennaPortSetState(true, true);
			//End by yingwei for antenna-port operation mode from ox00 to 0x01, 2010/07/21
			//resPowerSetState = m_ph->setAntennaPortSetState(true);
			//End by yingwei tseng for modulation tseting, 2010/05/10
			if (resPowerSetState == QrfeReaderInterface::OK){
				ui.infoEdit->append("-- Modulation on successfully. --");
				//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
				//showOK();
				check[7] = true;
				totalNum++;
				//End by yingwei tseng for checking OK/NOK, 2010/12/08
			}else{
				ui.infoEdit->append("-- Modulation on unsuccessfully. --");
				//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
				//showNOK();
				check[7] = false;  
				totalNum++;
				//End by yingwei tseng for checking OK/NOK, 2010/12/08
			}	
			//mod by yo chen for (transPatternTime*100)!¡Ò(transPatternTime), so that transPatternTime is a "time" but not a "times", 2010/11/10
			QrfeReaderInterface::Result resPortTransmitPattern = m_ph->setAntennaPortTransmitPattern((transPatternTime));
			if (resPortTransmitPattern == QrfeReaderInterface::OK){
			    ui.infoEdit->append("-- Transmit Pattern on successfully. --");
				//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
				//showOK();
				check[8] = true;
				totalNum++;
				//End by yingwei tseng for checking OK/NOK, 2010/12/08
			}else{
				ui.infoEdit->append("-- Transmit Pattern on unsuccessfully. --");
				//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
				//showNOK();
				check[8] = false; 
				totalNum++;
				//End by yingwei tseng for checking OK/NOK, 2010/12/08
			}
		//Add by yingwei tseng for pulse modulation tseting, 2010/10/12	
        }else{
		    QrfeReaderInterface::Result resPortTransmitPulse = m_ph->setAntennaPortTransmitPulse((transPatternTime));
			if (resPortTransmitPulse == QrfeReaderInterface::OK){
			    ui.infoEdit->append("-- Transmit pulse on successfully. --");
				//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
			    //showOK();
				check[9] = true; 
				totalNum++;
				//End by yingwei tseng for checking OK/NOK, 2010/12/08
			}else{
				ui.infoEdit->append("-- Transmit pulse on unsuccessfully. --");
				//Mod by yingwei tseng for checking OK/NOK, 2010/12/08
			    //showNOK();
				check[9] = false;
				totalNum++;
				//End by yingwei tseng for checking OK/NOK, 2010/12/08
			}		
		}	
		//End by yingwei tseng for pulse modulation tseting, 2010/10/12				
	}
	//End by yingwei tseng for frequency, 2010/01/04

	//Add by yingwei tseng for checking OK/NOK, 2010/12/08
	int count = 0;
    for (int m = 0; m < totalNum; m++)
    {
        if (check[m] == true)
			count++;
    }

	if (count == totalNum)
		showOK();
	else
		showNOK();
	//End by yingwei tseng for checking OK/NOK, 2010/12/08
}
//End by yingwei tseng for submit, 2009/12/03
//Add by yingwei tseng for modulation tseting, 2010/05/10
void CAmsLeoDialog::incrementTransPatternProgress()
{
    ui.transPatternProgressBar->setValue(ui.transPatternProgressBar->value() + 1000);
	ui.transPatternProgressLabel->setText(QString::number(ui.transPatternProgressBar->value()/1000) + " s");
}
/*!
 * @brief Slot that prints out the current scan time.
 */
void CAmsLeoDialog::showTransPatternTime()
{
	QTime t(0,0);
	QString time = t.addSecs(qAbs(m_transPatternStart.secsTo(QTime::currentTime()))).toString("h:mm:ss");
	ui.transPatternProgressLabel->setText(time);
}

void CAmsLeoDialog::stopTransPattern()
{
    //Mod by yingwei tseng for pulse modulation tseting, 2010/10/12	
	//if (ui.modulationBox->currentIndex() == 1){
	if (ui.modulationBox->currentIndex() != 0){
	//End by yingwei tseng for pulse modulation tseting, 2010/10/12		
		enabledAdvancedItems();		
		ui.fixedPowerOffButton->setEnabled(false);
		
		if ( m_transPatternTimeout > 0){
			/* Stop the scan timer */
			m_transPatternTimer->stop();
			QObject::disconnect(m_transPatternProgressTimer, SIGNAL(timeout()), this, SLOT(incrementTransPatternProgress()));
			m_transPatternProgressTimer->stop();

			/* Set progress bar to maximum */
			ui.transPatternProgressBar->setValue(m_transPatternTimeout);
			ui.transPatternProgressLabel->setText(QString::number(m_transPatternTimeout/1000) + " s");	
		}else{
			/* Stop the scan timer */
			m_transPatternTimer->stop();

			/* Stop the helper timer */
			QObject::disconnect(m_transPatternProgressTimer, SIGNAL(timeout()), this, SLOT(showTransPatternTime()));
			m_transPatternProgressTimer->stop();

			/* Prepare progress bar */
			ui.transPatternProgressBar->setEnabled(true);
		}
	}	
}

void CAmsLeoDialog::disabledAdvancedItems()
{
	ui.tagModeBox->setEnabled(false);
	ui.sensSlider->setEnabled(false);
	ui.regionBox->setEnabled(false);
	ui.powerBox->setEnabled(false);
	ui.freqFixedButton->setEnabled(false);
	ui.freqHoppingButton->setEnabled(false);
	ui.modulationBox->setEnabled(false);
	ui.setFrequencyBox->setEnabled(false);
	ui.rssiThresholdBox->setEnabled(false);
	ui.listenTimeBox->setEnabled(false);
	ui.idleTimeBox->setEnabled(false);
	ui.allocationTimeBox->setEnabled(false);
	ui.linkBox->setEnabled(false);
	ui.sessionBox->setEnabled(false);
	ui.millerBox->setEnabled(false);
	ui.qbegin->setEnabled(false);
	ui.pilotToneBox->setEnabled(false);
	ui.setAdvReaderButton->setEnabled(false);
	ui.closeButton->setEnabled(false);
	ui.transPatternTimeBox->setEnabled(false);
	ui.transPatternProgressBar->setEnabled(false);
}

void CAmsLeoDialog::enabledAdvancedItems()
{
	ui.tagModeBox->setEnabled(true);
	ui.sensSlider->setEnabled(true);
	ui.regionBox->setEnabled(true);
	ui.powerBox->setEnabled(true);
	ui.freqFixedButton->setEnabled(true);
	ui.freqHoppingButton->setEnabled(true);
	ui.modulationBox->setEnabled(true);
	ui.setFrequencyBox->setEnabled(true);
	ui.rssiThresholdBox->setEnabled(true);
	ui.listenTimeBox->setEnabled(true);
	ui.idleTimeBox->setEnabled(true);
	ui.allocationTimeBox->setEnabled(true);
	ui.linkBox->setEnabled(true);
	ui.sessionBox->setEnabled(true);
	ui.millerBox->setEnabled(true);
	ui.qbegin->setEnabled(true);
	ui.pilotToneBox->setEnabled(true);
	ui.setAdvReaderButton->setEnabled(true);
	ui.closeButton->setEnabled(true);
	ui.transPatternTimeBox->setEnabled(true);
	ui.transPatternProgressBar->setEnabled(true);
}

void CAmsLeoDialog::loadModulation()
{
	if (ui.modulationBox->currentIndex() == 0){
		ui.transPatternTimeBox->setEnabled(false);
		ui.transPatternProgressBar->setEnabled(false);
	}else{
		ui.transPatternTimeBox->setEnabled(true);
		ui.transPatternProgressBar->setEnabled(true);
	}
}
//Enf by yingwei tseng for modulation tseting, 2010/05/10
//Add by yingwei tseng for printer application, 2010/05/27
void CAmsLeoDialog::changeLength(int len)
{
	QString inputMask;
	currentLength = len;
	for ( int i = 0; i< len; i++)
	{
		inputMask += QString("HH-HH-");
	}
	inputMask.chop(1);
	ui.newEpcEdit->setInputMask(inputMask);
}

void CAmsLeoDialog::clearTagInfo()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::white));
	ui.infoEdit->setPalette(pal);
	ui.infoEdit->clear();
}
void CAmsLeoDialog::setPrinter()
{
	ushort pwrIncreaseEpc = ui.pwrIncreaseEpcBox->value();
	ushort pwrIncreaseWrite = ui.pwrIncreaseWriteBox->value();
	ushort pwrStartStep = ui.pwrStartStepBox->value();
	int pwr = 0, epcWritePwr = 0;
	bool findEpcFlag = false;

	clearTagInfo();
	
#if 0
	/* Stop each reader */
	foreach(QString id, m_activeReader)
	{
		if(m_reader.contains(id)){
			m_reader.value(id)->stopCyclicInventory();
			m_readRateCalc->readerStoppedInventory(id);
		}
	}	

	//m_tagViewManager->clearTags();
	/* Stop the view from updating */
	m_tagViewManager->stopGuiUpdate();	
#endif	

	/* Now all reader are active */
	m_activeReader = m_reader.keys();

	/* Reset read rate calculation */
	m_readRateCalc->reset();
		
	/* Notify tag manager that a scan is started */
	m_tagManager->startScan();


	for (pwr = pwrStartStep; pwr <= MAXPOWER; (pwr = pwr + pwrIncreaseEpc))
	{	
		//setup power
		//Mod by yingwei tseng for get power level, 2010/08/02
		//QrfeReaderInterface::Result resPowerState = m_ph->setAntennaPortSetPowerLevel(pwr);
		QrfeReaderInterface::Result resPowerState = m_ph->AntennaPortSetPowerLevel(pwr);	
        //End by yingwei tseng for get power level, 2010/08/02
		if (resPowerState == QrfeReaderInterface::OK){			
			ui.infoEdit->append("-- Set power level to " + QString::number(pwr) + " --");
			showOK();
		}else{
			ui.infoEdit->append("-- Could not set power level to " + QString::number(pwr) + " --");
			showNOK();
			continue;
		}	
#if 0
	QMessageBox::information(0, "Printer Test",
		"pwr test", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);		
#endif

		/* Clear all reader and tag infos in the view */
		m_tagViewManager->clearAllReaderTagInfos();

		/* Start every reader */
		foreach(QString readerId, m_activeReader)
		{
			if(m_reader.contains(readerId)){
				m_reader.value(readerId)->startCyclicInventory();
				m_readRateCalc->readerStartedInventory(readerId);
				//QMap<QString, STagInfo> tagsOfReader = m_readerTagsInfo.value(readerId);
				QStringList allTagsOfReader = m_tagManager->getCurrentTagsOfReader(readerId);
				// and every tag of the reader
				foreach(QString tagId, allTagsOfReader)
				{
#if 0
	QMessageBox::information(0, "Printer Test",
		tagId, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
#endif
					ui.tagIdEdit->setText(tagId);
					findEpcFlag = true;
					break;
				}
			}	
		}

		/* Set all reader active in the view */
		m_tagViewManager->setActive();

		m_tagViewManager->startGuiUpdate();

		if (findEpcFlag){
			QString tag = ui.tagIdEdit->text();
			initialLength = (tag.length() + 1)/6;
			ui.lengthBox->setValue(initialLength);
			changeLength(initialLength); /* for the case that the lengths was not different from the different/default value */
 
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

			m_readRateCalc->clearResults();
			m_tagViewManager->clearTags();	

#if 0
			/* Delete the devices that are to remove */
			foreach(QIODevice* dev, m_toDelete.keys()){
				QrfeReaderInterface* ph = m_toDelete.value(dev);
				ph->deleteLater();
			}
			m_toDelete.clear();			
#endif

			//Icrease power in order to write EPC	
			epcWritePwr = pwr + pwrIncreaseWrite;
			if (epcWritePwr > MAXPOWER)
				epcWritePwr = MAXPOWER;

			//Mod by yingwei tseng for get power level, 2010/08/02
			//QrfeReaderInterface::Result resPowerState = m_ph->setAntennaPortSetPowerLevel(epcWritePwr);	
			QrfeReaderInterface::Result resPowerState = m_ph->AntennaPortSetPowerLevel(epcWritePwr);	
            //End by yingwei tseng for get power level, 2010/08/02 
			if (resPowerState == QrfeReaderInterface::OK){			
				ui.infoEdit->append("-- Set power level to " + QString::number(epcWritePwr) + " for writing EPC--");
				showOK();
			}else{
				ui.infoEdit->append("-- Could not set power level to " + QString::number(epcWritePwr) + " for writing EPC--");
				showNOK();			
			}

			//Write EPC
			if(!ui.currentPasswordLineEdit->hasAcceptableInput()){
				//Mod by yingwei tseng for incorrect English grammer, 2010/12/09
				QMessageBox::critical(this, "Error", "The current password is invalid!");
				//QMessageBox::critical(this, "Error", "The current password is not a valid!");
				//End by yingwei tseng for incorrect English grammer, 2010/12/09
				return;
			}

			if(!ui.newEpcEdit->hasAcceptableInput()){
				//Mod by yingwei tseng for incorrect English grammer, 2010/12/09
				QMessageBox::critical(this, "Error", "The new epc is invalid!");
				//QMessageBox::critical(this, "Error", "The new epc is not a valid!");
				//End by yingwei tseng for incorrect English grammer, 2010/12/09
				return;
			}
	
			bool ok = false;
			QByteArray passw = QrfeProtocolHandler::stringToEpc(ui.currentPasswordLineEdit->text(), &ok);
			if(!ok){
				//Mod by yingwei tseng for incorrect English grammer, 2010/12/09
				QMessageBox::critical(this, "Error", "The current password is invalid!");
				//QMessageBox::critical(this, "Error", "The current password is not a valid!");
				//End by yingwei tseng for incorrect English grammer, 2010/12/09
				return;
			}

			m_newEPC = ui.newEpcEdit->text();

			if(m_newEPC == ui.tagIdEdit->text()){
				QMessageBox::critical(this, "Error", "EPC is the same!");
				return;
			}	
	
	        //Mod by yingwei tseng for fixed mem bank length, 2010/09/14
	        //QrfeGen2ReaderInterface::Gen2Result res = QrfeGen2ReaderInterface::Gen2_ERROR;
	        QrfeGen2ReaderInterface::Gen2Result res = QrfeGen2ReaderInterface::RFID_ERROR_SYS_MODULE_FAILURE;
	        //End by yingwei tseng for fixed mem bank length, 2010/09/14

			QByteArray pc;
			if (currentLength!=initialLength)
			{		
				res = m_ph->readFromTag(ui.tagIdEdit->text(),1,1,passw,1,pc);
				if (!res) pc[0] = pc[0] & 0x07 | (currentLength<<3);
					pc.append(QrfeProtocolHandler::stringToEpc(m_newEPC));
				//Mod by yingwei tseng for memory bank block write, 2011/03/14 	
				//if (!res) res = m_ph->writeToTag(ui.tagIdEdit->text(),1,1,passw,pc);
				if (!res) res = m_ph->writeToTag(ui.tagIdEdit->text(),1,1,passw, pc, MEM_SET);
				//End by yingwei tseng for memory bank block write, 2011/03/14 
			}			
			else
			{
#if 0			
	QMessageBox::information(0, "Printer Test",
		"writeTagId", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

	QMessageBox::information(0, "Printer Test",
		ui.tagIdEdit->text(), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

	QMessageBox::information(0, "Printer Test",
		m_newEPC, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
#endif
				res = m_ph->writeTagId(ui.tagIdEdit->text(), m_newEPC, passw);
			}
			
            //Mod by yingwei tseng for fixed mem bank length, 2010/09/14
            if(res == QrfeGen2ReaderInterface::RFID_STATUS_OK){
	        //if(res == QrfeGen2ReaderInterface::Gen2_OK){
	        //End by yingwei tseng for fixed mem bank length, 2010/09/14	
				ui.infoEdit->append("-- TagId set to " + m_newEPC + " - OK --");
				ui.tagIdEdit->setText(m_newEPC);
				//decrease power in order to read and check EPC
				//Mod by yingwei tseng for get power level, 2010/08/02
				//QrfeReaderInterface::Result resPowerState = m_ph->setAntennaPortSetPowerLevel(pwr);
				QrfeReaderInterface::Result resPowerState = m_ph->AntennaPortSetPowerLevel(pwr);	
                //End by yingwei tseng for get power level, 2010/08/02
				if (resPowerState == QrfeReaderInterface::OK){			
					//ui.infoEdit->append("-- Set power level to " + QString::number(pwr) + " --");
					showOK();
				}else{
					//ui.infoEdit->append("-- Could not set power level to " + QString::number(pwr) + " --");
					showNOK();		
				}

				res = m_ph->readFromTag(m_newEPC,1,1,passw,1,pc);
				if (res == QrfeReaderInterface::OK){
					ui.infoEdit->append("-- Read EPC match writed EPC --");
					showOK();			
				}else{
					ui.infoEdit->append("-- Read EPC doesn't match writed EPC  --");
					showNOK();
				}				
			}
			else{
				ui.infoEdit->append("-- Could not set epc " + m_newEPC + "--");
				showNOK();
				if (pwr > MAXPOWER)
					break;
				else
					continue;
			}
			
			break;	
		}	
	}

	if (!findEpcFlag){
		ui.infoEdit->append(QString("-- No tags found --"));
		showOK();
	}

	/* Start every reader */
	//ui.setPrinterButton->setEnabled(false);
	//ui.closeButton->setEnabled(false);

	//m_readerManager.cleanUp();
	/* Reset read rate calculation */
#if 0	
	m_readRateCalc->reset();
		
	/* Notify tag manager that a scan is started */
	m_tagManager->startScan();

	/* Clear all reader and tag infos in the view */
	m_tagViewManager->clearAllReaderTagInfos();
			
	foreach(QString readerId, m_activeReader)
	{
		if(m_reader.contains(readerId)){
			m_reader.value(readerId)->startCyclicInventory();
			m_readRateCalc->readerStartedInventory(readerId);
			//QMap<QString, STagInfo> tagsOfReader = m_readerTagsInfo.value(readerId);
			QStringList allTagsOfReader = m_tagManager->getCurrentTagsOfReader(readerId);
			// and every tag of the reader
			foreach(QString tagId, allTagsOfReader)
			{
				break;
			}
		}	
	}	

	/* Set all reader active in the view */
	m_tagViewManager->setActive();

	m_tagViewManager->startGuiUpdate();

	//QrfeSleeper::MSleepAlive(3000);

	//QrfeSleeper::MSleepAlive(5000);

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

	m_readRateCalc->clearResults();
	m_tagViewManager->clearTags();

	/* Delete the devices that are to remove */
	foreach(QIODevice* dev, m_toDelete.keys()){
		QrfeReaderInterface* ph = m_toDelete.value(dev);
		ph->deleteLater();
	}
	m_toDelete.clear();	
#endif	
	

//	QrfeSleeper::MSleepAlive(5000);

	/* Stop the view from updating */
#if 0
	m_tagViewManager->stopGuiUpdate();		
	
	ui.setPrinterButton->setEnabled(true);
	ui.closeButton->setEnabled(true);
#endif

	/* Stop each reader */
	#if 0
	foreach(QString id, m_activeReader)
	{
		if(m_reader.contains(id)){
			m_reader.value(id)->stopCyclicInventory();
			m_readRateCalc->readerStoppedInventory(id);
		}
	}	
	#endif		
}
void CAmsLeoDialog::selectTabWidget(int tabIndex)
{
	if(tabIndex == 0)
	{
		ui.setAdvReaderButton->show();
		ui.closeButton->show();
		ui.infoEdit->show();
	}
	else
	{
		ui.setAdvReaderButton->hide();
		ui.closeButton->hide();
		ui.infoEdit->hide();
	}
}
//End by yingwei tseng for printer application, 2010/05/27

//Add by yingwei tseng for pulse modulation tseting, 2010/10/12
void CAmsLeoDialog::setAntennaPortTransmitTime()
{
    if (ui.regionBox->currentIndex() == REG_EU)
    {
        ui.transPatternTimeBox->setMaximum(3600);
		ui.transPatternTimeBox->setMinimum(1);
		ui.label_37->setText("second (1-3600)");
    }else{
        ui.transPatternTimeBox->setMaximum(10000);
		ui.transPatternTimeBox->setMinimum(1);
		ui.label_37->setText("second (1-10000)");
		
    }	
}
//End by yingwei tseng for pulse modulation tseting, 2010/10/12

//rick¡õ
void CAmsLeoDialog::tabWidgetToGetRegion()
{
    QFile   regionCurrent(QDir::currentPath()+"/region");
	QString currentRegion[50];
	//Open region file
	if( regionCurrent.open(QIODevice::ReadOnly) == false)
	{
		QMessageBox::information( 0, 
								  "Get region file",
								  "Get region file fail", 
								  QMessageBox::Ok );
		return;
	}
	QTextStream scanRegionCurrent(&regionCurrent);

	for(int iIndex=0; !scanRegionCurrent.atEnd(); iIndex++)
	{
		//regionLineCurrent[iIndex] = scanRegionCurrent.readLine();
		currentRegion[iIndex]=scanRegionCurrent.readLine();
	}
	regionCurrent.close();
	
	/*QString currentRegion[]={"United States / Canada", "Europe", "Taiwan", 
			"China", "South Korea", "Australia / New Zealand", "Europe2", "Brazil", 
			"Hong Kong", "Malaysia", "Singapore", "Thailand", "Israel", "Russia Federation", 
			"India", "Saudi Arabia", "Jordan", "Mexico", "Indonesia"};*/
	QByteArray btRegion, btAddr, btData;
	QrfeReaderInterface::Result result = tab_ph->getRegion(btRegion, btData);	

	if (result != QrfeReaderInterface::OK)
	{
		QMessageBox::information( 0, 
								  "Get Region",
								  "Get Region Fail", 
								  QMessageBox::Ok );
	}
	else
	{
		ui.lineEdit_3->setText(currentRegion[btData[0]]);
	}	
}
//rick¡ô

//rick¡õ
void CAmsLeoDialog::setCommunicationPort()
{
	

}
//rick¡ô2014-04-02


//rick 2014-03-31