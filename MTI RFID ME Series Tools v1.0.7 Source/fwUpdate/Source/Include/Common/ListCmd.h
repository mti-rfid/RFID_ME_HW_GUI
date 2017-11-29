#ifndef LIST_CMD_H
#define LIST_CMD_H


#define CMD_COUNT  5

const TCHAR *strCmdName[] = 
{
	_T( "0x10  FwUpdateGetModuleID" ),
	_T( "0xD0  FwUpdateEnterUpdateMode" ),
	_T( "0xD4  FwUpdateBeginUpdate" ),
	_T( "0xD6  FwUpdateWriteImageData" ),
	_T( "0xD2  FwUpdateExitUpdateMode" ),
//	_T( "0xD8  FwUpdateWriteOEMCfgData" ),
};


#endif