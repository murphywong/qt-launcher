#ifndef  LOGINWIDGET_H_
#define  LOGINWIDGET_H_

#include "launchertypes.h"

#include <QWidget>
#include <QPushButton>

class QHBoxLayout;
class QLabel;
class QCheckBox;
class QPushButton;
class QLineEdit;
class QVBoxLayout;
class QPaintEvent;
class QShowEvent;

namespace ui {
	class InputMethod;
	class LoginWidget : public QWidget {
		Q_OBJECT

		public :
			explicit LoginWidget(WifiInfo info, QWidget *parent = 0);
			~LoginWidget();

		protected :
			void paintEvent(QPaintEvent *e);
			void showEvent(QShowEvent *e);

		signals :
			void login(WifiInfo info);
			void cancel();

		private slots :
			void onTextInput(QString);
			void onTextDel();
			void onSignIn();
			void onCancel();
			void onStateChanged(bool);

		private :
			QLabel *mNameLabel;
			QLineEdit *mLineEdit;
			QCheckBox *mCheckBox;
			QPushButton *mSignIn;
			QPushButton *mCancel;
			QVBoxLayout *mTopLayout;
			QVBoxLayout *mMidLayout;
			QHBoxLayout *mButtonLayout;
			QVBoxLayout *mLoginLayout;
			InputMethod *mInput;
			WifiInfo mData;
	};

	class CheckButoon : public QPushButton {
		Q_OBJECT

		public :
			explicit CheckButoon(QWidget *parent = 0);

		signals :
			void stateChanged(bool);

		private slots :
			void onClicked();

		private :
			bool state;
			QLabel *mBox;
	};
}




#endif  /*LOGINWIDGET_H*/
