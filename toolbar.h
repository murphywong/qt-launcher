#ifndef  TOOLBAR_H
#define  TOOLBAR_H

#include "launchertypes.h"

#include  <QWidget>

class QHBoxLayout;
class QPushButton;
class QPaintEvent;
class QLabel;

namespace ui {
	class ToolBar : public QWidget {
		Q_OBJECT
		
		public :

			explicit ToolBar(QWidget *parent = 0);
			~ToolBar();

			void setIndicateVisible(IndicateType type, bool visible);

		protected :
			void paintEvent(QPaintEvent *event);

		signals:
			void backAction();
			void homeAction();
			
		private slots :
			void onWpaEvent(WpaEventType);
			void onObatinIp(QString, bool);

		private :
			QHBoxLayout *mMainLayout;
			QHBoxLayout *mBottomLayout;
			QPushButton *mHomeButton;
			QPushButton *mBackButton;
			QLabel *mWifi;
			QLabel *mBlue;
			QLabel *mNet;
			QLabel *mTimeLabel;
	};
}




#endif  /*TOOLBAR_H*/
