
// OemToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OemTool.h"
#include "OemToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

private:
	CFont   m_clsFont;
	CString	m_cstrDllVersion;


	bool  ShowImageFromIDResource( UINT nID, LPCTSTR lpType,
								   int iWidth, int iHight, int iOffsetX,int iOffsetY);

public:
	void SetDllVersion(CString	r_DllVersion);
	afx_msg void OnPaint();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	m_cstrDllVersion = _T("Can't detected");

	m_clsFont.CreateFont(18, 0, 0, 0, FW_HEAVY,
                         FALSE, FALSE, FALSE, 0, 
						 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, 
						 _T("Arial")); 
}


void CAboutDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages


	//Show Logo
	ShowImageFromIDResource(IDB_PNG1, _T("PNG"), 140, 110, 100, 150);


	//Show Text
	this->GetDlgItem(IDC_STATIC)->SetWindowText( cstrToolName  + _T("\n") + 
		                                         cstrToolVer   + _T("\n") + 
												 cstrCopyRight + _T("\n") + 
												 _T("\n") +
												 _T("Transfer.dll version : ") + m_cstrDllVersion );

	GetDlgItem(IDC_STATIC)->SetFont(&m_clsFont);
}



bool  CAboutDlg::ShowImageFromIDResource(UINT nID, LPCTSTR lpType,
							  int iWidth, int iHight, int iOffsetX,int iOffsetY)  
{  
	HINSTANCE hInst = AfxGetResourceHandle();  
	HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),lpType);   
	if(hRsrc == NULL) 	return false; 

	DWORD dwLen = SizeofResource(hInst, hRsrc);  
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);  
	if (!lpRsrc)	    return false;


	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, dwLen);  
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);  
	memcpy(pmem,lpRsrc,dwLen);  
	IStream* pstm;  
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);    

	CDC      *pDC = GetDC();
	Graphics  graphics( pDC->m_hDC);
	Image     dlgImage(pstm);
	graphics.DrawImage(&dlgImage,iOffsetX,iOffsetY,iWidth,iHight);

	GlobalUnlock(m_hMem);  
	pstm->Release();  
	FreeResource(lpRsrc); 

	return true;
}


void CAboutDlg::SetDllVersion(CString r_DllVersion)
{
	if( r_DllVersion.IsEmpty() == true )
		return;

	m_cstrDllVersion = r_DllVersion;
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()





// COemToolDlg dialog=======================================================

COemToolDlg::COemToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COemToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COemToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_Memo);
	DDX_Control(pDX, IDC_BUTTON_OPEN, m_btnOpen);
	DDX_Control(pDX, IDC_COMBO_PORT, m_cmbPort);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressBar);
	DDX_Control(pDX, IDC_EDIT5, m_ModelName);
	DDX_Control(pDX, IDC_EDIT6, m_SerialNumber);
	DDX_Control(pDX, IDC_EDIT2, m_FWVersion);
	DDX_Control(pDX, IDC_EDIT3, m_OEMCfgID);
	DDX_Control(pDX, IDC_EDIT4, m_OEMCfgUpdateID);
}

BEGIN_MESSAGE_MAP(COemToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &COemToolDlg::OnBnClickedButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &COemToolDlg::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_MEMO, &COemToolDlg::OnBnClickedButtonClearMemo)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &COemToolDlg::OnBnClickedButtonLink)
	ON_MESSAGE( ON_WM_MESSAGE, OnMessage)
END_MESSAGE_MAP()


// COemToolDlg message handlers

BOOL COemToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	init();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void COemToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;

		//Set Dll version to About Dialog
		dlgAbout.SetDllVersion(m_cstrDllVer);

		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}



// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR COemToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void COemToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();


		//Show Logo
		ShowImageFromIDResource(IDB_PNG1, _T("PNG"), 90, 75, 360, 1);
	}

}

COemToolDlg::~COemToolDlg()
{
	if( NULL != m_clsThread)
	{
		//::SendMessage(this->m_hWnd, THREAD_CLOSE, (WPARAM)0, (LPARAM)0);
		::PostThreadMessage(m_clsThread->m_nThreadID, THREAD_CLOSE, (WPARAM)0, (LPARAM)0);
		WaitForSingleObject(m_clsThread->m_hThread, 2000);
		//delete m_clsThread;
	}

	if( NULL != m_clsDeviceInterface )
	{
		m_clsDeviceInterface->API_Close();
		delete m_clsDeviceInterface;
	}
}


void COemToolDlg::TransMessage(WPARAM wparam, LPARAM lparam, bool overlap)
{
	if(overlap)
		::PostMessage( m_hWnd, ON_WM_MESSAGE, (WPARAM)wparam, (LPARAM)lparam );	
	else
		::SendMessage( m_hWnd, ON_WM_MESSAGE, (WPARAM)wparam, (LPARAM)lparam );
}


void COemToolDlg::AddMemo(CString str, int InsertIdx)
{
	//m_Memo.AddString( tmp );
	::SendMessage( m_hWnd, ON_WM_MESSAGE, (WPARAM)MEMO_ADD_STRING, (LPARAM)str.GetString() );	
}



void COemToolDlg::ResetData()
{
	RxStatus  = 0;
	RxCmd	  = 0;
	RxChar	  = 0;	
	RxCnt     = 0;
	RxPktLen  = 0;
	RxDataLen = 0;
	uiTimeOut = 0;
	m_bUpdate = false;
	RxStep    = HEADER_01;
	ZeroMemory(RxBuf, sizeof(RxBuf));
	ZeroMemory(TxBuf, sizeof(TxBuf));
}


void COemToolDlg::init()
{
	m_clsThread			 = NULL;
	m_clsDeviceInterface = NULL;
	m_bOpen				 = false;

	m_clsDeviceInterface = new ClsDeviceInterface;

	//dcb
	m_Dcb.BaudRate = CBR_115200;
	m_Dcb.ByteSize = 8;
	m_Dcb.StopBits = ONESTOPBIT;
	m_Dcb.Parity = NOPARITY;


	ResetData();	
	m_strFilePath.Empty();

	//Combox
	DetectSerial();
	m_cmbPort.AddString( _T("USB") );
	m_cmbPort.SetCurSel(0);
	//CString cstrCom;
	//for(int index=1; index < 10; index++)
	//{
	//	cstrCom.Format( _T("COM%d"), index);
	//	m_cmbPort.AddString( cstrCom );
	//}
	//m_cmbPort.AddString( _T("USB") );
	//m_cmbPort.SetCurSel(0);


	//Process	
	m_ProgressBar.SetRange(0,10);
	m_ProgressBar.SetPos(0);
	m_ProgressBar.SetStep(1);

	//Cpture
	this->SetWindowText(cstrToolName);

	//Init GDI+
	GdiplusStartup((ULONG_PTR *)&m_pGdiToken,&m_gdiplusStartupInput,NULL);

	//Logo Caption Static Text	
	m_clsFontLogoCaption.CreateFont( 20, 0, 0, 0, FW_HEAVY,
									 FALSE, FALSE, FALSE, 0, 
									 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, 
									 _T("Arial") ); 
	GetDlgItem(IDC_STATIC_LOGO_CAPTION)->SetFont(&m_clsFontLogoCaption);
	GetDlgItem(IDC_STATIC_LOGO_CAPTION)->SetWindowText( cstrToolName ) ;

	//Load Library
	if( false == m_clsDeviceInterface->API_LoadLib() )
	{	
		AddMemo( _T("Load Transfer.dll unsuccessfully") );
	}
	else
	{
		m_clsDeviceInterface->API_AskVersion( m_cstrDllVer );
	}

	
	//Thread

	m_clsThread = AfxBeginThread( (AFX_THREADPROC)CheckThread, (LPVOID)this, 0, 0, CREATE_SUSPENDED, 0);
	if(m_clsThread == NULL)		return;

	//Start Thread
	m_clsThread-> ResumeThread();
}



bool COemToolDlg::DetectSerial()
{
  //What will be the return value from this function (assume the worst)
	bool bSuccess = FALSE;

	HKEY hSERIALCOMM;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_QUERY_VALUE, &hSERIALCOMM) == ERROR_SUCCESS)
	{
		//Get the max value name and max value lengths
		DWORD dwMaxValueNameLen = 0;
		DWORD dwMaxValueLen = 0;
		DWORD dwQueryInfo = RegQueryInfoKey(hSERIALCOMM, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &dwMaxValueNameLen, &dwMaxValueLen, NULL, NULL);
		
		if (dwQueryInfo == ERROR_SUCCESS)
		{
			DWORD dwMaxValueNameSizeInChars = dwMaxValueNameLen + 1; //Include space for the NULL terminator
			DWORD dwMaxValueNameSizeInBytes = dwMaxValueNameSizeInChars * sizeof(TCHAR);
			DWORD dwMaxValueDataSizeInChars = dwMaxValueLen/sizeof(TCHAR) + 1; //Include space for the NULL terminator
			DWORD dwMaxValueDataSizeInBytes = dwMaxValueDataSizeInChars * sizeof(TCHAR);
		
			//Allocate some space for the value name and value data			
			  ATL::CHeapPtr<TCHAR> szValueName;
			  ATL::CHeapPtr<BYTE> byValue;

			  if (szValueName.Allocate(dwMaxValueNameSizeInChars) && byValue.Allocate(dwMaxValueDataSizeInBytes))
			  {
					bSuccess = TRUE;

					//Enumerate all the values underneath HKEY_LOCAL_MACHINE\HARDWARE\DEVICEMAP\SERIALCOMM
					DWORD dwIndex = 0;
					DWORD dwType;
					DWORD dwValueNameSize = dwMaxValueNameSizeInChars;
					DWORD dwDataSize = dwMaxValueDataSizeInBytes;
					memset(szValueName.m_pData, 0, dwMaxValueNameSizeInBytes);
					memset(byValue.m_pData, 0, dwMaxValueDataSizeInBytes);
					LONG nEnum = RegEnumValue(hSERIALCOMM, dwIndex, szValueName, &dwValueNameSize, NULL, &dwType, byValue, &dwDataSize);
			
					while (nEnum == ERROR_SUCCESS)
					{
						//If the value is of the correct type, then add it to the array
						if (dwType == REG_SZ)
						{
							TCHAR* szPort = reinterpret_cast<TCHAR*>(byValue.m_pData);

							//Add Comport string to ComboBox
							m_cmbPort.AddString(szPort);						 				
						}

						//Prepare for the next time around
						dwValueNameSize = dwMaxValueNameSizeInChars;
						dwDataSize = dwMaxValueDataSizeInBytes;
						memset(szValueName.m_pData, 0, dwMaxValueNameSizeInBytes);
						memset(byValue.m_pData, 0, dwMaxValueDataSizeInBytes);
						++dwIndex;
						nEnum = RegEnumValue(hSERIALCOMM, dwIndex, szValueName, &dwValueNameSize, NULL, &dwType, byValue, &dwDataSize);
					}
			  }
			  else
				SetLastError(ERROR_OUTOFMEMORY);
		}
			
		//Close the registry key now that we are finished with it    
		RegCloseKey(hSERIALCOMM);

		if (dwQueryInfo != ERROR_SUCCESS)
				SetLastError(dwQueryInfo);
	}
	
	return bSuccess;
}



bool  COemToolDlg::ShowImageFromIDResource(UINT nID, LPCTSTR lpType,
								  int iWidth, int iHight, int iOffsetX,int iOffsetY)  
{  

	HINSTANCE hInst = AfxGetResourceHandle();  
	HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),lpType);   
	if(hRsrc == NULL) 	return false; 

	DWORD dwLen = SizeofResource(hInst, hRsrc);  
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);  
	if (!lpRsrc)	    return false;


	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, dwLen);  
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);  
	memcpy(pmem,lpRsrc,dwLen);  
	IStream* pstm;  
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);    

	CDC      *pDC = GetDC();
	Graphics  graphics( pDC->m_hDC);
	Image     dlgImage(pstm);
	graphics.DrawImage(&dlgImage,iOffsetX,iOffsetY,iWidth,iHight);

	GlobalUnlock(m_hMem);  
	pstm->Release();  
	FreeResource(lpRsrc); 

	return true;
}


//====================================Event======================================

void COemToolDlg::OnBnClickedButtonBrowse()
{
	// TODO: Add your control notification handler code here
	OPENFILENAME  Dialog;				        //Dialog struct
	TCHAR		  szFile[1024] = TEXT("\0");	//File Name buffer¡@¡@¡@¡@¡@¡@¡@¡@¡@

	//Initial Dialog
	ZeroMemory(&Dialog,  sizeof(OPENFILENAME) );
	Dialog.lStructSize = sizeof(OPENFILENAME);
	Dialog.hwndOwner   = m_hWnd;
	Dialog.lpstrFile   = szFile;

	Dialog.lpstrFile[0]    = _T('\0');
	Dialog.nMaxFile        = sizeof(szFile)/sizeof(TCHAR);
	Dialog.lpstrFilter     = _T("Dat\0*.dat\0All Files\0*.*\0");
	Dialog.nFilterIndex    = 1;
	Dialog.lpstrFileTitle  = NULL;
	Dialog.nMaxFileTitle   = 0;
	Dialog.lpstrInitialDir = NULL;
	Dialog.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	//Show dialog
	if( GetOpenFileName(&Dialog) )
	{
		m_strFilePath = szFile;

		//Show Path to edit box
		this->GetDlgItem(IDC_EDIT_PATH)->SetWindowText(m_strFilePath);
	}
}



void COemToolDlg::OnBnClickedButtonLink()
{
	bool bIsOpen, USBFlag;
	UCHAR CheckVal[64]={0}, ValChar;    
	UCHAR Rxbuf[64]={0}, FWVersionChar[40]={0}, OEMCfgIDChar[44]={0}, OEMCfgUpdateIDChar[64]={0};
	//Load Transfer.dll
	if( m_clsDeviceInterface->bIsLoad == false)
	{
		if( false == m_clsDeviceInterface->API_LoadLib() )
		{
			AddMemo( _T("Load Transfer.dll unsuccessfully") );
			return;
		}	
		else
		{	
			m_clsDeviceInterface->API_AskVersion( m_cstrDllVer );
		}
	}		

	// TODO: Add your control notification handler code here
	if( m_bOpen == true)
	{
		m_btnOpen.SetWindowText( _T("Open") );
		m_cmbPort.EnableWindow(true);
		m_bOpen = !m_bOpen;
		m_clsDeviceInterface->API_Close();
	}
	else
	{	
		CString strDevice;	
		m_cmbPort.GetWindowText(strDevice);

		//Link
		if( strDevice == _T("USB"))
		{
			USBFlag=true;
			//try to open uart and send command
			for(int i = 0; i < (sizeof(VENDOR_ID) / sizeof(UINT)); i++)
			{
				if(bIsOpen = !!m_clsDeviceInterface->API_USB_Open(PRODUCT_ID[i], VENDOR_ID[i]))
					break;
			}
/*			//================================ModelName
			UINT address=0x30;
			for(int i=0; i<16; i++)
			{
				if( ReadModelNameSerialNum( address, ValChar) != false )//a6 04 00 30 
				{
					CheckVal[i] = ValChar;
					address++;
				}
				else
				{
					AddMemo( _T("Get Model Name unsuccessfully.") );
					m_clsDeviceInterface->API_Close();		
				}
			}
			ModelName=CheckVal;
			m_ModelName.SetWindowText(ModelName);

			//================================SerialNumber
			address=0x50;
			for(int i=0; i<16; i++)
			{
				if( ReadModelNameSerialNum( address, ValChar) != false )//a6 04 00 50 
				{
					CheckVal[i] = ValChar;
					address++;
				}
				else
				{
					AddMemo( _T("Get Serial Number unsuccessfully.") );
					m_clsDeviceInterface->API_Close();		
				}
			}
			SerialNumber=CheckVal;
			m_SerialNumber.SetWindowText(SerialNumber);

			//================================FW_Version
			if( ReadOEMData( 0, CheckVal) == false )//10 03 00
			{
				AddMemo( _T("Get FW Version unsuccessfully.") );
				m_clsDeviceInterface->API_Close();		
			}
			else
			{
				m_clsDeviceInterface->API_Read(Rxbuf, 64);
				for(int i=0; i<40; i++)
				{
					FWVersionChar[i]=Rxbuf[i+3];
				}
				FWVersion=FWVersionChar;
				m_FWVersion.SetWindowText(FWVersion);
			}

			//================================OEMCfgID
			ZeroMemory(Rxbuf, sizeof(Rxbuf));
			if( ReadOEMData( 02, CheckVal) == false )//10 03 02
			{
				AddMemo( _T("Get OEMCfgID unsuccessfully.") );
				m_clsDeviceInterface->API_Close();		
			}
			else
			{
				m_clsDeviceInterface->API_Read(Rxbuf, 64);
				for(int i=0; i<44; i++)
				{
					OEMCfgIDChar[i]=Rxbuf[i+3];
				}
				OEMCfgID=OEMCfgIDChar;
				m_OEMCfgID.SetWindowText(OEMCfgID);
			}

			//================================OEMCfgUpdateID
			ZeroMemory(Rxbuf, sizeof(Rxbuf));
			if( ReadOEMData( 03, CheckVal) == false )//10 03 03
			{
				AddMemo( _T("Get OEMCfgUpdateID unsuccessfully.") );
				m_clsDeviceInterface->API_Close();		
			}
			else
			{
				m_clsDeviceInterface->API_Read(Rxbuf, 64);
				for(int i=0; i<44; i++)
				{
					OEMCfgUpdateIDChar[i]=Rxbuf[i+3];				
				}
				OEMCfgUpdateID=OEMCfgUpdateIDChar;
				m_OEMCfgUpdateID.SetWindowText(OEMCfgUpdateID);
			}
*/
			if( !bIsOpen )
			{
				AddMemo( _T("Open USB error.") );
				m_clsDeviceInterface->API_Close();
				return;
			}		
		}
		else
		{
			USBFlag=false;
			int iComm = 0;			
			_stscanf_s(strDevice.GetString(), _T("COM%d"), &iComm);

			//try to open uart and send command
			if( false == m_clsDeviceInterface->API_Serial_Open( iComm , m_Dcb) )
			{
				AddMemo( _T("Open communication port error.") );				
				m_clsDeviceInterface->API_Close();
				return;
			}
		}

/*		//================================uartModelName
		UINT address=0x30;
		for(int i=0; i<16; i++)
		{
			if( ReadModelNameSerialNum( address, ValChar) != false )//a6 04 00 30 
			{
				CheckVal[i] = ValChar;
				address++;
			}
			else
			{
				AddMemo( _T("Get Model Name unsuccessfully.") );
				m_clsDeviceInterface->API_Close();		
			}
		}
		ModelName=CheckVal;
		m_ModelName.SetWindowText(ModelName);

		//================================uartSerialNumber
		address=0x50;
		for(int i=0; i<16; i++)
		{
			if( ReadModelNameSerialNum( address, ValChar) != false )//a6 04 00 50 
			{
				CheckVal[i] = ValChar;
				address++;
			}
			else
			{
				AddMemo( _T("Get Serial Number unsuccessfully.") );
				m_clsDeviceInterface->API_Close();		
			}
		}
		SerialNumber=CheckVal;
		m_SerialNumber.SetWindowText(SerialNumber);

		//================================uartFW_Version
		if( ReadOEMData( 0, CheckVal) == false )//10 03 00
		{
			AddMemo( _T("Get FW Version unsuccessfully.") );
			m_clsDeviceInterface->API_Close();		
		}
		else
		{
			Sleep(10);
			m_clsDeviceInterface->API_Read(Rxbuf, 64);
			for(int i=0; i<40; i++)
			{
				FWVersionChar[i]=Rxbuf[i+8];
			}
			FWVersion=FWVersionChar;
			m_FWVersion.SetWindowText(FWVersion);
		}

			//================================uartOEMCfgID
		ZeroMemory(Rxbuf, sizeof(Rxbuf));
		if( ReadOEMData( 02, CheckVal) == false )//10 03 03
		{
			AddMemo( _T("Get OEMCfgUpdateID unsuccessfully.") );
			m_clsDeviceInterface->API_Close();		
		}
		else
		{
			Sleep(10);
			m_clsDeviceInterface->API_Read(Rxbuf, 64);
			for(int i=0; i<44; i++)
			{
				OEMCfgIDChar[i]=Rxbuf[i+8];
			}
			OEMCfgID=OEMCfgIDChar;
			m_OEMCfgID.SetWindowText(OEMCfgID);
		}

			//================================OEMCfgUpdateID
		ZeroMemory(Rxbuf, sizeof(Rxbuf));
		if( ReadOEMData( 03, CheckVal) == false )//10 03 03
		{
			AddMemo( _T("Get OEMCfgUpdateID unsuccessfully.") );
			m_clsDeviceInterface->API_Close();		
		}
		else
		{
			Sleep(10);
			m_clsDeviceInterface->API_Read(Rxbuf, 64);
			for(int i=0; i<44; i++)
			{
				OEMCfgUpdateIDChar[i]=Rxbuf[i+8];
			}
			OEMCfgUpdateID=OEMCfgUpdateIDChar;
			m_OEMCfgUpdateID.SetWindowText(OEMCfgUpdateID);
		}
*/
		//Send Command to check
		/*if ( false ==GetModuleID(1) )
		{
			AddMemo( _T("Connected with the device unsuccessfully.") );
			m_clsDeviceInterface->API_Close();		
		}
		else
		{
			m_btnOpen.SetWindowText( _T("Close") );
			AddMemo( _T("Open communication port successfully.") );			

			m_cmbPort.EnableWindow(false);
			m_bOpen = !m_bOpen;

			m_DeviceType = m_clsDeviceInterface->API_AskDevType();
		}*/

		/*if( ReadOEMData( 0, CheckVal) == false )
		{
			AddMemo( _T("Get FW Version unsuccessfully.") );
			m_clsDeviceInterface->API_Close();		
		}
		else
		{
			m_clsDeviceInterface->API_Read(Rxbuf, 64);
			for(int i=0; i<40; i++)
			{
				FWVersionChar[i]=Rxbuf[i+3];
			}
			FWVersion=FWVersionChar;
			m_FWVersion.SetWindowText(FWVersion);
		}*/

	}

	//Send Command to check
	if ( false ==GetModuleID(1) )
	{
		if(m_bOpen==true)
		{
			//AddMemo( _T("Disonnected with the device successfully.") );
			AddMemo( _T("Connected with the device unsuccessfully.") );
		}
		else
		{
			//AddMemo( _T("Connected with the device unsuccessfully.") );
			AddMemo( _T("Disonnected with the device successfully.") );
			m_clsDeviceInterface->API_Close();		
		}
	}
	else
	{
		m_btnOpen.SetWindowText( _T("Close") );
		AddMemo( _T("Open communication port successfully.") );			

		m_cmbPort.EnableWindow(false);
		m_bOpen = !m_bOpen;

		m_DeviceType = m_clsDeviceInterface->API_AskDevType();
	}

	
	if( USBFlag==true )
	{
	//================================ModelName
			UINT address=0x30;
			for(int i=0; i<16; i++)
			{
				if( ReadModelNameSerialNum( address, ValChar) != false )//a6 04 00 30 
				{
					CheckVal[i] = ValChar;
					address++;
				}
				else
				{
					AddMemo( _T("Get Model Name unsuccessfully.") );
					m_clsDeviceInterface->API_Close();		
				}
			}
			ModelName=CheckVal;
			m_ModelName.SetWindowText(ModelName);

			//================================SerialNumber
			address=0x50;
			for(int i=0; i<16; i++)
			{
				if( ReadModelNameSerialNum( address, ValChar) != false )//a6 04 00 50 
				{
					CheckVal[i] = ValChar;
					address++;
				}
				else
				{
					AddMemo( _T("Get Serial Number unsuccessfully.") );
					m_clsDeviceInterface->API_Close();		
				}
			}
			SerialNumber=CheckVal;
			m_SerialNumber.SetWindowText(SerialNumber);

			//================================FW_Version
			if( ReadOEMData( 0, CheckVal) == false )//10 03 00
			{
				AddMemo( _T("Get FW Version unsuccessfully.") );
				m_clsDeviceInterface->API_Close();		
			}
			else
			{
				m_clsDeviceInterface->API_Read(Rxbuf, 64);
				for(int i=0; i<40; i++)
				{
					FWVersionChar[i]=Rxbuf[i+3];
				}
				FWVersion=FWVersionChar;
				m_FWVersion.SetWindowText(FWVersion);
			}

			//================================OEMCfgID
			ZeroMemory(Rxbuf, sizeof(Rxbuf));
			if( ReadOEMData( 02, CheckVal) == false )//10 03 02
			{
				AddMemo( _T("Get OEMCfgID unsuccessfully.") );
				m_clsDeviceInterface->API_Close();		
			}
			else
			{
				m_clsDeviceInterface->API_Read(Rxbuf, 64);
				for(int i=0; i<44; i++)
				{
					OEMCfgIDChar[i]=Rxbuf[i+3];
				}
				OEMCfgID=OEMCfgIDChar;
				m_OEMCfgID.SetWindowText(OEMCfgID);
			}

			//================================OEMCfgUpdateID
			ZeroMemory(Rxbuf, sizeof(Rxbuf));
			if( ReadOEMData( 03, CheckVal) == false )//10 03 03
			{
				AddMemo( _T("Get OEMCfgUpdateID unsuccessfully.") );
				m_clsDeviceInterface->API_Close();		
			}
			else
			{
				m_clsDeviceInterface->API_Read(Rxbuf, 64);
				for(int i=0; i<44; i++)
				{
					OEMCfgUpdateIDChar[i]=Rxbuf[i+3];				
				}
				OEMCfgUpdateID=OEMCfgUpdateIDChar;
				m_OEMCfgUpdateID.SetWindowText(OEMCfgUpdateID);
			}
	}
	else
	{
		//uart
			//================================uartModelName
		UINT address=0x30;
		for(int i=0; i<16; i++)
		{
			if( ReadModelNameSerialNum( address, ValChar) != false )//a6 04 00 30 
			{
				CheckVal[i] = ValChar;
				address++;
			}
			else
			{
				AddMemo( _T("Get Model Name unsuccessfully.") );
				m_clsDeviceInterface->API_Close();		
			}
		}
		ModelName=CheckVal;
		m_ModelName.SetWindowText(ModelName);

		//================================uartSerialNumber
		address=0x50;
		for(int i=0; i<16; i++)
		{
			if( ReadModelNameSerialNum( address, ValChar) != false )//a6 04 00 50 
			{
				CheckVal[i] = ValChar;
				address++;
			}
			else
			{
				AddMemo( _T("Get Serial Number unsuccessfully.") );
				m_clsDeviceInterface->API_Close();		
			}
		}
		SerialNumber=CheckVal;
		m_SerialNumber.SetWindowText(SerialNumber);

		//================================uartFW_Version
		if( ReadOEMData( 0, CheckVal) == false )//10 03 00
		{
			AddMemo( _T("Get FW Version unsuccessfully.") );
			m_clsDeviceInterface->API_Close();		
		}
		else
		{
			Sleep(10);
			m_clsDeviceInterface->API_Read(Rxbuf, 64);
			for(int i=0; i<40; i++)
			{
				FWVersionChar[i]=Rxbuf[i+8];
			}
			FWVersion=FWVersionChar;
			m_FWVersion.SetWindowText(FWVersion);
		}

			//================================uartOEMCfgID
		ZeroMemory(Rxbuf, sizeof(Rxbuf));
		if( ReadOEMData( 02, CheckVal) == false )//10 03 03
		{
			AddMemo( _T("Get OEMCfgUpdateID unsuccessfully.") );
			m_clsDeviceInterface->API_Close();		
		}
		else
		{
			Sleep(10);
			m_clsDeviceInterface->API_Read(Rxbuf, 64);
			for(int i=0; i<44; i++)
			{
				OEMCfgIDChar[i]=Rxbuf[i+8];
			}
			OEMCfgID=OEMCfgIDChar;
			m_OEMCfgID.SetWindowText(OEMCfgID);
		}

			//================================OEMCfgUpdateID
		ZeroMemory(Rxbuf, sizeof(Rxbuf));
		if( ReadOEMData( 03, CheckVal) == false )//10 03 03
		{
			AddMemo( _T("Get OEMCfgUpdateID unsuccessfully.") );
			m_clsDeviceInterface->API_Close();		
		}
		else
		{
			Sleep(10);
			m_clsDeviceInterface->API_Read(Rxbuf, 64);
			for(int i=0; i<44; i++)
			{
				OEMCfgUpdateIDChar[i]=Rxbuf[i+8];
			}
			OEMCfgUpdateID=OEMCfgUpdateIDChar;
			m_OEMCfgUpdateID.SetWindowText(OEMCfgUpdateID);
		}
	}

}



void COemToolDlg::OnBnClickedButtonUpdate()
{
	// TODO: Add your control notification handler code here
	
	if( m_bOpen == false )
	{
		AddMemo( _T("Please select communication port amd press \"Open\" button .") );
		return;
	}

	if(m_strFilePath.IsEmpty() == true)
	{
		AddMemo( _T("Please select the file.") );
		return;
	}


	//Set local as current locaol
	locale::global( locale("") ); 	

	m_OEMfile.open( m_strFilePath );
	
	//Set local as standard library
	locale::global( locale("C") ); 

	if(m_OEMfile.is_open() == false)
	{
		AddMemo(  _T("Open file unsuccessfully.") );
		m_OEMfile.close();
		return;	
	}

	m_OEMfile.seekg(ios_base::beg);
	m_bUpdate = true;
}




void COemToolDlg::OnBnClickedButtonClearMemo()
{
	// TODO: Add your control notification handler code here
	m_Memo.ResetContent();
}




LRESULT COemToolDlg::OnMessage(WPARAM  wParam, LPARAM  lParam)
{
	switch(wParam)
	{
		case MEMO_ADD_STRING:
			m_Memo.AddString((TCHAR *)lParam);
			break;


		case PROGRESS_ADD_STEP:
			m_ProgressBar.StepIt();
			break;

		case PROGRESS_SET_0:
			m_ProgressBar.SetPos(0);
			break;

		case PROGRESS_SET_100:
			m_ProgressBar.SetPos(100);
			break;

	}

	return *(new LRESULT); 
}


//==============================Command============================================
bool COemToolDlg::GetModuleID(UINT uiID)
{
#ifdef TRY_DEBUG
	OutputDebugString( _T("OemTool GetModuleID\n") );
#endif

	UCHAR tmp[128];
	memset(tmp, 0, sizeof(tmp));

	tmp[0] = CMD_MAC_GET_MODULE_ID_A;
	tmp[1] = 0x03;
	tmp[2] = uiID & 0xFF;

	if(SendData(tmp) == false)
		return false;

	if(ReadData() == false)
		return false;	

	return 	true;
}

bool COemToolDlg::WriteOEM(UINT addr, UINT val)
{
#ifdef TRY_DEBUG
	OutputDebugString( _T("OemTool WriteOE\nM") );
#endif


	UCHAR tmp[128];
	memset(tmp, 0, sizeof(tmp));

	tmp[0] = CMD_WRITE_IMAGE_OEM_A;
	tmp[1] = 0x05;
	tmp[2] = (addr >> 8) & 0xFF;
	tmp[3] = addr & 0xFF;
	tmp[4] = val;

	if(SendData(tmp) == false)
		return false;

	if(ReadData() == false)
		return false;	

	return 	true;
}

bool COemToolDlg::ReadOEM(UINT addr, UINT &val)
{
#ifdef TRY_DEBUG
	OutputDebugString( _T("OemTool ReadOEM\n") );
#endif


	UCHAR tmp[128];
	memset(tmp, 0, sizeof(tmp));

	tmp[0] = CMD_READ_IMAGE_OEM_A;
	tmp[1] = 0x04;
	tmp[2] = (addr >> 8) & 0xFF;
	tmp[3] = addr & 0xFF;

	if(SendData(tmp) == false)
		return false;

	if(ReadData() == false)
		return false;
	
	val = RxBuf[3];

	return 	true;
}

//rick
//bool COemToolDlg::ReadOEMData(UINT addr, UINT &val)
bool COemToolDlg::ReadOEMData(UINT addr, UCHAR *val)
{
	UCHAR tmp[128];
	memset(tmp, 0, sizeof(tmp));
	UCHAR Rxbuf[64];
	memset(Rxbuf, 0, sizeof(Rxbuf));

	//tmp[0] = CMD_READ_IMAGE_OEM_A;
	tmp[0] = 0x10;
	tmp[1] = 0x03;
	tmp[2] = addr;
	//tmp[2] = (addr >> 8) & 0xFF;
	//tmp[3] = addr & 0xFF;

	if(SendData(tmp) == false)
		return false;

	//m_clsDeviceInterface->API_Read(Rxbuf, 64);
	return 	true;
}

bool COemToolDlg::ReadModelNameSerialNum(UINT addr, UCHAR &val)
{
	UCHAR tmp[128];
	memset(tmp, 0, sizeof(tmp));
	UCHAR Rxbuf[64];
	memset(Rxbuf, 0, sizeof(Rxbuf));

	tmp[0] = CMD_READ_IMAGE_OEM_A;
	tmp[1] = 0x04;
	tmp[2] = 0x00;
	tmp[3] = addr;

	if(SendData(tmp) == false)
		return false;

	Sleep(10);
	m_clsDeviceInterface->API_Read(Rxbuf, 64);
	if( Rxbuf[0] == 0x4d && Rxbuf[1] == 0x54 && Rxbuf[2] == 0x49 && Rxbuf[3] == 0x52)
	{
		val = Rxbuf[8];
	}
	else
	{
		val = Rxbuf[3];
	}
	return 	true;
}


bool COemToolDlg::SendData(UCHAR *buf)
{
#ifdef TRY_DEBUG
	OutputDebugString( _T("OemTool SendData\n") );
#endif

	m_DeviceType = m_clsDeviceInterface->API_AskDevType();

	if(m_DeviceType == "USB")
	{
		//USB doesn't need Header¡BLength¡BCRC
		return !!m_clsDeviceInterface->API_Write(buf, 64);
	}
	else
	{
		int length = 0;
		ZeroMemory(TxBuf, sizeof(TxBuf));

		TxBuf[0] = 'M';
		TxBuf[1] = 'T';
		TxBuf[2] = 'I';
		TxBuf[3] = 'C';
		TxBuf[4] = 0xff;
		memcpy(TxBuf+5, buf, buf[1]);

		length = TxBuf[6] + 5;

		USHORT tmpCRC   = ~CulCRC(TxBuf, length*8);
		TxBuf[length++] = tmpCRC >> 8;
		TxBuf[length++] = tmpCRC & 0xFF;

		return !!m_clsDeviceInterface->API_Write(TxBuf, length);
	}
	

//#ifdef _UNICODE
//	UCHAR tmpData[length];
//	ZeroMemory(tmpData, sizeof(tmpData));	
//	for(int i=0; i<length; i++)   tmpData[i] = *(TxBuf+i) & 0xFF;
//
//	return clsSerial->API_Write(tmpData);
//
//#else
	//return m_clsDeviceInterface->API_Write(TxBuf, length);

//#endif
}



bool COemToolDlg::ReadData()
{
#ifdef TRY_DEBUG
	OutputDebugString( _T("OemTool ReadData\n") );
#endif
	
	ENUM_RESULT result = NOTHING;

	while(1)
	{
		result = Regular(SLEEP_TIME);

		switch(result)
		{
			case CMD_MAC_GET_MODULE_ID_AR:
				if(RxStatus == 0)
				{
					//AddMemo( _T("Connected."));
					return true;
				}
				else
				{
					//AddMemo( _T("Disconnected."));
					return false;
				}
				
				break;

			case CMD_WRITE_IMAGE_OEM_AR:
				if(RxStatus == 0)
				{					
					return true;
				}
				else
				{
					CString tmp;
					tmp.Format( _T("Write OEM unsuccessfully. Error code: 0x%02X"), RxStatus);
					AddMemo( tmp );
					return false;
				}
				break;


			case CMD_READ_IMAGE_OEM_AR:
				if(RxStatus == 0)
				{					
					return true;
				}
				else
				{
					CString tmp;
					tmp.Format( _T("Read   OEM unsuccessfully. Error code: 0x%02X"), RxStatus);
					AddMemo( tmp );
					return false;
				}
				break;


			case TIME_OUT:
				return false;	
		}

		Sleep(SLEEP_TIME);
	}

	return false;

}


ENUM_RESULT COemToolDlg::Regular(UINT uiInval)
{
	ENUM_RESULT result = NOTHING;
	UINT	    Length = 0;

	if(uiTimeOut < 200)
	{
		uiTimeOut += uiInval;
	}
	else
	{
		ResetData();
		return TIME_OUT;
	}
	

	Length = m_clsDeviceInterface->API_AskRevCount();
	if(Length <=0)	return result;


	if(m_DeviceType == "USB")
	{
#ifdef TRY_DEBUG
	OutputDebugString( _T("OemTool Regular USB\n") );
#endif

		ResetData();

		if( m_clsDeviceInterface->API_Read(RxBuf, Length) <=0 )
			return result;

		RxStatus = RxBuf[2];
		result   = (ENUM_RESULT)RxBuf[0];

		return result;
	}

	switch(RxStep)
	{
		case HEADER_01:
			ResetData();

			m_clsDeviceInterface->API_Read(&RxChar, 1);

			if(RxChar == 'M')
			{
				RxBuf[RxCnt] = RxChar;
				RxCnt++;
				RxStep++;
			}
			break;


		case HEADER_02:
			m_clsDeviceInterface->API_Read(&RxChar, 1);

			if(RxChar == 'T')
			{
				RxBuf[RxCnt] = RxChar;
				RxCnt++;
				RxStep++;
			}
			else
			{
				RxStep = HEADER_01;;
			}
			break;

		case HEADER_03:
			m_clsDeviceInterface->API_Read(&RxChar, 1);

			if(RxChar == 'I')
			{
				RxBuf[RxCnt] = RxChar;
				RxCnt++;
				RxStep++;
			}
			else
			{
				RxStep = HEADER_01;
			}
			break;

		case HEADER_04:
			m_clsDeviceInterface->API_Read(&RxChar, 1);

			if(RxChar == 'R')
			{
				RxBuf[RxCnt] = RxChar;
				RxCnt++;
				RxStep++;
			}
			else
			{
				RxStep = HEADER_01;
			}
			break;


		case DEVICE_ID: //device ID
			m_clsDeviceInterface->API_Read(&RxChar, 1);

			RxBuf[RxCnt] = RxChar;
			RxCnt++;
			RxStep++;
			break;

		case RESPONSE_ID:	//Command
			m_clsDeviceInterface->API_Read(&RxChar, 1);

			RxBuf[RxCnt] = RxChar;
			RxCmd = RxChar;

			RxCnt++;
			RxStep++;
			break;	

		case DATA_LENTH:	//Data length
			m_clsDeviceInterface->API_Read(&RxChar, 1);

			RxBuf[RxCnt] = RxChar;
			RxDataLen = RxChar;
			RxPktLen  = RxDataLen + 7;	//Header(4 byte) + Device ID(1 byte) + CRC (2 byte)

			if(RxDataLen !=0 )
			{
				RxCnt++;
				RxStep++;
			}
			break;		

		case HANDLE_PKT:	//Handle
			//Data + ( CRC(2 byte) - command ID(1 byte)  - Data Length(1 byte) )
			if( m_clsDeviceInterface->API_AskRevCount() <  RxDataLen )	
				break;

			RxStep = HEADER_01;

			m_clsDeviceInterface->API_Read( RxBuf+RxCnt, RxDataLen );

			//CRC Check
			if( 0x1D0F == CulCRC(RxBuf, RxPktLen*8) )
			{
				RxStatus = RxBuf[7];
				result   = (ENUM_RESULT)RxCmd;

				memcpy(RxBuf, RxBuf + 5, RxDataLen);
			}
			break;


		default:
			RxStep = HEADER_01;

	}//switch

	return result;

}


//
//ENUM_RESULT COemToolDlg::Regular(UINT uiInval)
//{
//	ENUM_RESULT result = NOTHING;
//	UINT	    Length = 0;
//
//	if(uiTimeOut < 2000)
//	{
//		uiTimeOut += uiInval;
//	}
//	else
//	{
//		ResetData();
//		return TIME_OUT;
//	}
//
//
//	Length = m_clsDeviceInterface->API_AskRevCount();
//
//	if(Length <=0)	return result;
//
//	//USB doesn't need Header¡BLength¡BCRC
//	if(m_DeviceType == "USB")
//	{
//		//ResetData();
//		m_clsDeviceInterface->API_Read(RxBuf, Length);
//		RxStep = HEADER_01;
//		//RxStep = RESPONSE_ID;
//		//RxCnt  = RESPONSE_ID - HEADER_01;
//	}
//
//    for (int i = 0; i < RevCount; i++)
//    {
//		switch(RxStep)
//		{
//			case HEADER_01:
//				ResetData();	
//
//				if(m_DeviceType == "RS232")
//				{
//					m_clsDeviceInterface->API_Read(&RxChar, 1);
//					RxBuf[RxCnt] = RxChar;
//				}
//							
//
//				if(RxChar == 'M')
//				{
//					RxCnt++;
//					RxStep++;
//				}
//				break;
//
//
//			case HEADER_02:
//				if(m_DeviceType == "RS232")
//				{
//					m_clsDeviceInterface->API_Read(&RxChar, 1);
//					RxBuf[RxCnt] = RxChar;
//				}
//
//				if(RxChar == 'T')
//				{
//					RxCnt++;
//					RxStep++;
//				}
//				else
//				{
//					RxStep = HEADER_01;;
//				}
//				break;
//
//			case HEADER_03:
//				if(m_DeviceType == "RS232")
//				{
//					m_clsDeviceInterface->API_Read(&RxChar, 1);
//					RxBuf[RxCnt] = RxChar;
//				}
//				if(RxChar == 'I')
//				{
//					RxCnt++;
//					RxStep++;
//				}
//				else
//				{
//					RxStep = HEADER_01;;
//				}
//				break;
//
//			case HEADER_04:
//				if(m_DeviceType == "RS232")
//				{
//					m_clsDeviceInterface->API_Read(&RxChar, 1);
//					RxBuf[RxCnt] = RxChar;
//				}
//
//				if(RxChar == 'R')
//				{
//					RxCnt++;
//					RxStep++;
//				}
//				else
//				{
//					RxStep = HEADER_01;;
//				}
//				break;
//
//
//			case DEVICE_ID: //device ID
//				if(m_DeviceType == "RS232")
//				{
//					m_clsDeviceInterface->API_Read(&RxChar, 1);
//					RxBuf[RxCnt] = RxChar;
//				}
//
//				RxCnt++;
//				RxStep++;
//				break;
//
//			case RESPONSE_ID:	//Command
//				if(m_DeviceType == "RS232")
//				{
//					m_clsDeviceInterface->API_Read(&RxChar, 1);
//					RxBuf[RxCnt] = RxCmd = RxChar;
//				}
//
//				RxCnt++;
//				RxStep++;
//				break;	
//
//			case DATA_LENTH:	//Data length
//				if(m_DeviceType == "RS232")
//				{
//					m_clsDeviceInterface->API_Read(&RxChar, 1);
//					RxBuf[RxCnt] = RxDataLen = RxChar;
//					RxPktLen     = RxDataLen + 7;	//Header(4 byte) + Device ID(1 byte) + CRC (2 byte)
//
//					if(RxDataLen ==0 )
//					{
//						RxStep = HEADER_01;
//						break;
//					}					 
//				}		
//
//				RxCnt++;
//				RxStep++;				
//				break;		
//
//
//			case HANDLE:	//Handle
//				RxStep = HEADER_01;
//
//				if(m_DeviceType == "RS232")
//				{
//					//Data + ( CRC(2 byte) - command ID(1 byte)  - Data Length(1 byte) )
//					if( m_clsDeviceInterface->API_AskRevCount() <  RxDataLen )	
//						break;
//
//					m_clsDeviceInterface->API_Read( RxBuf+RxCnt, RxDataLen );
//
//					//CRC Check
//					if( 0x1D0F == CulCRC(RxBuf, RxPktLen*8) )
//					{
//						RxStatus = RxBuf[7];
//						result   = (ENUM_RESULT)RxCmd;
//					}
//				}	
//				break;
//
//
//			default:
//				RxStep = HEADER_01;;
//
//		}//switch
//
//	}//for
//
//	return result;
//
//}


#define POLY 0x1021
USHORT COemToolDlg::CulCRC(unsigned char *buf, unsigned short bit_length)
{
    unsigned short shift=0, data=0, val=0;
    int i;

	shift = 0xFFFF;

	for ( i = 0; i < bit_length; i++ ){
        if ((i % 8) == 0)
			data = (*buf++) << 8;

		val = shift ^ data;
		shift = shift << 1;
		data = data << 1;

		if (val & 0x8000)
			shift = shift ^ POLY;		
	}	

	return shift;
}





//=================================Thread========================================
UINT WINAPI COemToolDlg::CheckThread(LPVOID r_vpAppCls)
{
	ENUM_ERROR enumError = NO_ERR;
	COemToolDlg *thisCls =  (COemToolDlg *)r_vpAppCls;

	while(1)
	{
		
		//Check Thread
		if(PeekMessage(&thisCls->m_Msg,0,0,0,0) && THREAD_CLOSE == thisCls->m_Msg.message) //get msg from message queue
		{
			AfxEndThread(0);
		}


		if(thisCls->m_bOpen == false)	
		{			
			Sleep(200);
			continue;
		}

		if(thisCls->m_bUpdate == false)
		{
			Sleep(200);
			continue;
		}


		//Start to upgrade
		thisCls->AddMemo( _T("Start to update OEMCfg data.") );
		thisCls->TransMessage( (WPARAM)PROGRESS_SET_0, (LPARAM)0 );			
		enumError = WAIT_TO_GET_DATA_FROM_DAT_FILE;

		CString tmp;
		while( thisCls->m_OEMfile.eof() == false )
		{
			
			UINT addr = 0, val = 0, CheckVal = 0;
			char tmpBuf[256];
			ZeroMemory(tmpBuf, sizeof(tmpBuf));

			thisCls->m_OEMfile.getline( tmpBuf, sizeof(tmpBuf) );

			//Find upgrade data
			if( strncmp(tmpBuf, "0x", 2) != 0 )
				continue;
				
			sscanf_s(tmpBuf,"%x,%x", &addr, &val);
			enumError = NO_ERR;				
			
			//Write data to OEM
			if( thisCls->WriteOEM(addr, val) == false )
			{
				enumError = WRITE_DATA_ERROR;
				break;	
			}

			//Read data from OEM to check
			if( thisCls->ReadOEM( addr, CheckVal ) == false )
			{		
				enumError = READ_DATA_ERROR;	
				break;	
			}

			if( CheckVal != val)
			{
				enumError = CHECK_OEM_NOT_CORRECT;				
				break;
			}
			

			//enumError = NO_ERROR;	
			thisCls->TransMessage( (WPARAM)PROGRESS_ADD_STEP, (LPARAM)0 );
												
		}//middle while


		if(enumError != NO_ERR)
		{
			thisCls->ShowError(enumError);			
			thisCls->m_ProgressBar.SetPos(0);			
		}
		else
		{			
			thisCls->AddMemo( _T("Update completely.") );
			thisCls->TransMessage((WPARAM)PROGRESS_SET_100, (LPARAM)0);
		}

		//Close File
		thisCls->m_OEMfile.close();

		//Reset Update flag
		thisCls->m_bUpdate = false;

	}//Outside while

	return 0;
}


void COemToolDlg::ShowError(ENUM_ERROR enumError)
{
	switch(enumError)
	{
		case WRITE_DATA_ERROR:
			AddMemo( _T("Write OEM unsuccessfully.") );
			break;	

		case READ_DATA_ERROR:
			AddMemo( _T("Read OEM unsuccessfully.") );
			break;	

		case CHECK_OEM_NOT_CORRECT:
			AddMemo( _T("Read OEM to check. Thhe value doesn't correct.") );
			break;	

		case WAIT_TO_GET_DATA_FROM_DAT_FILE:
			AddMemo( _T("The file doesn't have upgrade data.") );
			break;
	}
}

