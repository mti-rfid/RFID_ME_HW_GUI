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

#include "UARTQtCommunication.hxx"

#define WRITE_MODE true
#define READ_MODE false
#define IDS_R13MP_HEADER_LENGTH_IN_BYTES 7
#define NO_RECEIVE_COUNTER_VALUE UINT_MAX

UARTQtCommunication::UARTQtCommunication(unsigned char comPort)
{
	this->comPort = comPort;
	this->port = NULL;
	this->comPorts.clear();

	// set defaults first
	comProperties.port = comPort;
	comProperties.speed = BAUD38400;
	comProperties.flow = FLOW_OFF;
	comProperties.parity = PAR_NONE;
	comProperties.databits = DATA_8;
	comProperties.stopbits = STOP_1;
}

UARTQtCommunication::~UARTQtCommunication()
{
}

bool UARTQtCommunication::AS3910IsConnected()
{
	QString answer;

	this->hwSendCommand("FF", &answer);
	if(answer.contains("[Work]", Qt::CaseInsensitive))
		return true;
	else
		return false;
}

void UARTQtCommunication::getCurrentComPortsList()
{
	HKEY keyHandle;
	DWORD Type;
	DWORD BufferSize = TOTALBYTES;
	DWORD cbData;
	DWORD dwRet;

	comPorts.clear();

	QSettings settings("HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM", QSettings::NativeFormat);	
	//QString sReturnedValue = settings.value( "/Device/VCP0", "0" ).toString();
	QStringList allKeys = settings.allKeys();
#if 1   // since windows does not use the same names on different PCs we use all keys we found
	QStringList keys = allKeys;
#else
	// get only virtual com ports
	QStringList keys = allKeys.filter("VCP");
	// get also SiLabs CP210x USB to UART bridge, they use a different identification string
	keys += allKeys.filter("slabser");
	// one and the same SiLabs chip has different names under different PCs on windows
	keys += allKeys.filter("Silabser");
#endif

	if(!keys.isEmpty())
	{
		// make the key names windows like
		keys.replaceInStrings("/", "\\");
		if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_QUERY_VALUE, &keyHandle) == ERROR_SUCCESS )
		{
			char * rgValue = (char*)malloc(BufferSize * sizeof(char));
			cbData = BufferSize;

			foreach(QString key, keys)
			{
				dwRet = RegQueryValueEx( keyHandle, key.toAscii(), NULL, &Type, (LPBYTE)rgValue, &cbData);
				while(dwRet == ERROR_MORE_DATA)
				{
					// get a buffer that is big enough
					BufferSize += BYTEINCREMENT;
					rgValue = (char*)realloc(rgValue, BufferSize * sizeof(char));
					cbData = BufferSize;
					dwRet = RegQueryValueEx( keyHandle, key.toAscii(), NULL, &Type, (LPBYTE)rgValue, &cbData);
				}
				if(dwRet == ERROR_SUCCESS)
					comPorts += rgValue;
			}

			free(rgValue);
		}
		RegCloseKey(keyHandle);
	}
}

AMSCommunication::Error UARTQtCommunication::doConnection()
{
	foreach(QString tryPort, comPorts)
	{
		tryPort.prepend("\\\\.\\");
		port = new QextSerialPort(tryPort);
		port->setBaudRate((BaudRateType)comProperties.speed);
		port->setFlowControl((FlowType)comProperties.flow);
		port->setParity((ParityType)comProperties.parity);
		port->setDataBits((DataBitsType)comProperties.databits);
		port->setStopBits((StopBitsType)comProperties.stopbits);

		/* open port with flag Unbuffered
		 * this speeds up the serial communication when using QextSerial */
		port->open(QIODevice::ReadWrite | QIODevice::Unbuffered);
		if (port->isOpen() == TRUE)
		{
			// for AS3910IsConnected to work, we have to set this for now
			this->comPort = tryPort.mid(7).toInt();
			connected = true;
			if(AS3910IsConnected())
			{
				return NoError;
			}
			// if we come here, the AS3910 is not connected, reset everything
			this->comPort = 0xFF;
			connected = false;
		}
		delete port;
		port = NULL;
	}

	return ConnectionError;
}

AMSCommunication::Error UARTQtCommunication::hwConnect()
{
	Error uartError = NoError;

	if(this->comPort == 0xFF)
	{
		// we have to find the correct com port out of the com port list
		// first populate the com port list with the entries from the registry
		getCurrentComPortsList();

		if(!comPorts.isEmpty())
		{
			// now try to find the correct com port
			uartError = doConnection();
		}
		else
		{
			// no virtual com ports found
			uartError = ConnectionError;
		}
	}
	else
	{
		// we can do it the old way
		port = new QextSerialPort(QString::QString("\\\\.\\COM%1").arg(QString::number(this->comProperties.port, 10)));
		port->setBaudRate((BaudRateType)comProperties.speed);
		port->setFlowControl((FlowType)comProperties.flow);
		port->setParity((ParityType)comProperties.parity);
		port->setDataBits((DataBitsType)comProperties.databits);
		port->setStopBits((StopBitsType)comProperties.stopbits);

		/* open port with flag Unbuffered
		 * this speeds up the serial communication when using QextSerial */
		port->open(QIODevice::ReadWrite | QIODevice::Unbuffered);
		if (port->isOpen() == TRUE)
		{
			connected = true;
		}
		else
		{
			delete port;
			port = NULL;
			uartError = ConnectionError;
		}
	}

	return uartError;
}

void UARTQtCommunication::hwDisconnect()
{
	UartError uartError = ERR_UART_NO_ERR;

	if(port && port->isOpen())
	{
		port->close();
		if(port->isOpen() == FALSE)
		{
			delete port;
			port = NULL;
			connected = false;

			if(!this->comPorts.isEmpty())
				this->comPort = 0xFF;
		}
	}
}

void UARTQtCommunication::deviceNodeChanged()
{
	if(connected)   // up to now we were connected, did we get disconnected? this would mean that the COM port is not valid anymore
	{
		getCurrentComPortsList();
		foreach(QString tryPort, comPorts)
		{
			if(tryPort == QString::QString("COM%1").arg(QString::number(this->comPort, 10)))
			{
				// the com port is still there, see if the AS3910 is connected
				if(AS3910IsConnected())
				{
					// seems as if the change on the USB bus did not affect us
					return;
				}
			}
		}
		disconnect();
	}
	else   // we were not connected, see if we can connect now
		connect();
}

AMSCommunication::Error UARTQtCommunication::hwReadRegister(unsigned char registerAddress, unsigned char* registerValue)
{
	UartError uartError = ERR_UART_NO_ERR;
	AMSCommunication::Error err = NoError;
	char buff[1024];
	int numBytes, openIdx, closeIdx, i;
	QString message;
	bool ok;

	if (connected)
	{
		memset(buff, 0x00, 1024);
		message = "12";   // read single register command
		message.append(convertValueToTransmitString(registerAddress));   // address
		generateTransmitPackage(message);
		i = port->write(message.toAscii(), message.length());

		if(port->waitForReadyRead(1000))
		{
			i = 0;
			do
			{
				numBytes = port->bytesAvailable();
				i += port->read(buff + i, numBytes);
			}while(buff[i-2] != '>');
			buff[i] = '\0';
		}
		message = buff;
		openIdx = message.lastIndexOf('[');
		closeIdx = message.lastIndexOf(']');
		message = message.mid(openIdx+1, closeIdx - openIdx - 1);
		*registerValue = message.toInt(&ok, 16);
		if(!ok)
			uartError = ERR_UART_RECEIVE;
	}
	else
	{
		uartError = ERR_UART_NOT_CONNECTED;
	}

	if (uartError == ERR_UART_NOT_CONNECTED)
		err = ConnectionError;
	else if (uartError == ERR_UART_NO_ERR)
		err = NoError;
	else
		err = ReadError;

	return err;
}


  /** UARTCommunication::writeByte
   * This function sends one Byte to the specified address
   * In case of success ERR_NO_ERR is returned
   */
AMSCommunication::Error UARTQtCommunication::hwWriteRegister(unsigned char registerAddress, unsigned char registerValue)
{
	UartError uartError = ERR_UART_NO_ERR;
	AMSCommunication::Error err = NoError;
	QString message;
	char buff[1024];
	int numBytes, i;

	if (connected)
	{
		memset(buff, 0x00, 1024);
		message = "10";   // write single register command
		message.append(convertValueToTransmitString(registerAddress));   // address
		message.append(convertValueToTransmitString(registerValue));   // value
		generateTransmitPackage(message);
		i = port->write(message.toAscii(), message.length());
		if(port->waitForReadyRead(1000))
		{
			i = 0;
			do
			{
				numBytes = port->bytesAvailable();
				i += port->read(buff + i, numBytes);
			}while(buff[i-2] != '>');
			buff[i] = '\0';
		}
		else
		{
			uartError = ERR_UART_TRANSMIT;
		}
	}
	else
	{
		uartError = ERR_UART_NOT_CONNECTED;
	}

	if (uartError == ERR_UART_NOT_CONNECTED)
		err = ConnectionError;
	else if (uartError == ERR_UART_NO_ERR)
		err = NoError;
	else
		err = WriteError;

	return err;
}

AMSCommunication::Error UARTQtCommunication::hwSendCommand(QString command, QString * answer)
{
	UartError uartError = ERR_UART_NO_ERR;
	AMSCommunication::Error err = NoError;
	char buff[0x2000], trash;
	unsigned int numBytes, noReceiveCounter, i;

	if (connected)
	{
		//*answer = command;
		memset(buff, 0x00, 0x2000);
		if(!command.isEmpty())
		{
			// whatever is currently in the buffer we throw it away
			while(port->bytesAvailable())
			{
				port->read(&trash, 1);
			}

			generateTransmitPackage(command);
			i = port->write(command.toAscii(), command.length());
		}

		if(port->waitForReadyRead(100))
		{
			i = 0;

			if(command.isEmpty())
			{
				// test if we already received something
				numBytes = port->bytesAvailable();
				// if so, try to receifve everything
				if(numBytes > 0)
				{
					do
					{
						numBytes = port->bytesAvailable();
						i += port->read(buff + i, 1);
						// definierte anzahl auslesen (1024) und dann gleich in QString geben, und weiter auslesen...
					}while(buff[i-2] != '>');
				}
				// else we leave already
			}
			else
			{
				do
				{
					numBytes = port->bytesAvailable();
					if(numBytes)
						noReceiveCounter = NO_RECEIVE_COUNTER_VALUE;
					else
						noReceiveCounter--;

					i += port->read(buff + i, 1);
					// definierte anzahl auslesen (1024) und dann gleich in QString geben, und weiter auslesen...
				}while(buff[i-2] != '>' && noReceiveCounter > 0);

				if(buff[i-2] != '>')
					uartError = ERR_UART_RECEIVE;
			}
			buff[i] = '\0';
			*answer = buff;
		}
		else
		{
			uartError = ERR_UART_RECEIVE;
		}
	}
	else
	{
		uartError = ERR_UART_NOT_CONNECTED;
	}

	if (uartError == ERR_UART_NOT_CONNECTED)
		err = ConnectionError;
	else if (uartError == ERR_UART_NO_ERR)
		err = NoError;
	else
		err = ReadError;

	return err;
}

QString UARTQtCommunication::convertValueToTransmitString(int value)
{
	return QString::QString("%1").arg(QString::number(value, 16), 2, '0');
}

void UARTQtCommunication::generateTransmitPackage(QString& message)
{
	int messageLength;

	messageLength = IDS_R13MP_HEADER_LENGTH_IN_BYTES + (message.size() / 2);

	message.prepend("000304");   // DEBUG_HEADER
	message.prepend(convertValueToTransmitString(messageLength));   // Number of bytes
	message.prepend("01");   // SOF

	message.append("0000");   // EOF
}

void UARTQtCommunication::setConnectionProperties(void* properties)
{
	this->comProperties = *((sProperties*)properties);
}

int UARTQtCommunication::getComPort()
{
	return this->comPort;
}
