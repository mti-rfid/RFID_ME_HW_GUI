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

#include <math.h>

#include "register_table.h"

const int TABLE_COLUMN_WIDTH = 36;
const int TABLE_ROW_HEIGHT = 24;
const int ADDRESS_COLUMN_NO = 0;

RegisterTable::RegisterTable(RegisterXml* regXml, QWidget *parent) :
    isHexValue(false)
{
    this->regXml = regXml;

    setColumnCount(10);

    QStringList tableHeaders;
    tableHeaders.append("Addr.");
    tableHeaders.append("7");
    tableHeaders.append("6");
    tableHeaders.append("5");
    tableHeaders.append("4");
    tableHeaders.append("3");
    tableHeaders.append("2");
    tableHeaders.append("1");
    tableHeaders.append("0");
    tableHeaders.append("Value");
    setHorizontalHeaderLabels(tableHeaders);

    int regNo = regXml->registersNo();
    setRowCount(regNo);

    // For all registers
    for (int i=0; i<regNo; i++)
    {
        Register reg = regXml->getRegister(i);

        QTableWidgetItem* regHeader = new QTableWidgetItem(reg.getName());
        regHeader->setToolTip(reg.getTooltip());
        setVerticalHeaderItem(i, regHeader); 

        QTableWidgetItem* address = new QTableWidgetItem(reg.getAddress());
        address->setFlags(Qt::ItemIsEnabled);
        address->setTextAlignment(Qt::AlignCenter);

        setItem(i, 0, address);

        setRegister(reg, i);

        // Value column
        QTableWidgetItem* value = new QTableWidgetItem("0");
//        if (regXml->isValueEditable())
            value->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
   //     else
			//value->setFlags(Qt::ItemIsEnabled);
        value->setTextAlignment(Qt::AlignCenter);
        setItem(i, VALUE_COLUMN_NO, value);

        setRowHeight(i, TABLE_ROW_HEIGHT);
    }

    setColumnWidth(0, TABLE_COLUMN_WIDTH + 8); // Address field is more width then other columns
    setColumnWidth(1, TABLE_COLUMN_WIDTH);
    setColumnWidth(2, TABLE_COLUMN_WIDTH);
    setColumnWidth(3, TABLE_COLUMN_WIDTH);
    setColumnWidth(4, TABLE_COLUMN_WIDTH);
    setColumnWidth(5, TABLE_COLUMN_WIDTH);
    setColumnWidth(6, TABLE_COLUMN_WIDTH);
    setColumnWidth(7, TABLE_COLUMN_WIDTH);
    setColumnWidth(8, TABLE_COLUMN_WIDTH);
    setColumnWidth(9, TABLE_COLUMN_WIDTH);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

RegisterTable::~RegisterTable()
{
}

void RegisterTable::setRegister(Register& reg, int row)
{
    QTableWidgetItem* bitItem;
    int bitNo = reg.bitsNo();
   
    // For all bits
    for (int j=0; j<8; j++)
    {
        Bit* bit = reg.getBit(j);

        if (bit != NULL && bit->used())
        {
            bitItem = new QTableWidgetItem("0");

            Qt::ItemFlags flags = Qt::ItemIsEnabled;
            if (!bit->readOnly())
                flags |= Qt::ItemIsSelectable;
            else
                bitItem->setBackground(QBrush(Qt::lightGray));
            bitItem->setFlags(flags);
            bitItem->setToolTip(bit->getTooltip());
            bitItem->setTextAlignment(Qt::AlignCenter);
        }
        else // Bit not used
        {
            bitItem = new QTableWidgetItem("");
            bitItem->setFlags(Qt::ItemIsEnabled);
        }

		setItem(row, 8-j, bitItem); // 8-j is because of bit-0 has column index 8, bit-1 column 7 and so on
    }
}

int RegisterTable::tableWidth()
{
    int width = 0;
    for (int i=0; i<columnCount(); i++)
		width += columnWidth(i);
    return (width + verticalHeader()->width() + 44); // Fixed 24 points is for the scrollbar
}

int RegisterTable::tableHeight()
{
    int heightOfAllRows = 0;

    for (int i = 0; i < rowCount(); i++)
      heightOfAllRows += rowHeight(i);

    return horizontalHeader()->height() + heightOfAllRows;
}

int RegisterTable::getRowValue(int row)
{
    QTableWidgetItem* valueItem = item(row, VALUE_COLUMN_NO);
    bool ok;
    int result;

    if (isHexValue)
        result = valueItem->text().toInt(&ok, 16);
    else // dec value
        result = valueItem->text().toInt(&ok);
    
    return ok ? result : -1;
}

void RegisterTable::changeValueBase()
{
    isHexValue = !isHexValue;
    
    for (int i=0; i<regXml->registersNo(); i++) // Refresh value display
    {
        refreshRowValue(i); 
        refreshRowAddress(i);
    }
}

void RegisterTable::refreshRowAddress(int row)
{
    QString address = item(row, ADDRESS_COLUMN_NO)->text();
    QString extension;
    int minusSign;

    if ((minusSign = address.indexOf('-')) != -1)
    {
        extension = address.mid(minusSign);
        address = address.left(minusSign);
    }

    bool ok;

    if (isHexValue)
        item(row, ADDRESS_COLUMN_NO)->setText("0x" + QString::number(address.toInt(&ok), 16) + extension);
    else
        item(row, ADDRESS_COLUMN_NO)->setText(QString::number(address.toInt(&ok, 16)) + extension);
}

void RegisterTable::refreshRowValue(int row)
{
    QTableWidgetItem* valueItem = item(row, VALUE_COLUMN_NO);
    QTableWidgetItem* currItem;
    QString bit;
    int value = 0;
    int bitWeight = 128; // Weight for most significant bit in the byte

    for (int i=1; i<=8; i++)
    {
        currItem = item(row, i);

        if (currItem != NULL)
        {
            bit = item(row, i)->text();
            
            if (bit != "" && bit.toInt() == 1)
                value += bitWeight;
        }

        bitWeight /= 2;
    }

    if (isHexValue)
        item(row, VALUE_COLUMN_NO)->setText(""+QString("0x%1").arg(value,2, 16,QLatin1Char('0')));
    else
        item(row, VALUE_COLUMN_NO)->setText(QString::number(value));
}

void RegisterTable::changeValue(int row, int value)
{
    QString bitString = QString::number(value, 2);
    
    int size = bitString.size();
    for(int i=0; i<8-size; i++)
        bitString.insert(0, '0');
    
    for (int i=0; i<8; i++)
    {
        // Update value also for read-only cells (with Qt::lightGray background)
        if (item(row, i+1)->flags().testFlag(Qt::ItemIsSelectable) || item(row, i+1)->background().color() == Qt::lightGray)
            item(row, i+1)->setText(bitString.at(i));
    }

    // Refresh row value 
    if (isHexValue)
		item(row, VALUE_COLUMN_NO)->setText(""+QString("0x%1").arg(value,2, 16,QLatin1Char('0')));
    else
        item(row, VALUE_COLUMN_NO)->setText(QString::number(value));
}

