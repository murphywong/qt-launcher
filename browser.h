#ifndef  BROWSER_H
#define  BROWSER_H

#include "basewidget.h"

#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QUrl>
#include <QString>
#include <QWebView>

class QWidget;
class QComboBox;

namespace ui {
	class AddressBar : public QWidget
	{
		Q_OBJECT
		public:
			explicit AddressBar(QWidget *parent = 0);    
			~AddressBar();

	signals:
			void go(QUrl);

			public slots:
				void goToSite();

		private:
			//QLineEdit *mAddressEdit;
			QComboBox *mAddressEdit;
			QPushButton *mGoButton;
			QHBoxLayout *mLayout;
	};

	class HtmlView : public QWebView
	{
		Q_OBJECT
		public:
			explicit HtmlView(QWidget *parent = 0);

		signals:

		public slots:
			void loadNewPage(const QUrl &url);

	};

	class Broswer : public BaseWidget {
		Q_OBJECT

		public :
			explicit Broswer(QWidget *parent = 0);
			~Broswer();

		private :
			AddressBar *mBar;
			HtmlView *mView;
	};

}


#endif  /*BROWSER_H*/
