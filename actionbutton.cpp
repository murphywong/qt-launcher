#include "actionbutton.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QPixmap>

namespace ui {
	ActionButton::ActionButton(QWidget *parent)
		:QPushButton(parent)
		,mIndex(-1)
	{
		createLayout();
	}

	ActionButton::ActionButton(int index, QIcon button, QPixmap label, QWidget *parent)
		:QPushButton(parent)
		,mIndex(index)
	{
		createLayout();
		setButtonIcon(button);
		setLabelPixmap(label);
	}

	ActionButton::~ActionButton()
	{
		delete mLabel;
		delete mButton;
		delete mHLayout;
	}

	void ActionButton::createLayout()
	{
		QVBoxLayout *layout = new QVBoxLayout(this);
		mHLayout = new QHBoxLayout;
		mButton = new QPushButton;
		mLabel = new QLabel;

		layout->addWidget(mButton);
		layout->addStretch(1);
		layout->addLayout(mHLayout);
		mHLayout->addStretch(1);
		mHLayout->addWidget(mLabel);
		mHLayout->addStretch(1);
		
		//clicked signal relay
		connect(mButton, SIGNAL(clicked()), SIGNAL(clicked()));

		mButton->setFlat(true);
		mButton->setIconSize(QSize(56, 56));
		setFlat(true);
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		setFixedSize(80, 100);
	}

	void ActionButton::init()
	{
	}
}
