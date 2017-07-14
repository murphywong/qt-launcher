#include "util.h"

#include <QFont>
#include <QMargins>
#include <QWidget>

void Util::setFontSize(QWidget *widget, int size)
{
	QFont font = widget->font();
	font.setPointSize(size);
	widget->setFont(font);
}

void Util::setLeftMargins(QWidget *widget, int left)
{
	QMargins margins = widget->contentsMargins();
	margins.setLeft(left);
	widget->setContentsMargins(margins);
}

void Util::setRightMargins(QWidget *widget, int right)
{
	QMargins margins = widget->contentsMargins();
	margins.setRight(right);
	widget->setContentsMargins(margins);
}

void Util::setTopMargins(QWidget *widget, int top)
{
	QMargins margins = widget->contentsMargins();
	margins.setTop(top);
	widget->setContentsMargins(margins);
}

void Util::setBottomMargins(QWidget *widget, int bottom)
{
	QMargins margins = widget->contentsMargins();
	margins.setBottom(bottom);
	widget->setContentsMargins(margins);
}
