// USBControlDlg.h : header file
//
#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "CMenuCtrl.h"
//Image
#include <comdef.h>
#include "GdiPlus.h"
using namespace Gdiplus;

 

// CUSBControlDlg dialog
class CUSBControlDlg : public CDialog
{
// Construction
public:
	CUSBControlDlg(CWnd* pParent = NULL);	// standard constructor
	~CUSBControlDlg();

// Dialog Data
	enum { IDD = IDD_USBCONTROL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnOK();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CFont	m_clsFontLogoCaption;
	CBitmap m_bmp;
	CString m_Msg;
	GdiplusStartupInput m_gdiplusStartupInput;
	unsigned long* m_pGdiToken;
	//=====
	CString			 m_strFilePath;

	//=====

	void Init();
	bool ShowImageFromIDResource(UINT nID, LPCTSTR lpType,
							  int iWidth, int iHight, int iOffsetX = 0,int iOffsetY = 0); 

public:
	CMenuCtrl m_tabPage;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1Bowser();
};
