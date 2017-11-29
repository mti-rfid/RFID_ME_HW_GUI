#ifdef __CPageBasic
extern "C" {
#endif


#pragma once
#include "afxwin.h"
#include "USBControl.h"
#include "TranxRFID.h"

//add by yo chen
#include "DeviceInterface.h"
#define SLEEP_TIME 5

//enum ENUM_RESULT
//{
//	CMD_MAC_GET_MODULE_ID_A  = 0x10,
//	CMD_MAC_GET_MODULE_ID_AR = 0x11,
//	CMD_WRITE_IMAGE_OEM_A    = 0xA4,
//	CMD_WRITE_IMAGE_OEM_AR   = 0xA5,
//	CMD_READ_IMAGE_OEM_A     = 0xA6,
//	CMD_READ_IMAGE_OEM_AR    = 0xA7,
//	TIME_OUT,
//	NOTHING
//};
//
//enum PKT_STEP
//{
//	HEADER_01,
//	HEADER_02,
//	HEADER_03,
//	HEADER_04,
//	DEVICE_ID,
//	RESPONSE_ID,
//	DATA_LENTH,
//	HANDLE_PKT
//};
//ENd add by yo chen



// CPageBasic dialog

class CPageBasic : public CDialog
{
	DECLARE_DYNAMIC(CPageBasic)
	

public:
	CPageBasic(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPageBasic();
	virtual BOOL OnInitDialog();
	void    API_CancelThread(void);
	bool    API_IsThdOn(void);
	void    API_SetThdEnable(bool bEnableFlag);
	CString		uartModelName;//add by rick
	CString		uartSerialNumber;//add by rick
	//CString           m_cstrModelName;

// Dialog Data
	enum { IDD = IDD_DIALOG1_BASIC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	DECLARE_MESSAGE_MAP()
	HANDLE			 m_hdlThread;
	UINT			 m_uiThreadID;
	UINT			 m_uiInterval;
	CFont			 m_clsFont;
	clsTRANX_RFID   *m_clsTranxRFID;
	CWinThread		*m_clsThread;
	CRITICAL_SECTION m_CriticalSection;
	//SYNC Data
	volatile bool	 m_bUpdateFlag;
	volatile bool	 m_bIsOpen;
	volatile bool	 m_bIsBusy;
	volatile bool	 m_bThdEnableFlag;
	volatile bool	 m_bQryExitFwMode;
	HANDLE			 m_hdlImageFile;
	CString			 m_strFilePath;
	CString			 m_cstrVersion;
	UCHAR			 m_UartModelName;         
//	CString           m_cstrModelName;
	CString			 m_Msg;
	LARGE_INTEGER    m_strcLargeInt;	
	//check time
	LARGE_INTEGER	 m_liBefore;
	LARGE_INTEGER	 m_liNow;
	LARGE_INTEGER	 m_liFreq;

	void     Init(void);
	void	 Reset(void);
	void     AddMemo(CString str);
	void	 EnableBtn(bool bEnableFlag);
	void     EnableBtnOpen(bool bEnableFlagOpen);//add by rick, 2014/04/07
	void     EnableBtnBrowse(bool bEnableFlagBrowse);//add by rick, 2014/04/07
	void     AnalyzePacket(ENUM_CMD enumRtCmd);
	void     UpdateStatus(void);
	bool     Update(void);	
	bool     CheckMode(TCHAR *CheckMode);
	CString  PrintError();
	static   UINT WINAPI CheckThread(LPVOID r_vpAppCls);
	static   UINT WINAPI CheckThread_UART_Update(LPVOID r_vpAppCls);//add by yo chen, 2011/09/15

	//add by yo chen for Thd
	long volatile endThd;
	long volatile endThd_UART;

	//add by yo chen
	CString				 m_cstrDllVer;
	DCB					 m_Dcb;
	CString		  m_DeviceType;
	volatile bool m_bStart;
	bool	isBootThd_UART;

	bool bootThd();
	bool bootThd_UART();	
	bool	UpdateUART(void);
	bool	isUSBThdRun;
	bool	isUARTRun;
	bool ExitBLMode();
	bool OpenUART();

	
	

	/*UCHAR	    RxBuf[256];
	UCHAR	    TxBuf[256];
	TBYTE	    RxStatus;*/
	//USHORT		CulCRC(unsigned char *buf, unsigned short bit_length);
	//ENUM_RESULT Regular(UINT uiInval);
	UINT		  uiTimeOut;
	/*void ResetData();*/
	bool volatile m_bUpdate;

	//Transfer Data
	//UCHAR	    RxChar;
	//UCHAR	    RxBuf[256];
	//UCHAR	    TxBuf[256];
	//TBYTE	    RxCmd;
	//TBYTE	    RxStep;
	//TBYTE	    RxCnt;
	//TBYTE	    RxPktLen;
	//TBYTE	    RxDataLen;
	//TBYTE	    RxStatus;
	//END add by yo chen

public:
	CEdit		  m_EditVersion;
	CEdit		  m_Memo;
	CProgressCtrl m_ProgressBar;
	CButton		  m_BtnUpdate;
	CStatic		  m_textLinkState;	
	CEdit            m_EditModelName;//add by rick 2014-04-14
	CEdit            m_EditSerialNumber;//add by rick 2014-04-16

	//add by yo chen, for detect COM port and usb, 2011/05/20
	
	bool DetectSerial();
	/*bool GetModuleID(UINT uiID);*/

	
	//void AddMemo(CString tmp, int InsertIdx = 0);

	afx_msg void OnBnClickedButton1_Select();
	afx_msg void OnBnClickedButton2_Update();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CButton m_BtnStart;
	CComboBox m_cmbPort;
	//afx_msg void OnCbnSelchangeCombo1sleport();
	afx_msg void OnBnClickedButton3_Open();
	CButton m_BtnBrowser;
};



#ifdef __CPageBasic
}
#endif