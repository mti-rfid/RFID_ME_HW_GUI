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

#include <usb_hid_wrapper.hxx>


USBHIDWrapper::USBHIDWrapper(unsigned char devAddr)
{
	this->devAddr = devAddr;
	connected = false;
}

USBHIDWrapper::~USBHIDWrapper()
{
}


AMSCommunication::Error USBHIDWrapper::hwConnect()
{
	if(connected)
		return NoError;
	return ConnectionError;
}

void USBHIDWrapper::hwDisconnect()
{
}

void USBHIDWrapper::update()
{
	if ( lastUpdate.isValid() && lastUpdate.elapsed() < 1000)
	{
		return;
	}
	lastUpdate.start();
	if (!ph->getRegs(valArray) == QrfeReaderInterface::OK)
	{
		valArray.clear();
		QByteArray f = QByteArray(100,0x55);
		valArray.append(f);
	}

}

AMSCommunication::Error USBHIDWrapper::hwReadRegister(unsigned char registerAddress, unsigned char* registerValue)
{
	int r = registerAddress;
	update();
	if (registerAddress > 0x17) r += 8;
	if (registerAddress > 0x12) r += 2;
	*registerValue = valArray.at(r);
	return NoError;
}

AMSCommunication::Error USBHIDWrapper::hwReadMultiByteRegister(unsigned char registerAddress, QByteArray* data, unsigned short length)
{
	int r = registerAddress;
	update();
	if (registerAddress > 0x17) r += 2;
	if (registerAddress > 0x16) r += 2;
	if (registerAddress > 0x15) r += 2;
	if (registerAddress > 0x14) r += 2;
	if (registerAddress > 0x12) r += 2;

	if (data == NULL)
		return ReadError;

	data->clear();
	data->append(valArray.mid(r,length));
	
	return NoError;
}

AMSCommunication::Error USBHIDWrapper::readSubRegister(unsigned char registerAddress, unsigned char *registerValue, unsigned char subAddress, bool doemit)
{
	int r = registerAddress;
	update();
	if (registerAddress > 0x17) r += 2;
	if (registerAddress > 0x16) r += 2;
	if (registerAddress > 0x15) r += 2;
	if (registerAddress > 0x14) r += 2;
	if (registerAddress > 0x12) r += 2;

    *registerValue = valArray.at(r+subAddress);
	
	return NoError;
}

  /** USBCommunication::writeByte
   * This function sends one Byte to the specified address
   * In case of success ERR_NO_ERR is returned
   */
AMSCommunication::Error USBHIDWrapper::hwWriteRegister(unsigned char registerAddress, unsigned char registerValue)
{
	QByteArray data;
	AMSCommunication::Error err = NoError;
	QrfeReaderInterface::Result res;
	
	data.resize(3);
	data[0] = registerValue;
	res = ph->setParam(registerAddress, data);
	if(res == QrfeReaderInterface::OK)
		err = NoError;
	return err;
}

AMSCommunication::Error USBHIDWrapper::writeSubRegister(unsigned char registerAddress, unsigned char registerValue, unsigned char subAddress, bool verify, bool doemit)
{
	AMSCommunication::Error err = NoError;
	QByteArray data;
	QrfeReaderInterface::Result res;

	if(subAddress > 3)
		return WriteError;

	mutex->lock();

	// first read mutlibyte register
	if(ReadError == hwReadMultiByteRegister(registerAddress, &data, 3))
		return ReadError;
	
	// overwrite corresponding byte
	data[subAddress] = registerValue;
	res = ph->setParam(registerAddress, data);
	if(res == QrfeReaderInterface::OK)
		err = NoError;

	mutex->unlock();
	return err;
}



AMSCommunication::Error USBHIDWrapper::hwSendCommand(QString command, QString * answer)
{
	return AMSCommunication::NoError;
}

void USBHIDWrapper::setConnectionProperties(void *)
{

}

void USBHIDWrapper::gotReader(QrfeReaderInterface* ph)
{
	this->ph = dynamic_cast<QrfeAmsReader*>( ph);
	this->connected = true;
}

void USBHIDWrapper::lostReader(QrfeReaderInterface* ph)
{
	if (this->ph != ph) return;
	this->ph = NULL;
	this->connected = false;
	// do some more clean up here
}
