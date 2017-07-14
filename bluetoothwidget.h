#include "basewidget.h"

#include <QThread>

class QLabel;
class QVBoxLayout;

namespace ui {
	class BluetoothWidget : public BaseWidget {
		Q_OBJECT

		public :
			explicit BluetoothWidget(QWidget *parent = 0);
			~BluetoothWidget();

		private slots :
			void updateList(QString);

		private :
			QVector<QLabel *> mLabels;
			QVBoxLayout *mItemLayout;
	};

	class CmdThread : public QThread {
		Q_OBJECT

		public :
			explicit CmdThread(QObject *parent = 0);
			void run();

		signals :
			void updateData(QString);

	};

}
