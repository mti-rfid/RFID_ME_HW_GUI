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
 * CTagManager.cpp
 *
 *  Created on: 16.06.2009
 *      Author: stefan.detter
 */

#include "CTagManager.h"

CTagManager::CTagManager(QObject* parent)
	: QObject(parent)
{

}

CTagManager::~CTagManager()
{

}

void CTagManager::addReader(QString reader)
{
	m_reader.insert(reader);
}

void CTagManager::cyclicInventoryResult(QString readerId, QString epc)
{
	if(!m_currentTags.keys().contains(readerId))m_currentTags.insert(readerId, QSet<QString>());
	if(!m_scanTags.keys().contains(readerId))m_currentTags.insert(readerId, QSet<QString>());
	if(!m_overallTags.keys().contains(readerId))m_currentTags.insert(readerId, QSet<QString>());
	m_currentTags[readerId].insert(epc);
	m_scanTags[readerId].insert(epc);
	m_overallTags[readerId].insert(epc);
}

void CTagManager::oldTagEntryRemoved(QString readerId, QString epc)
{
	m_currentTags[readerId].remove(epc);
}

void CTagManager::startScan()
{
	m_currentTags.clear();
	m_scanTags.clear();
}


QStringList CTagManager::getReader()
{
	QStringList reader(m_reader.toList());
	reader.sort();
	return reader;
}


QStringList	CTagManager::getCurrentTags()
{
	QSet<QString> allTags;
	foreach(QString reader, m_currentTags.keys()){
		allTags += m_currentTags.value(reader);
	}

	QStringList tags(allTags.toList());
	tags.sort();
	return tags;
}

QStringList	CTagManager::getCurrentTagsOfReader(QString reader)
{
	QStringList tags(m_currentTags.value(reader).toList());
	tags.sort();
	return tags;
}

QStringList	CTagManager::getScanTags()
{
	QSet<QString> allTags;
	foreach(QString reader, m_scanTags.keys()){
		allTags += m_scanTags.value(reader);
	}

	QStringList tags(allTags.toList());
	tags.sort();
	return tags;
}

QStringList	CTagManager::getScanTagsOfReader(QString reader)
{
	QStringList tags(m_scanTags.value(reader).toList());
	tags.sort();
	return tags;
}

QStringList	CTagManager::getOverallTags()
{
	QSet<QString> allTags;
	foreach(QString reader, m_overallTags.keys()){
		allTags += m_overallTags.value(reader);
	}
	QStringList tags(allTags.toList());
	tags.sort();
	return tags;
}

QStringList	CTagManager::getOverallTagsOfReader(QString reader)
{
	QStringList tags(m_overallTags.value(reader).toList());
	tags.sort();
	return tags;
}
