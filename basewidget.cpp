#include "basewidget.h"
#include "toolbar.h"

#include <QVBoxLayout>
#include <QPaintEvent>
#include <QPainter>

namespace ui {

	BaseWidget::BaseWidget(QWidget *parent, Qt::WindowFlags flags)
		//:QWidget(parent)
		:QWidget(parent, flags)
	{
		createLayout();
	}

	BaseWidget::BaseWidget(QPixmap background, QWidget *parent, Qt::WindowFlags flags)
		//:QWidget(parent)
		:QWidget(parent, flags)
		,mBackgroundPixmap(background)
	{
		createLayout();
	}

	BaseWidget::~BaseWidget()
	{
		delete mCentralLayout;
		delete mBar;
	}

	void BaseWidget::createLayout()
	{
		QVBoxLayout *mainLayout = new QVBoxLayout(this);
		mCentralLayout = new QVBoxLayout;
		mBar = new ToolBar;

		//mainLayout->addStretch(1);
		mainLayout->addLayout(mCentralLayout);
		//mainLayout->addStretch(1);
		mainLayout->addWidget(mBar);

		mainLayout->setContentsMargins(0, 0, 0, 0);
		mainLayout->setSpacing(0);

		connect(mBar, SIGNAL(backAction()), SLOT(onToolBackClicked()));
		connect(mBar, SIGNAL(homeAction()), SLOT(onToolHomeClicked()));
	}

	void BaseWidget::paintEvent(QPaintEvent *event)
	{
		if (!mBackgroundPixmap.isNull()){
			QPainter painter(this);
			painter.drawPixmap(0, 0, mBackgroundPixmap.scaled(size()));
		}

		QWidget::paintEvent(event);
	}

	void BaseWidget::onToolBackClicked()
	{
		emit quit();
		close();
	}

	void BaseWidget::onToolHomeClicked()
	{
		emit quit();
		close();
	}
}
