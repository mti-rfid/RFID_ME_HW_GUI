#ifndef TRANS_RFID_H
#define TRANS_RFID_H

//mod by yo chen
//#include "USBPort.h"
#include "DeviceInterface.h"
//END mod by yo chen
#include "Utility.h"

// SetupDiGetInterfaceDeviceDetail所需要的輸出長度，定義足夠大
#define INTERFACE_DETAIL_SIZE  1024
#define USB_LENGTH 	           64
#define UART_LENGTH 	       130 //[0]=CMD=0xD6, [1]=length=0x82, [129:2]=data

//Rx Step
#define STEP_RESPOEES_CMD   0
#define STEP_LENGTH         1  
#define STEP_DATA           2  

//RFID STATUS
#define STATUS_OK							0x00
#define STATUS_ERROR_INVALID_DATA_LENGTH    0x0E
#define STATUS_ERROR_INVALID_PARAMETER      0x0F
#define STATUS_ERROR_WRONG_PROCEDURE        0xE0
#define STATUS_ERROR_OVER_RANGE             0xE1
#define STATUS_ERROR_SYS_MODULE_FAILURE     0xFF

//Update Image Size per pagket
#define PER_PACKET_UPDATE_IMAGE    32
#define PER_PACKET_UPDATE_IMAGE_UART    128	//add by yo chen, 2011/08/29

//Change Cmd
#define CHANGE_CMD_TO_BRFORE	   0
#define CHANGE_CMD_TO_NEXT	   	   1

//Product Information
const UINT	VENDOR_ID[] = { 0x1325, 0x24E9, 0x24E9 };
const UINT	PRODUCT_ID[] = { 0xC029, 0x3993, 0x0865 };

enum ENUM_REGION
{
	REGION_US,
	REGION_EU,
	REGION_TW,
	REGION_CN,
	REGION_KR,
	REGION_AT,
	REGION_NZ,
	REGION_EU2,
};

enum ENUM_CMD
{
	CMD_NOTHING,
	CMD_VERSION_B = 0x10,
	CMD_VERSION__BR,			   
	CMD_ENTER_UPDATE_FW_MODE_B = 0xD0,
	CMD_ENTER_UPDATE_FW_MODE_BR,
	CMD_EXIT_UPDATE_FW_MODE_B,
	CMD_EXIT_UPDATE_FW_MODE_BR,
	CMD_BEGIN_UPDATE_B,
	CMD_BEGIN_UPDATE_BR,
	CMD_WRITE_IMAGE_B,
	CMD_WRITE_IMAGE_BR,
	CMD_SET_OEM_CONFIG_B,
	CMD_SET_OEM_CONFIG_BR,

	//====Define myself============================
	CMD_SEND_ONE_CMD_BY_USER,  //User
	CMD_TIME_OUT,
	CMD_TRANX_ALL_OK,
};

//=============
//add by yo for Regular

#define SLEEP_TIME 5
enum ENUM_RESULT//mod by yo chen, ENUM_RESULT to ENUM_CMD
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
//END add by yo chen


class clsTRANX_RFID
{
public:
	clsTRANX_RFID();
	~clsTRANX_RFID();

	//mod by yo chen
	//bool API_StartComm(void);
	bool API_USB_Open(/*UINT uiPID, UINT uiVID*/);
	bool API_Serial_Open(CString cstrComPort, DCB &r_Dcb);
	char* API_AskDevType();
	bool API_Write(UCHAR *cData, int iLength);
	UINT API_AskRevCount();
	UINT API_Read(UCHAR *cData, UINT iLength);
	void API_Close();

	void API_CloseComm(void);
	bool API_IsOpen(void);
	bool API_IsBusy();
	bool API_QueryOneCmd(UINT index);
	bool API_QueryOneCmd(CString cstrData);
	bool API_SetRegion(ENUM_REGION enumType);
	bool API_UpdateFw(HANDLE hdlImageFile, LARGE_INTEGER strcLargeInt, bool bAllProgress = true);
	bool API_bSetOemConfig(TBYTE ucAddr, TBYTE ucData);
	bool API_bVersion();
	CString  API_AskVersion(void);
	TCHAR    API_AskCmdStatus(void);
	UINT     API_AskProgressBarStep(void);
	ENUM_CMD WINAPI API_Regular(int iTime); //mod by yo chen, instead of this, I use the OemTool Regular(UINT uiInval)
	bool API_LoadLib();
	bool API_IsLoadLib();
	bool MacReadOemData(TBYTE OemAddr, TBYTE& OemValue);
	TBYTE API_RegularGetOemData();
	
	bool GetModuleID_new();
	bool GetModuleName_new(TBYTE OemAddr, UCHAR *OemData);//add by rick
	
	CString		fwVersion;//for draw to upper layer (CPageBasic.cpp)
	bool API_UpdateFw_UART_Init(HANDLE hdlImageFile, LARGE_INTEGER strcLargeInt);
	bool API_UpdateFw_UART_Write();
	void waitingResponseDelay(int sec);//add by rick

	//add by yo chen
	//ENUM_RESULT Regular(UINT uiInval);//mod by yo chen, it's new Regular() which is replacing API_Regular()
	//bool		SendData(UCHAR *buf);
	//bool		ReadData();	


private:
	//cUSBPort *m_clsUSBPort;

	//Transfer
	ENUM_CMD  m_enumTxCmd[10];
	TBYTE 	  m_ucTxCnt;
	TBYTE     m_ucTxBuf[USB_LENGTH];
	TBYTE     m_ucTxBuf_UART[UART_LENGTH];//add by yo chen
	TBYTE     m_ucRxBuf[USB_LENGTH];
	TBYTE 	  m_ucRxStep;
	TBYTE	  m_ucRxLength;
	TBYTE	  m_ucRxCnt;
	TBYTE     m_ucRxCmd;
	TBYTE     m_ucRxStatus;
	UINT	  m_TimeCnt;
	bool	  m_bIsOpen;
	//Data
	USHORT	  m_usOemAddr;
	TBYTE	  m_ucOemData;
	//Version Data
	CString   m_cstrVersion;
	UINT      m_uiCurFileCnt;
	UINT      m_uiTotalFileCnt;
	HANDLE    m_hdlImageFile;

	void SendCmd(void);
	bool Tx_bVersion();
	bool Tx_bEnterUpdateFwMode(void);
	bool Tx_bExitUpdateFwMode(void);
	bool Tx_bBeginUpdate();
	bool Tx_bWriteImage(void);
	bool Tx_bSetOemConfig(void);
	void ResetFlow(void);
	void CheckPacket(void);
	void AnalyzePacket(void);
	void HandleWriteImageResult(void);
	void LogMsg(TBYTE *ucData, UINT ucLength, TBYTE uc_Flag);
	bool WriteData(TBYTE *ucTxBuf);

	//add by yo chen
	CString DevType;
	ClsDeviceInterface  *m_clsDeviceInterface;

	//===only for API_UpdateFw_UART_Write
	UCHAR tempRxBuf_2[128];	
	TBYTE tempRxDataLen_2;// = 128;
	//TBYTE	m_ucRxStatus_2;
	//===

	//add by yo chen for Regular
	
	UINT		  uiTimeOut;
	void ResetData();
	CString		  m_DeviceType;
	volatile bool m_bUpdate;
	USHORT		CulCRC(unsigned char *buf, unsigned short bit_length);
	bool	sendUARTCmd(UCHAR *buf);
	//bool	stop_write_image;//add by yo chen, 2011/08/30
	//int		complete_times;//add by yo chen, 2011/08/31
	int		updateDev;
	CString curDevType;
	
	
	

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
	//END add by yo chen

	inline void ChangeCmd(bool bStep){(bStep==CHANGE_CMD_TO_NEXT)?m_ucTxCnt++:m_ucTxCnt--;};

};

#endif
