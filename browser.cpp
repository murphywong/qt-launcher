#include "browser.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QtDebug>

namespace ui {

	AddressBar::AddressBar(QWidget *parent) :
		QWidget(parent)
	{
		//mAddressEdit = new QLineEdit(parent);
		mAddressEdit = new QComboBox(parent);
		mGoButton = new QPushButton("Go", parent);

		mLayout = new QHBoxLayout(this);
		mLayout->addWidget(mAddressEdit);
		mLayout->addWidget(mGoButton);

		mAddressEdit->addItem("www.baidu.com");
		mAddressEdit->addItem("www.google.com");
		mAddressEdit->addItem("www.hao123.com");
		mAddressEdit->addItem("www.163.com");
		mAddressEdit->addItem("www.sina.com.cn");
		mAddressEdit->addItem("www.qq.com");

		mAddressEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		connect(mGoButton, SIGNAL(clicked()), this, SLOT(goToSite()));
		//connect(mAddressEdit, SIGNAL(returnPressed()), this, SLOT(goToSite()));
	}

	AddressBar::~AddressBar()
	{
#if 0
		delete mAddressEdit;
		delete mGoButton;
#endif
	}

	void AddressBar::goToSite()
	{
		//qDebug() << __FILE__ << __func__ << __LINE__;
		//QString address = mAddressEdit->text();
		QString address = mAddressEdit->currentText();
		if( !address.contains("http://") && !address.contains("https://") ) {
			address = "http://" + address;
		}
		//qDebug() << __FILE__ << __func__ << __LINE__;

		if (!address.isEmpty()){
			//qDebug() << __FILE__ << __func__ << __LINE__;
			emit go(QUrl(address));
		}
	}

	HtmlView::HtmlView(QWidget *parent) :
			QWebView(parent)
	{
	}

	void HtmlView::loadNewPage(const QUrl &url)
	{
			//qDebug() << __FILE__ << __func__ << __LINE__ << url;
			this->load(url);
	}

	Broswer::Broswer(QWidget *parent)
		:BaseWidget(parent)
	{
		QVBoxLayout *layout = centralLayout();
		mBar = new AddressBar;
		mView = new HtmlView;

		layout->addWidget(mBar);
		layout->addWidget(mView);
		layout->addStretch(1);

		connect(mBar, SIGNAL(go(QUrl)), mView, SLOT(loadNewPage(QUrl)));
		setWindowTitle("Browser v1.0");
		setFixedSize(800, 480);
	}

	Broswer::~Broswer()
	{
		delete mBar;
		delete mView;
	}
}
