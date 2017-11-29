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
/*
 * EPC_Defines.h
 *
 *  Created on: 06.05.2009
 *      Author: stefan.detter
 */

#ifndef EPC_DEFINES_H_
#define EPC_DEFINES_H_

#define EPC_TID_CLASS_ID_1		0xE0
#define EPC_TID_CLASS_ID_2		0xE2

#include <QMap>
#include <QStringList>


typedef enum {
	Impinj = 0x01,
	TI = 0x02,
	Alien = 0x03,
	Intelleflex = 0x04,
	Atmel = 0x05,
	NXP = 0x06,
	ST = 0x07,
	EP = 0x08,
	Motorola = 0x09,
	Sentech = 0x0A,
	EM = 0x0B,
	Renesas = 0x0C,
	Mstar = 0x0D,
	Tyco = 0x0E,
	Quanray = 0x0F,
	Fujitsu = 0x10,
	LSIS = 0x11,
	CAEN_RFID = 0x12
} MDID;

typedef struct{
	QString 	modelName;
	uint 		memorySize;
} TAG_MODEL_INFO;

extern QStringList									g_epcManufacturer;
extern QMap<QString, QMap<uint, TAG_MODEL_INFO> > 	g_tagInfo;

void initEPC_Constants();

typedef enum{
	MEM_RES 	= 0x00,
	MEM_EPC 	= 0x01,
	MEM_TID 	= 0x02,
	MEM_USER 	= 0x03,
} MEMORY_BANK;

//Add by yingwei tseng for memory bank block write, 2011/03/14 
typedef enum{
    MEM_SET     = 0x00,
	MEM_BLOCKSET= 0x01,	
} WRITE_MODE;
//End by yingwei tseng for memory bank block write, 2011/03/14 

//EPC Wordpointer Addresses
/** Definition for EPC wordpointer: Address for CRC value */
#define MEMADR_CRC        0x00
/** Definition for EPC wordpointer: Address for PC value Word position*/
#define MEMADR_PC         0x01
/** Definition for EPC wordpointer: Address for EPC value */
#define MEMADR_EPC        0x02

/** Definition for EPC wordpointer: Address for kill password value */
#define MEMADR_KILLPWD    0x00
/** Definition for EPC wordpointer: Address for access password value */
#define MEMADR_ACCESSPWD  0x02

/** Definition for EPC wordpointer: Address for TID value */
#define MEMADR_TID        0x00


typedef enum{
	UNLOCK 				= 0x00,
	LOCK 				= 0x01,
	PERMALOCK 			= 0x02,
	LOCK_AND_PERMALOCK 	= 0x03
} LOCK_MODE;

typedef enum{
	KILL_PASSWORD 	= 0x00,
	ACCESS_PASSWORD = 0x01,
	EPC 			= 0x02,
	TID 			= 0x03,
	USER 			= 0x04,
} LOCK_MEMORY_SPACE;

#endif /* EPC_DEFINES_H_ */
