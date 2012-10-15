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

#include <QtDebug>
#include "register_map.hxx"


RegisterMap::RegisterMap(QWidget* parent, AMSCommunication *com, QString filename) :
    settingsDockEdit(NULL)
{
	useVerify = false;
	this->com = com;
	try
    {
		regXml = new RegisterXml(".", filename);
    }
    catch(QString msg)
    {
        regXml = NULL;
		throw msg;
    }
	readMutex = new QMutex();
	readOnceRegs = regXml->getReadOnce();
    regTable = new RegisterTable(regXml, this);
    this->regXml = regXml;
    setCentralWidget(regTable);
    setWindowTitle(tr("Register Map")); 
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint);

    createActions();
    createMenus();

    changeValueBase(); // Change default value base to hex
	/* connect to the communication class dataChanged event */
	connect(com, SIGNAL(dataChanged(unsigned char, unsigned char, bool, unsigned char)), this,
			SLOT(onDataChanged(unsigned char, unsigned char, bool, unsigned char)));
	connect(regTable, SIGNAL(itemPressed(QTableWidgetItem*)), this, SLOT(onItemPressed(QTableWidgetItem*)));

	if(parent != 0)
	{
	    connect(parent, SIGNAL(modeChanged(bool)), this, SLOT(onModeChanged(bool)));
	}

    if (regXml->isValueEditable())
        connect(regTable, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(onItemDoubleClick(QTableWidgetItem*)));
	
	connect(this, SIGNAL(registerChanged( QString, unsigned char, bool, unsigned char)), this, 
            SLOT(onUpdateThreadChanged(QString, unsigned char, bool, unsigned char)), Qt::QueuedConnection);

	lastCallingItem = 0;
	readSettings();
    setMinimumHeight(200);
    setMinimumWidth(200);
	this->resize(this->regTable->width(),600);
	connect(regTable, SIGNAL(cellChanged(int, int)), this, SLOT(onItemChanged(int,int)));
}

RegisterMap::~RegisterMap()
{
    delete regTable;

    if (settingsDockEdit != NULL)
        delete settingsDockEdit;
}

void RegisterMap::closeEvent(QCloseEvent *event)
{
	this->closeWindow();
	event->accept();
}

void RegisterMap::closeWindow()
{
    writeSettings();
    close();
}

void RegisterMap::writeSettings()
{
    QSettings settings("austriamicrosystems", "AFE GUI");

    settings.beginGroup("RegMap_Window");
    settings.setValue("size", size());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("visible", isVisible());
    settings.endGroup();
}

void RegisterMap::readSettings()
{
    QSettings settings("austriamicrosystems", "AFE GUI");

    settings.beginGroup("RegMap_Window");
    resize(settings.value("size", QSize(regTable->tableWidth(), regTable->tableHeight())).toSize());
    restoreGeometry(settings.value("geometry").toByteArray()); 
    //setVisible(settings.value("visible", false).toBool());
    settings.endGroup();
}

void RegisterMap::onItemPressed(QTableWidgetItem* item)
{
	if (!item->flags().testFlag(Qt::ItemIsSelectable) || !item->flags().testFlag(Qt::ItemIsEnabled))
        return; // Bit not used or read-only and cannot be changed

    int bitIndex = 8 - item->column();
    int tableRow = item->row();
    toggleValue(item, tableRow);

    // Write value to the AFE
    unsigned char value = regTable->getRowValue(tableRow);
    Register reg = regXml->getRegister(tableRow);
	AMSCommunication::Error error;
    Control* ctrl = reg.getControl();

    if (ctrl != NULL)
    {
        this->com->setControlRegisterAddress(ctrl->getRegister(), ctrl->mask());
		error = this->com->writeSubRegister(reg.addressNumber(), value, reg.getSubAddress());
    }
    else
    {
		error = this->com->writeRegister(reg.addressNumber(), value,useVerify);
	}

    if (error)
    {
        // Toggle back row value
        toggleValue(item, tableRow);
        return; // Error message was displayed by AFE_Interface
    }

    // Notify others (GUI, logger,...) that register value has changed
    emit registerChanged(reg.getAddress(), value, ctrl, reg.getSubAddress());
}

void RegisterMap::toggleValue(QTableWidgetItem* item, int tableRow)
{
    // Toggle bit value
    if (item->text() == "0")
        item->setText("1");
    else
        item->setText("0");

    regTable->refreshRowValue(tableRow);
}

void RegisterMap::onModeChanged(bool on)
{
    if (on) // Superuser mode
    {
        if (settingsDockEdit != NULL) // If not deleted and created again, on/off wont work more then once
            delete settingsDockEdit;

        settingsDockEdit = new RegisterSettingsDock(this);
        addDockWidget(Qt::BottomDockWidgetArea, settingsDockEdit);
    }
    else // Normal mode
    {
        removeDockWidget(settingsDockEdit);

        delete settingsDockEdit;
        settingsDockEdit = NULL;
    }
}

void RegisterMap::createActions()
{
    readOnceAct = new QAction(tr("&Readout Registers"), this);
    readOnceAct->setShortcut(tr("Ctrl+R"));
    connect(readOnceAct, SIGNAL(triggered()), this, SLOT(readOnce()));

    hexDecAct = new QAction(tr("&Toggle Value Hex/Dec"), this);
    hexDecAct->setShortcut(tr("Ctrl+V"));
    hexDecAct->setCheckable(true);
    hexDecAct->setChecked(true);
    connect(hexDecAct, SIGNAL(triggered()), this, SLOT(changeValueBase()));
}

void RegisterMap::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(readOnceAct);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(hexDecAct);
}

void RegisterMap::changeValueBase()
{
    hexDecAct->setChecked(hexDecAct->isChecked());
    regTable->changeValueBase();
}


AMSCommunication::Error RegisterMap::modifyRegister(unsigned char address, unsigned char mask, unsigned char value)
{
	return com->modifyRegister(address, mask, value, useVerify);
}

void RegisterMap::onUpdateThreadChanged(QString regAddress, unsigned char regValue, bool isSubRegister, unsigned char regSubaddress)
{
  	QString fullAddr = regAddress;

    if (isSubRegister)
        fullAddr += "-" + QString::number(regSubaddress);

    unsigned short rowIndex = regXml->getRegIndex(fullAddr);
    if (rowIndex == (unsigned short) -1) // Could happen if multiplexed reg. addresses do not have notupdate="1" attribute in register map xml file.
        return; // This is a signal response method, not the best place to put interactive message box warning, so go ahead now.

    regTable->changeValue(rowIndex, regValue);
}

void RegisterMap::onItemDoubleClick(QTableWidgetItem* item)
{
    if (item->column() == VALUE_COLUMN_NO)
    {
        Register reg = regXml->getRegister(item->row());

        if (!reg.isReadOnly())
        {
            bool ok;
            int value = QInputDialog::getInteger(this, tr("Edit Register Value"),
                            tr("Register value:"), item->text().toInt(), 0, 255, 1, &ok);
            if (ok)
                changeRegisterValue(reg, value);
        }
    }
}

bool RegisterMap::changeRegisterValue(Register& reg, unsigned char value)
{   
    // Check does user is trying to change read-only bits and mask them first
    unsigned char mask = getReadOnlyMask(reg);
    bool status = false;
    Control* ctrl = reg.getControl();

    if (ctrl != NULL)
    {
        if (mask != 0xff)
        {
			this->com->setControlRegisterAddress(ctrl->getRegister(), ctrl->mask());
			this->com->modifySubRegister(reg.addressNumber(), mask, value, reg.getSubAddress());
        }
        else
        {
			this->com->setControlRegisterAddress(ctrl->getRegister(), ctrl->mask());
			this->com->writeSubRegister(reg.addressNumber(), value, reg.getSubAddress());
        }
    
    }
    else
    {
        if (mask != 0xff)
		{
            this->com->modifyRegister(reg.addressNumber(), mask, value, useVerify);
		}
        else
		{
            this->com->writeRegister(reg.addressNumber(), value, useVerify);
		}
    }

    if (!status)
        return false;

    regTable->changeValue(regXml->getRegIndex(reg.getAddress()), value);
    emit registerChanged(reg.getAddress(), value, ctrl, reg.getSubAddress());
    return status;
}

unsigned char RegisterMap::getReadOnlyMask(Register& reg)
{
    unsigned char mask = 0xff;

    for (int i=0; i<reg.bitsNo(); i++)
    {
        if (reg.getBit(i)->readOnly())
            mask &= BITMASK0[i];
    }

    return mask;
}

bool RegisterMap::writeFromRegSettings(QString& address, unsigned char value)
{
    Register reg = regXml->getRegister(regXml->getRegIndex(address));

    if (reg.isReadOnly())
        return false;

    return changeRegisterValue(reg, value);
}

bool RegisterMap::isValidRegAddress(QString& address)
{
    return regXml->isValidAddress(address);
}

void RegisterMap::showEvent(QShowEvent* event)
{
    // If setMaximumWidth() would be called in constructor, width won't be accurate
    setMaximumWidth(regTable->tableWidth());
	/* initial read out all registers */
	readOnce();
}

void RegisterMap::onGuiChanged(unsigned short address, unsigned char subAddress)
{
    unsigned char value;
    QString addr = QString::number(address);
    bool status = false;
	AMSCommunication::Error error;

    if (subAddress != 0)
    {
        addr += "-" + QString::number(subAddress);
        Control* ctrl = regXml->getControl(regXml->getRegIndex(addr));
            
        if (ctrl != NULL)
        {
            this->com->setControlRegisterAddress(ctrl->getRegister(), ctrl->mask());
			error = this->com->readSubRegister(address, &value, subAddress);
        }
    }
    else
    {
        // Read the register value
		error = com->readRegister(address, &value);
    }

    if (status)
        regTable->changeValue(regXml->getRegIndex(addr), value);
}

void RegisterMap::readOnce(bool doemit)
{
	unsigned char regValue;
    QString regAddress;
		
	if(this->com->isConnected() == false)
		return;

	if (!readMutex->tryLock())
		return;

	for (int i=0; i<readOnceRegs.size(); i++)
    {
        RegisterXml::ReadOnce regStruct = readOnceRegs.at(i);
        regAddress = QString::number(regStruct.reg.address);
		
        if (regStruct.isSubRegister) // Change control register value
        {
            regAddress += "-" + QString::number(regStruct.subAddress);
			Control* ctrl = regXml->getControl(regXml->getRegIndex(regAddress));
			com->setControlRegisterAddress(ctrl->getRegister(), ctrl->mask());
			com->readSubRegister(regStruct.reg.address, &regValue, regStruct.subAddress);
            regAddress = QString::number(regStruct.reg.address);
        }
        else
		{         
			this->com->readRegister(regStruct.reg.address, &regValue,doemit);
		}
        
		emit registerChanged(regAddress, regValue, regStruct.isSubRegister, regStruct.subAddress);
	}
	readMutex->unlock();
}

void RegisterMap::onDataChanged(unsigned char reg, unsigned char subreg, bool isSubreg, unsigned char val)
{
	emit registerChanged(QString::number(reg, 10), val, isSubreg, subreg);
}

void RegisterMap::onItemChanged(int row, int column)
{
	bool ok;
	unsigned char itemValue = 0;

	QTableWidgetItem * item = NULL;

	if (column != 9)
	{
		lastCallingItem = column;
		return;
	}

	item = regTable->item(row,column);
	if (NULL == item)
		return;
	if(lastCallingItem != column) // in this case a bit was changed, the value is already up-to-date
	{
		lastCallingItem = column;
		return;
	}
	regTable->blockSignals(true);
	
	if (!item->text().contains("0x") && hexDecAct->isChecked())
	{
		item->setText(QString("0x%1").arg(item->text()));
		itemValue = item->text().toUShort(&ok,16);
	}
	else
		itemValue = item->text().toUShort(&ok,10);

    QString bitString = QString::number(itemValue, 2); // convert to binary
    int size = bitString.size();

	for(int i=0; i<8-size; i++)
        bitString.insert(0, '0');
    
    for (int i=0; i<8; i++)
    {
        // Update value also for read-only cells (with Qt::lightGray background)
        if (regTable->item(row, i+1)->flags().testFlag(Qt::ItemIsSelectable))
            regTable->item(row, i+1)->setText(bitString.at(i));
		else
		{
			itemValue = (itemValue & (~(1 <<(8-i-1))));
		}
    }

	if (hexDecAct->isChecked())
		item->setText("0x" + QString::number(itemValue, 16));
	else
	    item->setText(QString::number(itemValue));

	regTable->blockSignals(false);
	lastCallingItem = 9;
	unsigned short regValue = regTable->item(row,0)->text().toUShort(&ok,16);
	this->com->blockSignals(true);
	this->com->writeRegister(regValue,itemValue,useVerify);
	this->com->blockSignals(false);
}

void RegisterMap::setVerify(bool on)
{
	useVerify = on;
}