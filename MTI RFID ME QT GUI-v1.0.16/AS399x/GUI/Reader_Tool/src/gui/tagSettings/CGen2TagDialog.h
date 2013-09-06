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

#ifndef CGEN2TAGDIALOG_H
#define CGEN2TAGDIALOG_H

#include <QtGui/QDialog>

#include "../../../build/tmp/ui/ui_CGen2TagDialog.h"

#include "CGen2PassDialog.h"
#include "CGen2LockDialog.h"
#include "CGen2SetPasswordDialog.h"
#include "CGen2SetEPCDialog.h"
#include "CNXPCommand.h"

#include "../../reader/QrfeReaderInterface.h"
#include "../../reader/QrfeGen2ReaderInterface.h"
#include "../../reader/epc/EPC_Defines.h"

#include "../helper/QrfeProgressBar.h"

class CGen2TagDialog : public QDialog
{
    Q_OBJECT

public:
	CGen2TagDialog(QWidget *parent = 0);
    ~CGen2TagDialog();

public slots:
	int exec ( QrfeReaderInterface* ph, QString tagId );

	void tryAccept();

	void readTID();

	void readMem();
	void setMem();

	void setEPC();
	void lock();
	void setPassword();
	void killTag();
	void nxpCommand();

private slots:
	void memBankChanged(int index);

	void itemChanged(QTableWidgetItem*);

signals:
	void easterKeyUnlocked();

private:
	bool readTIDRegisterFromTag(QrfeProgressBar* pb = 0);

	bool readMemoryFromTag(MEMORY_BANK bank, QrfeProgressBar* pb = 0, uint size = 0);

	void handleError(QrfeGen2ReaderInterface::Gen2Result result, QString text);

	void clearInfo();
	void showOK();
	void showTagUnreachable();
	void showWrongPassword();
	void showMemoryOverrun();
	void showMemoryLocked();
	void showInsufficentPower();
	void showNOK();

	QrfeReaderInterface* m_ph;
	QrfeGen2ReaderInterface* m_gen2Reader;

	QString m_tagId;

    QList<QLineEdit*> m_epcToWrite;

    bool 	m_readActive;
	int lowestChanged;
	int highestChanged;
	uint	m_readMemSize;

	MEMORY_BANK m_readBank;

	CGen2PassDialog* m_passwordDialog;
	CGen2LockDialog* m_lockDialog;
	CGen2SetPasswordDialog* m_setPasswordDialog;
	CGen2SetEPCDialog* m_setEPCDialog;
	CNXPCommand*	m_nxpCommandDialog;

    Ui::CGen2TagDialogClass ui;
};

#endif // CGEN2TAGDIALOG_H
