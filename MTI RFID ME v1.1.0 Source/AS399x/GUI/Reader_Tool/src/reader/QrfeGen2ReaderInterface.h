/*
 *****************************************************************************
 * Copyright @ 2009 by austriamicrosystems AG                                *
 * All rights are reserved.                                                  *
 *                                                                           *
 * IMPORTANT - PLEASE READ CAREFULLY BEFORE COPYING, INSTALLING OR USING     *
 * THE SOFTWARE.                                                             *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       * 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         *
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS         *
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT  *
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT          *
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
 *****************************************************************************
 */

/*!
 * @file	QrfeGen2ReaderInterface.h
 * @brief	Interface extension for gen2 reader
 */

#ifndef QRFEGEN2READERINTERFACE_H_
#define QRFEGEN2READERINTERFACE_H_

#include "QrfeReaderInterface.h"
#include "epc/EPC_Defines.h"

class QrfeGen2ReaderInterface
{
public:
	typedef enum {
		//Add by yingwei tseng for fixed mem bank length, 2010/09/14
        RFID_STATUS_OK                      = 0x00,
        RFID_ERROR_CMD_INVALID_DATA_LENGTH  = 0x0E,
        RFID_ERROR_INVALID_PARAMETER        = 0x0F,
        //Add by yingwei tseng for NXP alarm message, 2010/03/15
        RFID_ERROR_SYS_CHANNEL_TIMEOUT      = 0x0A,
        RFID_ERROR_HWOPT_READONLY_ADDRESS   = 0xA0,
        RFID_ERROR_HWOPT_UNSUPPORTED_REGION = 0xA1,
        RFID_ERROR_SYS_SECURITY_FAILURE     = 0xFE,
        //End by yingwei tseng for NXP alarm message, 2010/03/15
        RFID_ERROR_SYS_MODULE_FAILURE       = 0xFF,
        RFID_ERROR_18K6C_REQRN              = 0x01,
        RFID_ERROR_18K6C_ACCESS             = 0x02,
        RFID_ERROR_18K6C_KILL               = 0x03,
        RFID_ERROR_18K6C_NOREPLY            = 0x04,
        RFID_ERROR_18K6C_LOCK               = 0x05,
        RFID_ERROR_18K6C_BLOCKWRITE         = 0x06,
        RFID_ERROR_18K6C_BLOCKERASE         = 0x07,
        RFID_ERROR_18K6C_READ               = 0x08,
        RFID_ERROR_18K6C_SELECT             = 0x09,
        RFID_ERROR_18K6B_INVALID_CRC        = 0x11,
        RFID_ERROR_18K6B_RFICREG_FIFO       = 0x12,
        RFID_ERROR_18K6B_NO_RESPONSE        = 0x13,
        RFID_ERROR_18K6B_NO_ACKNOWLEDGE     = 0x14,
        RFID_ERROR_18K6B_PREAMBLE           = 0x15,
        RFID_ERROR_18K6C_EASCODE            = 0x20,    //Add by yingwei tseng for NXP alarm message, 2010/03/15
        RFID_ERROR_6CTAG_OTHER_ERROR        = 0x80,
        RFID_ERROR_6CTAG_MEMORY_OVERRUN     = 0x83,
        RFID_ERROR_6CTAG_MEMORY_LOCKED      = 0x84,
        RFID_ERROR_6CTAG_INSUFFICIENT_POWER = 0x8B,
        RFID_ERROR_6CTAG_NONSPECIFIC_ERROR  = 0x8F
		//End by yingwei tseng for fixed mem bank length, 2010/09/14
		//Del by yingwei tseng for fixed mem bank length, 2010/09/14
		#if 0
		Gen2_OK,
		Gen2_ERROR,
		Gen2_COMMERROR,
		Gen2_NORESPONSE,
		Gen2_NA,
		Gen2_TAG_UNREACHABLE,
		Gen2_OTHER,
		Gen2_MEM_OVERRUN,
		Gen2_MEM_LOCKED,
		Gen2_INSUFFICIENT_POWER,
		Gen2_NON_SPECIFIC,
		Gen2_WRONG_PASSW,
		Gen2_ACCESS_FAILED,
		Gen2_REQRN_FAILED,
		Gen2_NOREPLY,
		Gen2_CHANNEL_TIMEOUT,
		#endif
		//End by yingwei tseng for fixed mem bank length, 2010/09/14
	} Gen2Result;


	QrfeGen2ReaderInterface(){};
	virtual ~QrfeGen2ReaderInterface(){};

	virtual Gen2Result readFromTag ( QString tagId, uchar mem_bank, ushort address, QByteArray passwd, uchar count, QByteArray &data ) = 0;
	/*!<@brief Pure virtual function that allows to read data from tag */
    //Mod by yingwei tseng for memory bank block write, 2011/03/14 
	//virtual Gen2Result writeToTag ( QString tagId, uchar mem_bank, ushort address, QByteArray passwd, QByteArray data, int *writtenBytes = NULL ) = 0;
    virtual Gen2Result writeToTag ( QString tagId, uchar mem_bank, ushort address, QByteArray passwd, QByteArray data, int writeMode, int *writtenBytes = NULL ) = 0;
	//End by yingwei tseng for memory bank block write, 2011/03/14 
	/*!<@brief Pure virtual function that allows to write data to tag */

	//Mod by yingwei tseng for check EPC, 2010/07/06
#if 0
	virtual Gen2Result writeTagId ( QString tagId_Before, QString tagId_After, QByteArray passwd ) = 0;
#endif
	virtual Gen2Result writeTagId ( QString tagId_Before, QString tagId_After, QByteArray passwd, int *writtenBytes = 0 ) = 0;
	//End by yingwei tseng for check EPC, 2010/07/06
	/*!<@brief Pure virtual function that allows to write the tag id */

	virtual Gen2Result selectTagId( QString tagId_After) = 0;    //Add by yingwei tseng for check EPC, 2010/07/19 

	virtual Gen2Result lockTag ( QString tagId, LOCK_MODE mode, LOCK_MEMORY_SPACE memory, QByteArray passwd ) = 0;
	/*!<@brief Pure virtual function that allows to lock a tag*/
	virtual Gen2Result killTag ( QString tagId, QByteArray killPassword ) = 0;
	/*!<@brief Pure virtual function that allows to kill a tag */

	virtual int getCurrentTime() = 0;//Add by yingwei tseng for total times, 2010/02/03
};

#endif /* QRFEGEN2READERINTERFACE_H_ */
