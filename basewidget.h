#ifndef  BASEWIDGET_H_
#define  BASEWIDGET_H_

#include <QWidget>
#include <QPixmap>

class QVBoxLayout;
class QPaintEvent;

namespace ui {
	class ToolBar;
	class BaseWidget : public QWidget {
		Q_OBJECT

		public :
			explicit BaseWidget(QWidget *parent = 0, Qt::WindowFlags flags = Qt::FramelessWindowHint);
			explicit BaseWidget(QPixmap background, QWidget *parent = 0, Qt::WindowFlags flags = Qt::FramelessWindowHint);
			virtual ~BaseWidget();

			ToolBar *toolBar() {return mBar;}
			QVBoxLayout *centralLayout() {return mCentralLayout;}

		protected :
			void paintEvent(QPaintEvent *e);

		signals :
			void quit();

		protected slots :
			virtual void onToolBackClicked();
			virtual void onToolHomeClicked();

		private :
			void createLayout();

			ToolBar *mBar;
			QVBoxLayout *mCentralLayout;
			QPixmap mBackgroundPixmap;
	};
}




#endif  /*BASEWIDGET_H*/
