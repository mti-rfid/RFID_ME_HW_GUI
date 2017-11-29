
// USBControl.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CUSBControlApp:
// See USBControl.cpp for the implementation of this class
//

class CUSBControlApp : public CWinAppEx
{
public:
	CUSBControlApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CUSBControlApp theApp;