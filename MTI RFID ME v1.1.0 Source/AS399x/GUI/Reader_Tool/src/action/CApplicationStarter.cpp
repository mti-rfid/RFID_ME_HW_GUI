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
 * CApplicationStarter.cpp
 *
 *  Created on: 28.10.2008
 *      Author: stefan.detter
 */

#include "CApplicationStarter.h"
#include <QFile>
#include <QMessageBox>

CApplicationStarter::CApplicationStarter(QString epc, QString appPath, QString appParams, QTimer* timer)
	: QrfeTraceModule("CApplicationStarter")
{
	// Save the timer
	m_timer = timer;

	// Save the epc
	m_epc = epc;

	// Start the process
	m_process = new QProcess();

	trc(1, "Starting " + appPath + " with the parameters " + appParams);
	m_process->start("\"" + appPath + "\" " + appParams);
	if(m_process->pid() == 0)
	{
		trc(1, "Could not start... ");
		QMessageBox::critical(0, "Error", "The Application \"" + appPath + "\" could not be started!");
		return;
	}
	QObject::connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
}

CApplicationStarter::~CApplicationStarter()
{
	// On delete kill the process and delete it
	m_process->kill();
	m_process->deleteLater();
}

void CApplicationStarter::finished ( int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/ )
{
	// If the application was closed by the user, fire the timer, so the main application recongnizes
	m_timer->setInterval(0);
	m_timer->start();
}
