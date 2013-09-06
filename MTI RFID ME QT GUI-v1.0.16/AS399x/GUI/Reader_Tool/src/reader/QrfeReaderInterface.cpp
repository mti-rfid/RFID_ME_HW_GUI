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
 * @file	QrfeReaderInterface.cpp
 * @brief	The base implementation of a reader object
 */

#include "QrfeReaderInterface.h"
#include <QrfeSleeper.h>
#include <QCoreApplication>


/*!
 * @brief Constructor of the Reader Object
 * The constructor initializes variables and stores the used protocol handler.
 * @param 	ph		Pointer to the used protocol handler
 * @param	parent	Pointer to the parent object.
 */
QrfeReaderInterface::QrfeReaderInterface(QrfeProtocolHandler* ph, QObject* parent)
	: QObject(parent)
	, m_ph(ph)
{
	// initialize the variables
	m_currentState = STATE_OFFLINE;
	m_currentAction = ACTION_IDLE;

	m_readerID 			= "Unknown";
	m_readerType 		= TYPE_UNKOWN;
	m_readerVersion 	= VERSION_RFE_PUR_RM1;
	m_tagType			= TAG_UNKWON;
	m_enumDevType		= ENUM_PRODUCT::UNKNOW;


	m_ph->setParent(this);
}

/*! @brief Destructor deletes the used protocol handler */
QrfeReaderInterface::~QrfeReaderInterface ( )
{
	m_ph->deleteLater();
}

/*! @brief Returns the gen2 reader. Default 0 */
QrfeGen2ReaderInterface* QrfeReaderInterface::getGen2Reader()
{
	return 0;
}

/*! @brief Getter for Reader ID */
QString QrfeReaderInterface::readerId()
{
	return m_readerID;
}

/*! @brief Getter for Reader Type */
QrfeReaderInterface::ReaderType QrfeReaderInterface::readerType()
{
	return m_readerType;
}

/*! @brief Getter for Device Type */
QrfeReaderInterface::ENUM_PRODUCT QrfeReaderInterface::devType()
{
	return m_enumDevType;
}

/*! @brief Getter for Reader Version */
QrfeReaderInterface::ReaderVersion QrfeReaderInterface::readerVersion()
{
	return m_readerVersion;
}

/*! @brief Getter for used Tag Type */
QrfeReaderInterface::TagType QrfeReaderInterface::tagType()
{
	return m_tagType;
}

/*! @brief Getter for current state */
QrfeReaderInterface::HandlerState QrfeReaderInterface::currentState()
{
	return m_currentState;
}

/*! @brief Getter for current action */
QrfeReaderInterface::HandlerAction QrfeReaderInterface::currentAction()
{
	return m_currentAction;
}

/*! @brief Getter for current state as string */
QString QrfeReaderInterface::currentStateString()
{
	// return the state as human readable string
	switch(m_currentState){
	case STATE_ONLINE:
		return "Online";
	case STATE_OFFLINE:
		return "Offline";
	case STATE_NOTRESPONDING:
		return "Not Responding";
	case STATE_UNKOWN_ERROR:
		return "Unknown Error";
	default:
		return "Unknown";
	}
}

/*! @brief Getter for current action as string */
QString QrfeReaderInterface::currentActionString()
{
	// return the state as human readable string
	switch(m_currentAction){
	case ACTION_OFFLINE:
		return "Offline";
	case ACTION_IDLE:
		return "Idle";
	case ACTION_SCANNING:
		return "Scanning";
	case ACTION_WAITING:
		return "Waiting";
	default:
		return "Unknown";
	}
}

/*! @brief Getter for the device name of the used device */
QString	QrfeReaderInterface::deviceName()
{
	return m_ph->deviceName();
}

/*! @brief Setter for the current state */
void QrfeReaderInterface::setState(QrfeReaderInterface::HandlerState state)
{
	// set state and emit it only if it is different to the state before
	if(m_currentState != state){
		m_currentState = state;

		emit changedState(currentStateString());

		if(!m_ph->deviceRemoved() && m_currentState == STATE_OFFLINE)
			emit lostConnection();
	}
}

/*! @brief Setter for the current action */
void QrfeReaderInterface::setAction(QrfeReaderInterface::HandlerAction action)
{
	// set state and emit it only if it is different to the state before
	if(m_currentAction != action){
		m_currentAction = action;
		emit changedAction(currentActionString());

		// process events so everyone can get the signal
		//qApp->processEvents();
	}
}

/*! @brief Slot that is called if the reader should immediately release the used device */
void QrfeReaderInterface::releaseDeviceImmediatly()
{
	m_ph->releaseDeviceImmediatly();
	setState(STATE_OFFLINE);
}

/*! @brief Slot that is called if the used device was removed */
void QrfeReaderInterface::deviceWasRemoved()
{
	setState(STATE_OFFLINE);
	m_ph->deviceWasRemoved();
}

/*! @brief Protected function that translate a return code of the protocol handler into a return code of the reader object */
QrfeReaderInterface::Result QrfeReaderInterface::translateProtocolHandlerError(QrfeProtocolHandler::Result res)
{
	switch(res)
	{
	case QrfeProtocolHandler::OK:
		return OK;
	case QrfeProtocolHandler::ERROR:
		return ERROR;
	case QrfeProtocolHandler::COMMERROR:
		return COMMERROR;
	case QrfeProtocolHandler::NORESPONSE:
		return NORESPONSE;
	default:
		return ERROR;
	}
}

/*! @brief Protected function that handles a return code of the protocol handler */
void QrfeReaderInterface::handleProtocolHandlerError(QrfeProtocolHandler::Result res)
{
	switch(res)
	{
	case QrfeProtocolHandler::OK:
		setState(STATE_ONLINE);
		break;
	case QrfeProtocolHandler::ERROR:
		setState(STATE_ONLINE);
		break;
	case QrfeProtocolHandler::COMMERROR:
		setState(STATE_OFFLINE);
		break;
	case QrfeProtocolHandler::NORESPONSE:
		/* if we encounter two times not responding, assume offline */
		if (m_currentState == STATE_NOTRESPONDING)
			setState(STATE_OFFLINE);
		else
			setState(STATE_NOTRESPONDING);
		break;
	}
}

QStringList QrfeReaderInterface::getRssiChildren()
{
	QStringList sl;
	return sl;
}






