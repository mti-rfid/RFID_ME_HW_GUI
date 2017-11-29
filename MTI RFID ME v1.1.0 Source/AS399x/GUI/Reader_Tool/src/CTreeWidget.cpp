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
 * CTreeWidget.cpp
 *
 *  Created on: 07.05.2009
 *      Author: stefan.detter
 */

#include "CTreeWidget.h"

CTreeWidget::CTreeWidget( QWidget * parent )
	: QTreeWidget(parent)
{
}

CTreeWidget::~CTreeWidget()
{
}


void CTreeWidget::setColumnPercentWidth ( int column, uchar percent )
{
	m_percentWidth.resize(this->columnCount());

	m_percentWidth[column] = percent;
}

void CTreeWidget::resizeEvent ( QResizeEvent * event )
{
	int width = this->size().width();
	double percWidth = ((double)width)/101.0;
	for(int i = 0; i < m_percentWidth.size(); i++)
	{
		if(m_percentWidth.at(i) != 0){
			int colWidth = m_percentWidth.at(i) * percWidth;
			if(colWidth == 0) colWidth = 1;
			this->setColumnWidth(i, colWidth);
		}
	}

	QTreeWidget::resizeEvent(event);
}
