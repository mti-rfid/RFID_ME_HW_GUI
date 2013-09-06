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
 * \file register_map.h
 * 
 * \author Vitomir Spasojevic
 *
 * \brief  Register map main window.
 *
 * This file contains register map class declaration for the register map main window.
 * Central window widget is register map table but window may contain also additional text
 * field to write down register settings (in superuser mode).
 */

#ifndef REGISTER_MAP_H
#define REGISTER_MAP_H

#include <QMainWindow>

#include "register_xml.h"
#include "register_table.h"
#include "register_settings.hxx"
#include "AMSCommunication.hxx"



//! Register map main window class. 
/*!
  This class is extension of QMainWindow class responsible for events
  in register map window.
*/
class RegisterMap : public QMainWindow
{
	Q_OBJECT

public:
	RegisterMap(QWidget *parent = 0, AMSCommunication *com = 0, QString filename = "" );
	~RegisterMap();

    //! Saves window state and closes itself.
    void closeWindow();
    //! This method is used by RegisterSettings window to write a value to the register.
    //! \param address Register address to be changed.
    //! \param value Register value to be written to the \a address.
    //! \return none
    bool writeFromRegSettings(QString& address, unsigned char value);
    //! Checks if register address exists in the register map XML file.
    //! \param address Register address to be checked.
    //! \return True if exists, false otherwise.
    bool isValidRegAddress(QString& address);
    //! En/disables verify option for write command
    //! \param on set required mode
    //! \return none
	void setVerify(bool);
protected:
	void closeEvent(QCloseEvent *event);
	AMSCommunication *com;
	QVector<RegisterXml::ReadOnce> readOnceRegs; //!< Holds register information for read-once operation.
	RegisterXml* regXml;
	//! Toggle register bit value in the register map table.
    //! \param item Table item which should be changed.
    //! \param tableRow Register map table row which contains the \a item.
    void toggleValue(QTableWidgetItem* item, int tableRow);
	RegisterTable*			regTable; //!< Table widget for handling main register map user interaction.
	bool					useVerify;

signals:
    //! Signal emitted when register value has changed by user interaction with register map window.
    void registerChanged(QString regAddress, unsigned char regValue, bool isSubRegister, unsigned char regSubAddress);
    //! Signal emitted on menu readout register option activated.
    void readoutRegisters();

public slots:
    //! Slot called when register map table item has pressed.
    virtual void onItemPressed(QTableWidgetItem* item);
    //! Slot called when register map table item has pressed.
    void onItemChanged(int,int);
    //! Slot called when mode changed from/to super-user.
    void onModeChanged(bool on);
    //! Slot called when update thread detects register changes.
    void onUpdateThreadChanged(QString regAddress, unsigned char regValue, bool isSubRegister, unsigned char regSubaddress);
    //! Slot called when register map item is double-clicked.
    void onItemDoubleClick(QTableWidgetItem* item);
    //! Slot called when register has been changed by main GUI window and register map window should be updated.
    void onGuiChanged(unsigned short address, unsigned char subAddress);
    //! Slot called to trigger a readout of all registers once
    virtual void readOnce(bool doemit = true);

private slots:
    //! Slot called when number base changed from/to hex/dec.
    void changeValueBase();
    void showEvent(QShowEvent* event);
	virtual void onDataChanged(unsigned char reg, unsigned char subreg, bool isSubreg, unsigned char val);

private:
    void writeSettings(); //!< Writes window settings to the application settings file.
    void readSettings(); //!< Read window settings from the application settings file.
    void createActions();
    void createMenus();

    //! Writes value to the chip register.
    //bool writeRegister(unsigned char address, unsigned char value);
    //! Modify chip register value.
	AMSCommunication::Error modifyRegister(unsigned char address, unsigned char mask, unsigned char value);
    //! Writes new value to the register. This is used if register has some read-only bits which should be masked.
    //! \param reg Register to be changed.
    //! \param value New register value.
    //! \return True on change success, false otherwise.
    bool changeRegisterValue(Register& reg, unsigned char value);
    //! Returns register bit mask which has zeros for read-only bits.
    unsigned char getReadOnlyMask(Register& reg);

    QMenu*					fileMenu;
    QMenu*					viewMenu;
    QAction*				hexDecAct; //!< Action for changing number base from/to hex/decimal.
    QAction*				readOnceAct; //!< Action for reading once all the registers.
	QMutex*					readMutex;
    RegisterSettingsDock*	settingsDockEdit; //!< RegisterSettings docking window used in the super-user mode.
	unsigned int			lastCallingItem;
};

#endif // REGISTER_MAP_H
