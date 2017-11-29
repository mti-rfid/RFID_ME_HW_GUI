#ifndef _DEVICE_BASE
#define _DEVICE_BASE

class ClsDeviceBase
{
public:
	virtual ~ClsDeviceBase() {}

	virtual UINT  API_AskRevCount() = 0;
	virtual char* API_AskDevType() = 0;
	virtual bool  API_ClearBuffer() = 0;
	virtual UINT  API_Read(UCHAR *cData, UINT iLength) = 0;
	virtual bool  API_Write(UCHAR *cData, UINT iLength) = 0;
	virtual void  API_Close() = 0;
};

#endif