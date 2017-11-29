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

#ifndef CNXPCOMMAND_H
#define CNXPCOMMAND_H

#include <QtGui/QDialog>
#include "../../../build/tmp/ui/ui_CNXPCommand.h"

#include "../../reader/ams/QrfeAmsReader.h"

class CNXPCommand : public QDialog
{
    Q_OBJECT

public:
    CNXPCommand(QWidget *parent = 0);
    ~CNXPCommand();

public slots:
	int exec(QrfeAmsReader* reader, QString epc);

	void tryAccept();
	void loadCommand();    //Add by yingwei tseng for displaying configuration edit, 2010/12/07

private:
	void handleError(QrfeGen2ReaderInterface::Gen2Result result, QString text);

	void clearInfo();
	void showOK();
	void showTagUnreachable();
	void showWrongPassword();
	void showMemoryOverrun();
	void showMemoryLocked();
	void showInsufficentPower();
	void showNOK();

private:
    Ui::CNXPCommandClass ui;

    QrfeAmsReader* m_reader;
};

#endif // CNXPCOMMAND_H
