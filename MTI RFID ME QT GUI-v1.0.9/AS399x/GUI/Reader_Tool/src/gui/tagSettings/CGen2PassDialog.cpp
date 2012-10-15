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

#include "CGen2PassDialog.h"

#include <QMessageBox>

#include "../../reader/protocoll/QrfeProtocolHandler.h"


CGen2PassDialog::CGen2PassDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	QObject::connect(ui.buttonBox, 				SIGNAL(accepted()), 		this, SLOT(tryAccept()));
	QObject::connect(ui.buttonBox, 				SIGNAL(rejected()), 		this, SLOT(reject()));

}

CGen2PassDialog::~CGen2PassDialog()
{

}


int CGen2PassDialog::exec(QString text, QByteArray &passw)
{
	ui.textLabel->setText(text);
	ui.passwordLineEdit->setText(QrfeProtocolHandler::epcToString(passw));
	ui.passwordLineEdit->setCursorPosition(0);

	int ret = QDialog::exec();

	passw = m_passw;

	return ret;
}


void CGen2PassDialog::tryAccept()
{
	if(!ui.passwordLineEdit->hasAcceptableInput())
	{
	    //Mod by yingwei tseng for incorrect English grammer, 2010/12/09
	    QMessageBox::critical(this, "Error", "The password is invalid!");
		//QMessageBox::critical(this, "Error", "The password is not a valid!");
		//End by yingwei tseng for incorrect English grammer, 2010/12/09
		return;
	}

	bool ok = false;
	m_passw = QrfeProtocolHandler::stringToEpc(ui.passwordLineEdit->text(), &ok);
	if(!ok)
	{
	    //Mod by yingwei tseng for incorrect English grammer, 2010/12/09
		QMessageBox::critical(this, "Error", "The password is invalid!");
		//QMessageBox::critical(this, "Error", "The password is not a valid!");
		//End by yingwei tseng for incorrect English grammer, 2010/12/09
		return;
	}

	QDialog::accept();
}
