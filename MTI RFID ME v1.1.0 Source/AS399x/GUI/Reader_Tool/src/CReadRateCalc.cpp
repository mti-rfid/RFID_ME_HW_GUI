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
 * CReadRateCalc.cpp
 *
 *  Created on: 26.05.2009
 *      Author: stefan.detter
 */

#include "CReadRateCalc.h"

CReadRateCalc::CReadRateCalc(QObject* parent)
	: QObject(parent)
{
	// TODO Auto-generated constructor stub

}

CReadRateCalc::~CReadRateCalc()
{
	// TODO Auto-generated destructor stub
}

void CReadRateCalc::readerStartedInventory(QString readerId)
{
	ReaderInfo info;
	info.active = true;
	info.lastTimeStarted = QTime::currentTime();
	info.msecsActive = 0;
	info.readCount = 0;
	m_readerInfos.insert(readerId, info);
}

void CReadRateCalc::readerPausedInventory(QString readerId)
{
	ReaderInfo& info = m_readerInfos[readerId];
	info.active = false;
	info.msecsActive += qAbs(info.lastTimeStarted.msecsTo(QTime::currentTime()));
}

void CReadRateCalc::readerResumedInventory(QString readerId)
{
	if(!m_readerInfos.contains(readerId))
		return readerStartedInventory(readerId);

	ReaderInfo& info = m_readerInfos[readerId];
	info.active = true;
	info.lastTimeStarted = QTime::currentTime();
}

void CReadRateCalc::readerStoppedInventory(QString readerId)
{
	ReaderInfo& info = m_readerInfos[readerId];
	info.active = false;
	info.msecsActive += qAbs(info.lastTimeStarted.msecsTo(QTime::currentTime()));
}

void CReadRateCalc::reset()
{
	m_readerInfos.clear();
}

void CReadRateCalc::reset(QString readerId)
{
	m_readerInfos.remove(readerId);
}

void CReadRateCalc::clearResults()
{
	ReaderInfo info;
	info.active = true;
	info.lastTimeStarted = QTime::currentTime();
	info.msecsActive = 0;
	info.readCount = 0;

	foreach(QString id, m_readerInfos.keys())
	{
		m_readerInfos.insert(id, info);
	}
}

void CReadRateCalc::cyclicInventoryResult(QString readerId, QString epc)
{
	if(!m_readerInfos.contains(readerId))
		return;

	ReaderInfo& info = m_readerInfos[readerId];
	info.readCount++;
}


quint64 CReadRateCalc::getReadCount(QString readerId)
{
	if(!m_readerInfos.contains(readerId))
		return 0;
	return m_readerInfos[readerId].readCount;
}

double  CReadRateCalc::getReadRate(QString readerId)
{
	if(!m_readerInfos.contains(readerId))
		return 0.0;

	double speed;

	ReaderInfo& info = m_readerInfos[readerId];

	quint64 msecs = info.msecsActive;
	if(info.active)
		msecs += qAbs(info.lastTimeStarted.msecsTo(QTime::currentTime()));

	if(msecs > 0)
		speed = (double)info.readCount / (double)msecs;
	else
		speed = (double)info.readCount;

	return speed * 1000.0;
}
