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
 * CActionHandler.cpp
 *
 *  Created on: 02.02.2009
 *      Author: stefan.detter
 */

#include "CActionHandler.h"

CActionHandler::CActionHandler(CDataHandler* dataHandler, QObject* parent)
	: QObject(parent)
	, QrfeTraceModule("CActionHandler")
{
	m_doActionHandling = false;
	m_dataHandler = dataHandler;
}

CActionHandler::~CActionHandler()
{
}

void CActionHandler::startActionHandling()
{
	m_doActionHandling = true;
}


void CActionHandler::stopActionHandling()
{
	m_doActionHandling = false;
	stopAll();
	m_handledTags.clear();
}


void CActionHandler::cyclicInventroyResult(QString readerId, QString epc)
{
	if(!m_doActionHandling)
		return;

	trc(4, "-> handleNewTags");
	// If the tag was already handled, skip
	if(m_handledTags.contains(epc)){
		trc(3, "Tag already handled...");
		return;
	}

	// Mark tag as handled
	m_handledTags.append(epc);

	// If no configuration was saved for the tag, skip
	TagActionInfo t;
	if(!m_dataHandler->getConfig(epc, t)){
		trc(3, "No configuration for tag...");
		m_handledTags.removeAll(epc);
		return;
	}

	// If we should not do anything, skip
	if(!t.performAction){
		trc(3, "No action set for tag...");
		m_handledTags.removeAll(epc);
		return;
	}

	if(t.type == ShowPicture)
	{
		// Create new timer and connect it
		QTimer* timer = new QTimer();
		timer->setInterval(m_dataHandler->timeValToMsecs(t.time));
		timer->setSingleShot(true);
		QObject::connect(timer, SIGNAL(timeout()), this, SLOT(deleteShownPicture()));

		// Create the widget that is shown in the display tab
		CShowPictureWidget* widget = new CShowPictureWidget((QWidget*)QApplication::desktop(), epc, t.picPath, t.aliasName, readerId);
		widget->show();
		m_openPicture.insert(timer, widget);

		// Start timer
		timer->start();

	}
	else if(t.type == StartApp)
	{
		// Create new timer and connect it
		QTimer* timer = new QTimer();
		timer->setInterval(m_dataHandler->timeValToMsecs(t.time));
		timer->setSingleShot(true);
		QObject::connect(timer, SIGNAL(timeout()), this, SLOT(stoppApplication()));

		trc(1, "Starting application " + t.appPath);
		// Create the widget that is shown in the display tab
		CApplicationStarter* app = new CApplicationStarter(epc, t.appPath, t.appParams, timer);
		m_openApps.insert(timer, app);

		// Start timer
		timer->start();
	}
	else{
		m_handledTags.removeAll(epc);
	}

}

void CActionHandler::deleteShownPicture()
{
	trc(4, "-> deleteShownPicture");
	// Get the timer that ran to timeout
	QTimer* timer = (QTimer*) sender();
	timer->stop();

	// Get the widget that was connected with the timer
	CShowPictureWidget* widget = (CShowPictureWidget*)m_openPicture.value(timer);
	if(widget == 0)return;

	// Remove picture from the display widget and mark tag as unhandled
	widget->setVisible(false);

	m_openPicture.remove(timer);
	m_handledTags.removeAll(widget->epc());

	// Delete timer and widget
	widget->deleteLater();
	timer->deleteLater();
}

void CActionHandler::stoppApplication()
{
	trc(4, "-> stoppApplication");
	// Get the timer that ran to timeout
	QTimer* timer = (QTimer*) sender();
	timer->stop();

	// Get the app starter that was connected with the timer
	CApplicationStarter* app = m_openApps.value(timer);
	if(app == 0)return;

	// Remove app and mark tag as unhandled
	m_openApps.remove(timer);
	m_handledTags.removeAll(app->epc());

	// Delete timer and app starter, the app will then be killed
	app->deleteLater();
	timer->deleteLater();
}

void CActionHandler::stopAll()
{
	foreach(QTimer* timer, m_openPicture.keys()){
		timer->stop();

		// Get the widget that was connected with the timer
		CShowPictureWidget* widget = (CShowPictureWidget*)m_openPicture.value(timer);
		if(widget == 0)return;

		// Remove picture from the display widget and mark tag as unhandled
		widget->setVisible(false);

		m_openPicture.remove(timer);
		m_handledTags.removeAll(widget->epc());

		// Delete timer and widget
		widget->deleteLater();
		timer->deleteLater();
	}

	foreach(QTimer* timer, m_openApps.keys())
	{
		timer->stop();

		CApplicationStarter* starter = (CApplicationStarter*)m_openApps.value(timer);

		m_openApps.remove(timer);
		m_handledTags.removeAll(starter->epc());

		starter->deleteLater();
		timer->deleteLater();
	}

}
