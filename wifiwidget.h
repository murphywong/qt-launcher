#ifndef  WIFIWIDGET_H_
#define  WIFIWIDGET_H_

#include "basewidget.h"
#include "wifimanager.h"
#include "launchertypes.h"

#include <QScrollArea>
#include <QList>

class QVBoxLayout;
class QPaintEvent;

namespace ui {
	class SwitchBar;
	class LoginWidget;
	class WifiListButton;
	class ScrollWidget : public QWidget {
		Q_OBJECT

		public :
			explicit ScrollWidget(QWidget *parent = 0);

		protected :
			void paintEvent(QPaintEvent *e);
	};

	class WifiWidget : public BaseWidget {
		Q_OBJECT

		public :
			explicit WifiWidget(QWidget *parent = 0);
			~WifiWidget();

		private slots :
			void onSwitchStatusChanged(bool);
			void onUpdateResults();
			void onButtonClicked();
			void onLogin(WifiInfo);
			void onLoginCancel();
			void onObatinIp(QString, bool);
			void onWpaEvent(WpaEventType);

		private :
			WifiListButton *findButton(QString ssid);
			void clear();

			SwitchBar *mTopBar;
			LoginWidget *login;
			QVBoxLayout *mItemLayout;
			QList<WifiListButton *> mButtons;
			QString currentSsid;
			bool isConnected;
			bool isWlanOn;
			bool isConnecting;
	};
}



#endif  /*WIFIWIDGET_H*/
