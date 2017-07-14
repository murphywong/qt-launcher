#include "paintingwidget.h"

#include <QPen>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QtDebug>

namespace ui {
	PaintingWidget::PaintingWidget(QWidget *parent)
		:BaseWidget(parent)
	{
		centralLayout()->addStretch(1);
		setWindowTitle("IRDA Test");
		setFixedSize(QSize(800, 480));
	}

	PaintingWidget::~PaintingWidget()
	{
	}

	void PaintingWidget::paintEvent(QPaintEvent *event)
	{
        Q_UNUSED(event);

		QPainter painter(this);
		QPen pen(Qt::black);
		pen.setWidth(2);
		painter.setPen(pen);

		painter.drawPath(mPath);
	}

	void PaintingWidget::mousePressEvent(QMouseEvent *event)
	{
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
		mPath.moveTo(event->posF());
		mPrevPointF = event->posF();
#else
		mPath.moveTo(event->localPos());
		mPrevPointF = event->localPos();
#endif
		update();
		BaseWidget::mousePressEvent(event);
	}

	void PaintingWidget::mouseReleaseEvent(QMouseEvent *event)
	{
		update();
		BaseWidget::mouseReleaseEvent(event);
	}

	void PaintingWidget::mouseMoveEvent(QMouseEvent *event)
	{
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
		mPath.quadTo(mPrevPointF, event->posF());
		mPrevPointF = event->posF();
#else
		mPath.quadTo(mPrevPointF, event->localPos());
		mPrevPointF = event->localPos();
#endif
		update();
		BaseWidget::mouseMoveEvent(event);
	}
}
