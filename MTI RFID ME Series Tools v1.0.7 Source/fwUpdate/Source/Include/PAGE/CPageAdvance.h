#pragma once
#include "afxwin.h"
#include "TranxRFID.h"
//#include "stdafx.h"
#include "resource.h"

enum ENUM_USER_SEND
{
	USER_SEND_NOTHING,
	USER_SEND_SELECT,
	USER_SEDN_DATA,
};

// CPageAdvance dialog

class CPageAdvance : public CDialog
{
	DECLARE_DYNAMIC(CPageAdvance)

public:
	CPageAdvance(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPageAdvance();
	virtual BOOL OnInitDialog();
	void    API_CancelThread(void);
	bool    API_IsThdOn(void);
	void    API_SetThdEnable(bool bEnableFlag);

// Dialog Data
	enum { IDD = IDD_DIALOG2_ADVANCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()


private:
	HANDLE			 m_hdlThread;
	UINT			 m_uiThreadID;
	UINT			 m_uiInterval;
	CFont			 m_clsFont;
	clsTRANX_RFID   *m_clsTranxRFID;
	CWinThread		*m_clsThread;
	CRITICAL_SECTION m_CriticalSection;
	bool			 m_bShowVerToMumo;
	//SYNC Data
	volatile  bool	 m_bUpdateFlag;
	volatile  bool	 m_bIsOpen;
	volatile  bool	 m_bIsBusy;
	volatile  bool 	 m_bThdEnableFlag;
    ENUM_USER_SEND   m_enumUserCmdFlag;
	UINT			 m_uiUserCmd;
	HANDLE			 m_hdlImageFile;
	CString			 m_strFilePath;
	CString			 m_cstrVersion;
	CString			 m_cstrUserCmdData;
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
	void     AnalyzePacket(ENUM_CMD enumRtCmd);
	void     UpdateStatus(void);
	bool     Update(void);
	bool     CheckMode(TCHAR *CheckMode);
	CString  PrintError();
	static   UINT WINAPI CheckThread(LPVOID r_vpAppCls);
	static   UINT WINAPI CheckThread_UART(LPVOID r_vpAppCls);//add by yo chen, 2011/09/15

	//add by yo chen for Thd
	long volatile endThd;
	bool DetectSerial();

	//add by yo chen, for mark the stage of updating, 2011/09/21
	bool	stepEnter;
	bool	stepBegin;
	bool	stepWrite;
	bool	stepWriteDone;
	bool	stepExit;
	bool	isUSBThdRun;
	bool	isUARTThdRun;

	//Button3_Open
	DCB					 m_Dcb;
	CString				 m_cstrDllVer;
	volatile bool		 m_bStart;
	bool	isBootThd_UART;
	long volatile endThd_UART;
	bool bootThd();
	bool bootThd_UART();
	//bool ExitBLMode();
	bool OpenUART();


public:
	CListBox	  m_Memo;
	CEdit		  m_EditVersion;
	CProgressCtrl m_ProgressBar;
	CStatic		  m_textLinkState;
	CComboBox     m_ComboxSendCmd;
	CButton		  m_BtnSend;

	afx_msg void OnBnClickedButton1_Select();
	afx_msg void OnBnClickedButton2_SendCmd();
	afx_msg void OnBnClickedButton3_SendData();
	afx_msg void OnBnClickedButton4_ClearMemo();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CComboBox m_cmbPort;
	CButton m_BtnStart;
	afx_msg void OnBnClickedButton3_Open();
	CButton m_BtnBrowser;
};
