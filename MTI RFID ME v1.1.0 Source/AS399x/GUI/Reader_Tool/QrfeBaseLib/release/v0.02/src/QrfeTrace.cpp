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

#include "../inc/QrfeTrace.h"
#include <QtDebug>
#include <QCoreApplication>
#include <QMutex>

QrfeTrace QrfeTrace::m_theInstance;

quint32 QrfeTrace::Trace2Stdout = 0x00000001;
quint32 QrfeTrace::Trace2File = 0x00000002;
quint32 QrfeTrace::Trace2Signal = 0x00000004;
quint32 QrfeTrace::Trace2HTMLSignal = 0x00000008;

QrfeTrace::QrfeTrace()
{
	m_trcFileNr = 1;
	m_trcLevel = 1;
	m_trcFileInit = false;
}

QrfeTrace* QrfeTrace::getInstance()
{
	return &m_theInstance;
}

void QrfeTrace::init(quint8 trcLevel, quint32 mode)
{
	m_theInstance.m_trcLevel = trcLevel;
	m_theInstance.m_trcMode = mode;
	if ((mode & Trace2File) != 0)
	{
		m_theInstance.m_trcFile
				= new QFile(QString(QCoreApplication::applicationDirPath()
						+ "/trcFile%1.txt") .arg(m_theInstance.m_trcFileNr));
		m_theInstance.m_trcFile->open(QIODevice::WriteOnly);
		m_theInstance.m_trcFileInit = true;
	}
}

quint8 QrfeTrace::trcLevel ( )
{
	return m_theInstance.m_trcLevel;
}

void QrfeTrace::setTrcLevel(quint8 trcLevel)
{
	m_theInstance.m_trcLevel = trcLevel;
}

void QrfeTrace::trc(const int trcLevel, const QString msg)
{
	if (&m_theInstance == 0)
		return;
	m_theInstance.trace(trcLevel, msg);
}

void QrfeTrace::trace(const int trcLevel, const QString msg)
{
	if (trcLevel > m_theInstance.m_trcLevel)
		return;

	static QMutex mutex;
	mutex.lock();
	if ((m_trcMode & Trace2Signal) != 0)
		emit traceSignal(msg);

	if ((m_trcMode & Trace2HTMLSignal) != 0)
		emit traceSignalHTML(makeHTML(msg));

	if ((m_trcMode & Trace2File) != 0 && m_trcFileInit)
	{
		if (m_trcFile->size() > TRC_FILE_SIZE)
		{
			m_trcFile->close();
			if (++m_trcFileNr > TRC_FILE_COUNT)
				m_trcFileNr = 1;
			m_trcFile->setFileName(
					QString(QCoreApplication::applicationDirPath()
							+ "/trcFile%1.txt").arg(m_trcFileNr));
			m_trcFile->open(QIODevice::WriteOnly);
		}
		m_trcFile->write(msg.toAscii() + '\n');
	}

	if ((m_trcMode & Trace2Stdout) != 0)
		qDebug() << msg;

	mutex.unlock();
}

QString QrfeTrace::makeHTML(const QString trc)
{
	QString trcHTML = trc;
	//trcHTML.replace(" ", "&nbsp;");
	trcHTML.replace("\"", "&quot;");
	trcHTML.replace("&", "&amp;");
	trcHTML.replace("<", "&lt;");
	trcHTML.replace(">", "&gt;");
	return "<html>" + trcHTML + "</html>";
}

#define TIME_INFO   "[" + QTime::currentTime().toString("hh:mm:ss.zzz") + "] "
#define THREAD_INFO "|" + QString("0x%1").arg((ulong)QThread::currentThreadId(), 4, 16, QChar('0')) + "| - "

uint QrfeTraceModule::maxModuleNameLength = 0;

QrfeTraceModule::QrfeTraceModule(QString name)
{
	this->traceModuleName = name;
	maxModuleNameLength = MAX_MODULE_NAME_SIZE;
}

QrfeTraceModule::~QrfeTraceModule()
{

}

void QrfeTraceModule::warning(const QString msg)
{
	if ((uint) traceModuleName.size() < maxModuleNameLength)
		traceModuleName += QString(
				maxModuleNameLength - traceModuleName.size(), QChar(' '));

	QString trc = TIME_INFO + traceModuleName + THREAD_INFO;
	trc += "WARNING: " + msg;
	QrfeTrace::trc(0, trc);
	qFatal(trc.toAscii().data());
}

void QrfeTraceModule::error(const QString msg)
{
	if ((uint) traceModuleName.size() < maxModuleNameLength)
		traceModuleName += QString(
				maxModuleNameLength - traceModuleName.size(), QChar(' '));

	QString trc = TIME_INFO + traceModuleName + THREAD_INFO;
	trc += "ERROR: " + msg;
	QrfeTrace::trc(0, trc);
	qFatal(trc.toAscii().data());
}

void QrfeTraceModule::fatal(const QString msg)
{
	if ((uint) traceModuleName.size() < maxModuleNameLength)
		traceModuleName += QString(
				maxModuleNameLength - traceModuleName.size(), QChar(' '));

	QString trc = TIME_INFO + traceModuleName + THREAD_INFO;
	trc += "FATAL: " + msg;
	QrfeTrace::trc(0, trc);
	qFatal(trc.toAscii().data());
}

void QrfeTraceModule::trc(const int trcLevel, const QString msg)
{
	if ((uint) traceModuleName.size() < maxModuleNameLength)
		traceModuleName += QString(
				maxModuleNameLength - traceModuleName.size(), QChar(' '));

	QString trc = TIME_INFO + traceModuleName + THREAD_INFO;
	trc += msg;
	QrfeTrace::trc(trcLevel, trc);
}

void QrfeTraceModule::trcBytes(const int trcLevel, const QString msg,
		const QByteArray bytes, const QString pattern)
{
	QString data;
	for (int i = 0; i < bytes.size(); i++)
		data += QString(pattern).arg((unsigned char) bytes[i], 2, 16,
				QChar('0'));
	trc(trcLevel, msg + " " + QString::number(bytes.size()) + " Bytes: " + data);
}

void QrfeTraceModule::strc(const int trcLevel, const QString msg)
{
	QString trc = TIME_INFO + "STATIC              " + THREAD_INFO;
	trc += msg;
	QrfeTrace::trc(trcLevel, trc);
}

void QrfeTraceModule::strcBytes(const int trcLevel, const QString msg,
		const QByteArray bytes, const QString pattern)
{
	QString data;
	for (int i = 0; i < bytes.size(); i++)
		data += QString(pattern).arg((unsigned char) bytes[i], 2, 16,
				QChar('0'));
	strc(trcLevel, msg + " " + QString::number(bytes.size()) + " Bytes: "
			+ data);
}

void QrfeTraceModule::strc(const QString moduleName, const int trcLevel,
		const QString msg)
{
	QString traceModuleName = moduleName;
	if ((uint) traceModuleName.size() < MAX_MODULE_NAME_SIZE)
		traceModuleName += QString(MAX_MODULE_NAME_SIZE
				- traceModuleName.size(), QChar(' '));

	QString trc = TIME_INFO + traceModuleName + THREAD_INFO;
	trc += msg;
	QrfeTrace::trc(trcLevel, trc);
}

void QrfeTraceModule::strcBytes(const QString moduleName, const int trcLevel,
		const QString msg, const QByteArray bytes, const QString pattern)
{
	QString data;
	for (int i = 0; i < bytes.size(); i++)
		data += QString(pattern).arg((unsigned char) bytes[i], 2, 16,
				QChar('0'));
	strc(moduleName, trcLevel, msg + " " + QString::number(bytes.size())
			+ " Bytes: " + data);
}
