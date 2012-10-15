/*
 *****************************************************************************
 * Copyright @ 2010 by austriamicrosystems AG                                *
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
 *      PROJECT:   Austriamicrosystems EZ-USB Firmware
 *      $Revision: 1.1 $
 *      LANGUAGE:  C
 */

/*! \file   taskids.h
 *
 *  \author M. Arpa, C. Wurzinger
 *
 *  \brief  defines all used TASK IDS for interfacing with the PC
 */

#ifndef _TASKIDS_H__
#define _TASKIDS_H__



#define TSK_PING             0x01
#define TSK_I2C              0x02
#define TSK_ADC              0x03
#define TSK_FW_ID            0x04
#define TSK_DBG              0x05
#define TSK_PIO              0x06
#define TSK_OTP_ZAPP         0x07
#define TSK_OTP_WRITE        0x08
#define TSK_OTP_READ_DIG     0x09
#define TSK_OTP_READ_ANA     0x0A
#define TSK_READ_SCRATCHPAD  0x0B
#define TSK_WRITE_SCRATCHPAD 0x0C

#define TSK_SWI_EN1          0xC0
#define TSK_SWI_EN2          0xC1
#define TSK_SWI_FLASH        0xC2
#define TSK_SPI	             0xC3
#define TSK_STANDARD_SPI     0xC4
#define TSK_SWI		         0xC5           
#define TSK_SSPI			 0xC6
#define TSK_I2C_READSRAM     0xCC
#define TSK_PPTRIM           0x0D
#define TSK_PPTRIM_FUSE      0x0E
#define TSK_I2C_HARD         0x0F

#define TSK_PLAIN_SPI        0x11
#define TSK_PLAIN_I2C        0x12
#define TSK_VSYNC	     0x13


#define TSK_UNKNOWN 0xFF


#endif /* _TASKIDS_H_ */
