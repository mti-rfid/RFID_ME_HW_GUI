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
 * CReadRateCalc.h
 *
 *  Created on: 26.05.2009
 *      Author: stefan.detter
 */

#ifndef CREADRATECALC_H_
#define CREADRATECALC_H_

#include <QObject>
#include <QTime>
#include <QMap>

typedef struct
{
	bool		active;
	quint64 	msecsActive;
	QTime 		lastTimeStarted;
	quint64		readCount;
} ReaderInfo;

class CReadRateCalc : public QObject
{
	Q_OBJECT

public:
	CReadRateCalc(QObject* parent = 0);
	virtual ~CReadRateCalc();


public:
	void readerStartedInventory(QString readerId);
	void readerPausedInventory(QString readerId);
	void readerResumedInventory(QString readerId);
	void readerStoppedInventory(QString readerId);

	void reset();
	void reset(QString readerId);

public slots:
	void clearResults();

	void cyclicInventoryResult(QString readerId, QString epc);

public:
	quint64 getReadCount(QString readerId);
	double  getReadRate(QString readerId);

private:
	QMap<QString, ReaderInfo>	m_readerInfos;
};

#endif /* CREADRATECALC_H_ */
