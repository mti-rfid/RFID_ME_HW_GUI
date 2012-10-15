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
 * \file register_table.h
 * 
 * \author Vitomir Spasojevic
 *
 * \brief  Register map table widget.
 *
 * Register table is a table widget component initialized directly from
 * XML register map data. Each table row represents one AFE register.
 * Columns are register name, register address, bit value for every register
 * bit and register value column in hex or decimal representation.
 */

#ifndef REGISTER_TABLE_H
#define REGISTER_TABLE_H

#include <QtGui>
#include <QTableWidget>
#include "register_xml.h"

const int VALUE_COLUMN_NO = 9; //!< Register table column index for the register value. This is the last column in the table.

//! Register map table widget class. 
/*!
  Inherited from QTableWidget, this class is responsible for displaying register
  data in table form and user interaction with a table.
*/
class RegisterTable : public QTableWidget
{
public:
	RegisterTable(RegisterXml* regXml, QWidget *parent = 0);
	~RegisterTable();

    //! Returns table width value.
    int tableWidth();
    //! Returns table height value.
    int tableHeight();

    //! Returns row value column in decimal notation.
    //! \param row Register map table row number.
    //! \return Row value in decimal notation.
    int getRowValue(int row);
    //! Calculates new row value from the row bit items.
    void refreshRowValue(int row);
    //! Changes number value base from/to dec/hex.
    //! Address and value row fields are refreshed to display value in new number base.
    void changeValueBase();
    //! Change value for the specified row. Bit fields are update also according to the new value.
    //! \param row Row to be changed.
    //! \param value New value for the \a row.
    void changeValue(int row, int value);

private:
    //! Calculates row address in the new number base.
    void refreshRowAddress(int row);
    void setRegister(Register& reg, int row);

    RegisterXml* regXml;
    bool isHexValue; //!< True for hex register value representation, false for a decimal.
};

#endif // REGISTER_TABLE_H
