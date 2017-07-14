#ifndef  KEYWIDGET_H_
#define  KEYWIDGET_H_

#include "basewidget.h"

class QKeyEvent;
class QLabel;

namespace ui {
	class KeyWidget : public BaseWidget {
		Q_OBJECT

		public :
			explicit KeyWidget(QWidget *parent = 0);

		protected :
			void keyPressEvent(QKeyEvent *);

		private :
			QLabel *mKeyLabel;
	};
}




#endif  /*KEYWIDGET_H*/
