#include "loginwidget.h"
#include "inputmethod.h"
#include "util.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QCheckBox>
#include <QLineEdit>
#include <QtDebug>
#include <QLabel>

namespace ui {
	CheckButoon::CheckButoon(QWidget *parent)
		:QPushButton(parent)
		,state(false)
	{
		QHBoxLayout *layout = new QHBoxLayout(this);
		QLabel *text = new QLabel("Show Password", this);
		mBox = new QLabel(this);

		layout->addWidget(mBox);
		layout->addWidget(text);
		layout->addStretch(1);

		mBox->setPixmap(QPixmap(":/icons/unselect.png"));
		text->setPixmap(QPixmap(":/icons/show_pwd.png"));
		connect(this, SIGNAL(clicked()), SLOT(onClicked()));
		setFlat(true);
		setFixedSize(QSize(200, 40));
	}

	void CheckButoon::onClicked()
	{
		state = !state;
		if (state){
			mBox->setPixmap(QPixmap(":/icons/selected.png"));
		} else {
			mBox->setPixmap(QPixmap(":/icons/unselect.png"));
		}
		emit stateChanged(state);
	}

	LoginWidget::LoginWidget(WifiInfo info, QWidget *parent)
		//:QWidget(parent)
		:QWidget(parent, Qt::FramelessWindowHint)
		,mData(info)
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		QHBoxLayout *layout = new QHBoxLayout(this);
		mLoginLayout = new QVBoxLayout;

		mTopLayout = new QVBoxLayout;
		mNameLabel = new QLabel(mData.ssid);
		QLabel *line = new QLabel(this);
		mTopLayout->addWidget(mNameLabel);
		mTopLayout->addWidget(line);
		mTopLayout->addStretch(1);
		mNameLabel->setStyleSheet("color:white");
		mNameLabel->setAlignment(Qt::AlignCenter);

		mTopLayout->setContentsMargins(0, 0, 0, 0);
		mNameLabel->setContentsMargins(0, 0, 0, 0);
		line->setContentsMargins(0, 0, 0, 0);
		line->setPixmap(QPixmap(":/icons/login_line.png"));

		mMidLayout = new QVBoxLayout;
		mLineEdit = new QLineEdit;
		CheckButoon *check = new CheckButoon(this);
		mMidLayout->addWidget(mLineEdit);
		mMidLayout->addWidget(check);
		mMidLayout->setSpacing(30);

		mLineEdit->setStyleSheet("background:gray;color:white");
		Util::setFontSize(mLineEdit, 6);

		mButtonLayout = new QHBoxLayout;
#if 0
		mSignIn = new QPushButton("Sign In");
		mCancel = new QPushButton("Cancel");
#endif
		mSignIn = new QPushButton;
		mCancel = new QPushButton;
		mButtonLayout->addWidget(mSignIn);
		mButtonLayout->addWidget(mCancel);
		mButtonLayout->addStretch(1);
		mSignIn->setIcon(QIcon(":/icons/signin_normal.png"));
		mCancel->setIcon(QIcon(":/icons/cancel_normal.png"));
		QSize size(96, 28);
		mSignIn->setIconSize(size);
		mCancel->setIconSize(size);
		mSignIn->setFixedSize(size);
		mCancel->setFixedSize(size);
		mSignIn->setFlat(true);
		mCancel->setFlat(true);
#if 0
		mSignIn->setStyleSheet("QPushButton {image:url(:/icons/signin_normal.png)} QPushButton::pressed {image:url(:/icons/signin_pressed.png)}");
		mCancel->setStyleSheet("QPushButton {image:url(:/icons/cancel_normal.png)} QPushButton::pressed {image:url(:/icons/cancel_pressed.png)}");
#endif

		mLoginLayout->addLayout(mTopLayout);
		mLoginLayout->addLayout(mMidLayout);
		mLoginLayout->addStretch(1);
		mLoginLayout->addLayout(mButtonLayout);
		mLoginLayout->addStretch(1);
		mInput = new InputMethod(this);

		layout->addLayout(mLoginLayout);
		layout->addStretch(1);
		layout->addWidget(mInput);

		QMargins margin = mMidLayout->contentsMargins();
		margin.setTop(30);
		mMidLayout->setContentsMargins(margin);

		connect(mSignIn, SIGNAL(clicked()), SLOT(onSignIn()));
		connect(mCancel, SIGNAL(clicked()), SLOT(onCancel()));
		connect(mInput, SIGNAL(input(QString)), SLOT(onTextInput(QString)));
		connect(mInput, SIGNAL(del()), SLOT(onTextDel()));
		connect(mInput, SIGNAL(done()), SLOT(onSignIn()));
		connect(check, SIGNAL(stateChanged(bool)), SLOT(onStateChanged(bool)));

		mLineEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
		setWindowModality(Qt::ApplicationModal);
		setAttribute(Qt::WA_TranslucentBackground);
		setFixedSize(QSize(800, 300));
	}

	LoginWidget::~LoginWidget()
	{
		delete mNameLabel;
		delete mLineEdit;
		delete mSignIn;
		delete mCancel;
		delete mTopLayout;
		delete mMidLayout;
		delete mButtonLayout;
	}

	void LoginWidget::paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);
		painter.drawPixmap(0, 0, QPixmap(":/icons/login_background.png").scaled(size()));
		QWidget::paintEvent(event);
	}

	void LoginWidget::showEvent(QShowEvent *event)
	{
#if 1
		QRect rootRect = QApplication::desktop()->rect();
        move((rootRect.width() - rect().width())/2, (rootRect.height() - rect().height())/2);
#endif
		QWidget::showEvent(event);
	}

	void LoginWidget::onTextInput(QString text)
	{
		mLineEdit->insert(text);
	}
	
	void LoginWidget::onTextDel()
	{
		mLineEdit->backspace();
	}

	void LoginWidget::onSignIn()
	{
		QString pwd = mLineEdit->text();
		if (!pwd.isEmpty()){
			if ((mData.auth == AUTH_WPA_PSK ||
					mData.auth == AUTH_WPA2_PSK) &&
					pwd.length() < 8){
				return ;
			}
			mData.pwd = pwd;
			emit login(mData);
			close();
		}
	}

	void LoginWidget::onCancel()
	{
		emit cancel();
		close();
	}

	void LoginWidget::onStateChanged(bool state)
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		if (state){
			mLineEdit->setEchoMode(QLineEdit::Normal);
		} else {
			mLineEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
		}
	}
}
