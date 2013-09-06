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


#include "CNXPCommand.h"
#include <QMessageBox>

CNXPCommand::CNXPCommand(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	QObject::connect(ui.buttonBox, 				SIGNAL(accepted()), 		this, SLOT(tryAccept()));
	QObject::connect(ui.buttonBox, 				SIGNAL(rejected()), 		this, SLOT(reject()));
	//Add by yingwei tseng for displaying configuration edit, 2010/12/07
	QObject::connect(ui.commandLineEdit,		SIGNAL(currentIndexChanged(int)), 	this, SLOT(loadCommand()) );	
    //End by yingwei tseng for displaying configuration edit, 2010/12/07
}

CNXPCommand::~CNXPCommand()
{

}

int CNXPCommand::exec(QrfeAmsReader* reader, QString epc)
{
	m_reader = reader;

	ui.tagIdEdit->setText(epc);

	ui.passwordLineEdit->setText("00-00-00-00");

	loadCommand();    //Add by yingwei tseng for displaying configuration edit, 2010/12/07

	return QDialog::exec();
}

void CNXPCommand::tryAccept()
{
	clearInfo();

	if(!ui.passwordLineEdit->hasAcceptableInput())
	{
	    //Mod by yingwei tseng for incorrect English grammer, 2010/12/09
	    QMessageBox::critical(this, "Error", "The password is invalid!");
		//QMessageBox::critical(this, "Error", "The password is not a valid!");
		//End by yingwei tseng for incorrect English grammer, 2010/12/09
		return;
	}

	bool ok = false;
	QByteArray passw = QrfeProtocolHandler::stringToEpc(ui.passwordLineEdit->text(), &ok);
	if(!ok)
	{
	    //Mod by yingwei tseng for incorrect English grammer, 2010/12/09
	    QMessageBox::critical(this, "Error", "The password is invalid!");
		//QMessageBox::critical(this, "Error", "The password is not a valid!");
		//End by yingwei tseng for incorrect English grammer, 2010/12/09
		return;
	}
	//Add by yingwei tseng for new NXP function, 2010/04/01 
	if(!ui.configLineEdit->hasAcceptableInput())
	{
	    //Mod by yingwei tseng for incorrect English grammer, 2010/12/09
		QMessageBox::critical(this, "Error", "The config is invalid!");
		//QMessageBox::critical(this, "Error", "The config is not a valid!");
		//End by yingwei tseng for incorrect English grammer, 2010/12/09
		return;
	}

	ok = false;
	QByteArray cfg = QrfeProtocolHandler::stringToEpc(ui.configLineEdit->text(), &ok);
	if(!ok)
	{
	    //Mod by yingwei tseng for incorrect English grammer, 2010/12/09
	    QMessageBox::critical(this, "Error", "The config is invalid!");
		//QMessageBox::critical(this, "Error", "The config is not a valid!");
		//End by yingwei tseng for incorrect English grammer, 2010/12/09
		return;
	}
	//End by yingwei tseng for new NXP function, 2010/04/01 
	//Mod by yingwei tseng for NXP alarm message, 2010/03/15
	QByteArray easCode;
    QrfeGen2ReaderInterface::Gen2Result res = QrfeGen2ReaderInterface::RFID_ERROR_SYS_MODULE_FAILURE;
	
    if (ui.commandLineEdit->currentIndex() == 4)
    {
	    res = m_reader->nxpEasAlarmCommand(easCode);
		if(res == QrfeGen2ReaderInterface::RFID_STATUS_OK){
			showOK();
			ui.infoEdit->setText("-- EAS code is fixed hex number :\n" + QString("0x%1 0x%2 0x%3 0x%4 0x%5 0x%6 0x%7 0x%8")
				.arg((unsigned char)easCode.at(0), 2, 16, QChar('0'))
				.arg((unsigned char)easCode.at(1), 2, 16, QChar('0'))
				.arg((unsigned char)easCode.at(2), 2, 16, QChar('0'))
				.arg((unsigned char)easCode.at(3), 2, 16, QChar('0'))
				.arg((unsigned char)easCode.at(4), 2, 16, QChar('0'))
				.arg((unsigned char)easCode.at(5), 2, 16, QChar('0'))
				.arg((unsigned char)easCode.at(6), 2, 16, QChar('0'))
				.arg((unsigned char)easCode.at(7), 2, 16, QChar('0')) + " \n for G2iL+, G2iM and G2X series. -- ");
		}else
            handleError(res, "NXP EAS alarm error");
    }    
	else
	{
	    QStringList sl = ui.commandLineEdit->currentText().split(" ",QString::SkipEmptyParts);

	    QByteArray command = QrfeProtocolHandler::stringToEpc(sl.last(), &ok);
		if(!ok)
		{
			QMessageBox::critical(this, "Error", "The command is invalid!");
			return;
		}

		res = m_reader->nxpCommand(ui.tagIdEdit->text(), command, passw, cfg);

		if(res == QrfeGen2ReaderInterface::RFID_STATUS_OK){
		    showOK();
			QString action;
			ui.infoEdit->setText("-- Set command " + sl.last() + " to tag " + ui.tagIdEdit->text() + " - OK -- ");
			QMessageBox::information(this, "Set NXP command", "Set command " + sl.last() + " to tag " + ui.tagIdEdit->text());
			QDialog::accept();
		}
		else
			handleError(res, "Could not set NXP Command");
	}    
	//End by yingwei tseng for NXP alarm message, 2010/03/15

    //Del by yingwei tseng for NXP alarm message, 2010/03/15
#if 0    
    QStringList sl = ui.commandLineEdit->currentText().split(" ",QString::SkipEmptyParts);

    QByteArray command = QrfeProtocolHandler::stringToEpc(sl.last(), &ok);
	if(!ok)
	{
	    //Mod by yingwei tseng for incorrect English grammer, 2010/12/09
		QMessageBox::critical(this, "Error", "The command is invalid!");
		//QMessageBox::critical(this, "Error", "The command is not a valid!");
		//End by yingwei tseng for incorrect English grammer, 2010/12/09
		return;
	}

	//Mod by yingwei tseng for fixed mem bank length, 2010/09/14
	//QrfeGen2ReaderInterface::Gen2Result res = QrfeGen2ReaderInterface::Gen2_ERROR;
	QrfeGen2ReaderInterface::Gen2Result res = QrfeGen2ReaderInterface::RFID_ERROR_SYS_MODULE_FAILURE;
	//End by yingwei tseng for fixed mem bank length, 2010/09/14

	//Mod by yingwei tseng for new NXP function, 2010/04/01 
	#if 0
	res = m_reader->nxpCommand(ui.tagIdEdit->text(), command, passw);
	#endif
	res = m_reader->nxpCommand(ui.tagIdEdit->text(), command, passw, cfg);
	//End by yingwei tseng for new NXP function, 2010/04/01 

    //Mod by yingwei tseng for fixed mem bank length, 2010/09/14
	if(res == QrfeGen2ReaderInterface::RFID_STATUS_OK){
	//if(res == QrfeGen2ReaderInterface::Gen2_OK){
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
	    showOK();
		QString action;
		ui.infoEdit->setText("-- Set command " + sl.last() + " to tag " + ui.tagIdEdit->text() + " - OK -- ");
		QMessageBox::information(this, "Set NXP command", "Set command " + sl.last() + " to tag " + ui.tagIdEdit->text());
		QDialog::accept();
	}
	else
		handleError(res, "Could not set NXP Command");
#endif	
	//End by yingwei tseng for NXP alarm message, 2010/03/15
}




void CNXPCommand::handleError(QrfeGen2ReaderInterface::Gen2Result result, QString text)
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
	//Add by yingwei tseng for NXP alarm message, 2010/03/15
	else if (result == QrfeGen2ReaderInterface::RFID_ERROR_18K6C_EASCODE)
	{
        ui.infoEdit->setText("-- " + text + " - ERROR -- \n EAS Code can not be identified.");
        showNOK(); 
	}
	//End by yingwei tseng for NXP alarm message, 2010/03/15	
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

void CNXPCommand::clearInfo()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::white));
	ui.infoEdit->setPalette(pal);
	ui.infoEdit->clear();
}

void CNXPCommand::showOK()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::green).lighter());
	ui.infoEdit->setPalette(pal);
}

void CNXPCommand::showTagUnreachable()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::gray));
	ui.infoEdit->setPalette(pal);
}

void CNXPCommand::showWrongPassword()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::darkRed));
	ui.infoEdit->setPalette(pal);
}

void CNXPCommand::showMemoryOverrun()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::magenta).lighter());
	ui.infoEdit->setPalette(pal);
}

void CNXPCommand::showMemoryLocked()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::cyan).lighter());
	ui.infoEdit->setPalette(pal);
}

void CNXPCommand::showInsufficentPower()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::blue).lighter());
	ui.infoEdit->setPalette(pal);
}

void CNXPCommand::showNOK()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::red).lighter());
	ui.infoEdit->setPalette(pal);
}

//Add by yingwei tseng for displaying configuration edit, 2010/12/07
void CNXPCommand::loadCommand()
{  
    //Mod by yingwei tseng for NXP alarm message, 2010/03/15
    //if (ui.commandLineEdit->currentIndex() != 2)
    if (ui.commandLineEdit->currentIndex() != 5)
	//End by yingwei tseng for NXP alarm message, 2010/03/15	
        ui.configLineEdit->setEnabled(false);
    else      
        ui.configLineEdit->setEnabled(true);
}
//End by yingwei tseng for displaying configuration edit, 2010/12/07

