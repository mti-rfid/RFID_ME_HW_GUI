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
 * CTreeWidget.h
 *
 *  Created on: 07.05.2009
 *      Author: stefan.detter
 */

#ifndef CTREEWIDGET_H_
#define CTREEWIDGET_H_

#include <QTreeWidget>

class CTreeWidget : public QTreeWidget
{
public:
	CTreeWidget( QWidget * parent = 0 );

	virtual ~CTreeWidget();

	void setColumnPercentWidth ( int column, uchar percent );

protected:
	virtual void resizeEvent ( QResizeEvent * event );

private:
	QByteArray	m_percentWidth;
};

#endif /* CTREEWIDGET_H_ */

/*
 * QTreeWidget{
	background-color: white;
	background-image: url(:/rfe logos/rfeLogoLight);
	background-attachment: centered;
	background-position: center;
	background-attachment: fixed;
	background-repeat: no-repeat;
	}
 */
