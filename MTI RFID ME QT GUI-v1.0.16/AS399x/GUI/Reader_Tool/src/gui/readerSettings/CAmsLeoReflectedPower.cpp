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

#include "CAmsLeoReflectedPower.h"
#include <QMessageBox>
#include "qwt_dial_needle.h"
#define _USE_MATH_DEFINES
#include "math.h"

class DynamicNeedle : public QwtDialSimpleNeedle
{
public:
	DynamicNeedle() : QwtDialSimpleNeedle(Arrow,true,Qt::blue,Qt::red){}
	double min,max,current;
	virtual void draw 	(	QPainter * 	painter, 
		const QPoint & 	center, 
		int 	length, 
		double 	direction, 
		QPalette::ColorGroup 	cg = QPalette::Active	 
	) const
	{
		length = (int) (((double)length) * (current-min)/(max-min));
		QwtDialSimpleNeedle::draw(painter,center,length,direction,cg);
	}
};

CAmsLeoReflectedPower::CAmsLeoReflectedPower(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	needle = new DynamicNeedle();
	ui.vertexDial->setNeedle(needle);
	ui.vertexDial->setReadOnly(true);
	ui.vertexDial->setWrapping(true);
	ui.vertexDial->setOrigin(0);
	ui.vertexDial->setDirection(QwtDial::CounterClockwise);
	
//	QObject::connect(ui.buttonBox, 						SIGNAL(accepted()), 		this, SLOT(tryAccept()));

	timer.setInterval(50);
	timer.setSingleShot(false);

	QObject::connect(&timer,SIGNAL(timeout()),this, SLOT(updateDial()));

}

CAmsLeoReflectedPower::~CAmsLeoReflectedPower()
{
	// delete needle; widget will delete its needle on its own
}
void CAmsLeoReflectedPower::updateDial()
{
	//ulong freq = ui.frequencyBox->value() * 1000;    //Del by yingwei tseng for frequency, 2010/01/04
	QByteArray result;
	double i,q,dBm;
	double angle, length;
	QrfeReaderInterface::Result res = QrfeReaderInterface::ERROR;

	//Del by yingwei tseng for frequency, 2010/01/04
	//#if 0
	//res = m_ph->setFrequency(freq, 2, 0, 0, result);

	if (res !=QrfeReaderInterface::OK)
	{
		return;
	}
	//#endif
	//End by yingwei tseng for frequency, 2010/01/04

	i = result.at(0);
	q = result.at(1);
	
	dBm = length = sqrt(i*i+q*q);
	if (dBm == 0) dBm = 0.5;
	dBm = 20*log10(dBm/G);

	angle = atan2(i,q);

	if (angle < 0 ) angle += 2 * M_PI;

	angle = (angle / (2*M_PI)) *360.0;

	ui.ivalBox->setText(QString("%1").arg(i));
	ui.qvalBox->setText(QString("%1").arg(q));
	ui.angleBox->setText(QString("%1").arg(angle)+QChar(0x00b0));
	ui.dBmBox->setText(QString("%1 dBm").arg(dBm));
	this->needle->current = dBm;
	QPalette pal = ui.dBmBox->palette();
	if (dBm >= 5.0){
		pal.setColor(QPalette::Text,Qt::red);
	}
	else
	{
		pal.setColor(QPalette::Text,this->palette().color(QPalette::Text));
	}
	ui.dBmBox->setPalette(pal);
	ui.vertexDial->setValue(angle);
	ui.vertexDial->repaint();
}

int CAmsLeoReflectedPower::exec(QrfeReaderInterface* ph, double freq)
{
	int ret;
	m_ph = qobject_cast<QrfeAmsReader*>(ph);
	if( m_ph == 0 )
		return 0;

	G = m_ph->getG_rfp();
	needle->max = 20.0;
	needle->min = 20*log10(0.5/G);
	ui.frequencyBox->setValue(freq);
	timer.start();

	ret = QDialog::exec();

	timer.stop();

	return ret;
}