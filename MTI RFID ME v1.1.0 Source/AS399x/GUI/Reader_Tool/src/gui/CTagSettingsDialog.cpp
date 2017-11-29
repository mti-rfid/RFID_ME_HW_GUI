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

#include "CTagSettingsDialog.h"

#include <QFileDialog>
#include <QPixmap>
#include <QMessageBox>

CTagSettingsDialog::CTagSettingsDialog(CDataHandler* dataHandler, QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	m_dataHandler = dataHandler;

	QObject::connect(ui.buttonBox, 			SIGNAL(accepted()), 					this, SLOT(tryAccept()));
	QObject::connect(ui.timeSlider,			SIGNAL(valueChanged(int)),				this, SLOT(setTimeValue(int)));
	QObject::connect(ui.picPathButton,		SIGNAL(clicked()),						this, SLOT(searchForPicture()));
	QObject::connect(ui.appButton,			SIGNAL(clicked()),						this, SLOT(searchForApplication()));

}

CTagSettingsDialog::~CTagSettingsDialog()
{

}

int CTagSettingsDialog::exec(QString tagId)
{
	clearAll();
	loadData(tagId);
	return QDialog::exec();
}

void CTagSettingsDialog::tryAccept()
{
	if(ui.tagIdEdit->text().isEmpty())
		return;

	if(ui.aliasNameEdit->text().isEmpty()){
		QMessageBox::critical(this, "Error", "Please enter an alias name.");
		return;
	}

	if(ui.actionCheckBox->isChecked() && ui.showPictureButton->isChecked() && ui.picPathEdit->text().isEmpty()){
		QMessageBox::critical(this, "Error", "Please select a Picture.");
		return;
	}

	if(ui.actionCheckBox->isChecked() && ui.startAppButton->isChecked() && ui.appEdit->text().isEmpty()){
		QMessageBox::critical(this, "Error", "Please select an Application.");
		return;
	}

	save();
	accept();
}

void CTagSettingsDialog::clearAll()
{
	ui.tagIdEdit->clear();
	ui.aliasNameEdit->setEnabled(true);
	ui.infoLabel->setEnabled(true);
	ui.actionCheckBox->setEnabled(true);
	ui.aliasNameEdit->clear();
	ui.actionCheckBox->setChecked(false);
	ui.timeSlider->setValue(0);
	ui.picPathEdit->clear();
	ui.previewPicLabel->clear();
	ui.previewPicLabel->setText("Picture");
	ui.appEdit->clear();
	ui.appParamEdit->clear();
	ui.showPictureButton->setChecked(true);
}


void CTagSettingsDialog::loadData(QString epc)
{
	ui.tagIdEdit->setText(epc);

	// Get the configuration from the database
	TagActionInfo t;
	if(!m_dataHandler->getConfig(epc, t))
		return;

	// Set all fields and load image
	ui.aliasNameEdit->setText(t.aliasName);
	ui.actionCheckBox->setChecked(t.performAction);
	ui.timeSlider->setValue(t.time);
	if(t.type == ShowPicture)
	{
		ui.showPictureButton->setChecked(true);
		ui.picPathEdit->setText(t.picPath);
		if(QFile::exists(t.picPath))
		{
			QPixmap p(t.picPath);
			if((p.width()/p.height()) < (3/2))
				p = p.scaledToHeight(100);
			else
				p = p.scaledToWidth(150);
			ui.previewPicLabel->setPixmap(p);
		}
		else
			ui.previewPicLabel->setText("Could not find file.");
	}
	else if(t.type == StartApp){
		ui.startAppButton->setChecked(true);
		ui.appEdit->setText(t.appPath);
		ui.appParamEdit->setText(t.appParams);
	}
}

void CTagSettingsDialog::save()
{
	// Get all information from the input mask
	TagActionInfo t;
	t.aliasName = ui.aliasNameEdit->text();
	t.performAction = ui.actionCheckBox->isChecked();
	t.time = ui.timeSlider->value();
	t.type = (ui.showPictureButton->isChecked())?ShowPicture:StartApp;
	t.picPath = ui.picPathEdit->text();
	t.appPath = ui.appEdit->text();
	t.appParams = ui.appParamEdit->text();

	// Save configuration in the database
	m_dataHandler->saveConfig(ui.tagIdEdit->text(), t);

	loadData(ui.tagIdEdit->text());
	newSettingForTag(ui.tagIdEdit->text());

	return;
}


void CTagSettingsDialog::searchForPicture()
{
	// Get file path to the picture with the file dialog
	QString path = "/";
	if(!ui.picPathEdit->text().isEmpty())
		path = ui.picPathEdit->text();
	QString file = QFileDialog::getOpenFileName( this, "Find Picture", path, "Images (*.png *.xpm *.jpg)");

	if(file.isEmpty())
		return;

	// Set the text field and show the preview
	ui.picPathEdit->setText(file);
	QPixmap p(file);
	if((p.width()/p.height()) < (3/2))
		p = p.scaledToHeight(100);
	else
		p = p.scaledToWidth(150);
	ui.previewPicLabel->setPixmap(p);

}

void CTagSettingsDialog::searchForApplication()
{
	// Get file path to the application with the file dialog
	QString path = "/";
	if(!ui.appEdit->text().isEmpty())
		path = ui.appEdit->text();
	QString file = QFileDialog::getOpenFileName( this, "Find Application", path, "Application (*.exe)");

	if(!file.isEmpty())
		ui.appEdit->setText(file);

}


void CTagSettingsDialog::setTimeValue(int value)
{
	switch(m_dataHandler->timeValToMsecs(value)){
	case    1000: ui.timeLabel->setText(" 1 sec");break;
	case    2000: ui.timeLabel->setText(" 2 sec");break;
	case    3000: ui.timeLabel->setText(" 3 sec");break;
	case    5000: ui.timeLabel->setText(" 5 sec");break;
	case   10000: ui.timeLabel->setText("10 sec");break;
	case   20000: ui.timeLabel->setText("20 sec");break;
	case   30000: ui.timeLabel->setText("30 sec");break;
	case   60000: ui.timeLabel->setText(" 1 min");break;
	case  120000: ui.timeLabel->setText(" 2 min");break;
	case  180000: ui.timeLabel->setText(" 3 min");break;
	case  300000: ui.timeLabel->setText(" 5 min");break;
	case  600000: ui.timeLabel->setText("10 min");break;
	case 1200000: ui.timeLabel->setText("20 min");break;
	case 1800000: ui.timeLabel->setText("30 min");break;
	}
}

