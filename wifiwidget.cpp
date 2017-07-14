#include "wifiwidget.h"
#include "switchbar.h"
#include "toolbar.h"
#include "wifilistbutton.h"
#include "wifimanager.h"
#include "loginwidget.h"

#include <QIcon>
#include <QPixmap>
#include <QtDebug>
#include <QVBoxLayout>
#include <QPaintEvent>
#include <QPainter>

namespace ui {

	ScrollWidget::ScrollWidget(QWidget *parent)
		:QWidget(parent)
	{
	}

	void ScrollWidget::paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);
		painter.drawPixmap(0, 0, QPixmap(":/icons/wifi_background2.png").scaled(size()));
		QWidget::paintEvent(event);
	}

	WifiWidget::WifiWidget(QWidget *parent)
		:BaseWidget(QPixmap(":/icons/wifi_background2.png"), parent)
		,login(00000000)
		,isConnected(false)
		,isConnecting(false)
	{
		mTopBar = new SwitchBar(QPixmap(":/icons/switchbar_background.png"),
				QPixmap(":/icons/wifi_bar.png"), QPixmap(":/icons/wifi_title.png"),
				QIcon(":/icons/on_button.png"), QIcon(":/icons/off_button.png"));
		QScrollArea *scrollArea = new QScrollArea(this);
		ScrollWidget *widget = new ScrollWidget(this);
		mItemLayout = new QVBoxLayout(widget);
		scrollArea->setWidget(widget);
		scrollArea->setWidgetResizable(true);
		scrollArea->setFixedHeight(380);
		scrollArea->setFrameShape(QFrame::NoFrame);
		scrollArea->setWindowOpacity(1);
		centralLayout()->addWidget(mTopBar);
		centralLayout()->addWidget(scrollArea);
		centralLayout()->addStretch(1);
		mItemLayout->addStretch(1);

		connect(mTopBar, SIGNAL(switchAction(bool)), SLOT(onSwitchStatusChanged(bool)));
		connect(&WiFiManager::instance(), SIGNAL(obtainIpStatus(QString, bool)),
				SLOT(onObatinIp(QString, bool)));
		connect(&WiFiManager::instance(), SIGNAL(wpaEvent(WpaEventType)),
				SLOT(onWpaEvent(WpaEventType)));

		//check wifi init status
		connect(&WiFiManager::instance(), 
				SIGNAL(updateResults()),
				SLOT(onUpdateResults()));
		isWlanOn = WiFiManager::instance().isWlanOn();
		if (isWlanOn) {
			mTopBar->setSwitchStatus(true);
			WiFiManager::instance().scan();
		}

		setFixedSize(QSize(800, 480));
	}

	WifiWidget::~WifiWidget()
	{
		delete mTopBar;
		delete mItemLayout;
		delete login;

		foreach(WifiListButton *button, mButtons)
			delete button;
		mButtons.clear();
	}

	void WifiWidget::onSwitchStatusChanged(bool on)
	{
		qDebug() << __FILE__ << __func__ << __LINE__ << on;
		isWlanOn = on;
		if (on){
			mTopBar->setNotify("WiFi starting ...");
			if (!WiFiManager::instance().start()) {
				mTopBar->setNotify("Failed to start WiFi! Retry please!");
			}
		} else {
			mTopBar->setNotify("");
			toolBar()->setIndicateVisible(IndicateWiFi, false);
			clear();
			update();
			WiFiManager::instance().stop();
		}
	}

	void WifiWidget::onUpdateResults()
	{
		qDebug() << __FILE__ << __func__ << __LINE__;

		clear();

		if (!isConnecting){
			mTopBar->setNotify("");
		}

		if (!isWlanOn){
			return ;
		}

		QVector<WifiInfo> vector = WiFiManager::instance().scanResults();
		for (int i = 0; i < vector.size(); i++)
		{
			WifiInfo info = vector.at(i);
			WifiListButton *button = new WifiListButton(vector.at(i), 
					info.auth == AUTH_NONE_OPEN ? 
					QPixmap(":/icons/wifi_bar.png") : 
					QPixmap(QString(":/icons/signal%1.png").arg(info.level))); 
			mButtons << button;
			connect(button, SIGNAL(clicked()), SLOT(onButtonClicked()));
			mItemLayout->addWidget(button);
		}

		mItemLayout->addStretch(1);

		QString ssid = WiFiManager::instance().connectedSSID();
		if (!ssid.isEmpty()){
			isWlanOn = true;
			currentSsid = ssid;
			toolBar()->setIndicateVisible(IndicateWiFi, true);
			WifiListButton *button = findButton(ssid);
			if (button){
				button->setLevelPixmap(QPixmap(":/icons/wifi_connected.png"));
			}
		}
	}

	void WifiWidget::onButtonClicked()
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		WifiListButton *button = qobject_cast<WifiListButton *> (sender());

		if (button){
			const WifiInfo info = button->data();
			if (info.ssid == WiFiManager::instance().connectedSSID()){
				qDebug() << info.ssid << "aready connected!";
				return;
			}

			qDebug() << info.ssid << "clicked" << "Auth: " << info.auth;
			if (info.auth == AUTH_NONE_OPEN){
				onLogin(info);
			} else {
				if (login){
					delete login;
				}

				login = new LoginWidget(info);
				connect(login, SIGNAL(login(WifiInfo)), SLOT(onLogin(WifiInfo)));
				connect(login, SIGNAL(cancel()), SLOT(onLoginCancel()));
				login->show();
			}
		}
	}

	void WifiWidget::onLogin(WifiInfo info)
	{
		qDebug() << __FILE__ << __func__ << __LINE__ << "connect to: " << info.ssid << "password: " << info.pwd;
		mTopBar->setNotify(QString("Connecting to %1 ...").arg(info.ssid));
		isConnecting = true;
		WiFiManager::instance().connectToAp(info);
	}

	void WifiWidget::onLoginCancel()
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
	}

	void WifiWidget::onObatinIp(QString ssid, bool success)
	{
		qDebug() << __FILE__ << __func__ << __LINE__ << success;
		isConnecting = false;
		if (success){
			isConnected = true;
			currentSsid = ssid;
			mTopBar->setNotify("");
			WifiListButton *button = findButton(ssid);
			if (button){
				button->setLevelPixmap(QPixmap(":/icons/wifi_connected.png"));
			}
		} else {
			if (ssid == "pwderror"){
				mTopBar->setNotify("Password may by incorrect!");
			} else {
				mTopBar->setNotify(QString("Failed to connect %1").arg(ssid));
			}
		}
	}

	WifiListButton *WifiWidget::findButton(QString ssid)
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		foreach(WifiListButton *button, mButtons)
		{
			if (ssid == button->ssid()){
				return button;
			}
		}

		return 0;
	}

	void WifiWidget::clear()
	{
		QLayoutItem *child;
		while ((child = mItemLayout->takeAt(0)) != 0);
#if 1
		foreach(WifiListButton *button, mButtons)
			delete button;
			//button->hide();
#endif 
		mButtons.clear();
	}

	void WifiWidget::onWpaEvent(WpaEventType type)
	{
		if (type == WpaEventDisconnected){
			currentSsid = QString();
			WiFiManager::instance().scan();
		}
	}

} //end of namespace
