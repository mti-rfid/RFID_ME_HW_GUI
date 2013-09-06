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

/*
 *      PROJECT:   AMSCommunication
 *      $Revision: 1.1 $
 *      LANGUAGE: QT C++
 */

/*! \file
 *
 *  \author S. Puri
 *
 *  \brief  Communication class for communication via UART
 *
 *  This controls the communication via UART.
 *  Actual UART access is handled via qextserialport.lib.
 *
 *  http://code.google.com/p/qextserialport/
 */

#ifndef UARTQTCOMMUNICATION_H
#define UARTQTCOMMUNICATION_H

#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <winreg.h>
#include "AMSCommunication.hxx"
#include "qextserialport.h"

#define TOTALBYTES 12
#define BYTEINCREMENT 4

typedef enum {
  ERR_UART_NO_ERR, /**< No error occurred. */
  ERR_UART_CMD_FAILED, /**< The uC encountered an error. */
  ERR_UART_CANNOT_CONNECT, /**< The demoboard is probably not plugged in. */
  ERR_UART_NOT_CONNECTED, /**< Transfer function was called on a closed connection */
  ERR_UART_TRANSMIT, /**< An error occurred during the transmission of a command. */
  ERR_UART_RECEIVE, /**< An error occurred during reception. */
  ERR_UART_ILLEGAL_ARGUMENTS, /**< The arguments are out of bounds. */
  ERR_UART_GENERIC /**< An unspecified error occurred. */
} UartError;

struct sProperties
{
	char port;
	char speed;
	char databits;
	char stopbits;
	char flow;
	char parity;
};


class UARTQtCommunication : public AMSCommunication
{
	Q_OBJECT

public:
	UARTQtCommunication(unsigned char comPort = 0x1);
	UARTQtCommunication(QStringList comPorts);
	~UARTQtCommunication();

	AMSCommunication::Error hwConnect();
	void hwDisconnect();
	virtual AMSCommunication::Error hwReadRegister(unsigned char reg, unsigned char *val);
	virtual AMSCommunication::Error hwWriteRegister(unsigned char reg, unsigned char val);
	virtual AMSCommunication::Error hwSendCommand(QString command, QString * answer);
	void setConnectionProperties(void*);
	int getComPort();

public slots:
	void deviceNodeChanged();

private:
	QString convertValueToTransmitString(int value);
	void generateTransmitPackage(QString& message);
	AMSCommunication::Error doConnection();
	void getCurrentComPortsList();
	bool AS3910IsConnected();

	unsigned char comPort;
	QStringList comPorts;
	sProperties comProperties;

protected:	
	QextSerialPort *port;
};

#endif // UARTQTCOMMUNICATION_H
