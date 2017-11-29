/*
 * QrfeDeviceDetector.h
 *
 *  Created on: 08.01.2009
 *      Author: stefan.detter
 */

#ifndef QRFEDEVICEDETECTOR_H_
#define QRFEDEVICEDETECTOR_H_

#ifdef _TTY_POSIX_
#include "Posix_QrfeDeviceDetector.h"
#define QrfeDeviceDetectorBaseImpl Posix_QrfeDeviceDetector

/*MS WINDOWS CODE*/
#else
#include "Win_QrfeDeviceDetector.h"
#define QrfeDeviceDetectorBaseImpl Win_QrfeDeviceDetector
#endif

class QrfeDeviceDetector: public QrfeDeviceDetectorBaseImpl
{
public:
	QrfeDeviceDetector(QObject* parent = 0);
	virtual ~QrfeDeviceDetector();
};

#endif /* QRFEDEVICEMANAGER_H_ */
