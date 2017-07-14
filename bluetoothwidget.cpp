#include "bluetoothwidget.h"
#include "wifiwidget.h"
#include "toolbar.h"
#include "util.h"

#include <stdio.h>
#include <unistd.h>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include <QtDebug>

#define BUFSIZE	2046

namespace ui {
	BluetoothWidget::BluetoothWidget(QWidget *parent)
		:BaseWidget(QPixmap(":/icons/net_background2.png"), parent)
	{
		QScrollArea *scrollArea = new QScrollArea(this);
		ScrollWidget *widget = new ScrollWidget(this);
		mItemLayout = new QVBoxLayout(widget);
		scrollArea->setWidget(widget);
		scrollArea->setWidgetResizable(true);
		scrollArea->setFrameShape(QFrame::NoFrame);
		scrollArea->setFixedHeight(430);

		centralLayout()->addWidget(scrollArea);
		centralLayout()->addStretch(1);
		setFixedSize(QSize(800, 480));

		QLabel *label = new QLabel("Scanning ... ");
		label->setAlignment(Qt::AlignCenter);
		label->setStyleSheet("color:white");
		mItemLayout->addWidget(label);
		mLabels << label;

		CmdThread *thread = new CmdThread(this);
		connect(thread, SIGNAL(updateData(QString)), SLOT(updateList(QString)));
		thread->start();

	}

	BluetoothWidget::~BluetoothWidget()
	{
	}

	void BluetoothWidget::updateList(QString result)
	{
		QLayoutItem *child;
		while ((child = mItemLayout->takeAt(0)) != 0) {
			delete child;
		}

		foreach(QLabel *label, mLabels) {
			delete label;
		}
		mLabels.clear();

		qDebug() << result;
		if (!result.isEmpty()){
			foreach(QString line, result.split(QRegExp("\\n")))
			{
				if (line.contains("Scanning")){
					continue;
				}

				QLabel *label = new QLabel(line);
				label->setStyleSheet("color:white");
				mItemLayout->addWidget(label);
				mLabels << label;
			}

			mItemLayout->addStretch(1);
		} else {
			QLabel *label = new QLabel("No item found!");
			label->setAlignment(Qt::AlignCenter);
			mItemLayout->addWidget(label);
			label->setStyleSheet("color:white");
			mLabels << label;
			//mItemLayout->addStretch(1);
		}
	}

	CmdThread::CmdThread(QObject *parent)
		:QThread(parent)
	{
	}

	void CmdThread::run()
	{
		char buffer[BUFSIZE] = {0};
		FILE *fp = 0;
		//int ret = 0;
		popen("cciconfig hci0 up", "r");
		fp = popen("hcitool scan", "r");
		if (fp){
			//ret = fread(buffer, sizeof(char), BUFSIZE - 1, fp);
			fread(buffer, sizeof(char), BUFSIZE - 1, fp);
			//if (ret == 0){
				//qDebug() << "no data found!";
				//return ;
			//}
		} else {
			perror("popen");
			return ;
		}
		pclose(fp);

		emit updateData(QString(buffer));
	}
}
