#ifndef  WIFILISTBUTTON_H_
#define  WIFILISTBUTTON_H_

#include "launchertypes.h"

#include <QPushButton>
#include <QPixmap>
#include <QLabel>

class QWidget;
class QHBoxLayout;
class QPaintEvent;

namespace ui {

	class WifiListButton : public QPushButton {
		Q_OBJECT

		public :
			explicit WifiListButton(WifiInfo info, QPixmap level, QWidget *parent = 0);
			~WifiListButton();

			QString ssid() {return mSsidLabel->text();}
			void setSsid(QString &ssid) {mSsidLabel->setText(ssid);}
			void setLevelPixmap(QPixmap pixmap) {mLevelPixmap->setPixmap(pixmap);}
			WifiInfo data() { return mData; }

		protected :
			void paintEvent(QPaintEvent *e);

#if 0
		signals :
			void active();

		private slots :
			void onClicked();
#endif

		private :
			QLabel *mSsidLabel;
			QLabel *mLevelPixmap;
			QHBoxLayout *mItemLayout;
			WifiInfo mData;
	};
}



#endif  /*WIFILISTBUTTON_H*/
