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

#include <QDomDocument>
#include <QDomElement>

#include "register_map.hxx"
#include "register_settings.hxx"

const int TABLE_ROW_HEIGHT = 22;
const char* const XML_ROOT_NAME = "register_commands";

RegisterSettingsDock::RegisterSettingsDock(RegisterMap* regMap) :
    QDockWidget(regMap)
{
    setWindowTitle(tr("Register Commands"));

    RegisterSettings* widget = new RegisterSettings(regMap, this);
    setWidget(widget);
    setFeatures(QDockWidget::NoDockWidgetFeatures);
}

RegisterSettings::RegisterSettings(RegisterMap* regMap, QWidget* parent) :
    regMap(regMap)
{
    historyTable = new QTableWidget(this);
    historyTable->setMinimumHeight(40);
    historyTable->setColumnCount(2);
    historyTable->setColumnWidth(0, 100);
    historyTable->setColumnWidth(1, width() - 290);
    historyTable->setSelectionMode(QAbstractItemView::NoSelection);

    QStringList tableHeaders;
    tableHeaders.append("Address");
    tableHeaders.append("Value");
    historyTable->setHorizontalHeaderLabels(tableHeaders);
    historyTable->setRowHeight(0, TABLE_ROW_HEIGHT);

    QPalette p = historyTable->palette();
    p.setColor(QPalette::Base, QColor (Qt::lightGray));
    historyTable->setPalette(p);

    settingsTable = new QTableWidget(this);
    settingsTable->setMinimumHeight(40);
    settingsTable->setColumnCount(2);
    settingsTable->setColumnWidth(0, 100);
    settingsTable->setColumnWidth(1, width() - 290);
    settingsTable->setRowCount(1);
    settingsTable->setHorizontalHeaderLabels(tableHeaders);
    settingsTable->setRowHeight(0, TABLE_ROW_HEIGHT);

    QTableWidgetItem* defaultItem = new QTableWidgetItem("0");
    defaultItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
    settingsTable->setItemPrototype(defaultItem);

    QPushButton *buttonAdd = new QPushButton("&Add Command", this);
    QPushButton *buttonLoad = new QPushButton("&Load", this);
    QPushButton *buttonSend = new QPushButton("&Send", this);
    buttonSave = new QPushButton("Sa&ve", this);
    buttonSave->setDisabled(true);

    QSplitter *splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(historyTable);
    splitter->addWidget(settingsTable);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    QHBoxLayout* hLayout = new QHBoxLayout(this);
    vLayout->addWidget(splitter);
    hLayout->addWidget(buttonAdd);
    hLayout->addStretch();
    hLayout->addWidget(buttonLoad);
    hLayout->addWidget(buttonSave);
    hLayout->addWidget(buttonSend);
    vLayout->addLayout(hLayout);
    setLayout(vLayout);

    connect(buttonAdd, SIGNAL(released()), this, SLOT(addBtnReleased()));
    connect(buttonSend, SIGNAL(released()), this, SLOT(sendBtnReleased()));
    connect(buttonLoad, SIGNAL(released()), this, SLOT(loadBtnReleased()));
    connect(buttonSave, SIGNAL(released()), this, SLOT(saveBtnReleased()));
}

QSize RegisterSettings::sizeHint() const
{
    return QSize(size().width(), 250);
}

void RegisterSettings::addBtnReleased()
{
    int row = settingsTable->rowCount();
    settingsTable->insertRow(row);
    settingsTable->setRowHeight(row, TABLE_ROW_HEIGHT);
}

void RegisterSettings::insertTable(QTableWidget* table, QString& address, QString& value)
{
    int row = table->rowCount();
    table->insertRow(row);
    table->setRowHeight(row, TABLE_ROW_HEIGHT);

    QTableWidgetItem* addressItem = new QTableWidgetItem(address);
    table->setItem(row, 0, addressItem);

    QTableWidgetItem* valueItem = new QTableWidgetItem(value);
    table->setItem(row, 1, valueItem);
}

void RegisterSettings::sendBtnReleased()
{
    QVector<Command> cmds;
    Command currCommand;
    int rowsNo = settingsTable->rowCount();

    // First, check values for all the commands
    for (int i=0; i<rowsNo; i++)
    {
        QTableWidgetItem* addrItem = settingsTable->item(i, 0);
        QTableWidgetItem* valueItem = settingsTable->item(i, 1);

        if ((addrItem == NULL && valueItem == NULL) ||
            ((addrItem != NULL && addrItem->text() == "") && (valueItem != NULL && valueItem->text() == ""))) 
        {
            // Empty lines are ignored and will be deleted later
            continue;
        }

        if (addrItem == NULL || addrItem->text() == "")
        {
            QMessageBox::critical(this, tr("Register settings error"), tr("Address not set in line %1").arg(QString::number(i)));
            return;
        }
        else if (valueItem == NULL || valueItem->text() == "")
        {
            QMessageBox::critical(this, tr("Register settings error"), tr("Value not set in line %1").arg(QString::number(i)));
            return;
        }

        QString addr = checkAddress(addrItem->text());
        QString value = checkValue(valueItem->text());

        if (addr.isEmpty())
        {
            QMessageBox::critical(this, tr("Register settings error"), tr("Wrong address '%1' in line %2!").
                arg(settingsTable->item(i, 0)->text()).arg(QString::number(i)));
            return;
        }
        else if (value.isEmpty())
        {
            QMessageBox::critical(this, tr("Register settings error"), tr("Wrong value '%1' in line %2!").
                arg(settingsTable->item(i, 1)->text()).arg(QString::number(i)));
            return;
        }

        currCommand.address = addr;
        currCommand.value = value;
        cmds.append(currCommand);
    }

    // Remove empty lines
    bool hasEmpty;
    do
    {
        hasEmpty = false;
        for (int i=0; i<settingsTable->rowCount(); i++)
        {
            QTableWidgetItem* addrItem = settingsTable->item(i, 0);
            QTableWidgetItem* valueItem = settingsTable->item(i, 1);

            if ((addrItem == NULL && valueItem == NULL) ||
                (addrItem->text() == "" && valueItem->text() == ""))
            {
                settingsTable->removeRow(i);
                hasEmpty = true;
                break;
            }
        }
    }
    while (hasEmpty);

    if (!cmds.empty())
    {
        // Execute commands
        for (int i=0; i<cmds.size(); i++)
        {
            if (!regMap->writeFromRegSettings(cmds[i].address, cmds[i].value.toInt()))
            {
                //break;
            }
        }

        rowsNo = settingsTable->rowCount();
        // Move commands to the history
        for (int i=0; i<rowsNo; i++)
        {
            QTableWidgetItem* addrItem = settingsTable->item(0, 0);
            QTableWidgetItem* valueItem = settingsTable->item(0, 1);

            if (addrItem != NULL && valueItem != NULL)
            {
                insertTable(historyTable, addrItem->text(), valueItem->text());
                settingsTable->removeRow(0);
            }
        }

        buttonSave->setEnabled(true);
    }

    addBtnReleased(); // Add an empty row at the end
}

void RegisterSettings::loadBtnReleased()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Register Commands Load"),
													QDir::currentPath(), tr("XML Files (*.xml)"));
	if (fileName.isEmpty())
		return;
	
	QFile file(fileName);

	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::warning(this, tr("Register Commands Load"), tr("Cannot read file %1: \n%2.")
							.arg(fileName).arg(file.errorString()));
		return;
	}
	
    load(file); // Error message displayed in load() method
}

void RegisterSettings::saveBtnReleased()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Register Commands Save"),
				    								QDir::currentPath(), tr("XML Files (*.xml)"));

	if (fileName.isEmpty())
		return;
	
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
		QMessageBox::warning(this, tr("Register Commands Save"), tr("Cannot write file %1:\n%2.")
							.arg(fileName).arg(file.errorString()));
		return;
	}

	if (save(file))
	{
        QMessageBox::information(this, tr("Register Commands Save"), tr("File succsessfully saved"));

        // Empty history list
        emptyTable(historyTable);
        buttonSave->setDisabled(true);
	}
}

void RegisterSettings::emptyTable(QTableWidget* table)
{
    int rowsNo = table->rowCount();
    for (int i=0; i<rowsNo; i++)
    {
        table->removeRow(0);
    }
}

bool RegisterSettings::save(QFile& file)
{
	const int IndentSize = 4;
	QTextStream out(&file);
    QDomDocument doc;
    QDomElement root = doc.createElement(XML_ROOT_NAME); 
    doc.appendChild(root);
    QDomElement cmdElem;
    QDomText cmdElemText;

    for (int i=0; i<historyTable->rowCount(); i++)
    {
        cmdElem = doc.createElement("command");
        cmdElem.setAttribute("address", historyTable->item(i, 0)->text());
        cmdElemText = doc.createTextNode(historyTable->item(i, 1)->text());
        cmdElem.appendChild(cmdElemText);
        root.appendChild(cmdElem);
    }

	doc.save(out, IndentSize);
    return true;
}

bool RegisterSettings::load(QFile& file)
{
    QString errorStr;
	int errorLine, errorColumn;
    QDomDocument doc;

	if (!doc.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
	{
		QMessageBox::information(window(), tr("Register Commands Load"),
			tr("Parse error at line %1, column %2:\n%3")
			.arg(errorLine).arg(errorColumn).arg(errorStr));
		return false;
	}

    QDomElement root = doc.documentElement();
	if (root.tagName() != XML_ROOT_NAME)
    {
     	QMessageBox::information(window(), tr("Register Commands Load"), tr("The file is not a register commands file!"));
		return false;
	}

    QDomNodeList commandNodes = root.childNodes();
    QVector<Command> cmds;
    Command newCmd;

    for (int i=0; i<commandNodes.size(); i++)
    {
        QDomElement elem = commandNodes.item(i).toElement();
        newCmd.address = elem.attribute("address");
        newCmd.value = elem.text();
        cmds.append(newCmd);
    }

    emptyTable(settingsTable);
    
    for (int i=0; i<cmds.size(); i++)
        insertTable(settingsTable, cmds[i].address, cmds[i].value);

    return true;
}

QString RegisterSettings::checkValue(QString& value)
{
    int result;
    bool ok;

    if (value.startsWith("0x")) // Hex
    {
        result = value.mid(2).toInt(&ok, 16);
        if (!ok)
            return "";
    }
    else // Decimal
    {
        result = value.toInt(&ok);
        if (!ok)
            return "";
    }

    if (result > 255)
        return "";

    return QString::number(result);
}

QString RegisterSettings::checkAddress(QString& address)
{
    QString result;
    int minusIndex = address.indexOf('-');

    if (minusIndex != -1)
        result = checkValue(address.left(minusIndex)) + address.mid(minusIndex);
    else
        result = checkValue(address);

    if (!regMap->isValidRegAddress(result))
        return "";

    return result;
}
