#include "netwidget.h"
#include "switchbar.h"
#include "util.h"
#include "wifiwidget.h"
#include "ethernet.h"
#include "toolbar.h"
#include "inputmethod.h"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QPixmap>
#include <QPaintEvent>
#include <QPainter>
#include <QIcon>
#include <QtDebug>
#include <QPushButton>
#include <QEvent>

namespace ui {
	NetWidget::NetWidget(QWidget *parent)
		:BaseWidget(QPixmap(":/icons/net_background2.png"), parent)
	{
		mLayout = new QVBoxLayout;
		mTopBar = new SwitchBar(QPixmap(":/icons/switchbar_background.png"),
				QPixmap(":/icons/net_icon.png"), QPixmap(":/icons/net_text.png"),
				QIcon(":/icons/manual_switch.png"), QIcon(":/icons/auto_switch.png"));
		mAutoItem = new AutoWidget;
		mManualItem = new ManualWidget;
		mLayout->addWidget(mAutoItem);
		mLayout->addWidget(mManualItem);

		centralLayout()->addWidget(mTopBar);
		centralLayout()->addLayout(mLayout);

		connect(mTopBar, SIGNAL(switchAction(bool)), SLOT(onStateChanged(bool)));
		connect(mManualItem, SIGNAL(signIn()), SLOT(onSignIn()));

		onStateChanged(false);

		setFixedSize(QSize(800, 480));
	}

	NetWidget::~NetWidget()
	{
		delete mTopBar;
		delete mAutoItem;
		delete mManualItem;
		delete mLayout;
	}

	void NetWidget::onStateChanged(bool manual)
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		if (manual){
			mManualItem->show();
			mAutoItem->hide();
		} else {
			mManualItem->hide();
			mAutoItem->show();
			startDhcp();
		}
	}


	void NetWidget::startDhcp()
	{
		mTopBar->setNotify("Start to obatin IP ...");

		if (!EtherNet::isEthernetUp()){
			QString cmd = QString("ifconfig %1 up").arg(ETHERNET_IFACE);
			if (::system(qPrintable(cmd))){
				qDebug() << QString("Failed to start %1").arg(ETHERNET_IFACE);
			} else {
			}
		} else {
			qDebug() << "eth0 is up";
		}

		QProcess *process = new QProcess;
		connect(process, SIGNAL(finished(int, QProcess::ExitStatus)),
				SLOT(processFinished(int, QProcess::ExitStatus)));
		QString cmd = QString("udhcpc -i%1 -t6 -q -n").arg(ETHERNET_IFACE);

		process->start(cmd);
	}

	void NetWidget::processFinished(int exitCode, QProcess::ExitStatus status)
	{
		qDebug() << __FILE__ << __func__ << __LINE__ << exitCode << status;
		if (exitCode == 0){
			mAutoItem->updateConfig();
			mTopBar->setNotify("obtin IP success");
			toolBar()->setIndicateVisible(IndicateEthernet, true);
		} else {
			mTopBar->setNotify("Failed to obtin IP");
			toolBar()->setIndicateVisible(IndicateEthernet, false);
		}

		delete sender();
	}

	void NetWidget::onSignIn()
	{
		QString ip = mManualItem->ip();
		QString netmask = mManualItem->netmask();
		QString gateway = mManualItem->gateway();
		QString dns = mManualItem->dns();
		int ret = EtherNet::setConfig(ETHERNET_IFACE, qPrintable(ip), 
				qPrintable(netmask), qPrintable(dns), qPrintable(gateway));
		if (ret == 0){
			mTopBar->setNotify("Set network configurate success");
			toolBar()->setIndicateVisible(IndicateEthernet, true);
		} else {
			mTopBar->setNotify("Failed to set network configurate");
			toolBar()->setIndicateVisible(IndicateEthernet, false);
		}
	}

	PropertyWidget::PropertyWidget(QString name, QString property, QWidget *parent)
		:QWidget(parent)
		,mBackground(QPixmap(":/icons/property_background.png"))
	{
		QVBoxLayout *layout = new QVBoxLayout(this);
		mName = new QLabel(name);
		mProperty = new QLabel(property);

		layout->addWidget(mName);
		layout->addWidget(mProperty);

		mName->setStyleSheet("color:white");
		mProperty->setStyleSheet("color:white");
		Util::setFontSize(mName, 8);
		Util::setFontSize(mProperty, 8);
		Util::setLeftMargins(mName, 100);
		Util::setLeftMargins(mProperty, 100);

		//setFixedSize(QSize(789, 53));
		setFixedWidth(779);
	}

	PropertyWidget::~PropertyWidget()
	{
		delete mName;
		delete mProperty;
	}

	void PropertyWidget::paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);
		painter.drawPixmap(0, 0, mBackground.scaled(size()));
		QWidget::paintEvent(event);
	}

	AutoWidget::AutoWidget(QWidget *parent)
		:QWidget(parent)
		,mConf(new EtherNet(ETHERNET_IFACE))
	{
		QScrollArea *scrollArea = new QScrollArea(this);
		ScrollWidget *widget = new ScrollWidget(this);
		QVBoxLayout *layout = new QVBoxLayout(widget);
		TitleWidget *device = new TitleWidget("Ethernet device");
		Util::setLeftMargins(device, 50);

		mDevice = new PropertyWidget(ETHERNET_IFACE);
		mDevice->setBackground(QPixmap(":/icons/property_background2.png"));

		TitleWidget *conf = new TitleWidget("Ethernet configuration");
		Util::setLeftMargins(conf, 50);

		mMac = new PropertyWidget("MAC");
		mIp = new PropertyWidget("IP");
		mNetmask = new PropertyWidget("Netmask");
		mGateway = new PropertyWidget("Gate way");
		mDns = new PropertyWidget("DNS");

		layout->addWidget(device);
		layout->addWidget(mDevice);
		layout->addWidget(conf);
		layout->addWidget(mMac);
		layout->addWidget(mIp);
		layout->addWidget(mNetmask);
		layout->addWidget(mGateway);
		layout->addWidget(mDns);
		layout->setSpacing(0);

		scrollArea->setWidget(widget);
		widget->setFixedWidth(780);
		//scrollArea->setFixedWidth(779);

		QVBoxLayout *mainLayout = new QVBoxLayout(this);
		mainLayout->addWidget(scrollArea);
		mainLayout->setContentsMargins(0, 0, 0, 0);
		layout->setContentsMargins(0, 0, 0, 0);

#if 0
		mDevice->setProperty(mConf->mac());
		mMac->setProperty(mConf->mac());
		mIp->setProperty(mConf->ip());
		mNetmask->setProperty(mConf->netmask());
		mDns->setProperty(mConf->dns());
		mGateway->setProperty(mConf->gateway());
#endif

		updateConfig();

	}

	AutoWidget::~AutoWidget()
	{
		delete mDevice;
		delete mMac;
		delete mIp;
		delete mNetmask;
		delete mGateway;
		delete mDns;
	}

	void AutoWidget::updateConfig()
	{
		mConf->parseConfiguation();
		mDevice->setProperty(mConf->mac());
		mMac->setProperty(mConf->mac());
		mIp->setProperty(mConf->ip());
		mNetmask->setProperty(mConf->netmask());
		mDns->setProperty(mConf->dns());
		mGateway->setProperty(mConf->gateway());
	}


	TitleWidget::TitleWidget(QString title, QWidget *parent)
		:QLabel(title, parent)
	{
		setStyleSheet("color:white");
		setFixedSize(QSize(779, 50));
		setAlignment(Qt::AlignBottom);
	}

	void TitleWidget::paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);
		painter.drawPixmap(0, 0, QPixmap(":/icons/title_background.png").scaled(size()));
		QLabel::paintEvent(event);
	}

	EditableWidget::EditableWidget(QString name, QString content, QWidget *parent)
		:QWidget(parent)
	{
		QVBoxLayout *layout = new QVBoxLayout(this);
		mNameLabel = new QLabel(name);
		mLineEdit = new QLineEdit(content);

		layout->addWidget(mNameLabel);
		layout->addWidget(mLineEdit);

		//mLineEdit->setFrame(false);
		mLineEdit->setStyleSheet("background:gray;color:white");
		mNameLabel->setStyleSheet("color:white");
		mLineEdit->installEventFilter(this);
		Util::setFontSize(mLineEdit, 8);
		Util::setFontSize(mNameLabel, 8);
		Util::setLeftMargins(mNameLabel, 20);
		Util::setLeftMargins(mLineEdit, 40);
	}

	EditableWidget::~EditableWidget()
	{
		delete mNameLabel;
		delete mLineEdit;
	}

	void EditableWidget::paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);
		painter.drawPixmap(0, 0, QPixmap(":/icons/property_background.png").scaled(size()));
		QWidget::paintEvent(event);
	}

	bool EditableWidget::eventFilter(QObject *obj, QEvent *event)
	{
		if (obj == mLineEdit && event->type() == QEvent::FocusIn){
			emit focus(qobject_cast<QLineEdit *> (obj));
		}

		return QWidget::eventFilter(obj, event);
	}

	ManualWidget::ManualWidget(QWidget *parent)
		:QWidget(parent)
		,mFocusWidget(0)
	{
		QHBoxLayout *layout = new QHBoxLayout(this);
		mMidLayout = new QVBoxLayout;
		mIp = new EditableWidget("IP address");
		mNetmask = new EditableWidget("Netmask");
		mGateway = new EditableWidget("Default gateway");
		mDnsSever = new EditableWidget("DNS server");

		mSign = new QPushButton;
		mCancel = new QPushButton;

		mSign->setIcon(QIcon(":/icons/manual_signin.png"));
		mCancel->setIcon(QIcon(":/icons/manual_cancel.png"));
		mSign->setIconSize(QSize(96, 28));
		mCancel->setIconSize(QSize(96, 28));
		mSign->setFlat(true);
		mCancel->setFlat(true);

		mButtonLayout = new QHBoxLayout;
		mButtonLayout->addStretch(1);
		mButtonLayout->addWidget(mSign);
		mButtonLayout->addWidget(mCancel);
		mButtonLayout->addStretch(1);

		mMidLayout->addWidget(mIp);
		mMidLayout->addWidget(mNetmask);
		mMidLayout->addWidget(mGateway);
		mMidLayout->addWidget(mDnsSever);
		mMidLayout->addStretch(1);
		mMidLayout->addLayout(mButtonLayout);

		mNetmask->lineEdit()->setText("255.255.255.0");

		mMidLayout->setContentsMargins(0, 0, 0, 0);
		mMidLayout->setSpacing(0);
		//onFocus(mIp);

		mInputMethod = new InputMethod;
		mInputMethod->layout()->setRowStretch(0, 0);
		Util::setTopMargins(mInputMethod, 30);
		layout->addLayout(mMidLayout);
		layout->addWidget(mInputMethod);

		connect(mIp, SIGNAL(focus(QLineEdit *)), SLOT(onFocusChanged(QLineEdit *)));
		connect(mNetmask, SIGNAL(focus(QLineEdit *)), SLOT(onFocusChanged(QLineEdit *)));
		connect(mGateway, SIGNAL(focus(QLineEdit *)), SLOT(onFocusChanged(QLineEdit *)));
		connect(mDnsSever, SIGNAL(focus(QLineEdit *)), SLOT(onFocusChanged(QLineEdit *)));
		connect(mInputMethod, SIGNAL(input(QString)), SLOT(onTextInput(QString)));
		connect(mInputMethod, SIGNAL(del()), SLOT(onTextDel()));
		connect(mInputMethod, SIGNAL(done()), SLOT(onInputDone()));
		connect(mSign, SIGNAL(clicked()), SIGNAL(signIn()));
		//content(mCancel, SIGNAL(clicked()), SLOT(hide()));

		//mFocusWidget = mIp->lineEdit();
		mInputMethod->hide();
	}

	ManualWidget::~ManualWidget()
	{
		delete mIp;
		delete mNetmask;
		delete mGateway;
		delete mDnsSever;
		delete mSign;
		delete mCancel;
		delete mButtonLayout;
		delete mInputMethod;
		delete mMidLayout;
	}

	void ManualWidget::onFocusChanged(QLineEdit *widget)
	{
		if (widget != mFocusWidget){
			mFocusWidget = widget;
			mInputMethod->show();
		}
	}

	void ManualWidget::onTextInput(QString text)
	{
		if (mFocusWidget){
			mFocusWidget->insert(text);
		}
	}
	
	void ManualWidget::onTextDel()
	{
		if (mFocusWidget){
			mFocusWidget->backspace();
		}
	}

	void ManualWidget::onInputDone()
	{
		mInputMethod->hide();
	}

}
