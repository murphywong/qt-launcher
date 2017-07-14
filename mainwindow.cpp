#include "mainwindow.h"
#include "actionbutton.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include "browser.h"
#endif

#include "switchbar.h"
#include "wifiwidget.h"
#include "wifimanager.h"
#include "netwidget.h"
#include "filemanager.h"
#include "camerawidget.h"
#include "keywidget.h"
#include "paintingwidget.h"
#include "bluetoothwidget.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include "samegame/qmlapplicationviewer/qmlapplicationviewer.h"
#endif

#include <QVBoxLayout>
#include <QGridLayout>
#include <QKeyEvent>
#include <QTimer>
#include <QtDebug>
#include <QMessageBox>
#include <QNetworkInterface>

namespace ui {
	MainWindow::MainWindow(QWidget *parent)
		:BaseWidget(QPixmap(":/icons/background.png"), parent, 0)
		,clickable(true)
		,mLogoIndex(0)
		,mCarmera(0)
		,mEthernet(0)
		,mWifi(0)
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
		,mBrowser(0)
#endif
		,mTFCard(0)
		,mBluetooth(0)
		,mKey(0)
		,mPainting(0)
	{
		mCentralLayout = centralLayout();
		mItemLayout = new QGridLayout;

		mCentralLayout->addLayout(mItemLayout);
		mCentralLayout->addStretch(1);

		initButtons();
		setFixedSize(800, 480);
		mItemLayout->setVerticalSpacing(20);
		QMargins margins = mItemLayout->contentsMargins();
		margins.setTop(50);
		mItemLayout->setContentsMargins(margins);
		setWindowTitle("Qt Launcher");
	}

	MainWindow::~MainWindow()
	{
		delete mItemLayout;

		if (mCarmera){
			delete mCarmera;
		}


#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
		if (mBrowser){
			delete mBrowser;
		}
#endif

		if (mEthernet){
			delete mEthernet;
		}

		if (mWifi){
			delete mWifi;
		}

		if (mTFCard){
			delete mTFCard;
		}

		if (mKey){
			delete mKey;
		}

		if (mBluetooth){
			delete mBluetooth;
		}

#if 0
		if (mIrda){
			delete mIrda;
		}
#endif
	}

#if 0
	void MainWindow::keyPressEvent(QKeyEvent *event)
	{
		switch ( event->key() )
		{
			case Qt::Key_Escape :
				close();
				break;
		}
	}
#endif

	void MainWindow::onToolHomeClicked()
	{
		activateWindow();
	}

	void MainWindow::onToolBackClicked()
	{
		//close();
	}

	void MainWindow::initButtons()
	{
		ActionButton *wifiButton = new ActionButton(0, 
											QIcon(":/icons/wifi2.png"), 
											QPixmap(":/icons/wifi_l.png"), 
											this);
		ActionButton *blueButton = new ActionButton(1, 
											QIcon(":/icons/bluetooth2.png"), 
											QPixmap(":/icons/bluetooth_l.png"), 
											this);
		ActionButton *netButton = new ActionButton(2, 
											QIcon(":/icons/net2.png"), 
											QPixmap(":/icons/net_l.png"), 
											this);
		ActionButton *internetButton = new ActionButton(3, 
											QIcon(":/icons/internet2.png"), 
											QPixmap(":/icons/internet_l.png"), 
											this);
		ActionButton *keyButton = new ActionButton(4, 
											QIcon(":/icons/key2.png"), 
											QPixmap(":/icons/irda_l.png"), 
											this);
#if 0
		ActionButton *irdaButton = new ActionButton(5, 
											QIcon(":/icons/irda2.png"), 
											QPixmap(":/icons/irda_l.png"), 
											this);
#endif
		ActionButton *paintingButton = new ActionButton(5, 
											QIcon(":/icons/painting2.png"), 
											QPixmap(":/icons/painting_l.png"), 
											this);
		ActionButton *cameraButton = new ActionButton(6, 
											QIcon(":/icons/camera2.png"), 
											QPixmap(":/icons/camera_l.png"), 
											this);
		ActionButton *videoButton = new ActionButton(7, 
											QIcon(":/icons/video2.png"), 
											QPixmap(":/icons/video_l.png"), 
											this);
		ActionButton *gameButton = new ActionButton(8, 
											QIcon(":/icons/game2.png"), 
											QPixmap(":/icons/game_l.png"), 
											this);
		ActionButton *tfButton = new ActionButton(9, 
											QIcon(":/icons/IFcard2.png"), 
											QPixmap(":/icons/IFcard_l.png"), 
											this);
		ActionButton *otherButton = new ActionButton(10, 
											QIcon(":/icons/other2.png"), 
											QPixmap(":/icons/other_l.png"), 
											this);

		mItemLayout->addWidget(wifiButton, 0, 0);
		mItemLayout->addWidget(blueButton, 0, 1);
		mItemLayout->addWidget(netButton, 0, 2);
		mItemLayout->addWidget(internetButton, 0, 3);
		mItemLayout->addWidget(keyButton, 0, 4);
		mItemLayout->addWidget(paintingButton, 1, 0);
		mItemLayout->addWidget(cameraButton, 1, 1);
		mItemLayout->addWidget(videoButton, 1, 2);
		mItemLayout->addWidget(gameButton, 1, 3);
		mItemLayout->addWidget(tfButton, 1, 4);
		mItemLayout->addWidget(otherButton, 2, 0);

		connect(wifiButton, SIGNAL(clicked()), SLOT(onWifiButtonClicked()));
		connect(blueButton, SIGNAL(clicked()), SLOT(onBlueButtonClicked()));
		connect(netButton, SIGNAL(clicked()), SLOT(onNetButtonClicked()));
		connect(internetButton, SIGNAL(clicked()), SLOT(onInternetButtonClicked()));
		connect(keyButton, SIGNAL(clicked()), SLOT(onKeyButtonClicked()));
		connect(paintingButton, SIGNAL(clicked()), SLOT(onPaintingButtonClicked()));
		connect(cameraButton, SIGNAL(clicked()), SLOT(onCameraButtonClicked()));
		connect(videoButton, SIGNAL(clicked()), SLOT(onVideoButtonClicked()));
		connect(gameButton, SIGNAL(clicked()), SLOT(onGameButtonClicked()));
		connect(tfButton, SIGNAL(clicked()), SLOT(onTFButtonClicked()));
		connect(otherButton, SIGNAL(clicked()), SLOT(onOtherButtonClicked()));
	}

	void MainWindow::onWifiButtonClicked()
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		if (canExec()) {
			if (mWifi) {
				delete mWifi;
			}
			mWifi = new WifiWidget;
			connect(mWifi, SIGNAL(quit()), SLOT(onItemDestroyed()));
			mWifi->show();
		}
	}

	void MainWindow::onBlueButtonClicked()
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		if (canExec()) {
			if (mBluetooth) {
				delete mBluetooth;
			}

			mBluetooth = new BluetoothWidget;
			mBluetooth->show();
		}
	}

	void MainWindow::onNetButtonClicked()
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		if (canExec()) {
			if (mEthernet) {
				delete mEthernet;
			}
			mEthernet = new NetWidget;
			connect(mEthernet, SIGNAL(quit()), SLOT(onItemDestroyed()));
			mEthernet->show();
		}
	}

	void MainWindow::onInternetButtonClicked()
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
		if (canExec()) {
			if (mBrowser) {
				delete mBrowser;
			}
			mBrowser = new Broswer;
			mBrowser->show();
		}
#endif
	}

	void MainWindow::onKeyButtonClicked()
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		if (canExec()) {
			if (mKey) {
				delete mKey;
			}
			mKey = new KeyWidget;
			mKey->show();
		}
	}

	void MainWindow::onPaintingButtonClicked()
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		if (canExec()) {
#if 0
			if (mIrda){
				delete mIrda;
			}
#endif
			mPainting = new PaintingWidget;
			mPainting->show();
		}
	}

	void MainWindow::onCameraButtonClicked()
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		if (canExec()){
			if (mCarmera) {
				delete mCarmera;
			}

			if (QFile("/dev/video0").exists()) {
				mCarmera = new CameraWidget;
				mCarmera->show();
			} else {
				QMessageBox::warning(this,tr("error"),tr("Camera not found!"),QMessageBox::Ok);
			}
		}
	}

	void MainWindow::onVideoButtonClicked()
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		if (canExec()) {
			//::system("./CedarXPlayerTest testaac.avi");
			clickable = false;
			QProcess *process = new QProcess;
			connect(process, SIGNAL(finished(int, QProcess::ExitStatus)),
					SLOT(processFinished(int, QProcess::ExitStatus)));
			QString cmd = QString("vlc %1/%2").arg(VIDEO_PATH).arg(VIDEO_FILENAME);
			qDebug() << "cmd is:" << cmd;
			process->start(cmd);
		}
	}

	void MainWindow::onGameButtonClicked()
	{
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
		qDebug() << __FILE__ << __func__ << __LINE__;
		if (canExec()) {
#if 0
			MineWidget *mine = new MineWidget;
			mine->show();
#endif
			QmlApplicationViewer *viewer = new QmlApplicationViewer;
			viewer->setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
			viewer->setMainQmlFile(QLatin1String("samegame/qml/samegame/samegame.qml"));
			viewer->showExpanded();
		}
#endif
	}

	void MainWindow::onTFButtonClicked()
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		if (canExec()) {
			if (mTFCard) {
				delete mTFCard;
			}

			mTFCard = new FileManager(QDir(TFCARD_MOUNT_PATH));
			mTFCard->show();
		}
	}

	void MainWindow::onOtherButtonClicked()
	{
		if (canExec()) {
		}
		qDebug() << __FILE__ << __func__ << __LINE__;
	}

	void MainWindow::processFinished(int exitCode, QProcess::ExitStatus status)
	{
		qDebug() << __FILE__ << __func__ << __LINE__ << exitCode << status;
		clickable = true;
		activateWindow();
		update();

		delete sender();
	}

	bool MainWindow::canExec()
	{
		qDebug() << "can exec? " << clickable;
		return clickable;
	}

	void MainWindow::onLogoTimeout()
	{
		if (mLogoIndex > 30){
			mLogoIndex = 0;
		}
		mLogo->setPixmap(QPixmap(QString(":/icons/startup/%1-01.png").arg(mLogoIndex)));
		mLogo->update();
		++mLogoIndex;
	}

	void MainWindow::onSingleTimeout()
	{
		delete mLogo;
		delete mLogoTimer;
		show();
	}

	void MainWindow::start()
	{

#if 0
		mLogo = new QLabel(0, Qt::FramelessWindowHint);
		mLogo->setPixmap(QPixmap(":/icons/startup/0-01.png"));
		mLogo->setFixedSize(QSize(800, 480));
		mLogoTimer = new QTimer;
		mLogoTimer->start(50);
		connect(mLogoTimer, SIGNAL(timeout()), SLOT(onLogoTimeout()));
		QTimer::singleShot(3000, this, SLOT(onSingleTimeout()));
		mLogo->show();
#endif
		show();
	}

	void MainWindow::onItemDestroyed()
	{
		QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
		for (int i = 0; i < interfaces.size(); i++)
		{
			QNetworkInterface::InterfaceFlags flag = interfaces.at(i).flags();
			QString name = interfaces.at(i).humanReadableName();
			if (name == ETHERNET_IFACE && 
					flag & QNetworkInterface::IsUp && flag & QNetworkInterface::IsRunning){
				toolBar()->setIndicateVisible(IndicateEthernet, true);
			}
			if (name == WIRELESS_IFACE && 
					flag & QNetworkInterface::IsUp && flag & QNetworkInterface::IsRunning){
				toolBar()->setIndicateVisible(IndicateWiFi, true);
			}
		}
	}
}
