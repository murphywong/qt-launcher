#include "camerawidget.h"
#include "processimage.h"

#include <QVBoxLayout>

namespace ui {
	CameraWidget::CameraWidget(QWidget *parent)
		:BaseWidget(parent)
	{
		ProcessImage *image = new ProcessImage(this);
		image->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		centralLayout()->addWidget(image);
		setFixedSize(QSize(800, 480));
	}
}
