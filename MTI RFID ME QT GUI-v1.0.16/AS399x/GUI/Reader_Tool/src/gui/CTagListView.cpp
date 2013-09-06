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

#include "CTagListView.h"

CTagListView::CTagListView(CTagManager* manager, QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	QObject::connect(ui.buttonBox, 			SIGNAL(accepted()), 				this, SLOT(accept()));
	QObject::connect(ui.buttonBox, 			SIGNAL(rejected()), 				this, SLOT(reject()));
	QObject::connect(ui.showButton,			SIGNAL(clicked()), 					this, SLOT(showResult()));

	m_manager = manager;
}

CTagListView::~CTagListView()
{

}


int CTagListView::exec()
{
	QStringList reader = QStringList() << "All reader" << m_manager->getReader();

	ui.readerBox->clear();
	ui.readerBox->insertItems(0, reader);

	ui.textBrowser->clear();
	ui.modeBox->setCurrentIndex(0);
	ui.readerBox->setCurrentIndex(0);

	return QDialog::exec();
}

void CTagListView::showResult()
{
	QStringList tags;
	if(ui.readerBox->currentIndex() == 0){
		if(ui.modeBox->currentIndex() == 0)
			tags = m_manager->getOverallTags();
		else if(ui.modeBox->currentIndex() == 1)
			tags = m_manager->getScanTags();
		else if(ui.modeBox->currentIndex() == 2)
			tags = m_manager->getCurrentTags();
	}
	else
	{
		if(ui.modeBox->currentIndex() == 0)
			tags = m_manager->getOverallTagsOfReader(ui.readerBox->currentText());
		else if(ui.modeBox->currentIndex() == 1)
			tags = m_manager->getScanTagsOfReader(ui.readerBox->currentText());
		else if(ui.modeBox->currentIndex() == 2)
			tags = m_manager->getCurrentTagsOfReader(ui.readerBox->currentText());
	}

	ui.textBrowser->setText(tags.join("\n"));

}
