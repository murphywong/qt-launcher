#ifndef  ACTIONBUTTON_H
#define  ACTIONBUTTON_H

#include "actionbase.h"

#include <QPushButton>
#include <QLabel>

class QWidget;
class QIcon;
class QPixmap;
class QHBoxLayout;

namespace ui {
	class ActionButton : public QPushButton, ActionBase {
		Q_OBJECT
		
		public :
			explicit ActionButton(QWidget *parent = 0);
			explicit ActionButton(int index, QIcon button, QPixmap label, QWidget *parent = 0);
			~ActionButton();

			QLabel *label() {return mLabel;}
			QPushButton *Button() {return mButton;}

			void setButtonIcon(QIcon icon) {mButton->setIcon(icon);}
			void setLabelPixmap(QPixmap pixmap) {mLabel->setPixmap(pixmap);}
			
			void init();
			int index() {return mIndex;}

		private :
			void createLayout();

			QLabel *mLabel;
			QPushButton *mButton;
			QHBoxLayout *mHLayout;

			int mIndex;
	};
}




#endif  /*ACTIONBUTTON_H*/
