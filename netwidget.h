#ifndef  NETWIDGET_H_
#define  NETWIDGET_H_

#include "basewidget.h"

#include <QLabel>
#include <QLineEdit>
#include <QProcess>

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QPaintEvent;
class QHBoxLayout;
class QPushButton;
class QEvent;

namespace ui {
	class SwitchBar;
	class AutoWidget;
	class EtherNet;
	class ManualWidget;
	class InputMethod;
	class NetWidget : public BaseWidget {
		Q_OBJECT

		public :
			explicit NetWidget(QWidget *parent = 0);
			~NetWidget();

			void startDhcp();

		private slots :
			void onStateChanged(bool);
			void processFinished(int, QProcess::ExitStatus);
			void onSignIn();


		private :
			SwitchBar *mTopBar;
			AutoWidget *mAutoItem;
			ManualWidget *mManualItem;
			QVBoxLayout *mLayout;
	};

	class PropertyWidget : public QWidget {
		Q_OBJECT

		public :
			explicit PropertyWidget(QString name, QString property = QString(), QWidget *parent = 0);
			~PropertyWidget();

			void setName(QString name) { mName->setText(name); }
			void setProperty(QString property) { mProperty->setText(property); }
			void setBackground(QPixmap background) { mBackground = background; }
			QString name() { return mName->text(); }
			QString property() { return mProperty->text(); }

		protected :
			void paintEvent(QPaintEvent *e);

		private :
			QLabel *mName;
			QLabel *mProperty;
			QPixmap mBackground;
	};

	class EditableWidget: public QWidget {
		Q_OBJECT

		public :
			explicit EditableWidget(QString name, QString content = QString(), QWidget *parent = 0);
			~EditableWidget();

			QLineEdit *lineEdit() { return mLineEdit; } 
			QString name() { return mNameLabel->text(); }
			QString content() { return mLineEdit->text(); }
			void setName(QString name) { mNameLabel->setText(name); }
			void setContent(QString content) { mLineEdit->setText(content); }

		protected :
			void paintEvent(QPaintEvent *e);
			bool eventFilter(QObject *, QEvent *);

		signals :
			void focus(QLineEdit *);

		private :
			QLabel *mNameLabel;
			QLineEdit *mLineEdit;
	};

	class TitleWidget : public QLabel {
		Q_OBJECT

		public :
			explicit TitleWidget(QString title, QWidget *parent = 0);

		protected :
			void paintEvent(QPaintEvent *e);
	};

	class AutoWidget : public QWidget {
		Q_OBJECT

		public :
			explicit AutoWidget(QWidget *parent = 0);
			~AutoWidget();
			
			void updateConfig();
			bool autoDhcp();

		private :
			PropertyWidget *mDevice;
			PropertyWidget *mMac;
			PropertyWidget *mIp;
			PropertyWidget *mNetmask;
			PropertyWidget *mGateway;
			PropertyWidget *mDns;
			EtherNet *mConf;
	};

	class ManualWidget : public QWidget {
		Q_OBJECT

		public :
			explicit ManualWidget(QWidget *parent = 0);
			~ManualWidget();

			QString ip() { return mIp->content(); }
			QString netmask() { return mNetmask->content(); }
			QString gateway() { return mGateway->content(); }
			QString dns() { return mDnsSever->content(); }

		signals :
			void signIn();

		private slots :
			void onFocusChanged(QLineEdit *);
			void onTextInput(QString);
			void onTextDel();
			void onInputDone();

		private :
			EditableWidget *mIp;
			EditableWidget *mNetmask;
			EditableWidget *mGateway;
			EditableWidget *mDnsSever;
			InputMethod *mInputMethod;

			QPushButton *mSign;
			QPushButton *mCancel;
			QHBoxLayout *mButtonLayout;
			QVBoxLayout *mMidLayout;
			QLineEdit *mFocusWidget;
	};
}




#endif  /*NETWIGET_H*/
