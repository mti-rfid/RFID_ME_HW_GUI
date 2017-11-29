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

/*! \file   spi.h
 *
 *  \author M. Arpa, C. Wurzinger
 *
 *  \brief  defines and macros for interfacing with the PC
 */


#ifndef _USB_H_
#define _USB_H_


#include "taskids.h"


#ifndef MIN
#define MIN(a,b)   ((a) < (b) ? (a) : (b))
#endif


#ifndef MAX
#define MAX(a,b)   ((a) > (b) ? (a) : (b))
#endif


typedef enum { PORTA = 0, PORTB = 1, PORTC = 2} Port;


#define PIPE2IN (ULONG)  1
#define PIPE2OUT (ULONG) 2


#define USB_STATE_PROCESSED    0x01
#define USB_STATE_NEW          0x00


#define USB_TASK(buf)          ((buf)[0])
#define USB_STATE(buf)         ((buf)[1])

#define USB_SERIAL_WRITE       0x00
#define USB_SERIAL_READ        0x01
#define USB_SH_DEV_ADDR(buf)   ((buf)[2])
#define USB_SH_R_NW(buf)       ((buf)[3])
#define USB_SH_START_ADDR(buf) ((buf)[4])
#define USB_SH_NUM_BYTES(buf)  ((buf)[5])
#define USB_SERIAL_DATA(buf)   ((buf)+6)

#define USB_FW_ID_DATA(buf)    ((buf)+2)

#define USB_DBG_DATA(buf)      ((buf)+2)

#define USB_ADC_CHANNEL(buf)   ((buf)[2])
#define USB_ADC_DATA(buf)      ((buf)+3)

#define USB_PIO_WRITE          0x00
#define USB_PIO_READ           0x01
#define USB_PIO_SET_IN         0x02
#define USB_PIO_SET_OUT        0x03
#define USB_PIO_GET_DIR        0x04
#define USB_PIO_TSK(buf)       ((buf)[2])
#define USB_PIO_PORT(buf)      ((buf)[3])
#define USB_PIO_MASK(buf)      ((buf)[4])
#define USB_PIO_DATA(buf)      ((buf)[5])

#define USB_OTP_NUM_BITS_L(buf)   ((buf)[2])
#define USB_OTP_NUM_BITS_H(buf)   ((buf)[3])
#define USB_OTP_READ_DO_LOAD(buf) ((buf)[4])

//scratchpad definitions
#define USB_SP_START_L(buf)    ((buf)[2])
#define USB_SP_START_H(buf)    ((buf)[3])
#define USB_SP_NUM_BYTES(buf)  ((buf)[4])
#define USB_SP_DATA(buf)       ((buf)+5)
#define USB_SP_MAX_DATA        (USB_PACKET_SIZE - 6)

/* plain spi */
#define USB_PLAIN_SPI_MOSI_NBYTES(buf) ((buf)[2]) /* number of bytes to send over mosi */
#define USB_PLAIN_SPI_MISO_NBYTES(buf) ((buf)[3]) /* last <number of bytes> received over miso that are reported back over usb */
#define USB_PLAIN_SPI_DATA_PTR(buf)    ((buf)+4) /* user data pointer */

/* plain i2c */
#define USB_PLAIN_I2C_MOSI_NBYTES(buf) ((buf)[2]) /* number of bytes to send over mosi */
#define USB_PLAIN_I2C_MISO_NBYTES(buf) ((buf)[3]) /* last <number of bytes> received over miso that are reported back over usb */
#define USB_PLAIN_I2C_DATA_PTR(buf)    ((buf)+4) /* user data pointer */
#define USB_PLAIN_I2C_HEADER_SIZE       4         /* number of bytes we need for the header */
#define USB_PLAIN_I2C_MAX_DATA         (USB_PACKET_SIZE - USB_PLAIN_I2C_HEADER_SIZE) /* maximum number of bytes to be sent with plain i2c in usb packet */

/* vsync */
#define USB_VSYNC_RELOAD_HIGH(buf) ((buf)[2]) /* high byte of the reload value for the vsync signal */
#define USB_VSYNC_RELOAD_LOW(buf)  ((buf)[3]) /* low byte of reload value */
#define USB_VSYNC_REPEAT_RELOAD(buf) ((buf)[4]) /* how often to repeat the reloading before changing the signal polarity, 0 = off */

#define USB_PACKET_SIZE        64
#define USB_MAX_SERIAL_SIZE    (USB_PACKET_SIZE - 6)

#endif /* _USB_H_ */
