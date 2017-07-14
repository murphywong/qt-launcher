#include "inputmethod.h"
#include "util.h"

#include <QGridLayout>
#include <QPushButton>
#include <QStringList>
#include <QFont>

namespace ui {
	InputMethod::InputMethod(QWidget *parent)
		:QWidget(parent)
	{
		QStringList number;
		number << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "0";
		QStringList row2;
		row2 << "q" << "w" << "e" << "r" << "t" << "y" << "u" << "i" << "o" << "p";
		QStringList row3;
		row3 << "a" << "s" << "d" << "f" << "g" << "h" << "j" << "k" << "l" << "Del";
		QStringList row4;
		row4 << "z" << "x" << "c" << "v" << "b" << "n" << "m" << "." << ":" << "OK";
		mLayout = new QGridLayout(this);
		mLayout->setRowStretch(0, 1);
		for (int i = 0; i < number.size(); i++)
		{
			mLayout->addWidget(createButton(number.at(i)), 1, i);
		}

		for (int i = 0; i < row2.size(); i++)
		{
			mLayout->addWidget(createButton(row2.at(i)), 2, i);
		}

		for (int i = 0; i < row3.size(); i++)
		{
			mLayout->addWidget(createButton(row3.at(i)), 3, i);
		}

		for (int i = 0; i < row4.size(); i++)
		{
			mLayout->addWidget(createButton(row4.at(i)), 4, i);
		}

		mLayout->setRowStretch(5, 1);
		
	}

	void InputMethod::onClicked()
	{
		QPushButton *button = qobject_cast<QPushButton *> (sender());
		if (button){
			QString text = button->text();
			if (text == "OK"){
				emit done();
			} else if (text == "Del"){
				emit del();
			} else {
				emit input(text);
			}
		}
	}

	QPushButton *InputMethod::createButton(QString text)
	{
		QPushButton *button = new QPushButton(text, this);
		connect(button, SIGNAL(clicked()), SLOT(onClicked()));
		button->setFixedSize(QSize(45, 40));
		Util::setFontSize(button, 8);
#if 0
		if (text == "Done"){
			button->setFixedSize(QSize(90, 40));
		} else {
			button->setFixedSize(QSize(45, 40));
		}
#endif
		return button;
	}
}
