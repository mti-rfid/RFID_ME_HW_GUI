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
* \file register_xml.h
* 
* \author Vitomir Spasojevic
*
* \brief  Register map XML handling file.
*
* Register map XML handling contains of few classes. RegisterXml is QDomDocument wrapper and handles
* all accesses to the register map XML structure. There are also register and bit respresentaion classes.
* Register, Bit and BitExt are these classes and they wrap the register and bit data from the XML.
*/

#ifndef REGISTER_XML_H
#define REGISTER_XML_H

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QVector>

//! File name for the register definition XML data
const char* const REGISTER_MAP_FILE_NAME = "register_map.xml";

//! Bit mask for all of the bits in a byte. Mask values have one 0 bit value at the successive bit positions.
const unsigned char BITMASK0[8] = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f};
//! Bit mask for all of the bits in a byte. Mask values have one 1 bit value at the successive bit positions.
const unsigned char BITMASK1[8] = {0x1,  0x2,  0x4,  0x8,  0x10, 0x20, 0x40, 0x80};

//! Register bit class. 
/*!
This class represents the register bit properties defined in the XML data file.
Class have only property accessories, therefore, there is no processing code in it.
*/
class Bit
{
public:
    Bit();
    Bit(unsigned short no, bool readOnly, bool used, QString& tooltip, QString& linkTo);

    unsigned short getOrderNo() { return no; };
    bool readOnly() { return readOnlyBit; };
    bool used() { return usedBit; };
    QString getTooltip() { return tooltip; };
    QString getLinkedBit() { return linkTo; };

protected:
    unsigned short no; //!< Ordinal number of the bit in the byte (value from 0 to 7)
    bool readOnlyBit; //!< True if bit is read only, false otherwise.
    bool usedBit; //!< True when bit is used, false otherwise.
    QString tooltip; //!< Bit tooltip string.
    QString linkTo; //!< XPATH string to the linked bit. Linked bit is one which changes it's value when some other bit changes.
};

//! Register bit extended class. 
/*!
This class represents the register bit extended properties defined in the XML data file.
It inherits Bit class with the information of register address to which bit belongs.
*/
class BitExt : public Bit
{
public:
    BitExt(unsigned short no, bool readOnly, bool used, QString& tooltip, QString& linkTo, QString& regAddr) : 
      Bit(no, readOnly, used, tooltip, linkTo)
      { this->regAddr = regAddr; };

      QString regAddress() { return regAddr; };

private:
    QString regAddr; //!< Register address to which bit belongs.
};

//! Register control bits class. 
/*!
Some bits of the register may control which register is used among few
other registers which have the same address.
*/
class Control
{
public:
    Control(unsigned short reg, QVector<unsigned short>& bits);

    QVector<unsigned short>& getBits() { return bits; };
    unsigned short getRegister() { return reg; };
    unsigned char mask();

private:
    QVector<unsigned short> bits; //!< Register bits used as control bits.
    unsigned short reg; //!< Controlling register.
};

//! Register representation class. 
/*!
This class represents the register properties defined in the XML data file.
Class have only property accessories, therefore, there is no processing code in it.
*/
class Register
{
public:
    Register(QString& address, QString& name, QString& tooltip, unsigned short funcBlock, 
        QVector<Bit>& bits, Control* control = NULL);
    ~Register();

    QString getAddress() { return address; };
    QString getName() { return name; };
    QString getTooltip() { return tooltip; };
    unsigned short getFunctionalBlock() { return funcBlock; };
    Control* getControl() { return control; };
    unsigned short getSubAddress();
    bool isReadOnly() { return readOnly; };

    int bitsNo() { return bits.size(); };
    Bit* getBit(unsigned short bitIndex);

    //! Returns register address converted to number. 
    //! In case that register is multiplexed register, return value is value left of minus sign.
    //! \return Register address number.
    unsigned short addressNumber();

private:
    QString address; //!< Register address value.
    QString name; //!< Register name string value.
    QString tooltip; //!< Register tooltip string value.
    unsigned short funcBlock; //!< Functional block number value.
    QVector<Bit> bits; //!< Array of the bits in the register.
    Control* control;
    bool readOnly; //!< True if whole register is read-only.
};

//! Register map XML handling class. 
/*!
This class represents register map XML data handling class.
Internal data is initialized from the XML file. This data is run through
when specific register information is needed by the user.
All the methods from this class should be treated as thread safe
because they are not changing any DOM document data. Therefore, more
threads, for example, register map gui and update thread can use it
at the same time safely.
*/
class RegisterXml : protected QDomDocument
{
public:
    //! Functional block enumeration for every GUI block type.
    enum FuncBlock
    {
        NO_FUNC_BLOCK,
        AUDIO_BLOCK,
        CHARGER_BLOCK,
        HOUSEKEEPING_BLOCK,
        HOUSEKEEPING2_BLOCK,
        PMU_BLOCK,
        PMU2_BLOCK,
        TOUCHPANEL_BLOCK,
        REALTIME_CLOCK_BLOCK,
        AUDIOFADER_BLOCK,
        EQUILIZER_BLOCK,
        BACKLIGHT_BLOCK
    };

    //! Register data used by automatic update function from Update Thread
    typedef struct AutoUpdate
    {
        unsigned short address;
        unsigned short funcBlock;
    };

    typedef struct ReadOnce
    {
        AutoUpdate reg;
		bool isSubRegister;
        unsigned char subAddress;
        Control* control;
    };

    //! Register data used in interrupt polling. Used for interrupt enable mask registers.
    typedef struct EnableInterrupt
    {
        unsigned short address;
        unsigned short value;
    };

	RegisterXml(QString path, QString filename = "");
    ~RegisterXml() {};

    //! Gets chip name string from the XML file.
    QString getChipName();
    //! True if register map value column is editable.
    //! This is case is 'editvalue' attribute has value 1 in the XML file.
    bool isValueEditable();
    //! Gets register object for the specific register ordinal number.
    //! \param regIndex Register ordinal number index value.
    //! \return Register object.
    Register getRegister(unsigned short regIndex);
    //! Get control data of the register.
    //! \param regIndex Register index value.
    //! \return Control object or NULL if there is no existing 'control' subelement.
    Control* getControl(unsigned short regIndex);
    //! Returns number of registers in the XML file.
    int registersNo();
    //! Returns link bit object for the specific XPATH value.
    //! \param link XPATH to the linked bit in the XML file.
    //! \return Bit object pointed by \a link.
    BitExt* getLinkedBit(QString& link);
    //! Gets register ordinal number in the XML file.
    //! \param address Register address value.
    //! \return Register ordinal number.
    int getRegIndex(QString& address);
    //! Gets register functional block value from the register ordinal number index.
    unsigned short getRegisterBlock(unsigned short regIndex);
    //! Find out if register address exists.
    //! \param address Register address to be found.
    //! \return True if register address exists (is valid) or false otherwise.
    bool isValidAddress(QString& address);
    //! Find register bit tooltip.
    //! \param address Address of the register to whom \a bit belongs.
    //! \param bit Register bit ordinal number.
    //! \return Tooltip string value.
    QString getRegisterBitTooltip(unsigned short address, unsigned short bit);

    //! Returns register address converted to number. Static method.
    //! In case that register is multiplexed register, return value is value left of minus sign.
    //! \param address Input register address value.
    //! \return Register address number.
    static unsigned short addressNumber(QString& address);

    // Used by Update Thread

    //! Returns all the interrupt register addresses.
    //! \return Array of the interrupt register addresses from the XML file.
    QVector<unsigned short> getInterruptRegs();
    //! Find all the register for reading once option.
    QVector<ReadOnce> getReadOnce();
    //! Finds all the registers marked for automatic update polling by update thread.
    //! \return Array of registers marked for automatic update polling.
    QVector<AutoUpdate> getAutoUpdateRegs();
    //! Finds all the registers marked as interrupt enable mask registers.
    //! \return Array of registers marked as interrupt enable registers.
    QVector<EnableInterrupt> getEnableInterrupt();
    //! Returns register name for the specific register address.
    QString getRegCode(unsigned short address);

private:
    //! Method is used for XPATH parsing and finding XML document element which corresponds to the XPATH value.
    //! \param path XPATH to find.
    //! \param currElement Current position in the XML file. Root element at the beginning.
    //! \return Final XML dom element which is pointed by \a path.
    QDomElement getPathElement(QString& path, QDomElement* currElement);
    //! Finds XML dom element with specific attribute value.
    QDomElement getElement(QString& currPath, QDomElement* currElement);

    //! Finds value of 'address' attribute from the XPATH.
    //! \param link XPATH value.
    //! \return Address value string.
    QString getRegAddressFromXPath(QString& link);
    //! Parses 'control' XML subtree which is used for multiplexed registers controlled by some other register.
    //! \param currRegister Parent XML dom element of the 'control' element.
    //! \return Control object or NULL if there is no existing 'control' subelement.
    Control* initControl(QDomElement* currRegister);
};

#endif // REGISTER_XML_H
