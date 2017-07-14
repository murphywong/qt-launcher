#ifndef  MAINWINDOW_H
#define  MAINWINDOW_H

#include "toolbar.h"
#include "basewidget.h"

#include <QProcess>

class QVBoxLayout;
class QGridLayout;
class QKeyEvent;
class QFocusEvent;
class QWidget;
class QTimer;

namespace ui {
	class CameraWidget;

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
	class Broswer;
#endif

	class NetWidget;
	class WifiWidget;
	class FileManager;
	class KeyWidget;
	class PaintingWidget;
	class BluetoothWidget;
	class MainWindow : public BaseWidget {
		Q_OBJECT
		public :
			explicit MainWindow(QWidget *parent = 0);
			~MainWindow();

			void start();

#if 0
		protected :
			void focusInEvent(QFocusEvent *);
			//void keyPressEvent(QKeyEvent *e);
#endif

		protected slots :
			void onToolHomeClicked();
			void onToolBackClicked();

		private slots:
			//void onHomeAction();
			void onWifiButtonClicked();
			void onBlueButtonClicked();
			void onNetButtonClicked();
			void onInternetButtonClicked();
			void onKeyButtonClicked();
			void onPaintingButtonClicked();
			void onCameraButtonClicked();
			void onVideoButtonClicked();
			void onGameButtonClicked();
			void onTFButtonClicked();
			void onOtherButtonClicked();
			void processFinished(int, QProcess::ExitStatus);
			void onLogoTimeout();
			void onSingleTimeout();
			void onItemDestroyed();

		private :
			void initButtons();
			bool canExec();

			QVBoxLayout *mCentralLayout;
			QGridLayout *mItemLayout;
			QLabel *mLogo;
			QTimer *mLogoTimer;
			bool clickable;
			int mLogoIndex;

			CameraWidget *mCarmera;
			NetWidget *mEthernet;
			WifiWidget *mWifi;

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
			Broswer *mBrowser;
#endif
			FileManager *mTFCard;
			BluetoothWidget *mBluetooth;
			KeyWidget *mKey;
			PaintingWidget *mPainting;
	};
}

#endif  /*MAINWINDOW_H*/
