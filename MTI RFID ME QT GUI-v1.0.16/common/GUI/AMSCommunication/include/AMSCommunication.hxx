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
 *  \author C. Eisendle
 *
 *  \brief  AMSCommunication class header file
 *
 *  AMSCommunication is an abstract class used for abstraction
 *  of different communication interfaces between the PC
 *  and the demo boards.
 */

#ifndef AMSCOMMUNICATION_H
#define AMSCOMMUNICATION_H

#include <QtGui>
#include <QMutex>

typedef enum ComChannel
{
		NoCommunicationChannel = 0,
		USBBox		= 1,
		UARTQt		= 2,
		USBHID		= 3,
		USBWinApi	= 4,
		WINUSBDLL	= 5,
		UARTWin		= 6
} AMSCommunicationChannel;

class AMSCommunication : public QObject
{
	Q_OBJECT

public:
	AMSCommunication();

	int status;

	
	enum Error
	{
		NoError			= 0,            /*< no error occured */
		ConnectionError = -1,			/*< no connection */
		ReadError		= -2,			/*< read error */
		WriteError		= -3,			/*< write error */
		VerifyError		= -4			/*< verification error */
	};

	enum Access
	{
		Read		= 0,
		Write		= 1,
		Modify		= 2,
		Command		= 3,
		ControlReg	= 4
	};

	/*!
	 *****************************************************************************
	 *  \brief  Write to a register
	 *
	 *  writeRegister writes #val to register #reg
	 *
	 *  \param[in] reg : Address of the register to write
	 *  \param[in] val : Value to write
	 *
	 *  \return ConnectionError : Not connected
	 *  \return WriteError : Error during write
	 *  \return NoError : No error
	 *****************************************************************************
	 */
	virtual Error writeRegister(unsigned char reg, unsigned char val, bool verify=true, bool doEmit = true);

	/*!
	 *****************************************************************************
	 *  \brief  Write to a sub- register
	 *
	 *  writeSubRegister writes #val to register #reg
	 *
	 *  \param[in] reg : Address of the sub-register to write
	 *  \param[in] val : Value to write
	 *  \param[in] subAddress : Mask for control register to set up correct sub-address
	 *
	 *  \return ConnectionError : Not connected
	 *  \return WriteError : Error during write
	 *  \return NoError : No error
	 *****************************************************************************
	 */
	virtual Error writeSubRegister(unsigned char reg, unsigned char val, unsigned char subAddress, bool verify=true, bool doEmit = true);
	

	 /*!
	 *****************************************************************************
	 *  \brief  Modify a sub- register
	 *
	 *  modifySubRegister writes #val to register #reg with mask #mask
	 *
	 *  \param[in] reg : Address of the sub-register to write
	 *  \param[in] mask : Mask to use
	 *  \param[in] val : Value to write
	 *  \param[in] subAddress : Mask for control register to set up correct sub-address
	 *
	 *  \return ConnectionError : Not connected
	 *  \return WriteError : Error during write
	 *  \return NoError : No error
	 *****************************************************************************
	 */
	virtual Error modifySubRegister(unsigned char reg, unsigned char mask, unsigned char val, unsigned char subAddress, bool verify=true, bool doEmit = true);
	
	/*!
	 *****************************************************************************
	 *  \brief  Set Control Register for multiplexed registers
	 *
	 *  writeSubRegister writes #val to register #reg
	 *
	 *  \param[in] ctrlRegister : Master register for all multiplexed registers
	 *  
	 *
	 *****************************************************************************
	 */
	virtual void setControlRegisterAddress(unsigned short ctrlRegister, unsigned char ctrlMask);


	/*!
	 *****************************************************************************
	 *  \brief  Read from a register
	 *
	 *  readRegister reads the value from register #reg and stores it to #val
	 *
	 *  \param[in]  reg : Address of the register to read from
	 *  \param[out] val : Pointer to a variable where the read value should be stored
	 *
	 *  \return ConnectionError : Not connected
	 *  \return ReadError : Error during read
	 *  \return NoError : No error
	 *****************************************************************************
	 */
	virtual Error readRegister(unsigned char reg, unsigned char *val, bool doEmit = true);


	/*!
	 *****************************************************************************
	 *  \brief  Read from a sub-register
	 *
	 *  readSubRegister reads the value from register #reg and stores it to #val
	 *
	 *  \param[in]  reg : Address of the register to read from
	 *  \param[out] val : Pointer to a variable where the read value should be stored
	 *
	 *  \return ConnectionError : Not connected
	 *  \return ReadError : Error during read
	 *  \return NoError : No error
	 *****************************************************************************
	 */
	virtual Error readSubRegister(unsigned char reg, unsigned char *val, unsigned char subAddress, bool doEmit = true);
	
	
	/*!
	 *****************************************************************************
	 *  \brief  Modifies a register value
	 *
	 *  modifyRegister writes #val to register #reg masked by #mask
	 *
	 *  \param[in]  reg : Address of the register to write
	 *  \param[in] mask : Mask to use
	 *  \param[in]  val : Value to write
	 *
	 *  \return ConnectionError : Not connected
	 *  \return WriteError : Error during write access
	 *  \return ReadError : Error during read access
	 *  \return NoError : No error
	 *****************************************************************************
	 */
	virtual Error modifyRegister(unsigned char reg, unsigned char mask, unsigned char val, bool verify=true, bool doEmit = true);

	/*!
	 *****************************************************************************
	 *  \brief  Establish a connection to the board
	 *
	 *  connect establishes a new connection to the demo board
	 *  Implementation hint: signal amscomConnectionChanged needs to be emitted.
	 *  Protected member \connected can be used to store the connection state
	 *
	 *  \return ConnectionError : Connection can't be established
	 *  \return NoError : No error
	 *****************************************************************************
	 */
	virtual Error connect();

	/*!
	 *****************************************************************************
	 *  \brief  Close an open connection
	 *
	 *  disconnect closes an open connection.
	 *  Implementation hint: signal amscomConnectionChanged needs to be emitted.
	 *  Protected member \connected can be used to store the connection state
	 *
	 *****************************************************************************
	 */
	virtual void disconnect();

	/*!
	 *****************************************************************************
	 *  \brief  Get the connection status
	 *
	 *  isConnected returns the current state of the connection.
	 * 
	 *  \return true : board connected
	 *  \return false : board disconnected
	 *
	 *****************************************************************************
	 */
	virtual bool isConnected();

	/*!
	 *****************************************************************************
	 *  \brief  Sends a command
	 *
	 *  sendCommand sends the command #command and returns a possible answer in #answer
	 *
	 *  \param[in] command : command to send
	 *  \param[out] answer : possible answer returned
	 *
	 *  \return ConnectionError : Not connected
	 *  \return ReadError : Error during read
	 *  \return WriteError : Error during write
	 *  \return NoError : No error
	 *****************************************************************************
	 */
	virtual Error sendCommand(QString command, QString * answer);

		/*!
	 *****************************************************************************
	 *  \brief  Get COM port
	 *
	 *  getComPort returns the port used for communication (eg. receive COM port)
	 *
	 *  \param[out] port : port in use
	 *  
	 *****************************************************************************
	 */
	virtual int getComPort() {return 0;};

		/*!
	 *****************************************************************************
	 *  \brief  Allows to set connection properties
	 *
	 *  setConnectionProperties is called to set connection properties (eg. speed,
	 *  parity, stopbit for UART). This function has to be implemented by the
	 *  deriving class and has to cast the inout to a corresponding object/structure
	 *
	 *****************************************************************************
	 */
	virtual void setConnectionProperties(void*) = 0;

		/*!
	 *****************************************************************************
	 *  \brief  Allows to get connection properties
	 *
	 *  getConnectionProperties is called to get connection properties (eg. speed,
	 *  parity, stopbit for UART). This function has to be implemented by the
	 *  deriving class and has to cast the inout to a corresponding object/structure
	 *
	 *****************************************************************************
	 */
	virtual void getConnectionProperties(void*){};

		/*!
	 *****************************************************************************
	 *  \brief  Allows to emit the signal dataChanged
	 *
	 *  This functionality is needed if the readOnce function of the Register Map
	 *  is overwritten and does not use readRegister anymore. By manually emitting
	 *  this signal the connected AMSControls get updated.
	 *
	 *****************************************************************************
	 */
	virtual void doEmitDataChanged(unsigned char registerAddress, unsigned char registerValue);
	virtual void doEmitDataChanged(unsigned char registerAddress, unsigned char subAddress, unsigned char registerValue);

signals:
	void connectionChanged(bool isConnected);
	void dataChanged(unsigned char reg, unsigned char subreg, bool isSubreg, unsigned char value);
	void dataChanged(unsigned char reg, unsigned char subreg, bool isSubreg, unsigned char mask, unsigned char value, unsigned char access, int err, QString any);

public slots:
	virtual void deviceNodeChanged();

private :
	unsigned char subRegister;
	bool isSubregister;

protected:
	bool connected;
	QMutex *mutex;
	unsigned short ctrlRegisterAddr;
	unsigned char ctrlRegisterMask;
	unsigned char devAddr;

	/*!
	 *****************************************************************************
	 *  \brief  Read from a register
	 *
	 *  hwReadRegister is called from #readRegister and is the actual call to the
	 *  underlying hardware layer, i.e. this function needs to be implemented
	 *  by the specific implementation (e.g. USBBoxCommunication class)
	 *
	 *  \param[in]  reg : Address of the register to read from
	 *  \param[out] val : Pointer to a variable where the read value should be stored
	 *
	 *  \return ConnectionError : Not connected
	 *  \return ReadError : Error during read
	 *  \return NoError : No error
	 *****************************************************************************
	 */
	virtual Error hwReadRegister(unsigned char reg, unsigned char *val) = 0;
	/*!
	 *****************************************************************************
	 *  \brief  Write to a register
	 *
	 *  hwWriteRegister is called from #writeRegister and is the actual call to the
	 *  underlying hardware layer, i.e. this function needs to be implemented
	 *  by the specific implementation (e.g. USBBoxCommunication class)
	 *
	 *  \param[in] reg : Address of the register to write
	 *  \param[in] val : Value to write
	 *
	 *  \return ConnectionError : Not connected
	 *  \return WriteError : Error during write
	 *  \return NoError : No error
	 *****************************************************************************
	 */
	virtual Error hwWriteRegister(unsigned char reg, unsigned char val) = 0;
	/*!
	 *****************************************************************************
	 *  \brief  Sends a command
	 *
	 *  hwSendCommand sends the command #command and returns a possible answer in #answer
	 *
	 *  \param[in] command : command to send
	 *  \param[out] answer : possible answer returned
	 *
	 *  \return ConnectionError : Not connected
	 *  \return ReadError : Error during read
	 *  \return WriteError : Error during write
	 *  \return NoError : No error
	 *****************************************************************************
	 */
	virtual Error hwSendCommand(QString command, QString * answer) = 0;
	/*!
	 *****************************************************************************
	 *  \brief  Establish a connection to the board
	 *
	 *  hwConnect is called from #connect and is the actual call to the
	 *  underlying hardware layer, i.e. this function needs to be implemented
	 *  by the specific implementation (e.g. USBBoxCommunication class)
	 *
	 *  \return ConnectionError : Connection can't be established
	 *  \return NoError : No error
	 *****************************************************************************
	 */
	virtual Error hwConnect() = 0;

	/*!
	 *****************************************************************************
	 *  \brief  Close an open connection
	 *
	 *  hwDisconnect is called from #disconnect and is the actual call to the
	 *  underlying hardware layer, i.e. this function needs to be implemented
	 *  by the specific implementation (e.g. USBBoxCommunication class)
	 *
	 *****************************************************************************
	 */
	virtual void hwDisconnect() = 0;
};

#endif
