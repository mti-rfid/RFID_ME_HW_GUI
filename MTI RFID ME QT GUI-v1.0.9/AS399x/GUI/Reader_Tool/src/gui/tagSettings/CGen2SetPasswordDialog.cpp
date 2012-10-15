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

#include "CGen2SetPasswordDialog.h"

#include <QMessageBox>


CGen2SetPasswordDialog::CGen2SetPasswordDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	QObject::connect(ui.buttonBox, 			SIGNAL(accepted()), 				this, SLOT(tryAccept()));
	QObject::connect(ui.buttonBox, 			SIGNAL(rejected()), 				this, SLOT(reject()));
}

CGen2SetPasswordDialog::~CGen2SetPasswordDialog()
{

}


int CGen2SetPasswordDialog::exec(QrfeGen2ReaderInterface* reader, QString epc)
{
	m_reader = reader;

	ui.tagIdEdit->setText(epc);

	ui.currentPasswordLineEdit->setText("00-00-00-00");
	ui.newPasswordLineEdit->setText("00-00-00-00");

	ui.typeBox->setCurrentIndex(1);

	return QDialog::exec();
}

void CGen2SetPasswordDialog::tryAccept()
{
	clearInfo();

	if(!ui.currentPasswordLineEdit->hasAcceptableInput())
	{
	    //Mod by yingwei tseng for incorrect English grammer, 2010/12/09
	    QMessageBox::critical(this, "Error", "The current password is invalid!");
		//QMessageBox::critical(this, "Error", "The current password is not a valid!");
		//End by yingwei tseng for incorrect English grammer, 2010/12/09
		return;
	}
	if(!ui.newPasswordLineEdit->hasAcceptableInput())
	{
	    //Mod by yingwei tseng for incorrect English grammer, 2010/12/09
	    QMessageBox::critical(this, "Error", "The new password is invalid!");
		//QMessageBox::critical(this, "Error", "The new password is not a valid!");
		//End by yingwei tseng for incorrect English grammer, 2010/12/09
		return;
	}


	bool ok = false;
	QByteArray currentPassw = QrfeProtocolHandler::stringToEpc(ui.currentPasswordLineEdit->text(), &ok);
	if(!ok)
	{
	    //Mod by yingwei tseng for incorrect English grammer, 2010/12/09
	    QMessageBox::critical(this, "Error", "The current password is invalid!");
		//QMessageBox::critical(this, "Error", "The current password is not a valid!");
		//End by yingwei tseng for incorrect English grammer, 2010/12/09
		return;
	}
	QByteArray newPassw = QrfeProtocolHandler::stringToEpc(ui.newPasswordLineEdit->text(), &ok);
	if(!ok)
	{
	    //Mod by yingwei tseng for incorrect English grammer, 2010/12/09
	    QMessageBox::critical(this, "Error", "The new password is invalid!");
		//QMessageBox::critical(this, "Error", "The new password is not a valid!");
		//End by yingwei tseng for incorrect English grammer, 2010/12/09
		return;
	}

	uchar wordaddr = 0;

	if(ui.typeBox->currentIndex() == 1)
		wordaddr += 2;

	//Mod by yingwei tseng for fixed mem bank length, 2010/09/14
	//QrfeGen2ReaderInterface::Gen2Result res = QrfeGen2ReaderInterface::Gen2_ERROR;
	QrfeGen2ReaderInterface::Gen2Result res = QrfeGen2ReaderInterface::RFID_ERROR_SYS_MODULE_FAILURE;
	//End by yingwei tseng for fixed mem bank length, 2010/09/14

    //Mod by yingwei tseng for memory bank block write, 2011/03/14 
	//res = m_reader->writeToTag(ui.tagIdEdit->text(), MEM_RES, wordaddr, currentPassw, newPassw);
	res = m_reader->writeToTag(ui.tagIdEdit->text(), MEM_RES, wordaddr, currentPassw, newPassw, MEM_SET);
	//End by yingwei tseng for memory bank block write, 2011/03/14 

    //Mod by yingwei tseng for fixed mem bank length, 2010/09/14
    if(res == QrfeGen2ReaderInterface::RFID_STATUS_OK){
	//if(res == QrfeGen2ReaderInterface::Gen2_OK){
	//End by yingwei tseng for fixed mem bank length, 2010/09/14
		showOK();
		QString action;
		ui.infoEdit->setText("-- Set " + ui.typeBox->currentText() + " Password of tag " + ui.tagIdEdit->text() + " - OK -- ");
		QMessageBox::information(this, "Set password", "Set " + ui.typeBox->currentText() + " Password of tag " + ui.tagIdEdit->text());
		QDialog::accept();
	}
	else
		handleError(res, "Could not set password");

}




void CGen2SetPasswordDialog::handleError(QrfeGen2ReaderInterface::Gen2Result result, QString text)
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

void CGen2SetPasswordDialog::clearInfo()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::white));
	ui.infoEdit->setPalette(pal);
	ui.infoEdit->clear();
}

void CGen2SetPasswordDialog::showOK()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::green).lighter());
	ui.infoEdit->setPalette(pal);
}

void CGen2SetPasswordDialog::showTagUnreachable()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::gray));
	ui.infoEdit->setPalette(pal);
}

void CGen2SetPasswordDialog::showWrongPassword()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::darkRed));
	ui.infoEdit->setPalette(pal);
}

void CGen2SetPasswordDialog::showMemoryOverrun()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::magenta).lighter());
	ui.infoEdit->setPalette(pal);
}

void CGen2SetPasswordDialog::showMemoryLocked()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::cyan).lighter());
	ui.infoEdit->setPalette(pal);
}

void CGen2SetPasswordDialog::showInsufficentPower()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::blue).lighter());
	ui.infoEdit->setPalette(pal);
}

void CGen2SetPasswordDialog::showNOK()
{
	QPalette pal = ui.infoEdit->palette();
	pal.setColor(QPalette::Base, QColor(Qt::red).lighter());
	ui.infoEdit->setPalette(pal);
}
