#include "keywidget.h"

#include <QKeyEvent>
#include <QtDebug>
#include <QVBoxLayout>
#include <QLabel>

namespace ui {
	KeyWidget::KeyWidget(QWidget *parent)
		:BaseWidget(parent)
	{
		mKeyLabel = new QLabel("Press a key, please!", this);
		mKeyLabel->setAlignment(Qt::AlignCenter);
		centralLayout()->addStretch(1);
		centralLayout()->addWidget(mKeyLabel);
		centralLayout()->addStretch(1);

		setFixedSize(QSize(800, 480));
		setWindowTitle("Key Test");
	}

	void KeyWidget::keyPressEvent(QKeyEvent *event)
	{
		qDebug() << __FILE__ << __func__ << __LINE__ << event->key();
		mKeyLabel->setText(QString("Key code: %1 pressed").arg(event->key()));
		BaseWidget::keyPressEvent(event);
	}
}
