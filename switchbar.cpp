#include "switchbar.h"
#include "util.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QPaintEvent>
#include <QPainter>
#include <QSize>
#include <QFont>

namespace ui {
	SwitchBar::SwitchBar(QPixmap background, QPixmap icon, 
			QPixmap title, QIcon onIcon, QIcon offIcon, QWidget *parent)
		:QWidget(parent)
		,mBackground(background)
		,mOnIcon(onIcon)
		,mOffIcon(offIcon)
		,mSwitchStatus(false)
	{
		QHBoxLayout *layout = new QHBoxLayout(this);
		mIcon = new QLabel;
		mTitle = new QLabel;
		mSwitch = new QPushButton;
		mNotify = new QLabel;

		mIcon->setPixmap(icon);
		mTitle->setPixmap(title);
		mSwitch->setIcon(offIcon);
		mNotify->setStyleSheet("color:white");
		Util::setFontSize(mNotify, 10);

		mSwitch->setFlat(true);
		mSwitch->setIconSize(QSize(71, 17));

		mSwitch->setContentsMargins(0, 0, 200, 0);

		layout->addWidget(mIcon);
		layout->addWidget(mTitle);
		layout->addStretch(1);
		layout->addWidget(mNotify);
		layout->addStretch(1);
		layout->addWidget(mSwitch);

		connect(mSwitch, SIGNAL(clicked()), SLOT(onSwitchClicked()));
	}

	SwitchBar::~SwitchBar()
	{
		delete mIcon;
		delete mTitle;
		delete mSwitch;
		delete mNotify;
	}

	void SwitchBar::paintEvent(QPaintEvent *event)
	{
		if (!mBackground.isNull()) {
			QPainter painter(this);
			painter.drawPixmap(0, 0, mBackground.scaled(size()));
		}

		QWidget::paintEvent(event);
	}

	void SwitchBar::actionChanged()
	{
		if (mSwitchStatus) {
			mSwitch->setIcon(mOnIcon);
		} else {
			mSwitch->setIcon(mOffIcon);
		}

		emit switchAction(mSwitchStatus);
	}

	void SwitchBar::onSwitchClicked()
	{
		mSwitchStatus = !mSwitchStatus;
		actionChanged();
	}

	void SwitchBar::setSwitchStatus(bool on)
	{
		mSwitchStatus = on;
		actionChanged();
	}
}
