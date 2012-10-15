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

#ifndef CAMSLEODIALOG_H
#define CAMSLEODIALOG_H

#include <QtGui/QDialog>
#include "../../../build/tmp/ui/ui_CAmsLeoDialog.h"
#include "../../reader/QrfeReaderInterface.h"
#include "../../reader/ams/QrfeAmsReader.h"

//Add by yingwei tseng for printer application, 2010/05/27
#include "CReadRateCalc.h"    
#include "CTagViewManager.h"
#include "CTagManager.h"
//End by yingwei tseng for printer application, 2010/05/27
//Add by yingwei tseng for add TW/CN/KR/JP region, 2010/04/29
enum REGIN_CODE{
	REG_US = 0x00,    //United States
	REG_EU = 0x01,    //Europe
	REG_TW = 0x02,    //Taiwan
	REG_CN = 0x03,    //China
	REG_KR = 0x04,    //Korea
	REG_JP = 0x05,    //Japan
	REG_EU_300220 = 0x06    //Add by yingwei tseng for adding 869.85(EU) from OEMCfg, 2010/08/02
};
//End by yingwei tseng for add TW/CN/KR/JP region, 2010/04/29
//Add by yingwei tseng for printer application, 2010/05/27
#define MAXCHANNEL 52
#define MAXPOWER 24
//End by yingwei tseng for printer application, 2010/05/27
class CAmsLeoDialog : public QDialog
{
    Q_OBJECT

public:
	//Mod by yingwei tseng for printer application, 2010/05/27
    //CAmsLeoDialog(QWidget *parent = 0);
	CAmsLeoDialog(QWidget *parent = 0, CTagManager *tagManager = 0, CTagViewManager *tagViewManager = 0, CReadRateCalc *readRateCalc = 0);
	//End by yingwei tseng for printer application, 2010/05/27
    ~CAmsLeoDialog();

public slots:
	int exec ( QrfeReaderInterface* ph );

	void tryAccept();
	void setHopper( const QString &);
	void oneFreqChanged(double);
	void oneFreqChanged(int);

	void setInventoryInterval();
	void setFrequency();
	void setTagMode();
	void setGen2Configuration();

	void sweepFrequencys();
	void setLevel();
	void sendCommand();
	void levelSliderChanged(int);
	void sensSliderChanged(int);
	void openRFPDialog(void);
	void continuousSend(void);
	void setSensitivity(void);
	void loadRegion();              //Add by yingwei tseng for region, 2009/12/03 
	void setAdvancedReader();    //Add by yingwei tseng for submit, 2009/12/03
	//Add by yingwei tseng for frequency, 2010/01/04
	void loadFreqType();           
	void setFixedPowerOff();
	//End by yingwei tseng for frequency, 2010/01/04
	//Add by yingwei tseng for modulation tseting, 2010/05/10
	void incrementTransPatternProgress();
	void showTransPatternTime();
	void stopTransPattern();
	void disabledAdvancedItems();
	void enabledAdvancedItems();
	void loadModulation();
	//End by yingwei tseng for modulation tseting, 2010/05/10
	//Add by yingwei tseng for printer application, 2010/05/27
	void setPrinter();
	void changeLength(int);    
	void selectTabWidget(int tabIndex);
	void clearTagInfo();
	
	//End by yingwei tseng for printer application, 2010/05/27
	//Del by yingwei tseng for dongle region default setting, 2010/12/09
	//void loadProductPower();    	//Add by yingwei tseng for module and USB dongle power range, 2010/07/05
	//End by yingwei tseng for dongle region default setting, 2010/12/09

private:
	void displayPlot(double* ivals, double *qvals, double *svals, double *freqs, int size);
	void clearInfo();
	void showOK();
	void showNOK();
	void setAllocationParams();
	void readProfiles();
	void setAntennaPortTransmitTime();    //Add by yingwei tseng for pulse modulation tseting, 2010/10/12
	bool getGen2Config();
	bool getOutputLevel();
	bool getSensLevel();
	bool getFreqs();
	bool getPowerLevel();    //Add by yingwei tseng for get power level, 2010/08/02

	QMap<QString,QStringList> profiles;

	Ui::CAmsLeoDialogClass ui;

	QrfeAmsReader* m_ph;
	int m_tx_lev_coarse;
	int m_tx_lev_fine;
	//Add by yingwei tseng for modulation tseting, 2010/05/10
	quint32 m_transPatternTimeout;
	QTimer* m_transPatternTimer;
	QTimer* m_transPatternProgressTimer;
	QTime m_transPatternStart;
	//End by yingwei tseng for modulation tseting, 2010/05/10
	//Add by yingwei tseng for printer application, 2010/05/27
	QString m_newEPC;
	int initialLength, currentLength;
	QStringList					m_activeReader;
	QMap<QString, QrfeReaderInterface*>			m_reader;
	QMap<QIODevice*, QrfeReaderInterface*>	m_toDelete;
	CReadRateCalc *m_readRateCalc;
	CTagManager *m_tagManager;
	CTagViewManager *m_tagViewManager;
	QTimer* m_scanTimer;
	//End by yingwei tseng for printer application, 2010/05/27
};

#endif // CAMSLEODIALOG_H
