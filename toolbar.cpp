#include "util.h"
#include "toolbar.h"
#include "wifimanager.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QPaintEvent>
#include <QDateTime>
#include <QPainter>
#include <QLabel>
#include <QIcon>
#include <QtDebug>
#include <QList>
#include <QNetworkInterface>

namespace ui {

	ToolBar::ToolBar(QWidget *parent)
		:QWidget(parent)
	{
		mMainLayout = new QHBoxLayout(this);
		mBottomLayout = new QHBoxLayout;
		mBackButton = new QPushButton;
		mHomeButton = new QPushButton;
		mWifi = new QLabel;
		mBlue = new QLabel;
		mNet = new QLabel;
		mTimeLabel = new QLabel(QDateTime::currentDateTime().toString("hh:mm"));
		mTimeLabel->setStyleSheet("color:white");
		Util::setFontSize(mTimeLabel, 10);

		mBackButton->setIcon(QIcon(":/icons/back2.png"));
		mHomeButton->setIcon(QIcon(":/icons/home2.png"));
		mBackButton->setFlat(true);
		mHomeButton->setFlat(true);

		mWifi->setPixmap(QPixmap(":/icons/wifi_s2.png"));
		mBlue->setPixmap(QPixmap(":/icons/bluetooth_s2.png"));
		mNet->setPixmap(QPixmap(":/icons/net_s2.png"));

		mMainLayout->addWidget(mBackButton);
		mMainLayout->addWidget(mHomeButton);
		mMainLayout->addStretch(1);
		mMainLayout->addWidget(mTimeLabel);
		mMainLayout->addStretch(1);
		mMainLayout->addLayout(mBottomLayout);
		mBottomLayout->addWidget(mNet);
		mBottomLayout->addWidget(mBlue);
		mBottomLayout->addWidget(mWifi);

		mMainLayout->setContentsMargins(40, 0, 0, 0);

		mNet->hide();
		mBlue->hide();
		mWifi->hide();

		connect(mHomeButton, SIGNAL(clicked()), SIGNAL(homeAction()));
		connect(mBackButton, SIGNAL(clicked()), SIGNAL(backAction()));
		connect(&WiFiManager::instance(), SIGNAL(wpaEvent(WpaEventType)),
				SLOT(onWpaEvent(WpaEventType)));
		connect(&WiFiManager::instance(), SIGNAL(obtainIpStatus(QString, bool)),
				SLOT(onObatinIp(QString, bool)));

		QMargins margins = mBottomLayout->contentsMargins();
		margins.setRight(20);
		mBottomLayout->setContentsMargins(margins);
		mBottomLayout->setSpacing(15);

		QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
		for (int i = 0; i < interfaces.size(); i++)
		{
			QNetworkInterface::InterfaceFlags flag = interfaces.at(i).flags();
			QString name = interfaces.at(i).humanReadableName();
			if (name == ETHERNET_IFACE && 
					flag & QNetworkInterface::IsUp && flag & QNetworkInterface::IsRunning){
				mNet->show();
			}
			if (name == WIRELESS_IFACE && 
					flag & QNetworkInterface::IsUp && flag & QNetworkInterface::IsRunning){
				mWifi->show();
			}
		}
	}

	ToolBar::~ToolBar()
	{
		delete mHomeButton;
		delete mBackButton;
		delete mWifi;
		delete mBlue;
		delete mNet;
	}

	void ToolBar::paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);
		painter.drawPixmap(0, 0, QPixmap(":/icons/tool.png").scaled(size()));
		QWidget::paintEvent(event);
	}

	void ToolBar::onWpaEvent(WpaEventType type)
	{
		qDebug() << __FILE__ << __func__ << __LINE__ << type;
		if (type == WpaEventDisconnected){
			mWifi->hide();
		}
	}

	void ToolBar::onObatinIp(QString ssid, bool success)
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
        Q_UNUSED(ssid);

		if (success){
			mWifi->show();
		}
	}

	void ToolBar::setIndicateVisible(IndicateType type, bool visible)
	{
		switch (type){
			case IndicateWiFi :
				mWifi->setVisible(visible);
				break;
			case IndicateBluetooth :
				mBlue->setVisible(visible);
				break;
			case IndicateEthernet:
				mNet->setVisible(visible);
				break;
			default :
				break;
		}
	}
}
