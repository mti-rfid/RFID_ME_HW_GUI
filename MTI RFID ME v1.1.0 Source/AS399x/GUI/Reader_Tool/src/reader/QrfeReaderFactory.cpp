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
 * @file 	QrfeReaderFactory.cpp
 * @brief	Reader factory that tries to create reader objects with the given devices
 */

#include "QrfeReaderFactory.h"

#include "ams/QrfeAmsReader.h"

#include <QrfeSleeper.h>

QrfeReaderFactory::QrfeReaderFactory()
	: QrfeTraceModule("QrfeReaderFactory")
{

}

QrfeReaderFactory::~QrfeReaderFactory()
{
}

/*!
 * @brief 	Static function that creates an ams reader that is connected via the given device.
 * Static function that creates an ams reader that is connected via the given device.
 * @param	dev			IODevice over which the protocol handler can communicate with the reader
 * @param	deviceType	Specifies the type of the device
 * @param	parent		Pointer to the object that should be used as parent
 */
QrfeAmsReader* QrfeReaderFactory::getAmsReader(QIODevice* dev, QrfeProtocolHandler::DeviceType deviceType, QObject* parent)
{
	/* Create AMS protocol handler */
	strc("QrfeReaderFactory", 3, "Creating a ams protocol handler");
	QrfeProtocolHandlerAMS* ph = new QrfeProtocolHandlerAMS(dev, deviceType, parent);

	/* Create AMS reader object */
	strc("QrfeReaderFactory", 3, "Creating a ams leo object");
	return new QrfeAmsReader(ph, parent);
}
