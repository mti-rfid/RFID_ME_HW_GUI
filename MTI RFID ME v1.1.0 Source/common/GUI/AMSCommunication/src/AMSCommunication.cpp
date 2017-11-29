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
 *  \brief  AMSCommunication class
 *
 *  AMSCommunication is an abstract class used for abstraction
 *  of different communication interfaces between the PC
 *  and the demo boards.
 */

#include "AMSCommunication.hxx"

AMSCommunication::AMSCommunication()
{
	this->connected = false;
	mutex = new QMutex(QMutex::NonRecursive);
}

void AMSCommunication::deviceNodeChanged()
{
	// have a look if a board with the correct PID is connected
	if(isConnected())
	{
		// try to connect to this board, if we are already connected, nothing will happen
		connect();
	}
	else
	{
		// there is no board with correct PID connected
		// we try to disconnect
		disconnect();
	}
}

void AMSCommunication::disconnect()
{
	this->hwDisconnect();
	emit connectionChanged(false);
    connected = false;
}

AMSCommunication::Error AMSCommunication::connect()
{
	AMSCommunication::Error err = this->hwConnect();
	if (NoError == err)
	{
		connected = true;
	}
	else
	{	
		connected = false;
	}
	emit connectionChanged(connected);
	return err;

}

bool AMSCommunication::isConnected()
{
	return this->connected;
}

AMSCommunication::Error AMSCommunication::readRegister(unsigned char registerAddress, unsigned char* registerValue, bool doemit)
{
	AMSCommunication::Error err = NoError;

	if(!connected)
	{
		/* try to connect */
		err = this->connect();
	}
	mutex->lock();
	if (NoError == err)
	{
		err = this->hwReadRegister(registerAddress, registerValue);
		if (NoError == err)
		{
			doemit = true;
		}
	}
	mutex->unlock();
	if (doemit)
	{
		emit this->dataChanged(registerAddress, 0 /* don't care */, false, *registerValue);
	}
	emit this->dataChanged(registerAddress, 0 /* don't care */, false, 0, *registerValue, Read,err,"");
	return err;
}

AMSCommunication::Error AMSCommunication::writeRegister(unsigned char registerAddress, unsigned char registerValue, bool verify,bool doemit)
{
	AMSCommunication::Error err = NoError;
	unsigned char verifyValue;

	if(!connected)
	{
		/* try to connect */
		err = this->connect();
	}
	mutex->lock();
    if (NoError == err)
	{
		err = this->hwWriteRegister(registerAddress, registerValue);
		if (NoError == err)
		{
			doemit = true;
			if(verify == true) /* verify */
			{
				err = this->hwReadRegister(registerAddress, &verifyValue);
				if (NoError == err)
				{
					if (verifyValue != registerValue)
					{
						err = VerifyError;
						doemit = false;
					}
				}
			}
		}
	}
	mutex->unlock();
	if (doemit)
	{
		emit this->dataChanged(registerAddress, 0 /* don't care */, false, registerValue);
	}
	emit this->dataChanged(registerAddress, 0 /* don't care */, false, 0, registerValue, Write,err,"");
	return err;
}

AMSCommunication::Error AMSCommunication::modifyRegister(unsigned char reg, unsigned char mask, unsigned char val, bool verify,bool doemit)
{
	unsigned char readval=0, verifyValue = 0;
	Error res = NoError;

	if(!connected)
	{
		/* try to connect */
		res = this->connect();
	}
	mutex->lock();
    if (NoError == res)
	{
		res = this->hwReadRegister(reg, &readval);
		if (NoError == res)
		{
			readval &= ~mask;
			readval |= (val & mask);
			res = this->hwWriteRegister(reg, readval);
			if (NoError == res)
			{
				doemit = true;
				if(verify == true)
				{
					res = this->hwReadRegister(reg, &verifyValue);
					if (NoError != res || verifyValue != readval)
					{
						res = VerifyError;
					}
				}
			}
		}
	}
	mutex->unlock();
	if (doemit)
	{
		emit this->dataChanged(reg, 0 /* don't care */, false, readval);
	}
	emit this->dataChanged(reg, 0 /* don't care */, false, mask, val, Modify,res,"");
	return res;
}

void AMSCommunication::setControlRegisterAddress(unsigned short ctrlRegister, unsigned char ctrlMask)
{
	this->ctrlRegisterAddr = ctrlRegister;
	this->ctrlRegisterMask = ctrlMask;
	emit this->dataChanged(ctrlRegister, 0, 0, ctrlMask,0, ControlReg , NoError,"");
}

AMSCommunication::Error AMSCommunication::writeSubRegister(unsigned char reg, unsigned char val, unsigned char subAddress, bool verify, bool doemit)
{
	Error res = NoError;
	unsigned char readval, verifyvalue;

	if(!connected)
	{
		/* try to connect */
		res = this->connect();
	}
    mutex->lock();
	if (NoError == res)
	{
		res = this->hwReadRegister(ctrlRegisterAddr, &readval);
		if (NoError == res)
		{
			readval &= ~this->ctrlRegisterMask;
			readval |= (subAddress & this->ctrlRegisterMask);
			res = this->hwWriteRegister(ctrlRegisterAddr, readval);
			if (res == NoError)
			{
				res = this->hwWriteRegister(reg, val);
				if (NoError == res)
				{
					if(verify == true) /* verify */
					{
						/* write again to the master register */
						res = this->hwWriteRegister(ctrlRegisterAddr, readval);
						if (NoError == res)
						{
							doemit = true;
							res = this->hwReadRegister(reg, &verifyvalue);
							if (NoError != res || verifyvalue != val)
							{
								res = VerifyError;
							}
						}
					}
				}
			}
		}
	}
	mutex->unlock();
	if (doemit)
	{
		emit this->dataChanged(reg, subAddress, true, val);
	}
	emit this->dataChanged(reg, subAddress, true, 0,val, Write,res,"");
	return res;
}

AMSCommunication::Error AMSCommunication::readSubRegister(unsigned char reg, unsigned char *val, unsigned char subAddress, bool doemit)
{
	Error res = NoError;
	unsigned char readval;

	if(!connected)
	{
		/* try to connect */
		res = this->connect();
	}
    mutex->lock();
	if (NoError == res)
	{
		res = this->hwReadRegister(ctrlRegisterAddr, &readval);
		if (NoError == res)
		{
			readval &= ~this->ctrlRegisterMask;
			readval |= (subAddress & this->ctrlRegisterMask);
			res = this->hwWriteRegister(ctrlRegisterAddr, readval);
			if (res == 0)
			{
				res = this->hwReadRegister(reg, val);
				doemit = true;
			}
		}
	}

	mutex->unlock();
	if (doemit)
	{
		emit this->dataChanged(reg, subAddress, true, *val);
	}
	emit this->dataChanged(reg, subAddress, true, 0, *val, Read,res,"");
	return res;
}
	
AMSCommunication::Error AMSCommunication::modifySubRegister(unsigned char reg, unsigned char mask, unsigned char val, unsigned char subAddress, bool verify, bool doemit)
{
	unsigned char readval=0, readvalCtrl=0, verifyValue=0;
	Error res = NoError;

	if(!connected)
	{
		/* try to connect */
		res = this->connect();
	}
    mutex->lock();
	if (NoError == res)
	{
		res = this->hwReadRegister(ctrlRegisterAddr, &readvalCtrl);
		if (NoError == res)
		{
			readvalCtrl &= ~this->ctrlRegisterMask;
			readvalCtrl |= (subAddress & this->ctrlRegisterMask);
			res = this->hwWriteRegister(ctrlRegisterAddr, readvalCtrl);
			if (NoError == res)
			{
				res = this->hwReadRegister(reg, &readval);
				if (NoError == res)
				{
					res = this->hwWriteRegister(ctrlRegisterAddr, readvalCtrl);
					if (NoError == res)
					{
						readval &= ~mask;
						readval |= (val & mask);
						res = this->hwWriteRegister(reg, readval);
						if (NoError == res)
						{
							/* write again to the master register */
							res = this->hwWriteRegister(ctrlRegisterAddr, readvalCtrl);
							if (NoError == res)
							{
								doemit = true;
								if(verify == true)
								{
									res = this->hwReadRegister(reg, &verifyValue);
									if (NoError != res || verifyValue != readval)
									{
										res = VerifyError;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	mutex->unlock();
	if (doemit)
	{
		emit this->dataChanged(reg, subAddress, true, readval);
	}
	emit this->dataChanged(reg, subAddress, true, mask, val, Modify,res,"");
	return res;
}

AMSCommunication::Error AMSCommunication::sendCommand(QString command, QString * answer)
{
	AMSCommunication::Error err = NoError;
	bool doemit = true;

	if(!connected)
	{
		/* try to connect */
		err = this->connect();
	}
    mutex->lock();
	if (NoError == err)
	{
		err = this->hwSendCommand(command, answer);
		if (NoError == err)
		{
			doemit = true;
		}
	}
	mutex->unlock();
	if (doemit)
	{
	}
	emit this->dataChanged(0, 0, false, 0, 0, Command,err,command);
	return err;
}

void AMSCommunication::doEmitDataChanged(unsigned char registerAddress, unsigned char registerValue)
{
	emit this->dataChanged(registerAddress, 0 /* don't care */, false, registerValue);
}

void AMSCommunication::doEmitDataChanged(unsigned char registerAddress, unsigned char subAddress, unsigned char registerValue)
{
	emit this->dataChanged(registerAddress, subAddress, true, registerValue);
}

