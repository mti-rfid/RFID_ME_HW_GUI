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

#include "USBBoxCommunication.hxx"

#define WRITE_MODE true
#define READ_MODE false

USBBoxCommunication::USBBoxCommunication(unsigned char devAddr)
{
	this->hinstLib = NULL;
	this->hinstLib = LoadLibraryA("./libraries/usbcon.dll");
	this->devAddr = devAddr;
	if(this->hinstLib == NULL)
		return;

	readByteFunction = (AMSusbReadByte) GetProcAddress(hinstLib, "@AMSreadByte$qucpuc");
	writeByteFunction = (AMSusbWriteByte) GetProcAddress(hinstLib, "@AMSwriteByte$qucuc");
	setI2CDeviceAddressFunction = (AMSusbSetI2CDevAddr) GetProcAddress(hinstLib, "@AMSsetI2CDevAddr$quc");
	setPortAsOutput = (AMSsetPortAsOutput) GetProcAddress(hinstLib, "@AMSsetPortAsOutput$q4Portuc");
	setPortAsInput = (AMSsetPortAsInput) GetProcAddress(hinstLib, "@AMSsetPortAsInput$q4Portuc");
	writePort = (AMSwritePort) GetProcAddress(hinstLib, "@AMSwritePort$quc4Portuc");
	readPort = (AMSreadPort) GetProcAddress(hinstLib, "@AMSreadPort$qpuc4Portuc");
	usbIsConnected    = (AMSusbIsConnected)         GetProcAddress(hinstLib, "@AMSusbIsConnected$qv");
	usbFirmwareID     = (AMSusbFirmwareID)          GetProcAddress(hinstLib, "@AMSusbFirmwareID$qpuc");
	usbDebugMsg       = (AMSusbDebugMsg)            GetProcAddress(hinstLib, "@AMSusbDebugMsg$qpuc");
	setInterface      = (AMSsetInterface)           GetProcAddress(hinstLib, "@AMSsetInterface$q9Interface");
	blkRead           = (AMSblkRead)                GetProcAddress(hinstLib, "@AMSblkRead$qucpucuc");
	blkWrite          = (AMSblkWrite)               GetProcAddress(hinstLib, "@AMSblkWrite$qucpucuc");
	readADC           = (AMSreadADC)                GetProcAddress(hinstLib, "@AMSreadADC$qucpui");
	getPortDirection  = (AMSgetPortDirection)       GetProcAddress(hinstLib, "@AMSgetPortDirection$q4Portpuc");
	sendPacketDLL     = (AMSsendPacket)             GetProcAddress(hinstLib, "@AMSsendPacket$qpucit1i");
}

USBBoxCommunication::~USBBoxCommunication()
{
    if(hinstLib != NULL)
		FreeLibrary(hinstLib);
}

void USBBoxCommunication::setDevAddr(unsigned char devAddr)
{
	this->devAddr = devAddr;
}

AMSCommunication::Error USBBoxCommunication::hwConnect()
{
	Error usbError = NoError;

	if (hinstLib)
	{
		AMSusbConnect function = (AMSusbConnect) GetProcAddress(hinstLib, "@AMSusbConnect$qv");
		if (function)
		{
			if (function() != ERR_NO_ERR)
			{
				usbError = ConnectionError;
			}
			else
			{
				connected = true;
			}
		}
		else
		{
			usbError = ConnectionError;
		}
	}
	else
	{
		usbError = ConnectionError;
	}
	return usbError;
}

void USBBoxCommunication::hwDisconnect()
{
	UsbError usbError = ERR_NO_ERR;

	if (hinstLib)
	{
		AMSusbDisconnect disconnectFunction = (AMSusbDisconnect) GetProcAddress(hinstLib, "@AMSusbDisconnect$qv");
		if (disconnectFunction)
		{
			disconnectFunction();
			connected = false;
		}
	}
}

UsbError USBBoxCommunication::setVsyncFrequency ( DWORD frequencyInHertz )
{
	UsbError usbError = ERR_NO_ERR;
	char outPacket[ USB_PACKET_SIZE ]; 
	DWORD ticks = 0xFFFF;

	if ( frequencyInHertz > 0 )
	{ /* 1/freq = time per pulse. As 8051 runs with 2MHz timer-ticks
	     we get: ticks = 1/freq * 2 (for ticks per usec) */
		DWORD microsecs = ( 1000000 / frequencyInHertz ) - 10; /* for high pulse */
		ticks -= ( microsecs * 2 ); /* we have 2 ticks per microsecond */
	}
	
	/* build a new usb packet */
	USB_TASK( outPacket ) = TSK_VSYNC;
	USB_STATE( outPacket ) = 0;
	USB_VSYNC_RELOAD_HIGH( outPacket ) = static_cast<BYTE>( ticks >> 8 );
	USB_VSYNC_RELOAD_LOW( outPacket ) = static_cast<BYTE>( ticks & 0xFF );
	USB_VSYNC_REPEAT_RELOAD( outPacket ) = ( frequencyInHertz ? 1 : 0 );
	
	QByteArray tempIn;
	QByteArray tempOut( outPacket, USB_PACKET_SIZE );


	/* send and receive */
	usbError = sendReceivePacket( &tempIn, &tempOut );

	return usbError;
}

UsbError USBBoxCommunication::sendReceiveI2CPacket(char startRegister, QByteArray * in, const QByteArray * out )
{
	UsbError usbError = ERR_NO_ERR;
	/* as the size of the data must be stored in a byte, the outpacket can have
	   a maximum of 256 data bytes*/
	char outPacket[ 256 ]; 
	char * outData = outPacket;
	int outSize = out->size();
	int inSize = in->size();

	/* clear in array -> to be filled later */
	in->clear();

	/* setup chip specific data (like i2c slave address and register address */
	outData[ 0 ] = devAddr << 1; /* first is always a write */
	outData[ 1 ] = startRegister; /* second is always the register we want to access */
	if ( inSize > 0 ) /* we have a write-read combination */
	{	
		outData[ 2 ] = ( devAddr << 1 ) | 1; /* indicate an i2c-read */
		outData += 3;
		outSize += 3;/* +1 for slave address, +1 for register address, +1 for repeated slave address */
	}
	else /* we have a write only */
	{
		outData += 2;
		outSize += 2; /* +1 for slave address, +1 for register address */
	}

	memcpy( outData, out->data(), out->size() );/* now the complete data is in "packet" */
	outData = outPacket; /* reset out pointer, to use in loop */
	do 
	{
		const int tempOutSize = MIN( USB_PLAIN_I2C_MAX_DATA, outSize );
		char header[ USB_PLAIN_I2C_HEADER_SIZE ];
		QByteArray tempIn;

		/* build a new usb packet */
		USB_TASK( header ) = TSK_PLAIN_I2C;
		USB_STATE( header ) = 0;
		USB_PLAIN_I2C_MOSI_NBYTES( header ) = outSize; /* is only evaluated in first packet of a series */
		USB_PLAIN_I2C_MISO_NBYTES( header ) = inSize; /* is only evaluated in first packet of a series */
		QByteArray tempOut( header, USB_PLAIN_I2C_HEADER_SIZE );
		tempOut.append( outData, tempOutSize );

		/* send and receive */
		usbError = sendReceivePacket( &tempIn, &tempOut );

		outSize -= tempOutSize;  /* correct size and pointer , since we already */
		outData += tempOutSize;     /* sent+received some bytes */
		if ( inSize > 0 )
		{ /* we are waiting for input -> concatenate it */
			tempIn.remove( 0, USB_PLAIN_I2C_HEADER_SIZE );
			in->append( tempIn );
			inSize -= USB_PLAIN_I2C_MAX_DATA; /* we always receive a full usb-packet */
		}
	} while ( ( outSize > 0 || inSize > 0 ) && usbError == ERR_NO_ERR );
	return usbError;
}

UsbError USBBoxCommunication::sendReceivePacket(QByteArray *in, const QByteArray *out)
{
	UsbError usberror = ERR_NO_ERR;

	in->resize(64); // make space for complete USB packet
	
	usberror = this->sendPacketDLL(in->data(),in->size(),out->constData(),out->size());

	return usberror;
}

bool USBBoxCommunication::isConnected()
{
	UsbError usberror;
	unsigned char fwIdBuf[20];

	connected = false;

	readFirmwareId = (AMSusbFirmwareID) GetProcAddress(hinstLib, "@AMSusbFirmwareID$qpuc");
	if(readFirmwareId)
	{
		usberror = readFirmwareId(&fwIdBuf[0]);
		if (usberror == ERR_NO_ERR)
		{
			if (strncmp((const char *)&fwIdBuf[0], "Firmware", 8) == 0)
			{
				connected = true;
			}
		}
	}
	return connected;
}

AMSCommunication::Error USBBoxCommunication::hwReadRegister(unsigned char registerAddress, unsigned char* registerValue)
{
	UsbError usbError = ERR_NO_ERR;
	AMSCommunication::Error err = NoError;

	if (connected)
	{
		if(hinstLib)
		{
			usbError = setI2CDeviceAddress(devAddr);
			if (!usbError)
			{
				if (readByteFunction)
				{
					usbError = readByteFunction(registerAddress, registerValue);
				}
				else
				{
					usbError = ERR_GENERIC;
				}
			}

		}
	}
	else
	{
		usbError = ERR_NOT_CONNECTED;
	}

	if (usbError  == ERR_NOT_CONNECTED)
		err = ConnectionError;
	else if (usbError == ERR_NO_ERR)
		err = NoError;
	else
		err = ReadError;
	return err;
}


  /**
   * This function sends one Byte to the specified address
   * In case of success ERR_NO_ERR is returned
   */
AMSCommunication::Error USBBoxCommunication::hwWriteRegister(unsigned char registerAddress, unsigned char registerValue)
{
	UsbError usbError = ERR_NO_ERR;
	AMSCommunication::Error err = NoError;

	if(hinstLib)
	{
		if (connected)
		{
			usbError = setI2CDeviceAddress(devAddr);
			if (!usbError)
			{
				if (writeByteFunction)
				{
					usbError = writeByteFunction(registerAddress, registerValue);
				}
				else
				{
					usbError = ERR_GENERIC;
				}
			}
		}
		else
		{
			usbError = ERR_NOT_CONNECTED;
		}
	}
	if (usbError  == ERR_NOT_CONNECTED)
		err = ConnectionError;
	else if (usbError == ERR_NO_ERR)
		err = NoError;
	else
		err = WriteError;
	return err;
}

  /**
   * This function can be used to send special commands to the device
   * Dummy implementation for now, ERR_NO_ERR is returned
   */
AMSCommunication::Error USBBoxCommunication::hwSendCommand(QString command, QString * answer)
{
	unsigned char buffer[256];
	int openIdx, closeIdx, i, nrOfBlockWrites;
	unsigned char start_reg_address;
	bool ok;

	UsbError usbError = ERR_NO_ERR;
	AMSCommunication::Error err = NoError;

	if(command.contains("Bulk_Write"))
	{
		openIdx = command.indexOf('(');
		closeIdx = command.indexOf(')', openIdx);
		start_reg_address = command.mid(openIdx+1, 2).toUInt(&ok, 16);
		command = command.mid(openIdx + 3, closeIdx - openIdx - 1);
		i = 0;
		while((i*2 < command.size()) && (i < 256))
		{
			buffer[i++] = command.mid(i*2, 2).toInt(&ok, 16);
		}

		usbError = setI2CDeviceAddress(devAddr);
		if (!usbError)
		{
			nrOfBlockWrites = ((command.size()-1)/2) / BLOCK_WRITE_MAX_BYTES;

			for (i=0; i<nrOfBlockWrites; i++)
			{
				if (blkWrite)
				{
					usbError = blkWrite((BYTE)start_reg_address, (BYTE*)(buffer + i*BLOCK_WRITE_MAX_BYTES), BLOCK_WRITE_MAX_BYTES );
					start_reg_address += BLOCK_WRITE_MAX_BYTES;
				}
				else
				{
					usbError = ERR_GENERIC;
				}
			}

			if (blkWrite)
			{
				usbError = blkWrite((BYTE)start_reg_address, (BYTE*)(buffer + nrOfBlockWrites*BLOCK_WRITE_MAX_BYTES), (((command.size()-1)/2) % BLOCK_WRITE_MAX_BYTES) );
			}
			else
			{
				usbError = ERR_GENERIC;
			}

		}
	}

	if (usbError  == ERR_NOT_CONNECTED)
		err = ConnectionError;
	else if (usbError == ERR_NO_ERR)
		err = NoError;
	else
		err = WriteError;
	return err;
}

UsbError USBBoxCommunication::setI2CDeviceAddress(unsigned char registerAddress)
{
	UsbError usbError = ERR_NO_ERR;

	if (connected)
	{
		if(hinstLib)
		{
			if (setI2CDeviceAddressFunction)
			{
				usbError = setI2CDeviceAddressFunction(registerAddress);
				devAddr = registerAddress;
			}
			else
			{
				usbError = ERR_GENERIC;
			}
		}
	}
	else
	{
		usbError = ERR_NOT_CONNECTED;
	}
	return usbError;
}

void USBBoxCommunication::setConnectionProperties(void *in)
{
	if(in != NULL)
	{
		this->setI2CDeviceAddress(((usbBoxConfigStructure*)in)->deviceI2CAddress);
	}
}

void USBBoxCommunication::getConnectionProperties(void *out)
{
	if(out != NULL)
	{
		((usbBoxConfigStructure*)out)->deviceI2CAddress = devAddr;
	}
}
