#include "wifilistbutton.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QtDebug>
#include <QFont>

namespace ui {

	WifiListButton::WifiListButton(WifiInfo info, QPixmap level, QWidget *parent)
		:QPushButton(parent)
		,mData(info)
	{
		QVBoxLayout *layout = new QVBoxLayout(this);
		mItemLayout = new QHBoxLayout;
		mSsidLabel = new QLabel(info.ssid.isEmpty() ? "null" : info.ssid);
		mLevelPixmap = new QLabel;

		mItemLayout->addWidget(mSsidLabel);
		mItemLayout->addStretch(1);
		mItemLayout->addWidget(mLevelPixmap);

		layout->addLayout(mItemLayout);

		mLevelPixmap->setPixmap(level);

		QFont font = mSsidLabel->font();
		font.setPointSize(7);
		mSsidLabel->setFont(font);
		mSsidLabel->setStyleSheet("color:white");
		mItemLayout->setContentsMargins(20, 0, 20, 0);
		setFocusPolicy(Qt::StrongFocus);

		//connect(this, SIGNAL(clicked()), SLOT(onClicked()));
		setFlat(true);

		setFixedHeight(60);
	}

	WifiListButton::~WifiListButton()
	{
		delete mItemLayout;
		delete mSsidLabel;
		delete mLevelPixmap;
	}

	void WifiListButton::paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);
		painter.drawPixmap(0, 0, QPixmap(":/icons/wifibutton_background.png").scaled(size()));
		QPushButton::paintEvent(event);
	}

#if 0
	void WifiListButton::onClicked()
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		emit active();
	}
#endif
}
