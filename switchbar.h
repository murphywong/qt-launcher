#ifndef  SWITCHBAR_H_
#define  SWITCHBAR_H_

#include <QWidget>
#include <QPixmap>
#include <QIcon>
#include <QPushButton>
#include <QLabel>

class QPaintEvent;

namespace ui {
	class SwitchBar : public QWidget {
		Q_OBJECT

		public :
			explicit SwitchBar(QPixmap background, QPixmap icon, 
					QPixmap title, QIcon onIcon,
					QIcon offIcon, QWidget *parent = 0);
			~SwitchBar();
			
			void setSwitchStatus(bool on);
			void setSwitchIconSize(QSize &size) { mSwitch->setIconSize(size); }
			void setNotify(QString notify) { mNotify->setText(notify); }

		protected :
			void paintEvent(QPaintEvent *e);

		signals :
			void switchAction(bool);

		private slots :
			void onSwitchClicked();

		private :
			void actionChanged();

			QLabel *mNotify;
			QLabel *mIcon;
			QLabel *mTitle;
			QPushButton *mSwitch;
			QPixmap mBackground;
			QIcon mOnIcon;
			QIcon mOffIcon;

			bool mSwitchStatus;
	};
}

#endif  /*SWITCHBAR_H*/
