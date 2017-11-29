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

#include "CGen2SetEPCDialog.h"

#include <QMessageBox>
#include <qdatetime.h>   //Add by yingwei tseng for total times, 2010/02/03

int *writtenBytesHeadLoc=0, *writtenBytesTailLoc=0, writtenValue=0;
CGen2SetEPCDialog::CGen2SetEPCDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	QObject::connect(ui.buttonBox, 			SIGNAL(accepted()), 				this, SLOT(tryAccept()));
	//QObject::connect(ui.buttonBox, 			SIGNAL(rejected()), 				this, SLOT(reject()));
	QObject::connect(ui.buttonBox, 			SIGNAL(rejected()), 				this, SLOT(tryReject()));
	QObject::connect(ui.lengthBox,			SIGNAL(valueChanged(int)),	 		this, SLOT(changeLength(int)));
}

CGen2SetEPCDialog::~CGen2SetEPCDialog()
{

}

QString CGen2SetEPCDialog::newEPC()
{
	return m_newEPC;
}

void CGen2SetEPCDialog::changeLength(int len)
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

int CGen2SetEPCDialog::exec(QrfeGen2ReaderInterface* reader, QString epc)
{
	m_reader = reader;

	initialLength = (epc.length() + 1)/6;
	ui.lengthBox->setValue(initialLength);
	changeLength(initialLength); /* for the case that the lengths was not different from the different/default value */
	ui.tagIdEdit->setText(epc);
	ui.newEpcEdit->setText(epc);
	//changeLength(initialLength);

	ui.currentPasswordLineEdit->setText("00-00-00-00");

	return QDialog::exec();
}

void CGen2SetEPCDialog::tryReject()
{
    //tagIdEditStr=ui.tagIdEdit->text();
    //ui.newEpcEdit->setText(ui.tagIdEdit->text());
	//m_newEPC=tagIdEditStr;
    if(UnwrittenEPC == "")
	{
		m_newEPC=ui.tagIdEdit->text();
	}
	else
	{
		m_newEPC = UnwrittenEPC;
	    //QDialog::reject();
	}
	//m_newEPC = UnwrittenEPC;
	QDialog::reject();
}

void CGen2SetEPCDialog::tryAccept()
{
    //Del by yingwei tseng for read time error that read must be less than write , 2010/08/19
    #if 0
	//Add by yingwei tseng for total times, 2010/02/03
	int startTime = 0, endTime = 0;
	startTime = m_reader->getCurrentTime();
	//End by yingwei tseng for total times, 2010/02/03
	#endif
	//End by yingwei tseng for read time error that read must be less than write , 2010/08/19
	int writtenBytes = 0;    //Add by yingwei tseng for check EPC, 2010/07/06
	clearInfo();

	if(!ui.currentPasswordLineEdit->hasAcceptableInput())
	{
	    //Mod by yingwei tseng for incorrect English grammer, 2010/12/09
	    QMessageBox::critical(this, "Error", "The current password is invalid!");
		//QMessageBox::critical(this, "Error", "The current password is not a valid!");
		//End by yingwei tseng for incorrect English grammer, 2010/12/09
		return;
	}
	if(!ui.newEpcEdit->hasAcceptableInput())
	{
	    //Mod by yingwei tseng for incorrect English grammer, 2010/12/09
	    QMessageBox::critical(this, "Error", "The new epc is invalid!");
		//QMessageBox::critical(this, "Error", "The new epc is not a valid!");
		//End by yingwei tseng for incorrect English grammer, 2010/12/09
		return;
	}


	bool ok = false;
	QByteArray passw = QrfeProtocolHandler::stringToEpc(ui.currentPasswordLineEdit->text(), &ok);
	if(!ok)
	{
	    //Mod by yingwei tseng for incorrect English grammer, 2010/12/09
	    QMessageBox::critical(this, "Error", "The current password is invalid!");
		//QMessageBox::critical(this, "Error", "The current password is not a valid!");
		//End by yingwei tseng for incorrect English grammer, 2010/12/09
		return;
	}

	m_newEPC = ui.newEpcEdit->text();
	m_pastEPC = ui.tagIdEdit->text();//Rick

	// 20120314 MTI ignore the same EPC problem and force to rewrite again 
	//if(m_newEPC == ui.tagIdEdit->text()){
	//	QMessageBox::critical(this, "Error", "EPC is the same!");
	//	return;
	//}

    //Mod by yingwei tseng for fixed mem bank length, 2010/09/14
	//QrfeGen2ReaderInterface::Gen2Result res = QrfeGen2ReaderInterface::Gen2_ERROR;
	QrfeGen2ReaderInterface::Gen2Result res = QrfeGen2ReaderInterface::RFID_ERROR_SYS_MODULE_FAILURE;
	//End by yingwei tseng for fixed mem bank length, 2010/09/14

	//Add by yingwei tseng for read time error that read must be less than write , 2010/08/19
	int startTime = 0, endTime = 0;
	startTime = m_reader->getCurrentTime();
	//End by yingwei tseng for read time error that read must be less than write , 2010/08/19
	if (currentLength!=initialLength)
	{
		QByteArray pc;
		res = m_reader->readFromTag(ui.tagIdEdit->text(),1,1,passw,1,pc);
		if (!res) pc[0] = pc[0] & 0x07 | (currentLength<<3);
		pc.append(QrfeProtocolHandler::stringToEpc(m_newEPC));
		//Mod by yingwei tseng for memory bank block write, 2011/03/14 
		//if (!res) res = m_reader->writeToTag(ui.tagIdEdit->text(),1,1,passw,pc);
		if (!res) res = m_reader->writeToTag(ui.tagIdEdit->text(),1,1,passw, pc, MEM_SET);
		//End by yingwei tseng for memory bank block write, 2011/03/14 
		writtenBytes = pc.size() - 2;    //Add by yingwei tseng for check EPC, 2010/07/06		
	}
	else
	{
		//Add by yingwei tseng for check EPC, 2010/07/06
		#if 0
		res = m_reader->writeTagId(ui.tagIdEdit->text(), m_newEPC, passw);
		#endif		
        writtenValue=writtenBytes;//rick
		res = m_reader->writeTagId(ui.tagIdEdit->text(), m_newEPC, passw, &writtenBytes);
		//End by yingwei tseng for check EPC, 2010/07/06		
	}

    //Mod by yingwei tseng for fixed mem bank length, 2010/09/14
    if(res == QrfeGen2ReaderInterface::RFID_STATUS_OK)
	//if(res == QrfeGen2ReaderInterface::Gen2_OK)
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
    {
		//Add by yingwei tseng for check EPC, 2010/07/19
		#if 0
		QrfeGen2ReaderInterface::Gen2Result resSelect = m_reader->selectTagId(m_newEPC);
		if (resSelect != QrfeGen2ReaderInterface::Gen2_OK){
			QMessageBox::critical(this, tr("Gen2 warning"),
				"Could not set complete epc to " + m_newEPC);			
			handleError(res, "Could not set complete epc to " + m_newEPC);
		}else{
		#endif
		//End by yingwei tseng for check EPC, 2010/07/19 
			//m_newTmpEPC=m_pastEPC.mid(1);
			ui.infoEdit->setText("-- TagId set to " + m_newEPC + " - OK --");
			showOK();
			//Add by yingwei tseng for total times, 2010/02/03
			#if 0
			QMessageBox::information(this, "Set EPC", "Set EPC to " + m_newEPC + ".");
			#endif
			endTime = m_reader->getCurrentTime();
			QMessageBox::information(this, "Set EPC", "Set EPC to " + m_newEPC + "\n" + "Total time : " + 
			QString::number(endTime - startTime) + "(ms)");		
			//End by yingwei tseng for total times, 2010/02/03				
			QDialog::accept();
		//}//Add by yingwei tseng for check EPC, 2010/07/19 	
	}
	else
	//Mod by yingwei tseng for check EPC, 2010/07/06
#if 0
		handleError(res, "Could not set epc");
#endif
	{
	    //Add by Rick for judge which words write fail, 2013/06/05
		int retryWriteCNT=0,LoopCNT=3;

		do
		{	
			//m_new1TmpEPC=alterEPCHeadZero(m_pastEPC, m_newEPC, writtenBytes);
		    if(currentLength > initialLength)//short -> long
			{		
			    handleError(res, "Could not set complete epc to " + m_newEPC);
				break;
			}

			if(currentLength < initialLength)//long -> short
			{
				m_newTmpEPC=alterEPCDiffLeng(m_pastEPC, m_newEPC, currentLength);
				QrfeGen2ReaderInterface::Gen2Result resSelectDiffLeng = m_reader->selectTagId(m_newTmpEPC);
				if(resSelectDiffLeng == QrfeGen2ReaderInterface::RFID_STATUS_OK)
				{					
					res=m_reader->writeTagId(m_newTmpEPC,m_newEPC, passw, &writtenBytes);				
				}
				else
				{		
			        handleError(res, "Could not set complete epc to " + m_newEPC);
					UnwrittenEPC=m_pastEPC;
					break;
				}
				initialLength=currentLength;
				m_pastEPC=m_newTmpEPC;
			}
			else
			{
				m_newTmpEPC=alterEPCHeadWritten(m_pastEPC, m_newEPC);
				QrfeGen2ReaderInterface::Gen2Result resSelectWriteHead = m_reader->selectTagId(m_newTmpEPC);
				if(resSelectWriteHead == QrfeGen2ReaderInterface::RFID_STATUS_OK)
				{
					if(retryWriteCNT<LoopCNT-1)
					{ 
						res=m_reader->writeTagId(m_newTmpEPC,m_newEPC, passw, &writtenBytes);
					}
					 else
					{
						 UnwrittenEPC=m_newTmpEPC;
					 }			     
				}
				else
				{		
			        handleError(res, "Could not set complete epc to " + m_newEPC);
					UnwrittenEPC=m_pastEPC;
					break;
				}
			}			
		    retryWriteCNT++;
		}while((res!=QrfeGen2ReaderInterface::RFID_STATUS_OK) && (retryWriteCNT<LoopCNT));

		if(res!=QrfeGen2ReaderInterface::RFID_STATUS_OK)
		{
			QMessageBox::critical(this, tr("Gen2 warning"),
			    		"Could not set complete epc to " + m_newEPC);			
			handleError(res, "Could not set complete epc to " + m_newEPC);
		}
		else
		{
		    ui.infoEdit->setText("-- TagId set to " + m_newEPC + " - OK --");
			showOK();
			endTime = m_reader->getCurrentTime();
			QMessageBox::information(this, "Set EPC", "Set EPC to " + m_newEPC + "\n" + "Total time : " + 
			QString::number(endTime - startTime) + "(ms)");		
			QDialog::accept();
		}
	    //End by Rick for judge which words write fail,  2013/06/05		

		/*******************
		*******************/

	}
	//End by yingwei tseng for check EPC, 2010/07/06
}




void CGen2SetEPCDialog::handleError(QrfeGen2ReaderInterface::Gen2Result result, QString text)
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
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_SYS_CHANNEL_TIMEOUT)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n Occupational time of operational channel is overtime.");
        showNOK(); 
	}
    //Add by yingwei tseng for NXP alarm message, 2010/03/15
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
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n The specified memory location locked and/or perm locked and is either not writable or not readable.");
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
		ui.infoEdit->setText("-- " + text + " - OK -- ");
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
	else{
		ui.infoEdit->setText("-- " + text + " - ERROR --");
		showNOK();
	}
	#endif
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
}

void CGen2SetEPCDialog::clearInfo()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::white));
	ui.infoEdit->setPalette(pal);
	ui.infoEdit->clear();
}

void CGen2SetEPCDialog::showOK()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::green).lighter());
	ui.infoEdit->setPalette(pal);
}

void CGen2SetEPCDialog::showTagUnreachable()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::gray));
	ui.infoEdit->setPalette(pal);
}

void CGen2SetEPCDialog::showWrongPassword()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::darkRed));
	ui.infoEdit->setPalette(pal);
}

void CGen2SetEPCDialog::showMemoryOverrun()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::magenta).lighter());
	ui.infoEdit->setPalette(pal);
}

void CGen2SetEPCDialog::showMemoryLocked()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::cyan).lighter());
	ui.infoEdit->setPalette(pal);
}

void CGen2SetEPCDialog::showInsufficentPower()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::blue).lighter());
	ui.infoEdit->setPalette(pal);
}

void CGen2SetEPCDialog::showNOK()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::red).lighter());
	ui.infoEdit->setPalette(pal);
}

QString CGen2SetEPCDialog::alterEPCDiffLeng(QString Before, QString After, int curLength)
{
    QString  m_headEPC, m_tailEPC, EPC;
	EPC=Before.mid(0, (3*(curLength*2))-1  );
	//EPC=Before.mid(0,(curLength*4)+curLength);
	return EPC;


/*    if(wBytes==0)
	{
		EPC=Before.mid(5);
		EPC=m_zeroEPC.append(EPC);
		return EPC;
	}  
	else    
	{
		m_headEPC=After.mid(0, (wBytes*2+wBytes));
		m_headEPC=m_headEPC.append(m_zeroEPC);
		m_tailEPC=Before.mid((wBytes*2+wBytes)-1);
		EPC=m_headEPC.append(m_tailEPC);
		return EPC;
	}*/

	//m_headEPC = m_newEPC.mid(0,5);//getHead2Bytes
	//EPC = EPC.mid(5);
    //m_tailEPC = m_pastEPC.mid(5);//delHead2Bytes
	//EPC = m_zeroEPC.append(EPC);
	//return EPC ;
	//m_new2TmpEPC = m_headEPC.append(m_tailEPC);
}

QString CGen2SetEPCDialog::alterEPCHeadWritten(QString Before, QString After)
{
	QString m_headEPC, m_tailEPC, EPC;
	//int wBytes=writtenBytes;
//	if(wBytes==0)
//	{
	    m_headEPC=After.mid(0,5);
		m_tailEPC=Before.mid(5);
		EPC=m_headEPC.append(m_tailEPC);
		return EPC;
//	}
/*	else
	{
		m_headEPC=After.mid(0, (wBytes*2+wBytes));	
		m_tailEPC=Before.mid((wBytes*2+wBytes));
		m_headEPC=m_headEPC.append(m_tailEPC);
		EPC=m_headEPC;
		return EPC;
	}*/

	//EPCpast=EPCpast.mid(5);
	//EPCnew=EPCnew.mid(0,5);
	//m_pastTmpEPC=EPCnew.append(EPCpast);
	//return m_pastTmpEPC;
}