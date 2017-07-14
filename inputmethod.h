#ifndef  INPUTMETHOD_H_
#define  INPUTMETHOD_H_

#include <QWidget>

class QPushButton;
class QGridLayout;

namespace ui {
	class InputMethod : public QWidget {
		Q_OBJECT

		public :
			explicit InputMethod(QWidget *parent = 0);
			//~InputMethod();

			QGridLayout *layout() { return mLayout; }

		signals :
			void input(QString text);
			void del();
			void done();

		private slots :
			QPushButton *createButton(QString text);
			void onClicked();

		private :
			QGridLayout *mLayout;
	};
}




#endif  /*INPUTMETHOD_H*/
