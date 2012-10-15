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
 * @file 	main.cpp
 * @brief	This file starts the application and a few used components
 */

/*!
 * @mainpage	Reader Tool Project
 * @brief		The demo tool for RFID reader
 *
 * @section 	Project_intro Introduction
 * This tool is used to demonstrate the functionalities of the used rfid reader.
 *
 * @section		Project_architecture Architecture
 * The architecture of the project is set up like follows:
 * On the side of the application there is the communication to the reader. The communication is split up into a protocol handler and
 * a reader implementation. There is also a component needed, that detects the reader when the are plugged in and manages this reader.
 * On the other side of the application is the implementation of the user interface. In the middle of these parts a control unit is needed.
 *
 * @subsection		Project_communication Communication
 * The most important architecture is the architecture of the communication part of the application.
 *
 * @subsubsection	Project_protcolhandler Protocol Handler
 * The protocol handler is responsible for the whole low level communication with the reader. It gets an QIODevice, with which it communicate
 * with the reader. The implementation of the defined protocol is in the protocol handler. The protocol handler offers the functions to the
 * application that implement every command defined in the protocol. These functions are blocking. So if a function of the protocol handler is
 * called the protocol handler sends the command and waits for the response of the reader. This wait interval is defined for a specific maximum
 * time. If this time exceeds, the protocol handler returns an error. If the response of the reader is returned within the specified time, the
 * result is returned to the calling application. Because of the blocking functions the protocol handler needs a special way to wait for response
 * to not block block the gui or anything else.
 *
 * @subsubsection	Project_readerinterface Reader Interface
 * The reader interface implements the logic of a reader. It combines specific functions of the protocol handler to offer the application a more
 * high level interface to control the reader. It also manages for example cyclic inventory implementation and so on.
 *
 *
 */

#include "CReaderTool.h"

#include <QtGui>
#include <QApplication>

#ifdef QrfeDATABASEINTERFACE_DEBUG
#include <QrfeTrace.h>
#endif

#include "reader/epc/EPC_Defines.h"


const char* APPLICATION_NAME = "Reader Settings MTI RFID ME"; //SW Caption
const char* VERSION			 = "MTI RFID ME v1.0.9"; // SW Version


/*!
 * @brief Main function
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Initialize the trace component
	QrfeTrace::init(0, QrfeTrace::Trace2Signal | QrfeTrace::Trace2Stdout | QrfeTrace::Trace2File);
	// Initialize the EPC constants for further use
	initEPC_Constants();

	// Create Reader Tool and show it
    CReaderTool w;
    w.show();
    return a.exec();
}
