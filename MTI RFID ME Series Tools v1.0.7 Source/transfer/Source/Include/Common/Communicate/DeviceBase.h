#ifndef _DEVICE_BASE
#define _DEVICE_BASE

class ClsDeviceBase
{
public:
	ClsDeviceBase();
	virtual ~ClsDeviceBase() {}

	virtual bool   API_ClearBuffer() = 0;
	virtual RESULT API_Write(UINT uiMode, UCHAR *cData, UINT iLength) = 0;
	virtual RESULT API_Read(UINT uiMode, UCHAR *cData, UINT *iLength) = 0;
	virtual void   API_Close() = 0;
	//deesn't respones result
	virtual UINT   API_AskRevCount() = 0;
	virtual INT    API_AskDevType() = 0;
	virtual void   API_SetOverlapTime(UINT r_uiWtOverlapTime, UINT r_uiRdOverlapTime);
	virtual char*  API_AskUSBName() = 0;

	UINT uiWtOverlapTime;
	UINT uiRdOverlapTime;

};

#endif