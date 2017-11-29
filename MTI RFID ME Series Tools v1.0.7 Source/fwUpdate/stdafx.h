
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


//Define by myself================
//#define TRY_DEBUG
#include "Utility.h"


//===Unicode===//
//*
const CString cstrToolName   = _T( "MTI RFID ME Series F/W Update" );
const CString cstrToolVer    = _T( "MTI RFID ME Series F/W Update v1.0.7" );
const CString cstrCopyRight  = _T( "Copyright \u00A9 MTI");
//*/
//===Unicode===//

//===no Set===//
/*
const CString cstrToolName   = _T( "MTI RFID ME HW F/W Update" );//TRADE MARK SIGN, \u2122
const CString cstrToolVer    = _T( "MTI RU-888 RFID Module F/W Update v1.0.2" );
const CString cstrCopyRight  = _T( "Copyright 2010 MTI" );//COPYRIGHT SIGN, \u00A9
//*/
//===no Set===//

//const CString cstrToolName;//   = "MTI RFID ME\u2122 HW F/W Update";
//const CString cstrToolVer;//    = "MTI RU-888 RFID Module F/W Update v1.0.1";
//const CString cstrCopyRight;//  = "Copyright \u00A9 2010 MTI";

#define TIMER_INTERVAL		  5
#define ASK_VERSION_INTERVAL  3000

//Mode
#define AP_MODE	_T( "[AP]" )
#define BL_MODE _T( "[BL]" )



//#include <afxdlgs.h>
//================================

