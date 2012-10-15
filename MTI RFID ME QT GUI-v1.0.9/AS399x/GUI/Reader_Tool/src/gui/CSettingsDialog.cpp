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

#include "CSettingsDialog.h"

#include <QMessageBox>

CSettingsDialog::CSettingsDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	QObject::connect(ui.buttonBox, 			SIGNAL(accepted()), 					this, SLOT(tryAccept()));

    m_showAlias 			= false;

    m_useTtl 				= true;
    m_msecsToShowInactive 	= 1000;
    m_msecsToShowOutOfRange = 5000;
    m_msecsToDelete			= 15000;

    m_useTrace				= false;
    m_traceLevel 			= 0;

    m_useMultiplex 			= false;
    m_multiplexTime 		= 500;

	m_useRegisterUpdate		= false;
	m_registerUpdateTime	= 1000;


}

CSettingsDialog::~CSettingsDialog()
{

}

int CSettingsDialog::exec()
{
	ui.showAliasCheckBox->setChecked(m_showAlias);

	ui.useTtlCheckBox->setChecked(m_useTtl);
	ui.inactiveSpinBox->setValue(m_msecsToShowInactive);
	ui.outOfRangeSpinBox->setValue(m_msecsToShowOutOfRange);
	ui.deleteSpinBox->setValue(m_msecsToDelete);

	ui.useTraceBox->setChecked(m_useTrace);
	ui.traceLevelBox->setValue(m_traceLevel);

	ui.useMultiplexBox->setChecked(m_useMultiplex);
	ui.multiplexReadTimeBox->setValue(m_multiplexTime);

	ui.registerMapUpdateEnable->setChecked(m_useRegisterUpdate);
	ui.pollingIntervalSB->setValue(m_registerUpdateTime);


	return QDialog::exec();
}

void CSettingsDialog::tryAccept()
{
	if(ui.inactiveSpinBox->value() <= ui.outOfRangeSpinBox->value() && ui.outOfRangeSpinBox->value() <= ui.deleteSpinBox->value())
	{
		m_showAlias = ui.showAliasCheckBox->isChecked();

		m_useTtl = ui.useTtlCheckBox->isChecked();
		m_msecsToShowInactive = ui.inactiveSpinBox->value();
		m_msecsToShowOutOfRange = ui.outOfRangeSpinBox->value();
		m_msecsToDelete = ui.deleteSpinBox->value();

		m_useTrace = ui.useTraceBox->isChecked();
		m_traceLevel = ui.traceLevelBox->value();

		m_useMultiplex = ui.useMultiplexBox->isChecked();
		m_multiplexTime = ui.multiplexReadTimeBox->value();

		m_useRegisterUpdate = ui.registerMapUpdateEnable->isChecked();
		m_registerUpdateTime = ui.pollingIntervalSB->value();

		accept();
	}
	else
	{
		QMessageBox::critical(this, "Error", "The time settings must be in order.");
	}

}

bool CSettingsDialog::showAlias()
{
	return m_showAlias;
}

void CSettingsDialog::setShowAlias(bool on)
{
	m_showAlias = on;
}


bool CSettingsDialog::useTtl()
{
	return m_useTtl;
}

void CSettingsDialog::setUseTtl(bool on)
{
	m_useTtl = on;
}


uint CSettingsDialog::msecsToShowInactive()
{
	return m_msecsToShowInactive;
}

void CSettingsDialog::setMsecsToShowInactive(uint msecs)
{
	m_msecsToShowInactive = msecs;
}


uint CSettingsDialog::msecsToShowOutOfRange()
{
	return m_msecsToShowOutOfRange;
}

void CSettingsDialog::setMsecsToShowOutOfRange(uint msecs)
{
	m_msecsToShowOutOfRange = msecs;
}


uint CSettingsDialog::msecsToDelete()
{
	return m_msecsToDelete;
}

void CSettingsDialog::setMsecsToDelete(uint msecs)
{
	m_msecsToDelete = msecs;
}

bool CSettingsDialog::useTrace()
{
	return m_useTrace;
}

void CSettingsDialog::setUseTrace(bool on)
{
	m_useTrace = on;
}


uchar CSettingsDialog::traceLevel()
{
	return m_traceLevel;
}

void CSettingsDialog::setTraceLevel(uchar level)
{
	m_traceLevel = level;
}


bool CSettingsDialog::useMultiplex()
{
	return m_useMultiplex;
}

void CSettingsDialog::setUseMultiplex(bool on)
{
	m_useMultiplex = on;
}

uint CSettingsDialog::multiplexTime()
{
	return m_multiplexTime;
}

void CSettingsDialog::setMultiplexTime(uint time)
{
	m_multiplexTime = time;
}

bool CSettingsDialog::useRegisterUpdate()
{
	return m_useRegisterUpdate;
}

void CSettingsDialog::setUseRegisterUpdate(bool on)
{
	m_useRegisterUpdate = on;
}

uint CSettingsDialog::registerUpdateTime()
{
	return m_registerUpdateTime;
}

void CSettingsDialog::setRegisterUpdateTime(uint time)
{
	m_registerUpdateTime = time;
}
