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

#include "math.h"
#include "register_xml.h"

RegisterXml::RegisterXml(QString path, QString filename) : QDomDocument()
{
	if(filename.isEmpty())
	{
		filename = REGISTER_MAP_FILE_NAME;
	}

    QFile file(path + "/" + filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        // throw file.errorString() + " (" + QDir::currentPath() + "/" + filename + ")";
        throw "The file " + filename + " could not be found. This file is needed to describe the register map of your chip. Without this file the Register Map feature can not be used. Please contact your contact at austriamicrosystems for advice.";
    }

    if (!setContent(&file))
        throw QObject::tr("XML parse error in file: ") + QDir::currentPath() + "/" + filename + "!";

    file.close();
}

QString RegisterXml::getChipName()
{
    return documentElement().attribute("chip");
}

bool RegisterXml::isValueEditable()
{
    return documentElement().attribute("editvalue") == "1";
}

int RegisterXml::registersNo()
{
    return documentElement().firstChildElement().childNodes().size();
}

Register RegisterXml::getRegister(unsigned short regIndex)
{
    QDomElement domElement = documentElement().firstChildElement().childNodes().item(regIndex).toElement();
    QDomNodeList bits = domElement.elementsByTagName("bits").item(0).childNodes();
    QVector<Bit> bitVector(8);
	bool regRO = (domElement.attribute("ro")=="1");

    for (int i = 0; i < 8; i++) 
    { 
		Bit b(i,regRO,true,QString(""),QString(""));
        bitVector[i]= b;
	}

    for (int i = 0; i < bits.size(); i++) 
    { 
		bool ro;
        QDomElement elem = bits.item(i).toElement();
		if(elem.attribute("ro") == QString())
		{
			ro = regRO;
		}
		else
		{
			ro = (elem.attribute("ro") == "1"); 
		}
        Bit b(elem.attribute("no").toShort(),
            ro,
            elem.attribute("notused") != "1",
            elem.firstChildElement("tooltip").text(), 
            QString());//elem.firstChildElement("link").text());
        bitVector[elem.attribute("no").toShort()]= b;
    }

    return Register(domElement.attribute("address"), 
        domElement.firstChildElement("name").text(),
        domElement.firstChildElement("tooltip").text(), 
        domElement.attribute("block").toShort(), 
        bitVector,
        initControl(&domElement));
}

Control* RegisterXml::getControl(unsigned short regIndex)
{
    QDomElement domElement = documentElement().firstChildElement().childNodes().item(regIndex).toElement();
    return initControl(&domElement);
}

BitExt* RegisterXml::getLinkedBit(QString& link)
{    
    QDomElement bitDomElem = getPathElement(link, &documentElement());

    if (!bitDomElem.isNull())
    {
        BitExt* bit = new BitExt(bitDomElem.attribute("no").toShort(),
            bitDomElem.attribute("ro") == "1",
            bitDomElem.attribute("notused") != "1",
            bitDomElem.firstChildElement("tooltip").text(), 
            QString(), //bitDomElem.firstChildElement("link").text(), 
            getRegAddressFromXPath(link));
        return bit;
    }

    return NULL;
}

QDomElement RegisterXml::getPathElement(QString& path, QDomElement* currElement)
{
    QDomElement elem;
    QString subPath = path.section('/', 1);

    if (subPath.indexOf('/') != -1) // Call GetPathElement() recursively with subPath as parameter
    {
        QString currPath = path.section('/', 1, 1);
        QDomElement el = getElement(currPath, currElement);
        elem = getPathElement(subPath, &el);
    }
    else if (path.size() > 0) // Last path part
        elem = getElement(path, currElement);

    return elem;
}

QDomElement RegisterXml::getElement(QString& currPath, QDomElement* currElement)
{
    // Check does currPath has attribute value to search for
    int attrIndex = currPath.indexOf('[');
    QString attrName, attrValue; 
    QDomElement nullElem;
    nullElem.clear();

    if (attrIndex != -1) // Has attribute value
    {
        int equalIndex = currPath.indexOf('=', attrIndex + 1);
        attrName = currPath.mid(attrIndex + 2, equalIndex - attrIndex - 2); // 2 is for '[' and '@' 
        int valueIndex = currPath.indexOf('\'', equalIndex + 2);

        if (valueIndex == -1) // Syntax error! No "'" sign found!
            return nullElem;

        attrValue = currPath.mid(equalIndex + 2, valueIndex - equalIndex - 2);

        // Find xml element with attribute value
        QDomNodeList elemList = currElement->childNodes();
        for (int i=0; i<elemList.size(); i++)
        {
            QDomElement newCurrElem = elemList.item(i).toElement();

            if (newCurrElem.attribute(attrName) == attrValue)
                return newCurrElem;
        }
    }
    else // Plain path element - no attributes
        return currElement->firstChildElement(currPath);

    return nullElem;
}

QString RegisterXml::getRegAddressFromXPath(QString& link)
{
    QString address = "address";
    int addrIndex = link.indexOf(address);

    if (addrIndex != -1)
    {
        int valueIndex = link.indexOf('\'', addrIndex + address.size() + 2);
        if (valueIndex != -1)
        {
            QString addrString = link.mid(addrIndex + address.size() + 2, valueIndex - (addrIndex+address.size() + 2));
            return addrString;
        }
    }

    return "";
}

int RegisterXml::getRegIndex(QString& address)
{
    QDomNodeList nodeList = documentElement().firstChildElement().childNodes();

    for (int i=0; i<nodeList.size(); i++)
        if (nodeList.item(i).toElement().attribute("address") == address)
            return i;

    return -1; // Not found
}

QVector<unsigned short> RegisterXml::getInterruptRegs()
{
    QDomNodeList nodeList = documentElement().firstChildElement().childNodes();
    QVector<unsigned short> regs;

    for (int i=0; i<nodeList.size(); i++)
    {
        if (nodeList.item(i).toElement().attribute("interrupt") == "1")
            regs.append(nodeList.item(i).toElement().attribute("address").toShort());
    }

    return regs;
}

QVector<RegisterXml::AutoUpdate> RegisterXml::getAutoUpdateRegs()
{
    QVector<AutoUpdate> regs;
    QDomNodeList nodeList = documentElement().firstChildElement().childNodes();

    for (int i=0; i<nodeList.size(); i++)
    {
        if (nodeList.item(i).toElement().attribute("notupdate") != "1")
        {
            AutoUpdate newAutoUpdate;
            newAutoUpdate.address = addressNumber(nodeList.item(i).toElement().attribute("address"));
            newAutoUpdate.funcBlock = nodeList.item(i).toElement().attribute("block").toShort();
            regs.append(newAutoUpdate);
        }
    }

    return regs;
}

QVector<RegisterXml::ReadOnce> RegisterXml::getReadOnce()
{
    QVector<ReadOnce> regs;
    QDomNodeList nodeList = documentElement().firstChildElement().childNodes();

    for (int i=0; i<nodeList.size(); i++)
    {
        if (nodeList.item(i).toElement().attribute("interrupt") != "1" && 
			nodeList.item(i).toElement().attribute("wo") != "1") // Skip interrupt registers and write only registers
        {
            QDomElement domElement = nodeList.item(i).toElement();
            QString address = domElement.attribute("address");

            ReadOnce newReg;
            newReg.reg.funcBlock = domElement.attribute("block").toShort();

            int minusIndex = address.indexOf('-');
            if (minusIndex != -1)
            {
				newReg.isSubRegister = true;
                newReg.subAddress = address.mid(minusIndex + 1).toShort();
                newReg.control = initControl(&domElement);
            }
            else
            {
				newReg.isSubRegister = false;
                newReg.subAddress = 0xFF;
                newReg.control = NULL;
            }

            newReg.reg.address = address.left(minusIndex).toShort();
            regs.append(newReg);
        }
    }

    return regs;
}

QVector<RegisterXml::EnableInterrupt> RegisterXml::getEnableInterrupt()
{
    QVector<EnableInterrupt> regs;
    QDomNodeList nodeList = documentElement().firstChildElement().childNodes();

    for (int i=0; i<nodeList.size(); i++)
    {
        QDomElement regElem = nodeList.item(i).toElement();

        if (regElem.hasAttribute("enableinterrupt"))
        {
            EnableInterrupt newEnableInterrupt;
            newEnableInterrupt.address = addressNumber(regElem.attribute("address"));
            newEnableInterrupt.value = regElem.attribute("enableinterrupt").toShort();
            regs.append(newEnableInterrupt);
        }
    }

    return regs;
}

Control* RegisterXml::initControl(QDomElement* currRegister)
{
    QDomElement controlElem = currRegister->firstChildElement("control");

    if (controlElem.isNull()) // No control register
        return NULL;

    QDomElement reg = controlElem.firstChildElement("ctrlregister");

    QVector<unsigned short> bits;
    QDomElement elem = controlElem.firstChildElement("ctrlbits");
    if (!elem.isNull())
    {
        QString bitsStr = elem.text();

        if (bitsStr.size() != 3) // Wrong control bit syntax
            return NULL;

        for (unsigned short i=bitsStr.left(1).toShort(); i<=bitsStr.right(1).toShort(); i++)
            bits.append(i);
    }

    return new Control(reg.text().toShort(), bits);
}

unsigned short RegisterXml::addressNumber(QString& address)
{
    int minusIndex = address.indexOf('-');

    if (minusIndex != -1)
        return address.left(minusIndex).toShort();

    return address.toShort();
}

unsigned short RegisterXml::getRegisterBlock(unsigned short regIndex)
{
    QDomElement domElement = documentElement().firstChildElement().childNodes().item(regIndex).toElement();
    return domElement.attribute("block").toShort();
}

bool RegisterXml::isValidAddress(QString& address)
{
    QDomNodeList nodeList = documentElement().firstChildElement().childNodes();

    for (int i=0; i<nodeList.size(); i++)
    {
        if (nodeList.item(i).toElement().attribute("address") == address)
            return true;
    }

    return false;
}

QString RegisterXml::getRegCode(unsigned short address)
{
    QDomNodeList nodeList = documentElement().firstChildElement().childNodes();
    QDomElement reg;

    for (int i=0; i<nodeList.size(); i++)
    {
        reg = nodeList.item(i).toElement();
        if (reg.attribute("address") == QString::number(address))
            return reg.firstChildElement("name").text();
    }

    return "";
}

QString RegisterXml::getRegisterBitTooltip(unsigned short address, unsigned short bit)
{
    QDomNodeList nodeList = documentElement().firstChildElement().childNodes();
    QDomElement reg;

    for (int i=0; i<nodeList.size(); i++)
    {
        reg = nodeList.item(i).toElement();
        if (reg.attribute("address") == QString::number(address))
        {
            QDomNodeList bits = reg.elementsByTagName("bits").item(0).childNodes();

            for (int i = 0; i < bits.size(); i++) 
            { 
                QDomElement elem = bits.item(i).toElement();

                if (elem.attribute("no").toShort() == bit)
                {
                    // Remove HTML tags with reg. expression
                    return elem.firstChildElement("tooltip").text().replace(QRegExp("<[^>]*>"), ""); 
                }
            }
        }
    }

    return "";
}

////////////////////////
//
// Control class
//
////////////////////////

Control::Control(unsigned short reg, QVector<unsigned short>& bits)
{
    this->reg = reg;
    this->bits = bits;
}

unsigned char Control::mask()
{
    unsigned char mask = 0;
    for (int i=0; i<bits.size(); i++)
        mask += pow(2.0, bits[i]);

    return mask;
}

////////////////////////
//
// Register class
//
////////////////////////

Register::Register(QString& address, QString& name, QString& tooltip, 
                   unsigned short funcBlock, QVector<Bit>& bits, Control* control)
{
    this->address = address;
    this->name = name;
    this->tooltip = tooltip;
    this->funcBlock = funcBlock;
    this->bits = bits;
    this->control = control;

    // Check if whole register is read-only
    int readOnlyBits = 0;
    for (int i=0; i<bits.size(); i++)
    {
        if (bits[i].readOnly())
            readOnlyBits++;
    }

    if (readOnlyBits == bits.size())
        readOnly = true;
    else
        readOnly = false;
}

Register::~Register()
{
    if (control != NULL)
        delete control;
}

Bit* Register::getBit(unsigned short bitIndex) 
{ 
    if (bits.size() > bitIndex)
        return const_cast<Bit*>(&(bits.at(bitIndex))); 

    return NULL;
};

unsigned short Register::addressNumber()
{
    int minusIndex = address.indexOf('-');

    if (minusIndex != -1)
        return address.left(minusIndex).toShort();

    return address.toShort();
}

unsigned short Register::getSubAddress()
{
    int minusIndex = address.indexOf('-');

    if (minusIndex != -1)
        return address.mid(minusIndex + 1).toShort();

    return 0; // 0 is invalid subaddress
}

////////////////////////
//
// Bit class
//
////////////////////////

Bit::Bit()
{
    no = 0;
    readOnlyBit = false;
}

Bit::Bit(unsigned short no, bool readOnlyBit, bool usedBit, QString& tooltip, QString& linkTo)
{
    this->no = no;
    this->readOnlyBit = readOnlyBit;
    this->usedBit = usedBit;
    this->tooltip = tooltip;
    this->linkTo = linkTo;
}
