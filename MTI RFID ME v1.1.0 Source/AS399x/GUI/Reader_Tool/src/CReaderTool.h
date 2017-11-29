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
#ifndef CREADERTOOL_H
#define CREADERTOOL_H

#include <QtGui/QMainWindow>
#include "../build/tmp/ui/ui_CReaderTool.h"

#include <QrfeTrace.h>

#include <QMap>
#include <QTimer>

#include "CReaderManager.h"
#include "CTagViewManager.h"
#include "CReadRateCalc.h"
#include "CDataHandler.h"
#include "CTagManager.h"

#include "reader/QrfeReaderInterface.h"

#include "gui/helper/QrfeAboutDialog.h"
#include "gui/key/QrfeKeyWindow.h"
#include "gui/CSettingsDialog.h"
#include "gui/CTagListView.h"
#include "gui/CTagSettingsDialog.h"
#include "gui/readerSettings/CAmsLeoDialog.h"
#include "gui/tagSettings/CGen2TagDialog.h"

#include "action/CActionHandler.h"

#include "register_map.hxx"
#include "register_xml.h"

#include <usb_hid_wrapper.hxx>
#include <AMSAutoUpdateThread.hxx>



class CReaderTool : public QMainWindow
	, QrfeTraceModule
{
    Q_OBJECT

public:
    CReaderTool(QWidget *parent = 0);
    ~CReaderTool();

public slots:
	void gotReader(QrfeReaderInterface* ph);
	void lostReader(QrfeReaderInterface* ph);
	void readerChangedState(QString state);
	void readerChangedAction(QString action);

	void selectReader(int readerIndex);

	void startScan(bool start);
	void stopScan();
	void incrementScanProgress();
	void showScanTime();
	void showRegisterMap();
	void clearOfflineReader();

	void multiplexISR();

	void showSettings();
	void addSerialReader();

	void requestTagSettingsDialog(QString tagId);
	void requestTagAdvancedSettingsDialog(QString readerId, QString tagId);
	void requestReaderAdvancedSettingsDialog(QString readerId);
	void requestReaderRegisterMap(QString readerId);

	void showAliasNames(bool show);
	void useTimeToLive(bool use);
	void handleActionsToggled(bool checked);

	void easterKeyUnlocked();
	void autoUpdate(bool update);

	void currentReaderChanged(QString);
    //Add by yingwei tseng for using bar to set power, 2010/07/09
    void powerSliderChange(int);
	//End by yingwei tseng for using bar to set power, 2010/07/09	
	void countTotalTags(QString);

signals:
	void currentReaderChanged(QrfeReaderInterface* ph);

public:
	QrfeReaderInterface*	getInterface(QString readerId);
	virtual void show(QRect);

private:
    Ui::Reader_ToolClass ui;

    CActionHandler*				m_actionHandler;
    CDataHandler				m_dataHandler;

    QrfeAboutDialog*			m_aboutDialog;
    CTagListView*				m_tagListDialog;
    QrfeKeyWindow*              m_keyDialog;
    CSettingsDialog* 			m_settingsDialog;
    CTagSettingsDialog* 		m_tagSettingsDialog;
    QMap<QrfeReaderInterface*, CAmsLeoDialog*>     m_amsLEOSettingsDialogs;
	CGen2TagDialog*				m_gen2SettingsDialog;

    CReaderManager 				m_readerManager;
    CReadRateCalc*				m_readRateCalc;
    CTagViewManager* 			m_tagViewManager;
    CTagManager*				m_tagManager;

	RegisterMap*				m_regMapWindow;
	AMSCommunication*			m_amsComWrapper;
	AMSAutoUpdateThread*		m_updateThread;	//!< Automitc Update Thread

    QMap<QString, QrfeReaderInterface*>			m_reader;
	QMap<QrfeReaderInterface*, CGen2TagDialog*>	m_gen2SettingsDialogs;


    QStringList					m_activeReader;

    bool	m_scanActive;
    quint32 m_scanTimeout;
    QTimer* m_scanTimer;
    QTimer* m_scanProgressTimer;
    QTime	m_scanStart;

    QTimer*	m_multiplexTimer;
    int		m_multiplexCurrentReader;
    bool 	m_multiplexReaderOn;
	bool	m_regFileAvailable;

	void closeEvent(QCloseEvent *);
	void ActivateSettings(void);

	uint m_totalTagsCount;
	bool m_timeoutStop;
};

#endif // CREADERTOOL_H
