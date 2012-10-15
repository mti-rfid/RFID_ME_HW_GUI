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
 * EPC_Defines.cpp
 *
 *  Created on: 06.05.2009
 *      Author: stefan.detter
 */

#include "EPC_Defines.h"

QStringList									g_epcManufacturer;
QMap<QString, QMap<uint, TAG_MODEL_INFO> >	g_tagInfo;


void initEPC_Constants()
{
	g_epcManufacturer <<	"Unknown" <<
							"Impinj" <<
							"Texas Instruments " <<
							"Alien Technology" <<
							"Intelleflex" <<
							"Atmel" <<
							"NXP" <<
							"ST Microelectronics" <<
							"EP Microelectronics" <<
							"Motorola" <<
							"Sentech Snd Bhd" <<
							"EM Microelectronics" <<
							"Renesas Technology Corp." <<
							"Mstar" <<
							"Tyco International" <<
							"Quanray Electronics" <<
							"Fujitsu" <<
							"LSIS" <<
							"CAEN RFID srl";


	QMap<uint,TAG_MODEL_INFO> models;

	g_tagInfo.insert("Unknown", models);

	{
		{
			TAG_MODEL_INFO t = {QString("Monza"), 0};
			models.insert(0x50, t);
		}
		g_tagInfo.insert("Impinj", models);
		models.clear();
	}

	g_tagInfo.insert("Texas Instruments", models);
	g_tagInfo.insert("Alien Technology", models);
	g_tagInfo.insert("Intelleflex", models);
	g_tagInfo.insert("Atmel", models);

	{
		{
			TAG_MODEL_INFO t = {QString("UCode EPC G2"), 28};
			models.insert(0x01, t);
		}
		{
			TAG_MODEL_INFO t = {QString("UCode EPC G2XM"), 0};
			models.insert(0x03, t);
		}
		{
			TAG_MODEL_INFO t = {QString("UCode EPC G2XL"), 64};
			models.insert(0x04, t);
		}
		g_tagInfo.insert("NXP", models);
		models.clear();
	}

	g_tagInfo.insert("ST Microelectronics", models);
	g_tagInfo.insert("EP Microelectronics", models);
	g_tagInfo.insert("Sentech Snd Bhd", models);
	g_tagInfo.insert("EM Microelectronics", models);
	g_tagInfo.insert("Renesas Technology Corp.", models);
	g_tagInfo.insert("Mstar", models);
	g_tagInfo.insert("Tyco International", models);
	g_tagInfo.insert("Quanray Electronics", models);
	g_tagInfo.insert("Fujitsu", models);
	g_tagInfo.insert("LSIS", models);
	g_tagInfo.insert("CAEN RFID srl", models);


}
