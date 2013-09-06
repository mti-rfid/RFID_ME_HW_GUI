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

#include "CShowPictureWidget.h"

#include <QFile>
#include <QPixmap>
#include <QDateTime>

CShowPictureWidget::CShowPictureWidget(QWidget *parent, QString epc, QString picturePath, QString info, QString readerId)
    : QWidget(parent)
{
	ui.setupUi(this);

	// Save the epc
	m_epc = epc;

	this->setWindowTitle(info);
	this->setWindowOpacity(1.0);

	// Set up the widget with all needed data and load the picture
	ui.infoLabel->setText(info);
	ui.readerIdLabel->setText("<html>&nbsp;&nbsp;&nbsp;" + readerId + "</html>");
	ui.timeDetectedLabel->setText("<html>&nbsp;&nbsp;&nbsp;" + QDateTime::currentDateTime().toString() + "</html>");
	if(QFile::exists(picturePath))
	{
		QPixmap p(picturePath);
		if((p.width()/3) < (p.height()/2))
			p = p.scaledToHeight(150);
		else
			p = p.scaledToWidth(225);
		ui.pictureLabel->setPixmap(p);
	}
	else
		ui.pictureLabel->setText("Could not find file.");

}

CShowPictureWidget::~CShowPictureWidget()
{

}
