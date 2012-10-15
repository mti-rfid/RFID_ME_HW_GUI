/*
Module : enumser.cpp
Purpose: Implementation for a class to enumerate the serial ports installed on a PC using a number
         of different approaches. 
Created: PJN / 03-10-1998
History: PJN / 23-02-1999 Code now uses QueryDosDevice if running on NT to determine 
                          which serial ports are available. This avoids having to open 
                          the ports at all. It should operate a lot faster in addition.
         PJN / 12-12-1999 Fixed a problem in the Win9x code path when trying to detect 
                          deactivated IRDA-ports. When trying to open those, you will 
                          get the error-code ERROR_GEN_FAILURE. 
         PJN / 17-05-2000 Code now uses GetDefaultCommConfig in all cases to detect 
                          the ports.
         PJN / 29-03-2001 1. Reverted code to use CreateFile or QueryDosDevice as it is 
                          much faster than using the GetDefaultCommConfig method
                          2. Updated copyright message
         PJN / 25-06-2001 1. Guess what, You now have the choice of using the GetDefaultCommConfig
                          thro the use of three versions of the function. You take your pick.
                          2. Fixed problem where port fails to be reported thro the CreateFile
                          mechanism when the error code is ERROR_SHARING_VIOLATION i.e. someone
                          has the port already open
         PJN / 11-08-2001 1. Made code path which uses QueryDosDevice more robust by checking to 
                          make sure the device name is of the form "COMxyz.." where xyz are numeric
         PJN / 13-08-2001 1. Made the code in IsNumeric more robust when sent an empty string
                          2. Optimized the code in EnumerateSerialPorts2 somewhat. Thanks to Dennis 
                          Lim for these suggestions.
         PJN / 22-05-2003 1. Updated copyright details.
                          2. Addition of a "EnumerateSerialPorts4" which uses Device Manager API
         PJN / 20-09-2003 1. Addition of a "EnumerateSerialPorts5" method. This method (hopefully
                          the last method!) uses EnumPorts and was provided by Andi Martin.
         PJN / 12-12-2003 1. Updated the sample app to VC 6.
                          2. Addition of a "EnumerateSerialPorts6" (See Note 4 below) which uses WMI.
                          3. You can now optionally exclude each function using preprocessor defines
                          of the form "NO_ENUMSERIAL_USING_XYX".
                          4. Made the functions members of a C++ class and renamed them to 
                          use more meaningful names
         PJN / 13-05-2004 1. Extended CEnumerateSerial::UsingSetupAPI to now also return the friendly
                          name of the port. Thanks to Jay C. Howard for prompting this update.
         PJN / 08-07-2006 1. Updated copyright details.
                          2. Addition of a CENUMERATESERIAL_EXT_CLASS macro to allow the code to be 
                          easily added to an extension dll.
                          3. Code now uses newer C++ style casts instead of C style casts.
                          4. Updated the code to clean compile on VC 2005.
                          5. Updated the documentation to use the same style as the web site.
         PJN / 08-11-2006 1. Extended CEnumerateSerial::UsingWMI to now also return the friendly
                          name of the port. Thanks to Giovanni Bajo for providing this update.
                          2. Fixed a bug where CEnumerateSerial::UsingSetupAPI forget to empty out 
                          the Friendly name array on start.
                          3. VariantInit is now called for the 2 VARIANT structs used in the UsingWMI
                          method code.
         PJN / 29-01-2007 1. Updated copyright details.
                          2. UsingSetupAPI code now uses the GUID_DEVINTERFACE_COMPORT guid to enumerate
                          COM ports. Thanks to David McMinn for reporting this nice addition.
                          3. Detection code which uses CreateFile call, now treats the error code
                          of ERROR_SEM_TIMEOUT as indication that a port is present.
         PJN / 09-06-2007 1. Following feedback from John Miles, it looks like my previous change of the
                          29 January 2007 to use GUID_DEVINTERFACE_COMPORT in the UsingSetupAPI method 
                          had the unintended consequence of causing this method not to work on any 
                          versions of Windows prior to Windows 2000. What I have now done is reinstate 
                          the old mechanism using the name UsingSetupAPI2 so that you can continue to use 
                          this approach if you need to support NT 4 and Windows 9x. The new approach of 
                          using GUID_DEVINTERFACE_COMPORT has been renamed to UsingSetupAPI1.
         PJN / 05-07-2007 1. Updated the code to work if the code does not include MFC. In this case, 
                          CUIntArray parameters becomes the ATL class CSimpleArray<UINT> and CStringArray
                          parameters become the ATL class CSimpleArray<CString>. Please note that this
                          support requires a recentish copy of Visual Studio and will not support Visual
                          C++ 6.0 as the code makes use of the ATL CString class. Thanks to Michael Venus
                          for prompting this update.
                          2. CEnumerateSerial::UsingWMI method now uses ATL smart pointers to improve
                          robustness of the code.
         PJN / 20-03-2008 1. Updated copyright details
                          2. Updates to preprocessor logic to correctly include UsingSetupAPI1 and
                          UsingSetupAPI2 functionality
                          3. Updated sample app to clean compile on VC 2008
         PJN / 23-11-2008 1. Updated code to compile correctly using _ATL_CSTRING_EXPLICIT_CONSTRUCTORS define
                          2. The code now only supports VC 2005 or later. 
                          3. Code now compiles cleanly using Code Analysis (/analyze)
                          4. Yes, Addition of another method called "UsingComDB" to enumerate serial ports!. 
                          This function uses the so called "COM Database" functions which are part of the 
                          Windows DDK which device drivers can use to support claiming an unused port number 
                          when the device driver is being installed. Please note that the list returning from 
                          this function will only report used port numbers. The device may or may not be 
                          actually present, just that the associated port number is currently "claimed". 
                          Thanks to Dmitry Nikitin for prompting this very nice addition. The code now 
                          supports a total of 8 different ways to enumerate serial ports!
         PJN / 29-11-2008 1. Addition of a ninth and hopefully final method to enumerate serial ports. The
                          function is called "UsingRegistry" and enumerates the ports by examining the 
                          registry location at HKEY_LOCAL_MACHINE\HARDWARE\DEVICEMAP\SERIALCOMM. Thanks to
                          Martin Oberhuber for prompting this update.
                          2. Fixed a bug where the last error value was not being preserved in 
                          CEnumerateSerial::UsingComDB.
         PJN / 30-04-2009 1. Updated copyright details.
                          2. Updated the sample app's project settings to more modern default values.
                          3. Updated the sample app to log the time taken for the various methods.
         
Copyright (c) 1998 - 2009 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////////  Includes  //////////////////////////////////

//#include "stdafx.h"
#include "enumser.h"
#ifdef ENUMSERIAL_USE_QT
#include <tchar.h>
#endif

#if !defined(NO_ENUMSERIAL_USING_SETUPAPI1) || !defined(NO_ENUMSERIAL_USING_SETUPAPI2)
  #ifndef _INC_SETUPAPI
//  #pragma message("To avoid this message, please put setupapi.h in your pre compiled header (normally stdafx.h)")
  #include <setupapi.h>
  #endif

  #ifndef GUID_DEVINTERFACE_COMPORT
  DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0x86E0D1E0L, 0x8089, 0x11D0, 0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73);
  #endif
  
  typedef HKEY (__stdcall SETUPDIOPENDEVREGKEY)(HDEVINFO, PSP_DEVINFO_DATA, DWORD, DWORD, DWORD, REGSAM);
  typedef BOOL (__stdcall SETUPDICLASSGUIDSFROMNAME)(LPCTSTR, LPGUID, DWORD, PDWORD);
  typedef BOOL (__stdcall SETUPDIDESTROYDEVICEINFOLIST)(HDEVINFO);
  typedef BOOL (__stdcall SETUPDIENUMDEVICEINFO)(HDEVINFO, DWORD, PSP_DEVINFO_DATA);
  typedef HDEVINFO (__stdcall SETUPDIGETCLASSDEVS)(LPGUID, LPCTSTR, HWND, DWORD);
  typedef BOOL (__stdcall SETUPDIGETDEVICEREGISTRYPROPERTY)(HDEVINFO, PSP_DEVINFO_DATA, DWORD, PDWORD, PBYTE, DWORD, PDWORD);
#endif  

#ifndef NO_ENUMSERIAL_USING_ENUMPORTS
  #ifndef _WINSPOOL_
  #pragma message("To avoid this message, please put winspool.h in your pre compiled header (normally stdafx.h)")
  #include <winspool.h>
  #endif
#endif

#ifndef NO_ENUMSERIAL_USING_WMI
  #ifndef __IWbemLocator_FWD_DEFINED__
  #pragma message("To avoid this message, please put WBemCli.h in your pre compiled header (normally stdafx.h)")
  #include <WbemCli.h>
  #endif
  
  #ifndef _INC_COMDEF
  #pragma message("To avoid this message, please put comdef.h in your pre compiled header (normally stdafx.h)")
  #include <comdef.h>
  #endif

  #ifndef __ATLBASE_H__
  #pragma message("EnumSerialPorts as of v1.16 requires ATL support to implement its functionality. If your project is MFC only, then you need to update it to include ATL support")
  #endif
  
  #ifdef _AFX
    #ifndef __AFXPRIV_H__
    #pragma message("To avoid this message, please put afxpriv.h in your pre compiled header (normally stdafx.h)")
    #include <afxpriv.h>
    #endif
  #endif

  //Automatically pull in the library WbemUuid.Lib since we need the WBem Guids
  #pragma comment(lib, "WbemUuid.Lib")
#endif

#ifndef NO_ENUMSERIAL_USING_COMDB
  #ifndef HCOMDB
  DECLARE_HANDLE(HCOMDB);
  typedef HCOMDB *PHCOMDB;
  #endif
  
  #ifndef CDB_REPORT_BYTES
  #define CDB_REPORT_BYTES 0x1  
  #endif
  
  typedef LONG (__stdcall COMDBOPEN)(PHCOMDB);
  typedef LONG (__stdcall COMDBCLOSE)(HCOMDB);
  typedef LONG (__stdcall COMDBGETCURRENTPORTUSAGE)(HCOMDB, PBYTE, DWORD, ULONG, LPDWORD);
#endif


///////////////////////////// Implementation //////////////////////////////////

#ifndef NO_ENUMSERIAL_USING_CREATEFILE
#ifdef _AFX
BOOL CEnumerateSerial::UsingCreateFile(CUIntArray& ports)
#else
BOOL CEnumerateSerial::UsingCreateFile(CSimpleArray<UINT>& ports)
#endif
{
  //Make sure we clear out any elements which may already be in the array
  ports.RemoveAll();

  //Up to 255 COM ports are supported so we iterate through all of them seeing
  //if we can open them or if we fail to open them, get an access denied or general error error.
  //Both of these cases indicate that there is a COM port at that number. 
  for (UINT i=1; i<256; i++)
  {
    //Form the Raw device name
    CString sPort;
    sPort.Format(_T("\\\\.\\COM%d"), i);

    //Try to open the port
    BOOL bSuccess = FALSE;
    HANDLE hPort = ::CreateFile(sPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if (hPort == INVALID_HANDLE_VALUE)
    {
      DWORD dwError = GetLastError();

      //Check to see if the error was because some other app had the port open or a general failure
      if (dwError == ERROR_ACCESS_DENIED || dwError == ERROR_GEN_FAILURE || dwError == ERROR_SHARING_VIOLATION || dwError == ERROR_SEM_TIMEOUT)
        bSuccess = TRUE;
    }
    else
    {
      //The port was opened successfully
      bSuccess = TRUE;

      //Don't forget to close the port, since we are going to do nothing with it anyway
      CloseHandle(hPort);
    }

    //Add the port number to the array which will be returned
    if (bSuccess)
      ports.Add(i);
  }

  //Return the success indicator
  return TRUE;
}
#endif

BOOL CEnumerateSerial::IsNumeric(LPCTSTR pszString, BOOL bIgnoreColon)
{
  size_t nLen = _tcslen(pszString);
  if (nLen == 0)
    return FALSE;

  //Assume the best
  BOOL bNumeric = TRUE;

  for (size_t i=0; i<nLen && bNumeric; i++)
  {
    bNumeric = (_istdigit(pszString[i]) != 0);
    if (bIgnoreColon && (pszString[i] == _T(':')))
      bNumeric = TRUE;
  }

  return bNumeric;
}

#ifndef NO_ENUMSERIAL_USING_QUERYDOSDEVICE
#ifdef _AFX
BOOL CEnumerateSerial::UsingQueryDosDevice(CUIntArray& ports)
#else
BOOL CEnumerateSerial::UsingQueryDosDevice(CSimpleArray<UINT>& ports)
#endif
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Make sure we clear out any elements which may already be in the array
  ports.RemoveAll();

  //Determine what OS we are running on
  OSVERSIONINFO osvi;
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  BOOL bGetVer = GetVersionEx(&osvi);

  //On NT use the QueryDosDevice API
  if (bGetVer && (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT))
  {
    //Use QueryDosDevice to look for all devices of the form COMx. Since QueryDosDevice does
    //not consitently report the required size of buffer, lets start with a reasonable buffer size
    //of 4096 characters and go from there
    int nChars = 4096;
    BOOL bWantStop = FALSE;
    while (nChars && !bWantStop)
    {
      ATL::CHeapPtr<TCHAR> szDevices;
      if (szDevices.Allocate(nChars))
      {
        DWORD dwChars = QueryDosDevice(NULL, szDevices, nChars);
        if (dwChars == 0)
        {
          DWORD dwError = GetLastError();
          if (dwError == ERROR_INSUFFICIENT_BUFFER)
          {
            //Expand the buffer and  loop around again
            nChars *= 2;
          }
          else
            bWantStop = TRUE;
        }
        else
        {
          bSuccess = TRUE;
          bWantStop = TRUE;
          size_t i=0;
          while (szDevices[i] != _T('\0'))
          {
            //Get the current device name
            TCHAR* pszCurrentDevice = &szDevices[i];

            //If it looks like "COMX" then
            //add it to the array which will be returned
            size_t nLen = _tcslen(pszCurrentDevice);
            if (nLen > 3)
            {
              if ((_tcsnicmp(pszCurrentDevice, _T("COM"), 3) == 0) && IsNumeric(&pszCurrentDevice[3], FALSE))
              {
                //Work out the port number
                int nPort = _ttoi(&pszCurrentDevice[3]);
                ports.Add(nPort);
              }
            }

            //Go to next device name
            i += (nLen + 1);
          }
        }
      }
      else
      {
        bWantStop = TRUE;
        SetLastError(ERROR_OUTOFMEMORY);        
      }
    }
  }
  else
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

  return bSuccess;
}
#endif

#ifndef NO_ENUMSERIAL_USING_GETDEFAULTCOMMCONFIG
#ifdef _AFX
BOOL CEnumerateSerial::UsingGetDefaultCommConfig(CUIntArray& ports)
#else
BOOL CEnumerateSerial::UsingGetDefaultCommConfig(CSimpleArray<UINT>& ports)
#endif
{
  //Make sure we clear out any elements which may already be in the array
  ports.RemoveAll();

  //Up to 255 COM ports are supported so we iterate through all of them seeing
  //if we can get the default configuration
  for (UINT i=1; i<256; i++)
  {
    //Form the Raw device name
    CString sPort;
    sPort.Format(_T("COM%d"), i);

    COMMCONFIG cc;
    DWORD dwSize = sizeof(COMMCONFIG);
    if (GetDefaultCommConfig(sPort, &cc, &dwSize))
      ports.Add(i);
  }

  //Return the success indicator
  return TRUE;
}
#endif

#ifndef NO_ENUMSERIAL_USING_SETUPAPI1
#ifdef ENUMSERIAL_USE_QT
BOOL CEnumerateSerial::UsingSetupAPI1(QList<UINT>& ports, QStringList& sFriendlyNames)
#else
#ifdef _AFX
BOOL CEnumerateSerial::UsingSetupAPI1(CUIntArray& ports, CStringArray& sFriendlyNames)
#else
BOOL CEnumerateSerial::UsingSetupAPI1(CSimpleArray<UINT>& ports, CSimpleArray<CString>& sFriendlyNames)
#endif
#endif
{
  //Make sure we clear out any elements which may already be in the array(s)
#ifdef ENUMSERIAL_USE_QT
  ports.clear();
  sFriendlyNames.clear();
#else
  ports.RemoveAll();
  sFriendlyNames.RemoveAll();
#endif

  //Get the various function pointers we require from setupapi.dll
  HINSTANCE hSetupAPI = LoadLibrary(_T("SETUPAPI.DLL"));
  if (hSetupAPI == NULL)
    return FALSE;

  SETUPDIOPENDEVREGKEY* lpfnLPSETUPDIOPENDEVREGKEY = reinterpret_cast<SETUPDIOPENDEVREGKEY*>(GetProcAddress(hSetupAPI, "SetupDiOpenDevRegKey"));
#ifdef _UNICODE
  SETUPDIGETCLASSDEVS* lpfnSETUPDIGETCLASSDEVS = reinterpret_cast<SETUPDIGETCLASSDEVS*>(GetProcAddress(hSetupAPI, "SetupDiGetClassDevsW"));
  SETUPDIGETDEVICEREGISTRYPROPERTY* lpfnSETUPDIGETDEVICEREGISTRYPROPERTY = reinterpret_cast<SETUPDIGETDEVICEREGISTRYPROPERTY*>(GetProcAddress(hSetupAPI, "SetupDiGetDeviceRegistryPropertyW"));
#else
  SETUPDIGETCLASSDEVS* lpfnSETUPDIGETCLASSDEVS = reinterpret_cast<SETUPDIGETCLASSDEVS*>(GetProcAddress(hSetupAPI, "SetupDiGetClassDevsA"));
  SETUPDIGETDEVICEREGISTRYPROPERTY* lpfnSETUPDIGETDEVICEREGISTRYPROPERTY = reinterpret_cast<SETUPDIGETDEVICEREGISTRYPROPERTY*>(GetProcAddress(hSetupAPI, "SetupDiGetDeviceRegistryPropertyA"));
#endif
  SETUPDIDESTROYDEVICEINFOLIST* lpfnSETUPDIDESTROYDEVICEINFOLIST = reinterpret_cast<SETUPDIDESTROYDEVICEINFOLIST*>(GetProcAddress(hSetupAPI, "SetupDiDestroyDeviceInfoList"));
  SETUPDIENUMDEVICEINFO* lpfnSETUPDIENUMDEVICEINFO = reinterpret_cast<SETUPDIENUMDEVICEINFO*>(GetProcAddress(hSetupAPI, "SetupDiEnumDeviceInfo"));

  if ((lpfnLPSETUPDIOPENDEVREGKEY == NULL) || (lpfnSETUPDIDESTROYDEVICEINFOLIST == NULL) ||
      (lpfnSETUPDIENUMDEVICEINFO == NULL) || (lpfnSETUPDIGETCLASSDEVS == NULL) || (lpfnSETUPDIGETDEVICEREGISTRYPROPERTY == NULL))
  {
    //Unload the setup dll
    FreeLibrary(hSetupAPI);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
  }
  
  //Now create a "device information set" which is required to enumerate all the ports
  GUID guid = GUID_DEVINTERFACE_COMPORT;
  HDEVINFO hDevInfoSet = lpfnSETUPDIGETCLASSDEVS(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
  if (hDevInfoSet == INVALID_HANDLE_VALUE)
  {
		DWORD dwLastError = GetLastError();
  
    //Unload the setup dll
    FreeLibrary(hSetupAPI);
    
    SetLastError(dwLastError);

    return FALSE;
  }

  //Finally do the enumeration
  BOOL bMoreItems = TRUE;
  int nIndex = 0;
  SP_DEVINFO_DATA devInfo;
  while (bMoreItems)
  {
    //Enumerate the current device
    devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
    bMoreItems = lpfnSETUPDIENUMDEVICEINFO(hDevInfoSet, nIndex, &devInfo);
    if (bMoreItems)
    {
      //Did we find a serial port for this device
      BOOL bAdded = FALSE;

      //Get the registry key which stores the ports settings
      HKEY hDeviceKey = lpfnLPSETUPDIOPENDEVREGKEY(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
      if (hDeviceKey)
      {
        //Read in the name of the port
        TCHAR pszPortName[256];
        DWORD dwSize = sizeof(pszPortName);
        DWORD dwType = 0;
  	    if ((RegQueryValueEx(hDeviceKey, _T("PortName"), NULL, &dwType, reinterpret_cast<LPBYTE>(pszPortName), &dwSize) == ERROR_SUCCESS) && (dwType == REG_SZ))
        {
          //If it looks like "COMX" then
          //add it to the array which will be returned
          size_t nLen = _tcslen(pszPortName);
          if (nLen > 3)
          {
            if ((_tcsnicmp(pszPortName, _T("COM"), 3) == 0) && IsNumeric(&pszPortName[3], FALSE))
            {
              //Work out the port number
              int nPort = _ttoi(&pszPortName[3]);
#ifdef ENUMSERIAL_USE_QT
              ports.append(nPort);
#else
              ports.Add(nPort);
#endif

              bAdded = TRUE;
            }
          }
        }

        //Close the key now that we are finished with it
        RegCloseKey(hDeviceKey);
      }

      //If the port was a serial port, then also try to get its friendly name
      if (bAdded)
      {
        TCHAR pszFriendlyName[256];
        DWORD dwSize = sizeof(pszFriendlyName);
        DWORD dwType = 0;
        if (lpfnSETUPDIGETDEVICEREGISTRYPROPERTY(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, reinterpret_cast<PBYTE>(pszFriendlyName), dwSize, &dwSize) && (dwType == REG_SZ))
#ifdef ENUMSERIAL_USE_QT
			sFriendlyNames.append(QString::fromUtf16(pszFriendlyName));
#else
          sFriendlyNames.Add(pszFriendlyName);
#endif
        else
#ifdef ENUMSERIAL_USE_QT
          sFriendlyNames.append(QString(""));
#else
          sFriendlyNames.Add(_T(""));
#endif
      }
    }

    ++nIndex;
  }

  //Free up the "device information set" now that we are finished with it
  lpfnSETUPDIDESTROYDEVICEINFOLIST(hDevInfoSet);

  //Unload the setup dll
  FreeLibrary(hSetupAPI);

  //Return the success indicator
  return TRUE;
}
#endif

#ifndef NO_ENUMSERIAL_USING_SETUPAPI2
#ifdef _AFX
BOOL CEnumerateSerial::UsingSetupAPI2(CUIntArray& ports, CStringArray& sFriendlyNames)
#else
BOOL CEnumerateSerial::UsingSetupAPI2(CSimpleArray<UINT>& ports, CSimpleArray<CString>& sFriendlyNames)
#endif
{
  //Make sure we clear out any elements which may already be in the array(s)
  ports.RemoveAll();
  sFriendlyNames.RemoveAll();

  //Get the function pointers to "SetupDiGetClassDevs", "SetupDiGetClassDevs", "SetupDiEnumDeviceInfo", "SetupDiOpenDevRegKey" 
  //and "SetupDiDestroyDeviceInfoList" in setupapi.dll
  HINSTANCE hSetupAPI = LoadLibrary(_T("SETUPAPI.DLL"));
  if (hSetupAPI == NULL)
    return FALSE;

  SETUPDIOPENDEVREGKEY* lpfnLPSETUPDIOPENDEVREGKEY = reinterpret_cast<SETUPDIOPENDEVREGKEY*>(GetProcAddress(hSetupAPI, "SetupDiOpenDevRegKey"));
#ifdef _UNICODE
  SETUPDICLASSGUIDSFROMNAME* lpfnSETUPDICLASSGUIDSFROMNAME = reinterpret_cast<SETUPDICLASSGUIDSFROMNAME*>(GetProcAddress(hSetupAPI, "SetupDiClassGuidsFromNameW"));
  SETUPDIGETCLASSDEVS* lpfnSETUPDIGETCLASSDEVS = reinterpret_cast<SETUPDIGETCLASSDEVS*>(GetProcAddress(hSetupAPI, "SetupDiGetClassDevsW"));
  SETUPDIGETDEVICEREGISTRYPROPERTY* lpfnSETUPDIGETDEVICEREGISTRYPROPERTY = reinterpret_cast<SETUPDIGETDEVICEREGISTRYPROPERTY*>(GetProcAddress(hSetupAPI, "SetupDiGetDeviceRegistryPropertyW"));
#else
  SETUPDICLASSGUIDSFROMNAME* lpfnSETUPDICLASSGUIDSFROMNAME = reinterpret_cast<SETUPDICLASSGUIDSFROMNAME*>(GetProcAddress(hSetupAPI, "SetupDiClassGuidsFromNameA"));
  SETUPDIGETCLASSDEVS* lpfnSETUPDIGETCLASSDEVS = reinterpret_cast<SETUPDIGETCLASSDEVS*>(GetProcAddress(hSetupAPI, "SetupDiGetClassDevsA"));
  SETUPDIGETDEVICEREGISTRYPROPERTY* lpfnSETUPDIGETDEVICEREGISTRYPROPERTY = reinterpret_cast<SETUPDIGETDEVICEREGISTRYPROPERTY*>(GetProcAddress(hSetupAPI, "SetupDiGetDeviceRegistryPropertyA"));
#endif
  SETUPDIDESTROYDEVICEINFOLIST* lpfnSETUPDIDESTROYDEVICEINFOLIST = reinterpret_cast<SETUPDIDESTROYDEVICEINFOLIST*>(GetProcAddress(hSetupAPI, "SetupDiDestroyDeviceInfoList"));
  SETUPDIENUMDEVICEINFO* lpfnSETUPDIENUMDEVICEINFO = reinterpret_cast<SETUPDIENUMDEVICEINFO*>(GetProcAddress(hSetupAPI, "SetupDiEnumDeviceInfo"));

  if ((lpfnLPSETUPDIOPENDEVREGKEY == NULL) || (lpfnSETUPDICLASSGUIDSFROMNAME == NULL) || (lpfnSETUPDIDESTROYDEVICEINFOLIST == NULL) ||
      (lpfnSETUPDIENUMDEVICEINFO == NULL) || (lpfnSETUPDIGETCLASSDEVS == NULL) || (lpfnSETUPDIGETDEVICEREGISTRYPROPERTY == NULL))
  {
    //Unload the setup dll
    FreeLibrary(hSetupAPI);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
  }
  
  //First need to convert the name "Ports" to a GUID using SetupDiClassGuidsFromName
  DWORD dwGuids = 0;
  lpfnSETUPDICLASSGUIDSFROMNAME(_T("Ports"), NULL, 0, &dwGuids);
  if (dwGuids == 0)
  {
		DWORD dwLastError = GetLastError();
  
    //Unload the setup dll
    FreeLibrary(hSetupAPI);
    
    SetLastError(dwLastError);

    return FALSE;
  }

  //Allocate the needed memory
  ATL::CHeapPtr<GUID> pGuids;
  if (!pGuids.Allocate(dwGuids))
  {
    //Unload the setup dll
    FreeLibrary(hSetupAPI);
    
    SetLastError(ERROR_OUTOFMEMORY);

    return FALSE;
  }

  //Call the function again
  if (!lpfnSETUPDICLASSGUIDSFROMNAME(_T("Ports"), pGuids, dwGuids, &dwGuids))
  {
		DWORD dwLastError = GetLastError();
  
    //Unload the setup dll
    FreeLibrary(hSetupAPI);
    
    SetLastError(dwLastError);

    return FALSE;
  }

  //Now create a "device information set" which is required to enumerate all the ports
  HDEVINFO hDevInfoSet = lpfnSETUPDIGETCLASSDEVS(pGuids, NULL, NULL, DIGCF_PRESENT);
  if (hDevInfoSet == INVALID_HANDLE_VALUE)
  {
		DWORD dwLastError = GetLastError();
  
    //Unload the setup dll
    FreeLibrary(hSetupAPI);
    
    SetLastError(dwLastError);

    return FALSE;
  }

  //Finally do the enumeration
  BOOL bMoreItems = TRUE;
  int nIndex = 0;
  SP_DEVINFO_DATA devInfo;
  while (bMoreItems)
  {
    //Enumerate the current device
    devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
    bMoreItems = lpfnSETUPDIENUMDEVICEINFO(hDevInfoSet, nIndex, &devInfo);
    if (bMoreItems)
    {
      //Did we find a serial port for this device
      BOOL bAdded = FALSE;

      //Get the registry key which stores the ports settings
      HKEY hDeviceKey = lpfnLPSETUPDIOPENDEVREGKEY(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
      if (hDeviceKey)
      {
        //Read in the name of the port
        TCHAR pszPortName[256];
        DWORD dwSize = sizeof(pszPortName);
        DWORD dwType = 0;
  	    if ((RegQueryValueEx(hDeviceKey, _T("PortName"), NULL, &dwType, reinterpret_cast<LPBYTE>(pszPortName), &dwSize) == ERROR_SUCCESS) && (dwType == REG_SZ))
        {
          //If it looks like "COMX" then
          //add it to the array which will be returned
          size_t nLen = _tcslen(pszPortName);
          if (nLen > 3)
          {
            if ((_tcsnicmp(pszPortName, _T("COM"), 3) == 0) && IsNumeric(&pszPortName[3], FALSE))
            {
              //Work out the port number
              int nPort = _ttoi(&pszPortName[3]);
              ports.Add(nPort);

              bAdded = TRUE;
            }
          }
        }

        //Close the key now that we are finished with it
        RegCloseKey(hDeviceKey);
      }

      //If the port was a serial port, then also try to get its friendly name
      if (bAdded)
      {
        TCHAR pszFriendlyName[256];
        DWORD dwSize = sizeof(pszFriendlyName);
        DWORD dwType = 0;
        if (lpfnSETUPDIGETDEVICEREGISTRYPROPERTY(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, reinterpret_cast<PBYTE>(pszFriendlyName), dwSize, &dwSize) && (dwType == REG_SZ))
          sFriendlyNames.Add(pszFriendlyName);
        else
          sFriendlyNames.Add(_T(""));
      }
    }

    ++nIndex;
  }

  //Free up the "device information set" now that we are finished with it
  lpfnSETUPDIDESTROYDEVICEINFOLIST(hDevInfoSet);

  //Unload the setup dll
  FreeLibrary(hSetupAPI);

  //Return the success indicator
  return TRUE;
}
#endif

#ifndef NO_ENUMSERIAL_USING_ENUMPORTS
#ifdef _AFX
BOOL CEnumerateSerial::UsingEnumPorts(CUIntArray& ports)
#else
BOOL CEnumerateSerial::UsingEnumPorts(CSimpleArray<UINT>& ports)
#endif
{
  //Make sure we clear out any elements which may already be in the array
  ports.RemoveAll();

  //Call the first time to determine the size of the buffer to allocate
  DWORD cbNeeded = 0;
  DWORD dwPorts = 0;
  EnumPorts(NULL, 1, NULL, 0, &cbNeeded, &dwPorts);

  //What will be the return value
  BOOL bSuccess = FALSE;

  //Allocate the buffer and recall
  ATL::CHeapPtr<BYTE> pPorts;
  if (pPorts.Allocate(cbNeeded))
  {
    bSuccess = EnumPorts(NULL, 1, pPorts, cbNeeded, &cbNeeded, &dwPorts);
    if (bSuccess)
    {
      PORT_INFO_1* pPortInfo = reinterpret_cast<PORT_INFO_1*>(pPorts.m_pData);
      for (DWORD i=0; i<dwPorts; i++)
      {
        //If it looks like "COMX" then
        //add it to the array which will be returned
        size_t nLen = _tcslen(pPortInfo->pName);
        if (nLen > 3)
        {
          if ((_tcsnicmp(pPortInfo->pName, _T("COM"), 3) == 0) && IsNumeric(&pPortInfo->pName[3], TRUE))
          {
            //Work out the port number
            int nPort = _ttoi(&pPortInfo->pName[3]);
            ports.Add(nPort);
          }
        }

        pPortInfo++;
      }
    }
  }
  else
    SetLastError(ERROR_OUTOFMEMORY);        
  
  return bSuccess;
}
#endif

#ifndef NO_ENUMSERIAL_USING_WMI
#ifdef _AFX
BOOL CEnumerateSerial::UsingWMI(CUIntArray& ports, CStringArray& sFriendlyNames)
#else
BOOL CEnumerateSerial::UsingWMI(CSimpleArray<UINT>& ports, CSimpleArray<CString>& sFriendlyNames)
#endif
{
  //Make sure we clear out any elements which may already be in the array(s)
  ports.RemoveAll();
  sFriendlyNames.RemoveAll();

  //What will be the return value
  BOOL bSuccess = FALSE;

  //Create the WBEM locator
  CComPtr<IWbemLocator> locator;
  HRESULT hr = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<void**>(&locator));
  if (SUCCEEDED(hr))
  {
    CComPtr<IWbemServices> services;
    hr = locator->ConnectServer(_bstr_t("\\\\.\\root\\cimv2"), NULL, NULL, NULL, 0, NULL, NULL, &services);
    if (SUCCEEDED(hr))
    {
      // Execute the query
      CComPtr<IEnumWbemClassObject> classObject;
      hr = services->CreateInstanceEnum(_bstr_t("Win32_SerialPort"), WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL, &classObject);
      if (SUCCEEDED(hr))
      {
        bSuccess = TRUE;

        //Now enumerate all the ports
        hr = WBEM_S_NO_ERROR;

        //Final Next will return WBEM_S_FALSE
        while (hr == WBEM_S_NO_ERROR)
        {
          ULONG uReturned;
          CComPtr<IWbemClassObject> apObj[10];
          hr = classObject->Next(WBEM_INFINITE, 10, reinterpret_cast<IWbemClassObject**>(apObj), &uReturned);
          if (SUCCEEDED(hr))
          {
            for (ULONG n=0; n<uReturned; n++)
            {
              CComVariant varProperty1;
              HRESULT hrGet = apObj[n]->Get(L"DeviceID", 0, &varProperty1, NULL, NULL);
              if (SUCCEEDED(hrGet) && (varProperty1.vt == VT_BSTR) && (wcslen(varProperty1.bstrVal) > 3))
              {
                CW2T szPort(varProperty1.bstrVal);

                //If it looks like "COMX" then add it to the array which will be returned
                if ((_tcsnicmp(szPort, _T("COM"), 3) == 0) && IsNumeric(&szPort[3], TRUE))
                {
                  //Work out the port number
                  int nPort = _ttoi(&szPort[3]);
                  ports.Add(nPort);

                  //Also get the friendly name of the port
                  CString sFriendlyName;
                  CComVariant varProperty2;
                  if (SUCCEEDED(apObj[n]->Get(L"Name", 0, &varProperty2, NULL, NULL)) && (varProperty2.vt == VT_BSTR))
                    sFriendlyName = varProperty2.bstrVal;
                  sFriendlyNames.Add(sFriendlyName);    
                }
              }
            }
          }
        }
      }
    }
  }
  
  return bSuccess;
}
#endif

#ifndef NO_ENUMSERIAL_USING_COMDB
#ifdef _AFX
BOOL CEnumerateSerial::UsingComDB(CUIntArray& ports)
#else
BOOL CEnumerateSerial::UsingComDB(CSimpleArray<UINT>& ports)
#endif
{
  //Make sure we clear out any elements which may already be in the array(s)
  ports.RemoveAll();

  //What will be the return value
  BOOL bSuccess = FALSE;
  
  //Used to preserve the last error value
  DWORD dwLastError = ERROR_SUCCESS;

  //Get the function pointers to "ComDBOpen", "ComDBClose" & "ComDBGetCurrentPortUsage" in msports.dll
  HINSTANCE hMSPorts = LoadLibrary(_T("MSPORTS.DLL"));
  if (hMSPorts == NULL)
    return FALSE;

  COMDBOPEN* lpfnLPCOMDBOPEN = reinterpret_cast<COMDBOPEN*>(GetProcAddress(hMSPorts, "ComDBOpen"));
  COMDBCLOSE* lpfnLPCOMDBCLOSE = reinterpret_cast<COMDBCLOSE*>(GetProcAddress(hMSPorts, "ComDBClose"));
  COMDBGETCURRENTPORTUSAGE* lpfnCOMDBGETCURRENTPORTUSAGE = reinterpret_cast<COMDBGETCURRENTPORTUSAGE*>(GetProcAddress(hMSPorts, "ComDBGetCurrentPortUsage"));
  if ((lpfnLPCOMDBOPEN != NULL) && (lpfnLPCOMDBCLOSE != NULL) && (lpfnCOMDBGETCURRENTPORTUSAGE != NULL))
  {
    //First need to open up the DB
    HCOMDB hComDB;
    DWORD dwComOpen = lpfnLPCOMDBOPEN(&hComDB);
    if (dwComOpen == ERROR_SUCCESS)
    {
      //Work out the size of the buffer required
      DWORD dwMaxPortsReported = 0;
      DWORD dwPortUsage = lpfnCOMDBGETCURRENTPORTUSAGE(hComDB, NULL, 0, CDB_REPORT_BYTES, &dwMaxPortsReported);
      if (dwPortUsage == ERROR_SUCCESS)
      {
        //Allocate some heap space and recall the function
        ATL::CHeapPtr<BYTE> portBytes;
        if (portBytes.Allocate(dwMaxPortsReported))
        {
          bSuccess = TRUE;
          if (lpfnCOMDBGETCURRENTPORTUSAGE(hComDB, portBytes, dwMaxPortsReported, CDB_REPORT_BYTES, &dwMaxPortsReported) == ERROR_SUCCESS)
          {
            //Work thro the byte bit array for ports which are in use
            for (DWORD i=0; i<dwMaxPortsReported; i++)
            {
              if (portBytes[i])
                ports.Add(i + 1);
            }
          }
        }
        else
          SetLastError(ERROR_OUTOFMEMORY);        
      }
      else
				dwLastError = dwPortUsage;
    
      //Close the DB
      lpfnLPCOMDBCLOSE(hComDB);
    }
    else
			dwLastError = dwComOpen;
  }
  else
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

  //Unload the msports dll
  FreeLibrary(hMSPorts);
  
  //Reinstate the last error
  if (dwLastError)
		SetLastError(dwLastError);
 
  return bSuccess;
}
#endif

#ifndef NO_ENUMSERIAL_USING_REGISTRY
#ifdef _AFX
BOOL CEnumerateSerial::UsingRegistry(CStringArray& ports)
#else
BOOL CEnumerateSerial::UsingRegistry(CSimpleArray<CString>& ports)
#endif
{
  //Make sure we clear out any elements which may already be in the array(s)
  ports.RemoveAll();

  //What will be the return value
  BOOL bSuccess = FALSE;

  HKEY hSERIALCOMM;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_QUERY_VALUE, &hSERIALCOMM) == ERROR_SUCCESS)
  {
		//Get the max value name and max value lengths
		DWORD dwMaxValueNameLen;
		DWORD dwMaxValueLen;
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
						ports.Add(szPort);	
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
#endif
