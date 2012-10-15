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

#ifndef CSETTINGSDIALOG_H
#define CSETTINGSDIALOG_H

#include <QtGui/QDialog>
#include "../../build/tmp/ui/ui_CSettingsDialog.h"

class CSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    CSettingsDialog(QWidget *parent = 0);
    ~CSettingsDialog();

public slots:
    int exec();
    void tryAccept();

public:
    bool 	showAlias();
    void 	setShowAlias(bool on);

    bool 	useTtl();
    void 	setUseTtl(bool on);
    uint 	msecsToShowInactive();
    void 	setMsecsToShowInactive(uint msecs);
    uint 	msecsToShowOutOfRange();
    void 	setMsecsToShowOutOfRange(uint msecs);
    uint 	msecsToDelete();
    void 	setMsecsToDelete(uint msecs);

    bool 	useTrace();
    void 	setUseTrace(bool on);
    uchar 	traceLevel();
    void 	setTraceLevel(uchar level);

    bool 	useMultiplex();
    void 	setUseMultiplex(bool on);
    uint 	multiplexTime();
    void 	setMultiplexTime(uint time);

	bool	useRegisterUpdate();
	void	setUseRegisterUpdate(bool);
	uint	registerUpdateTime();
	void	setRegisterUpdateTime(uint);

private:
    Ui::CSettingsDialogClass ui;

    bool m_showAlias;

    bool m_useTtl;
    uint m_msecsToShowInactive;
    uint m_msecsToShowOutOfRange;
    uint m_msecsToDelete;

    bool  m_useTrace;
    uchar m_traceLevel;

    bool  m_useMultiplex;
    uint  m_multiplexTime;

    bool  m_useRegisterUpdate;
    uint  m_registerUpdateTime;

};

#endif // CSETTINGSDIALOG_H
