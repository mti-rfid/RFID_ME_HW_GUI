
// OemToolDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "DeviceInterface.h"
#include <fstream>
#include "afxcmn.h"
using namespace std;

//Image
#include <comdef.h>
#include "GdiPlus.h"
using namespace Gdiplus;

//#define TRY_DEBUG
#define SLEEP_TIME 5

enum ENUM_RESULT
{
	CMD_MAC_GET_MODULE_ID_A  = 0x10,
	CMD_MAC_GET_MODULE_ID_AR = 0x11,
	CMD_WRITE_IMAGE_OEM_A    = 0xA4,
	CMD_WRITE_IMAGE_OEM_AR   = 0xA5,
	CMD_READ_IMAGE_OEM_A     = 0xA6,
	CMD_READ_IMAGE_OEM_AR    = 0xA7,
	TIME_OUT,
	NOTHING
};


enum ENUM_ERROR
{
	WAIT_TO_GET_DATA_FROM_DAT_FILE,
	WRITE_DATA_ERROR,
	READ_DATA_ERROR,
	CHECK_OEM_NOT_CORRECT,

	NO_ERR
};


enum UI_MSG
{
	//
	MEMO_ADD_STRING,

	//Progress bar
	PROGRESS_ADD_STEP,
	PROGRESS_SET_0,
	PROGRESS_SET_100,

	//Thread
	THREAD_CLOSE
};


enum PKT_STEP
{
	HEADER_01,
	HEADER_02,
	HEADER_03,
	HEADER_04,
	DEVICE_ID,
	RESPONSE_ID,
	DATA_LENTH,
	HANDLE_PKT
};



//Product Information
#define USB_TYPE		_T("USB")
#define UART_TYPE		_T("RS232")
const UINT	VENDOR_ID[] = { 0x1325, 0x24E9, 0x24E9 };
const UINT	PRODUCT_ID[] = { 0xC029, 0x3993, 0x0865 };

const CString cstrToolName   = _T( "MTI RFID ME Series OEMCfg Update" );
const CString cstrToolVer    = _T( " v1.0.7" );
const CString cstrCopyRight  = _T("Copyright \u00A9 MTI");



// COemToolDlg dialog
class COemToolDlg : public CDialog
{
// Construction
public:
	COemToolDlg(CWnd* pParent = NULL);	// standard constructor
	~COemToolDlg();

// Dialog Data
	enum { IDD = IDD_OEMTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


private:
	//Transfer Data
	UCHAR	    RxChar;
	UCHAR	    RxBuf[256];
	UCHAR	    TxBuf[256];
	TBYTE	    RxCmd;
	TBYTE	    RxStep;
	TBYTE	    RxCnt;
	TBYTE	    RxPktLen;
	TBYTE	    RxDataLen;
	TBYTE	    RxStatus;

	//Sync	
	ifstream	  m_OEMfile;
	CString		  m_strFilePath;
	CString		  m_DeviceType;
	volatile bool m_bOpen;
	volatile bool m_bUpdate;
	UINT		  uiTimeOut;
	MSG			  m_Msg;
	
	//GDI
	GdiplusStartupInput  m_gdiplusStartupInput;	
	unsigned long		*m_pGdiToken;

	//Other
	ClsDeviceInterface  *m_clsDeviceInterface;
	CWinThread			*m_clsThread;
	DCB					 m_Dcb;	
	CFont		 		 m_clsFontLogoCaption;
	CString				 m_cstrDllVer;



	//Communicate
	ENUM_RESULT Regular(UINT uiInval);
	USHORT		CulCRC(unsigned char *buf, unsigned short bit_length);
	bool		SendData(UCHAR *buf);
	bool		ReadData();	
	static UINT WINAPI CheckThread(LPVOID r_vpAppCls);

	//GDI
	bool  ShowImageFromIDResource(UINT nID, LPCTSTR lpType,
								  int iWidth, int iHight, int iOffsetX,int iOffsetY);

	//Other
	void init();
	void ResetData();
	void AddMemo(CString tmp, int InsertIdx = 0);
	bool DetectSerial();
	void TransMessage(WPARAM wparam, LPARAM lparam, bool Overlap = true);
	void ShowError(ENUM_ERROR enumError);



public:
	bool GetModuleID(UINT uiID);
	bool WriteOEM(UINT addr, UINT val);
	bool ReadOEM(UINT addr, UINT &val);
	bool ReadOEMData(UINT addr, UCHAR *val);//rick
	bool ReadModelNameSerialNum(UINT addr, UCHAR &val);
	CString        ModelName;
	CEdit            m_ModelName;
	CString        SerialNumber;
	CEdit            m_SerialNumber;
	CString        FWVersion;
	CEdit            m_FWVersion;
	CString        OEMCfgID;
	CEdit            m_OEMCfgID;
	CString        OEMCfgUpdateID;
	CEdit            m_OEMCfgUpdateID;


public:
	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnBnClickedButtonBrowse();
	afx_msg void OnBnClickedButtonClearMemo();
	afx_msg void OnBnClickedButtonLink();
	
	//define event by myself
	afx_msg LRESULT OnMessage(WPARAM  wParam, LPARAM  lParam);

	CListBox	  m_Memo;
	CButton		  m_btnOpen;
	CComboBox	  m_cmbPort;
	CProgressCtrl m_ProgressBar;
};
