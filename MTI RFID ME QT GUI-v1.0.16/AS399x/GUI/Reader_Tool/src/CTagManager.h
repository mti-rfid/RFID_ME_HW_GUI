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
 * CTagManager.h
 *
 *  Created on: 16.06.2009
 *      Author: stefan.detter
 */

#ifndef CTAGMANAGER_H_
#define CTAGMANAGER_H_

#include <QMap>
#include <QSet>
#include <QString>
#include <QStringList>

class CTagManager : public QObject
{
	Q_OBJECT

public:
	CTagManager(QObject* parent = 0);
	virtual ~CTagManager();

	QStringList	getReader();

	QStringList	getCurrentTags();
	QStringList	getCurrentTagsOfReader(QString reader);
	QStringList	getScanTags();
	QStringList	getScanTagsOfReader(QString reader);
	QStringList	getOverallTags();
	QStringList	getOverallTagsOfReader(QString reader);

public slots:
	void addReader(QString reader);
	void cyclicInventoryResult(QString readerId, QString epc);
	void oldTagEntryRemoved(QString readerId, QString epc);
	void startScan();

private:
	QSet<QString>						m_reader;
	QMap<QString, QSet<QString> >		m_currentTags;
	QMap<QString, QSet<QString> >		m_scanTags;
	QMap<QString, QSet<QString> >		m_overallTags;
};

#endif /* CTAGMANAGER_H_ */
