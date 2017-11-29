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

#include "CGen2TagDialog.h"
#include <QMessageBox>

#include "../../reader/QrfeGen2ReaderInterface.h"


CGen2TagDialog::CGen2TagDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	m_passwordDialog = new CGen2PassDialog(this);
	m_lockDialog = new CGen2LockDialog(this);
	m_setPasswordDialog = new CGen2SetPasswordDialog(this);
	m_setEPCDialog = new CGen2SetEPCDialog(this);
	m_nxpCommandDialog = new CNXPCommand(this);

	for(int i = 0; i < 8; i++)
		ui.memoryTableWidget->setColumnWidth(i, 40);

	QObject::connect(ui.buttonBox, 				SIGNAL(accepted()), 		this, SLOT(tryAccept()));
	QObject::connect(ui.refreshTIDButton, 		SIGNAL(clicked()), 			this, SLOT(readTID()));

	QObject::connect(ui.readMemButton, 			SIGNAL(clicked()), 			this, SLOT(readMem()));
	QObject::connect(ui.setMemButton, 			SIGNAL(clicked()), 			this, SLOT(setMem()));
	//Add by yingwei tseng for memory bank block write, 2011/03/14 
    QObject::connect(ui.blockSetMemButton, 		SIGNAL(clicked()), 			this, SLOT(setMem()));
	//End by yingwei tseng for memory bank block write, 2011/03/14 

	QObject::connect(ui.setEPCButton, 			SIGNAL(clicked()), 			this, SLOT(setEPC()));
	QObject::connect(ui.setPasswordButton,		SIGNAL(clicked()), 			this, SLOT(setPassword()));
	QObject::connect(ui.lockButton, 			SIGNAL(clicked()), 			this, SLOT(lock()));

	QObject::connect(ui.killTagButton, 			SIGNAL(clicked()), 			this, SLOT(killTag()));
	QObject::connect(ui.nxpCommandButton, 		SIGNAL(clicked()), 			this, SLOT(nxpCommand()));

	QObject::connect(ui.memBankBox, 			SIGNAL(currentIndexChanged(int)), 		this, SLOT(memBankChanged(int)));
	QObject::connect(ui.memoryTableWidget, 		SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(itemChanged(QTableWidgetItem*)));

	this->resize(500, this->height());

	m_readActive = false;

    //ui.nxpCommandButton->hide();    //Add by yingwei tseng for hiding NXP command, 2010/12/09	
}

CGen2TagDialog::~CGen2TagDialog()
{
	delete m_passwordDialog;
	delete m_lockDialog;
	delete m_setPasswordDialog;
	delete m_setEPCDialog;
}

int CGen2TagDialog::exec(QrfeReaderInterface* ph, QString tagId)
{
	m_ph = ph;

	if(m_ph->tagType() != QrfeReaderInterface::TAG_GEN2)
		return 0;

	if((m_gen2Reader = m_ph->getGen2Reader()) == 0)
		return 0;

	m_tagId = tagId;
	test_tagId="99-88-55-66-44-44-77-77-72-22-22-22";//tagId;
	bool testok=false;
	qByteArrayData=QrfeProtocolHandler::stringToEpc(test_tagId, &testok);


	ui.tagIdEdit->setText(m_tagId);
	ui.readerIdEdit->setText(ph->readerId());
	ui.accessPasswordEdit->setText("00-00-00-00");
	ui.memBankBox->setCurrentIndex(0);
	//ui.setMemButton->setVisible(false);    //Del by yingwei tseng for memory bank block write, 2011/03/14 

	clearInfo();

	QrfeProgressBar* pb = new QrfeProgressBar("Reading data from tag...", 60, qobject_cast<QWidget*>(parent()));
	pb->show();

	for(int i = 0; i < 4; i++){
		if(readTIDRegisterFromTag(pb))
			break;
	}

	pb->increasePorgressBar();

	pb->hide();
	delete pb;

	ui.memoryTableWidget->setRowCount(0);
	//Mod by yingwei tseng for fixed mem bank length, 2010/09/14
	//ui.userMemSizeEdit->setText("0 Bytes");
	ui.addressEdit->setText("0");
    //Mod by yingwei tseng for modify read/write tag format, 2010/10/7 
	//ui.userMemSizeEdit->setText("0");
	ui.userMemSizeEdit->setText("1");
	//End by yingwei tseng for modify read/write tag format, 2010/10/7 
	//End by yingwei tseng for fixed mem bank length, 2010/09/14

	return QDialog::exec();
}

void CGen2TagDialog::tryAccept()
{
	QDialog::accept();
}


void CGen2TagDialog::readTID()
{
	QrfeProgressBar* pb = new QrfeProgressBar("Reading data from tag...", 2, qobject_cast<QWidget*>(parent()));
	pb->show();
	pb->raise();

	readTIDRegisterFromTag(pb);


	pb->hide();
	delete pb;
}


void CGen2TagDialog::readMem()
{	
    //Del by yingwei tseng for read time error that read must be less than write , 2010/08/19
    #if 0
	//Add by yingwei tseng for total times, 2010/02/03
	int startTime = 0, endTime = 0;	
	startTime = m_gen2Reader->getCurrentTime();	
	//End by yingwei tseng for total times, 2010/02/03
	#endif
	//End by yingwei tseng for read time error that read must be less than write , 2010/08/19
	QrfeProgressBar* pb = new QrfeProgressBar("Reading data from tag...", 60, qobject_cast<QWidget*>(parent()));
	pb->show();
	pb->raise();

	m_readBank = (MEMORY_BANK)ui.memBankBox->currentIndex();

	uint size = 0;
	if(m_readBank == MEM_RES) size = 8;

	readMemoryFromTag(m_readBank, pb, size);

	pb->hide();
	delete pb;

    //Del by yingwei tseng for read time error that read must be less than write , 2010/08/19
    #if 0
	//Add by yingwei tseng for total times, 2010/02/03	
	endTime = m_gen2Reader->getCurrentTime();	
	QMessageBox::information(this, "Read Memory Bank Time", "Total time : " + 	QString::number(endTime - startTime) + "(ms)");	
	//End by yingwei tseng for total times, 2010/02/03	
	#endif
	//End by yingwei tseng for read time error that read must be less than write , 2010/08/19
}

void CGen2TagDialog::setMem()
{   
	int written = 0;
	QrfeProgressBar* pb = new QrfeProgressBar("Writing data to tag...", 60, qobject_cast<QWidget*>(parent()));
	pb->show();
	pb->raise();

	if(!ui.accessPasswordEdit->hasAcceptableInput())
	{
	    QMessageBox::critical(this, "Error", "The access password is invalid!");
		return;
	}

	bool ok = false;
	QByteArray passw = QrfeProtocolHandler::stringToEpc(ui.accessPasswordEdit->text(), &ok);
	if(!ok)
	{
	    QMessageBox::critical(this, "Error", "The access password is invalid!");
		return;
	}

	pb->increasePorgressBar();

	QByteArray data, epcSection;
	int icol=0;
	for(uint i = 0; i < m_readMemSize; i++)
	{
		bool ok = false;
		data.append(ui.memoryTableWidget->item(i/ui.memoryTableWidget->columnCount(), i%ui.memoryTableWidget->columnCount())->text().toInt(&ok, 16));
		epcSection.append(ui.memoryTableWidget->item(i/ui.memoryTableWidget->columnCount(), i%ui.memoryTableWidget->columnCount())->text().toInt(&ok, 16));
		epcSection.append("-");
		//icol=ui.memoryTableWidget->item(i/ui.memoryTableWidget->columnCount(), i%ui.memoryTableWidget->columnCount())->text().toInt(&ok, 16);
		if(!ok)
		{
			pb->hide();
			delete pb;
			QMessageBox::critical(this, "Error", "The data written in the memory table are invalid!");
			return;
		}
	}
    
	pb->increasePorgressBar();

    
	uchar address = 0;

    //Mod by yingwei tseng for memory bank block write, 2011/03/14 
    QByteArray tempData;
    if (ui.setMemButton->isChecked() == true)
	{
		if(m_readBank == MEM_EPC)
		{			
			//data = data.mid(2, data.size() - 2);	//del by yo chen, let the modification of the EPC bank can more make sense, 2011/06/17
			address = 0;	//mod by yo chen, let the modification of the EPC bank can more make sense, 2011/06/17
		}
		else if(m_readBank == MEM_RES)
		{
			data = data.left(8);
		}
		else
		{
			this->lowestChanged = this->lowestChanged / 2 * 2; /* To be 16-bit word aligned. */
			this->highestChanged = this->highestChanged / 2 * 2 + 1;
			//data = data.mid(this->lowestChanged,this->highestChanged+1-this->lowestChanged);//mod by Rick for Writing memory only read write diff, 2013/05/24
			//address = this->lowestChanged / 2;
			address = 0;//mod by Rick for Writing memory only read write diff, 2013/05/24
		}
    }
	else
	{	
	    if(m_readBank == MEM_EPC)
			tempData = data.mid(4, data.size() - 4);
	    
		this->lowestChanged = this->lowestChanged / 2 * 2; /* To be 16-bit word aligned. */
		this->highestChanged = this->highestChanged / 2 * 2 + 1;
		//data = data.mid(this->lowestChanged,this->highestChanged+1-this->lowestChanged);//mod by Rick, 2013/05/24
		//address = this->lowestChanged / 2;//mod by Rick
		address=0;//mod by Rick for Writing memory only read write diff, 2013/05/24
	}
	//End by yingwei tseng for memory bank block write, 2011/03/14 	

	address += ui.addressEdit->text().toInt();    //Add by yingwei tseng for read memory address, 2010/10/04

	QrfeGen2ReaderInterface::Gen2Result res = QrfeGen2ReaderInterface::RFID_ERROR_SYS_MODULE_FAILURE;
	int startTime = 0, endTime = 0;	
	startTime = m_gen2Reader->getCurrentTime();	

    //Mod by yingwei tseng for memory bank block write, 2011/03/14 	
	if (ui.setMemButton->isChecked() == true)
	    res = m_gen2Reader->writeToTag(m_tagId, m_readBank, address, passw, data, MEM_SET, &written);
	else
	    res = m_gen2Reader->writeToTag(m_tagId, m_readBank, address, passw, data, MEM_BLOCKSET, &written);
	//End by yingwei tseng for memory bank block write, 2011/03/14 

    if(res == QrfeGen2ReaderInterface::RFID_STATUS_OK)
	{		
		ui.infoEdit->setText("-- Wrote data to tag memory - OK --");
		QString text;
		for (int i = 0; i < data.size(); i++)
			text += QString("%1 ").arg((unsigned char) data[i], 2, 16, QChar('0'));
		ui.infoEdit->append("The data: " +  text);

		if(m_readBank == MEM_EPC)
		{
		    //Mod by yingwei tseng for memory bank block write, 2011/03/14 
		    if (ui.setMemButton->isChecked() == true)
		    {
			    //m_tagId = QrfeProtocolHandler::epcToString(data);
				int saveLength=m_tagId.length();
				qByteArrayData=CGen2TagDialog::pruneEPC(m_tagId, address, data);
				m_tagId = QrfeProtocolHandler::epcToString(qByteArrayData);//rick
				m_tagId=m_tagId.mid(0,saveLength);
			    ui.tagIdEdit->setText(m_tagId);
		    }
			else
			{								
                m_tagId = QrfeProtocolHandler::epcToString(tempData); 
                ui.tagIdEdit->setText(m_tagId);
			}
			//End by yingwei tseng for memory bank block write, 2011/03/14 
		}
		else if(m_readBank == MEM_RES)
			ui.accessPasswordEdit->setText("00-00-00-00");

		this->lowestChanged = 65000; // some high never reached number....
		this->highestChanged = -1;
		ui.memoryTableWidget->blockSignals(true);
		for(uint i = 0; i < m_readMemSize; i++)
		{
			QFont font;
			font = ui.memoryTableWidget->item(i/8,i%8)->font();
			font.setBold(false);
			ui.memoryTableWidget->item(i/8,i%8)->setFont(font);
		}
		ui.memoryTableWidget->blockSignals(false);

        //Add by yingwei tseng for memory bank block write, 2011/03/14
        #if 0
        if(m_readBank == MEM_EPC)
		{
		    if (ui.blockSetMemButton->isChecked() == true)
		    {
		        QMessageBox::information(this, "tempData-2", tempData.toHex());	
                m_tagId = QrfeProtocolHandler::epcToString(tempData);    
                ui.tagIdEdit->setText(data);
		    }	
        }	
		#endif
		//End by yingwei tseng for memory bank block write, 2011/03/14 
		
		showOK();
	}
	else
		handleError(res, QString("Wrote %1 bytes of %2 bytes at %3")
		                 .arg(written).arg(data.size()).arg(address));

    //Add by yingwei tseng for memory bank block write, 2011/03/14 
    ui.setMemButton->setChecked(false);	
    ui.blockSetMemButton->setChecked(false);
	//End by yingwei tseng for memory bank block write, 2011/03/14 

	endTime = m_gen2Reader->getCurrentTime();	
	QMessageBox::information(this, "Set Memory Bank Time", "Total time : " + 	QString::number(endTime - startTime) + "(ms)");				

	pb->hide();
	delete pb;

	// T  h  i  s  I  s  T  h  e  K  e  y
	// 54 68 69 73 49 73 54 68 65 4B 65 79

	// G  O
	// 47 4F
	if( m_tagId.contains("54-68-69-73-49-73-54-68-65-4B-65-79", Qt::CaseInsensitive) &&
		data.size() >= 2 &&
		data.at(0) == 0x47 &&
		data.at(1) == 0x4F )
		emit easterKeyUnlocked();
}



void CGen2TagDialog::setEPC()
{
	/*if(m_setEPCDialog->exec(m_gen2Reader, m_tagId) == QDialog::Accepted)
	{
		m_tagId = m_setEPCDialog->newEPC();
		ui.tagIdEdit->setText(m_tagId);
	}*/
	m_setEPCDialog->exec(m_gen2Reader, m_tagId);
	m_tagId = m_setEPCDialog->newEPC();
    ui.tagIdEdit->setText(m_tagId);
}

void CGen2TagDialog::lock()
{
	m_lockDialog->exec(m_gen2Reader, m_tagId);
}

void CGen2TagDialog::setPassword()
{
	if(m_setPasswordDialog->exec(m_gen2Reader, m_tagId) == QDialog::Accepted)
		ui.accessPasswordEdit->setText("00-00-00-00");
}


void CGen2TagDialog::killTag()
{
	clearInfo();

	QByteArray passw(4, (char)0x00);

	while(m_passwordDialog->exec("Insert kill password if needed", passw) == QDialog::Accepted)
	{

		if(passw.size() != 4)
			continue;

		//Mod by yingwei tseng for fixed mem bank length, 2010/09/14
	    //QrfeGen2ReaderInterface::Gen2Result res = QrfeGen2ReaderInterface::Gen2_ERROR;
	    QrfeGen2ReaderInterface::Gen2Result res = QrfeGen2ReaderInterface::RFID_ERROR_SYS_MODULE_FAILURE;
	    //End by yingwei tseng for fixed mem bank length, 2010/09/14
		res = m_gen2Reader->killTag(m_tagId, passw);

        //Mod by yingwei tseng for fixed mem bank length, 2010/09/14
        if(res == QrfeGen2ReaderInterface::RFID_STATUS_OK){
	    //if(res == QrfeGen2ReaderInterface::Gen2_OK){
	    //End by yingwei tseng for fixed mem bank length, 2010/09/14
			ui.infoEdit->setText("-- Killed tag " + m_tagId + " - OK --");
			showOK();
			return;
		}
		else
			handleError(res, "Could not kill tag");
	}

}

void CGen2TagDialog::nxpCommand()
{
	m_nxpCommandDialog->exec(qobject_cast<QrfeAmsReader*>(m_ph), m_tagId);
}


void CGen2TagDialog::itemChanged(QTableWidgetItem* item)
{
	int pos = item->column() + item->row()*8;
	if(m_readActive)
		return;

	QFont f = item->font();
	f.setBold(true);
	item->setFont(f);
	if (this->lowestChanged > pos) this->lowestChanged = pos;
	if (this->highestChanged < pos) this->highestChanged = pos;
}

void CGen2TagDialog::memBankChanged(int index)
{
    //Del by yingwei tseng for memory bank block write, 2011/03/14 
#if 0    
	if(index == MEM_USER)
		ui.setMemButton->setVisible(true);	
	else
		ui.setMemButton->setVisible(false);
#endif
	//End by yingwei tseng for memory bank block write, 2011/03/14 
	//Add by yingwei tseng for memory bank block write, 2011/03/14 
    if (index == MEM_TID)
    {
        ui.blockSetMemButton->setVisible(false);	
		ui.setMemButton->setVisible(false);
    }
	else
	{
		ui.blockSetMemButton->setVisible(true);	
		ui.setMemButton->setVisible(true);	
	}
	//End by yingwei tseng for memory bank block write, 2011/03/14 

	ui.memoryTableWidget->setRowCount(0);
}


bool CGen2TagDialog::readTIDRegisterFromTag(QrfeProgressBar* pb)
{
	clearInfo();

	QByteArray tid;
	bool 	gotInfos = false;
	quint16 manufacturer = 0;
	quint64 modelNr = 0;
	QString serial;

    //Add by yingwei tseng for modify read/write tag format, 2010/10/7 
	bool ok = false;	
	QByteArray passw = QrfeProtocolHandler::stringToEpc(ui.accessPasswordEdit->text(), &ok);
	if(!ok)
	{
	    //Mod by yingwei tseng for incorrect English grammer, 2010/12/09
	    QMessageBox::critical(this, "Error", "The access password is invalid!");
		//QMessageBox::critical(this, "Error", "The access password is not a valid!");
		//End by yingwei tseng for incorrect English grammer, 2010/12/09
		return false;
	}	
	//End by yingwei tseng for modify read/write tag format, 2010/10/7 

	//Mod by yingwei tseng for fixed mem bank length, 2010/09/14
	//QrfeGen2ReaderInterface::Gen2Result res = QrfeGen2ReaderInterface::Gen2_ERROR;
	QrfeGen2ReaderInterface::Gen2Result res = QrfeGen2ReaderInterface::RFID_ERROR_SYS_MODULE_FAILURE;
	//End by yingwei tseng for fixed mem bank length, 2010/09/14

    //Add by yingwei tseng for modify read/write tag format, 2010/10/7 
    //Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	//res = m_gen2Reader->readFromTag(m_tagId, MEM_TID, 0, QByteArray(4, (char)0), 0, tid);
	//res = m_gen2Reader->readFromTag(m_tagId, MEM_TID, 0, QByteArray(4, (char)0), 2, tid);
    //End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04
    res = m_gen2Reader->readFromTag(m_tagId, MEM_TID, 0, passw, 2, tid);
    //End by yingwei tseng for modify read/write tag format, 2010/10/7 

    //Mod by yingwei tseng for fixed mem bank length, 2010/09/14
    if(tid.size() > 0 && res == QrfeGen2ReaderInterface::RFID_STATUS_OK)
	//Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	//if(tid.size() > 0 && res == QrfeGen2ReaderInterface::Gen2_MEM_OVERRUN)
	//if(tid.size() > 0 && res == QrfeGen2ReaderInterface::Gen2_OK)
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
	{
		if(tid.size()>7 && (uchar)tid.at(0) == EPC_TID_CLASS_ID_1)
		{
				manufacturer = (quint8)tid.at(1);
				serial += QString("%1 ").arg((unsigned char)tid.at(2),2,16,QChar('0'));
				serial += QString("%1 ").arg((unsigned char)tid.at(3),2,16,QChar('0'));
				serial += QString("%1 ").arg((unsigned char)tid.at(4),2,16,QChar('0'));
				serial += QString("%1 ").arg((unsigned char)tid.at(5),2,16,QChar('0'));
				serial += QString("%1 ").arg((unsigned char)tid.at(6),2,16,QChar('0'));
				serial += QString("%1 ").arg((unsigned char)tid.at(7),2,16,QChar('0'));
				gotInfos = true;
		}
		else if(tid.size()>3 && (uchar)tid.at(0) == EPC_TID_CLASS_ID_2)
		{
				manufacturer += ((quint16)tid.at(1)) << 4;
				manufacturer += ((quint16) (tid.at(2) & 0xF0)) >> 4;
				modelNr += ((quint16) (tid.at(2) & 0x0F)) << 8;
				modelNr += (uchar)tid.at(3);
				for ( int i = 4; i< tid.size(); i++)
				{
					serial += QString("%1 ").arg((unsigned char)tid.at(i),2,16,QChar('0'));
				}
				gotInfos = true;

		}
		else
		{
			ui.infoEdit->setText("-- The tag has no valid epc class id - ERROR --");
			showNOK();
			return false;
		}
	}
	else
		handleError(res, "Could not read TID or too short TID");

	if(pb)
		pb->increasePorgressBar();

	QString manu;
	QString model;
	QString userMemSize;
	if(gotInfos)
	{
		manu = QString("0x%1").arg(manufacturer, 4, 16, QChar('0'));
		if(manufacturer > 0 && manufacturer < g_epcManufacturer.size())
			manu = g_epcManufacturer.at(manufacturer);

		if(g_tagInfo.contains(manu) && g_tagInfo.value(manu).contains(modelNr)){
			model = g_tagInfo.value(manu).value(modelNr).modelName;
			userMemSize = QString::number(g_tagInfo.value(manu).value(modelNr).memorySize) + " Bytes";
		}
		else{
			model = QString("0x%1").arg(modelNr, 6, 16, QChar('0'));
			userMemSize = "-";
		}

		ui.infoEdit->setText("-- Refreshed informations - OK --");
		showOK();
	}
	else
	{
		manu = "-";
		model = "-";
		userMemSize = "-";
		serial = "-";
	}

	ui.manufacturerInfoEdit->setText(manu);
	ui.modelInfoEdit->setText(model);
	ui.userMemSizeInfoEdit->setText(userMemSize);
	ui.serialNumberEdit->setText(serial);

	return gotInfos;
}

bool CGen2TagDialog::readMemoryFromTag(MEMORY_BANK bank, QrfeProgressBar* pb, uint /*size*/)
{
	int timeout = 3;
	int tagDataLength = 0;//Add by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 

	this->lowestChanged = 65000; // some high never reached number....
	this->highestChanged = -1;

	clearInfo();

	//Mod by yingwei tseng for fixed mem bank length, 2010/09/14
	//QrfeGen2ReaderInterface::Gen2Result res = QrfeGen2ReaderInterface::Gen2_ERROR;
	QrfeGen2ReaderInterface::Gen2Result res = QrfeGen2ReaderInterface::RFID_ERROR_SYS_MODULE_FAILURE;
	//End by yingwei tseng for fixed mem bank length, 2010/09/14

    //Add by yingwei tseng for modify read/write tag format, 2010/10/7 
	bool ok = false;	
	QByteArray passw = QrfeProtocolHandler::stringToEpc(ui.accessPasswordEdit->text(), &ok);
	if(!ok)
	{
	    //Mod by yingwei tseng for incorrect English grammer, 2010/12/09
	    QMessageBox::critical(this, "Error", "The access password is invalid!");
		//QMessageBox::critical(this, "Error", "The access password is not a valid!");
		//End by yingwei tseng for incorrect English grammer, 2010/12/09
		return false;
	}	
	//End by yingwei tseng for modify read/write tag format, 2010/10/7 
	
	m_readActive = true;

	uint byteCount = 0;

	ui.memoryTableWidget->setRowCount(0);

	//Add by yingwei tseng for read time error that read must be less than write , 2010/08/19
	int startTime = 0, endTime = 0;	
	startTime = m_gen2Reader->getCurrentTime();	
	//End by yingwei tseng for read time error that read must be less than write , 2010/08/19
	
	while(timeout--)
	{
		QByteArray mem;

        //Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
		//res = m_gen2Reader->readFromTag(m_tagId, bank, 0 + (byteCount/2), QByteArray(4, (char)0), 0, mem);
		//Del by yingwei tseng for fixed mem bank length, 2010/09/14
		#if 0
        switch(bank){
            case MEM_RES:
				tagDataLength = 4;
				break;
			case MEM_EPC:
				tagDataLength = 8;
				break;
			case MEM_TID:
				tagDataLength = 2;
				break;
			case MEM_USER:
				//tagDataLength = 0;
				tagDataLength = 30;
				break;
			default:
				break;
        }
		#endif
		//End by yingwei tseng for fixed mem bank length, 2010/09/14

		#if 0
	    //Add by yingwei tseng for fixed mem bank length, 2010/09/14	
        if (ui.userMemSizeEdit->text().toInt() > 60){
			QMessageBox::critical(this, "Memory Size", "The memory szie should not be greater than 60 bytes");
			break;
        }	
	    //End by yingwei tseng for fixed mem bank length, 2010/09/14
        #endif		

		//Mod by yingwei tseng for fixed mem bank length, 2010/09/14
		tagDataLength = ui.userMemSizeEdit->text().toInt();
		//Mod by yingwei tseng for modify read/write tag format, 2010/10/7 
		//Mod by yingwei tseng for read memory address, 2010/10/04
		//res = m_gen2Reader->readFromTag(m_tagId, bank, 0 + (byteCount/2), QByteArray(4, (char)0), (uchar)tagDataLength, mem);
		//res = m_gen2Reader->readFromTag(m_tagId, bank, ui.addressEdit->text().toInt(), QByteArray(4, (char)0), (uchar)tagDataLength, mem);
		res = m_gen2Reader->readFromTag(m_tagId, bank, ui.addressEdit->text().toInt(), passw, (uchar)tagDataLength, mem);
		//End by yingwei tseng for read memory address, 2010/10/04
		//End by yingwei tseng for modify read/write tag format, 2010/10/7 
		//End by yingwei tseng for fixed mem bank length, 2010/09/14
        //End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 

		if(pb)
			pb->increasePorgressBar();

		//Mod by yingwei tseng for fixed mem bank length, 2010/09/14
        //Mod by yingwei tseng for fixed mem bank length, 2010/09/14
        if(res == QrfeGen2ReaderInterface::RFID_STATUS_OK)
	    //if(res == QrfeGen2ReaderInterface::Gen2_OK)
	    //End by yingwei tseng for fixed mem bank length, 2010/09/14
        {
            //Mod by yingwei tseng for read memory address, 2010/10/04
			//for(int i = ui.addressEdit->text().toInt(); i < (tagDataLength*2); i++)
			for(int i = 0; i < (tagDataLength*2); i++)
			//End by yingwei tseng for read memory address, 2010/10/04	
			{
				if(byteCount%ui.memoryTableWidget->columnCount() == 0){
					ui.memoryTableWidget->setRowCount((byteCount/ui.memoryTableWidget->columnCount()) + 1);
					ui.memoryTableWidget->setRowHeight(ui.memoryTableWidget->rowCount()-1, 18);
				}

				ui.memoryTableWidget->setItem(
						(byteCount/ui.memoryTableWidget->columnCount()),
						(byteCount%ui.memoryTableWidget->columnCount()),
						new QTableWidgetItem(QString("%1").arg((uchar)mem.at(i), 2, 16, QChar('0'))));

				byteCount ++;
			}			
            break;
		}
		
        #if 0
		for(int i = 0; i < mem.size(); i++)
		{
			if(byteCount%ui.memoryTableWidget->columnCount() == 0){
				ui.memoryTableWidget->setRowCount((byteCount/ui.memoryTableWidget->columnCount()) + 1);
				ui.memoryTableWidget->setRowHeight(ui.memoryTableWidget->rowCount()-1, 18);
			}

			ui.memoryTableWidget->setItem(
					(byteCount/ui.memoryTableWidget->columnCount()),
					(byteCount%ui.memoryTableWidget->columnCount()),
					new QTableWidgetItem(QString("%1").arg((uchar)mem.at(i), 2, 16, QChar('0'))));

			byteCount ++;
		}

        //Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
        //if(res == QrfeGen2ReaderInterface::Gen2_MEM_OVERRUN){
		if(res == QrfeGen2ReaderInterface::RFID_STATUS_OK){
		//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 	
            break;
        }
        #endif
		//End by yingwei tseng for fixed mem bank length, 2010/09/14
		
		if(pb)
			pb->increasePorgressBar();
	}

    //Add by yingwei tseng for read time error that read must be less than write , 2010/08/19
	endTime = m_gen2Reader->getCurrentTime();	
	QMessageBox::information(this, "Read Memory Bank Time", "Total time : " + 	QString::number(endTime - startTime) + "(ms)");	
	//End by yingwei tseng for read time error that read must be less than write , 2010/08/19		

	//ui.userMemSizeEdit->setText(QString::number(byteCount) + " Bytes");    //Del by yingwei tseng for fixed mem bank length, 2010/09/14
    //Mod by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 
	//if(res == QrfeGen2ReaderInterface::Gen2_MEM_OVERRUN){
	//Mod by yingwei tseng for fixed mem bank length, 2010/09/14
    if(res == QrfeGen2ReaderInterface::RFID_STATUS_OK)
	//if(res == QrfeGen2ReaderInterface::Gen2_OK)
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
    {
	//End by yingwei tseng for add packet header and CRC in UART mode, 2010/08/04 	
		ui.infoEdit->setText("-- Read everything (" + QString::number(byteCount) + " Bytes) - OK --");
		showOK();
	}
	else
		//Add by yingwei tseng for total times, 2010/02/03		
		#if 0
		handleError(res, "Could not read more data from tag");	
		#endif
		handleError(res, "Tag information is not complete. Please try again.");	
	    //End by yingwei tseng for total times, 2010/02/03

	m_readActive = false;
	m_readMemSize = byteCount;

	return true;
}


void CGen2TagDialog::handleError(QrfeGen2ReaderInterface::Gen2Result result, QString text)
{
    //Add by yingwei tseng for fixed mem bank length, 2010/09/14
    if (result == QrfeGen2ReaderInterface::RFID_STATUS_OK)
    {
        ui.infoEdit->setText("-- \n Performed result of command is success or tags have been found.");
		showOK();
    }
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_CMD_INVALID_DATA_LENGTH)
	{
	    ui.infoEdit->setText("-- " + text + " - ERROR -- \n The value range in the data length field of command is invalid.");
        showNOK();    
	}
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_INVALID_PARAMETER)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n One of the function parameters of command is invalid.");
        showNOK(); 
	}
    //Add by yingwei tseng for NXP alarm message, 2010/03/15
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_SYS_CHANNEL_TIMEOUT)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n Occupational time of operational channel is overtime.");
        showNOK(); 
	}
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_HWOPT_READONLY_ADDRESS)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n The OEMCfg address is read only.");
        showNOK(); 
	}
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_HWOPT_UNSUPPORTED_REGION)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n The region selection is unsupported.");
        showNOK(); 
	}
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_SYS_SECURITY_FAILURE)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n Checking security protection of product is failed.");
        showNOK(); 
	}    
	//End by yingwei tseng for NXP alarm message, 2010/03/15	
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_SYS_CHANNEL_TIMEOUT)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n Occupational time of operational channel is overtime.");
        showNOK(); 
	}
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_SYS_MODULE_FAILURE)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n The underlying module encountered an error. \n The RFID module indicated a failure. \n For tag operation, the RFID module did not get any reply from tag.");
        showNOK();         
	}
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_18K6C_REQRN)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n Performed result of ReqRN command of ISO 18000-6C is fail.");
        showNOK();           
	}
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_18K6C_ACCESS)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n Performed result of Access command of ISO 18000-6C is fail.");
        showNOK(); 
	}
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_18K6C_KILL)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n Performed result of Kill command of ISO 18000-6C is fail.");
        showNOK(); 
	}
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_18K6C_NOREPLY)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n No reply for ISO 18000-6C standard command.");
        showNOK(); 
	}	
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_18K6C_LOCK)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n Performed result of Lock command of ISO 18000-6C is fail.");
        showNOK(); 
	}
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_18K6C_BLOCKWRITE)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n Performed result of BlockWrite command of ISO 18000-6C is fail.");
        showNOK(); 
	}	
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_18K6C_BLOCKERASE)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n Performed result of BlockErase command of ISO 18000-6C is fail.");
        showNOK(); 
	}	
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_18K6C_READ)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n Performed result of Read command of ISO 18000-6C is fail.");
        showNOK(); 
	}	
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_18K6C_SELECT)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n Performed result of Select command of ISO 18000-6C is fail.");
        showNOK(); 
	}	
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_18K6B_INVALID_CRC)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n CRC error occurred.");
        showNOK(); 
	}	
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_18K6B_RFICREG_FIFO)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n FIFO error occurred in the RFIC.");
        showNOK(); 
	}	
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_18K6B_NO_RESPONSE)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n No response from any tag.");
        showNOK(); 
	}	
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_18K6B_NO_ACKNOWLEDGE)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n The tag sends not acknowledge.");
        showNOK(); 
	}	
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_18K6B_PREAMBLE)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n Preamble error occurred.");
        showNOK(); 
	}	
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_6CTAG_OTHER_ERROR)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n Catch-all for errors not covered by other codes.");
        showNOK(); 
	}
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_6CTAG_MEMORY_OVERRUN)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n The specified memory location does not exist or the PC value is not supported by the tag.");
        showNOK(); 
	}	
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_6CTAG_MEMORY_LOCKED)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n The specified memory location locked and/or perm locked and is either not writeable or not readable.");
        showNOK(); 
	}	
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_6CTAG_INSUFFICIENT_POWER)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n The tag has insufficient power to perform the memory-write operation.");
        showNOK(); 
	}
	else
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n The tag does not support error-specific codes.");
        showNOK(); 
	}	
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
    //Del by yingwei tseng for fixed mem bank length, 2010/09/14
    #if 0
	if(result == QrfeGen2ReaderInterface::Gen2_OK)
	{
		showOK();
	}
	else if(result == QrfeGen2ReaderInterface::Gen2_TAG_UNREACHABLE)
	{
		ui.infoEdit->setText("-- " + text + " - ERROR -- \n Tag is not in the range of the reader...");
		showTagUnreachable();
	}
	else if(result == QrfeGen2ReaderInterface::Gen2_WRONG_PASSW)
	{
		ui.infoEdit->setText("-- " + text + " - ERROR -- \n Wrong password...");
		showWrongPassword();
	}
	else if(result == QrfeGen2ReaderInterface::Gen2_MEM_OVERRUN)
	{
		ui.infoEdit->setText("-- " + text + " - ERROR -- \n Memory overrun...");
		showMemoryOverrun();
	}
	else if(result == QrfeGen2ReaderInterface::Gen2_MEM_LOCKED)
	{
		ui.infoEdit->setText("-- " + text + " - ERROR -- \n Memory is locked, you must specify a access password...");
		showMemoryLocked();
	}
	else if(result == QrfeGen2ReaderInterface::Gen2_INSUFFICIENT_POWER)
	{
		ui.infoEdit->setText("-- " + text + " - ERROR -- \n Tag has insufficient power...");
		showInsufficentPower();
	}
	else if(result == QrfeGen2ReaderInterface::Gen2_ACCESS_FAILED)
	{
		ui.infoEdit->setText("-- " + text + " - ERROR -- \n Access failed.");
		showNOK();
	}
	else if(result == QrfeGen2ReaderInterface::Gen2_NOREPLY)
	{
		ui.infoEdit->setText("-- " + text + " - ERROR -- \n Tag did not return reply.");
		showNOK();
	}
	else if(result == QrfeGen2ReaderInterface::Gen2_REQRN_FAILED)
	{
		ui.infoEdit->setText("-- " + text + " - ERROR -- \n ReqRN failed.");
		showNOK();
	}
	else if(result == QrfeGen2ReaderInterface::Gen2_CHANNEL_TIMEOUT)
	{
		ui.infoEdit->setText("-- " + text + " - ERROR -- \n Channel timed out, command took too long.");
		showNOK();
	}
	else if(result == QrfeGen2ReaderInterface::Gen2_NON_SPECIFIC)
	{
		ui.infoEdit->setText("-- " + text + " - ERROR -- \n Tag returned the non-specific error.");
		showNOK();
	}
	else if(result == QrfeGen2ReaderInterface::Gen2_OTHER)
	{
		ui.infoEdit->setText("-- " + text + " - ERROR -- \n Tag returned the so called \"other\" error.");
		showNOK();
	}
	else{
		ui.infoEdit->setText("-- " + text + " - ERROR --");
		showNOK();
	}
	#endif
	//End by yingwei tseng for fixed mem bank length, 2010/09/14

}


void CGen2TagDialog::clearInfo()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::white));
	ui.infoEdit->setPalette(pal);
	ui.infoEdit->clear();
}

void CGen2TagDialog::showOK()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::green).lighter());
	ui.infoEdit->setPalette(pal);
}

void CGen2TagDialog::showTagUnreachable()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::gray));
	ui.infoEdit->setPalette(pal);
}

void CGen2TagDialog::showWrongPassword()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::darkRed));
	ui.infoEdit->setPalette(pal);
}

void CGen2TagDialog::showMemoryOverrun()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::magenta).lighter());
	ui.infoEdit->setPalette(pal);
}

void CGen2TagDialog::showMemoryLocked()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::cyan).lighter());
	ui.infoEdit->setPalette(pal);
}

void CGen2TagDialog::showInsufficentPower()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::blue).lighter());
	ui.infoEdit->setPalette(pal);
}

void CGen2TagDialog::showNOK()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::red).lighter());
	ui.infoEdit->setPalette(pal);
}

QByteArray CGen2TagDialog::pruneEPC(QString Id, ushort address, QByteArray data)
{
	QByteArray EPC;
	QString front, tail, qStringData;
	int dataLeng=data.length();
	int epcLength = 0;

	EPC = QrfeProtocolHandler::stringToEpc(Id);
	epcLength = EPC.length();
	EPC.replace((address - 2) * 2, dataLeng, data);
	EPC.resize(epcLength);

/*
	qStringData=QrfeProtocolHandler::epcToString(data);
	if(address<3)
	{
		front=Id.mid(0,5*(address-2));
	}
	else
	{
		front=Id.mid(0,(5*(address-2))+(address-2-1));
		front=front.append("-");
	}
	front=front.append(qStringData);
	tail=Id.mid(front.length());
	front=front.append(tail);
	EPC=QrfeProtocolHandler::stringToEpc(front);/////
	*/
	return EPC;
}
