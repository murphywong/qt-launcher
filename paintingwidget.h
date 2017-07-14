#ifndef  PAINTINGWIDGET_H_
#define  PAINTINGWIDGET_H_
#include "basewidget.h"

#include <QPainterPath>

class QPaintEvent;
class QMouseEvent;

namespace ui {
	class PaintingWidget : public BaseWidget {
		Q_OBJECT

		public :
			explicit PaintingWidget(QWidget *parent = 0);
			~PaintingWidget();

		protected :
			void paintEvent(QPaintEvent *);
			void mousePressEvent(QMouseEvent *);
			void mouseReleaseEvent(QMouseEvent *);
			void mouseMoveEvent(QMouseEvent *);

		private :
			QPainterPath mPath;
			QPointF mPrevPointF;
	};
}

#endif  /*PAINTINGWIDGET_H*/
