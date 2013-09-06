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
 * \file register_settings.h
 * 
 * \author Vitomir Spasojevic
 *
 * \brief  Register settings widget used in superuser mode.
 *
 * Basically this is the text edit box for editing AFE register commands.
 * Clicking on a send button, will parse and,
 * if no errors found, try to execute these commands.
 */

#ifndef REGISTER_SETTINGS_H
#define REGISTER_SETTINGS_H

#include <QtGui>

class RegisterMap;

//! Small wrapper class around the RegisterSettings class.
//! Only reason to have such wrapper is that without reimplementing
//! sizeHint() method by the RegisterSettings, size of the widget cannot be set.
class RegisterSettingsDock : public QDockWidget
{
    Q_OBJECT

public:
    RegisterSettingsDock(RegisterMap* parent); 

private:
};

//! Register settings window class. 
/*!
  This class is extension of QDockWidget class responsible for superuser mode
  register command. This widget contains of two tables - one for editing register commands
  and the other as command history. There is a send button for executing command sequence from the text box.
  Commands are parsed and if no error occurs, they are executed.
  Commands can be saved to the file and loaded back.
*/ 
class RegisterSettings : public QWidget
{
    Q_OBJECT

public:
    RegisterSettings(RegisterMap* regMap, QWidget* parent = 0);
    QSize sizeHint() const;

private slots:
    void addBtnReleased();
    void sendBtnReleased();
    void loadBtnReleased();
    void saveBtnReleased();

private:
    //! Register command representation structure.
    struct Command
    {
        QString address;
        QString value;
    };

    //! Check does input value is in allowed boundaries between 0 and 255.
    //! \param value Value to check.
    //! \return If valid, this is a decimal value string or empty string for invalid value.
    QString checkValue(QString& value);
    //! Check does input address value is existing register address.
    //! \param address Address value to check.
    //! \return If valid, this is a decimal address string or empty string for invalid value.
    //! If address is for multiplexed register (17-1), return value is without the subaddress (17).
    QString checkAddress(QString& address);
    //! Inserts a new row to the register settings table.
    //! \param table Pointer to the register settings table.
    //! \param address Value for the address column of the new row.
    //! \param value Value for the value column of the new row.
    void insertTable(QTableWidget* table, QString& address, QString& value);
    //! Remove all the rows from the table.
    void emptyTable(QTableWidget* table);
    //! Save contents of the history table to the XML file.
    bool save(QFile& file);
    //! Load register commands from the XML file to the register settings table.
    bool load(QFile& file);

    QTableWidget* historyTable;
    QTableWidget* settingsTable;
    QPushButton *buttonSave;
    RegisterMap* regMap;
};

#endif // REGISTER_SETTINGS_H
