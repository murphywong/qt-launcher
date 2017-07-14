#include "filemanager.h"
#include "wifiwidget.h"
#include "toolbar.h"
#include "util.h"

#include <QHBoxLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QFileInfoList>
#include <QTextCodec>
#include <QtDebug>

namespace ui {
	FileManager::FileManager(QDir dir, QWidget *parent)
		:BaseWidget(QPixmap(":/icons/net_background2.png"), parent)
		,rootDir(dir)
		,currentDir(dir)
	{
		QScrollArea *scrollArea = new QScrollArea(this);
		ScrollWidget *widget = new ScrollWidget(this);
		mItemLayout = new QVBoxLayout(widget);
		scrollArea->setWidget(widget);
		scrollArea->setWidgetResizable(true);
		scrollArea->setFrameShape(QFrame::NoFrame);
		scrollArea->setFixedHeight(430);

		centralLayout()->addWidget(scrollArea);
		centralLayout()->addStretch(1);
		setFixedSize(QSize(800, 480));

		//setWindowTitle(dir.dirName());
		setWindowTitle(dir.absolutePath());
		updateDir(dir);
	}

	FileManager::~FileManager()
	{
	}

	void FileManager::updateDir(QDir dir)
	{
		QLayoutItem *child;
		while ((child = mItemLayout->takeAt(0)) != 0);
		foreach(FileItem *item, mFileItems)
		{
			delete item;
		}
		mFileItems.clear();

		setWindowTitle(dir.absolutePath());
		QFileInfoList itemList = dir.entryInfoList(QDir::NoFilter, QDir::DirsFirst | QDir::Name);
		foreach(QFileInfo info, itemList)
		{
			if (info.fileName() == "." || info.fileName() == ".."){
				continue;
			}

			FileItem *item = new FileItem(info);
			connect(item, SIGNAL(clicked()), SLOT(onItemClicked()));
			mFileItems << item;
			mItemLayout->addWidget(item);
		}

		mItemLayout->addStretch(1);
	}

	void FileManager::onItemClicked()
	{
		//qDebug() << __FILE__ << __func__ << __LINE__;
		FileItem *item = qobject_cast<FileItem *> (sender());
		if (item){
			QFileInfo info = item->fileInfo();
			if (info.isDir()){
				currentDir.cd(info.fileName());
				updateDir(currentDir);
			}
		}
	}

	void FileManager::onToolBackClicked()
	{
		//qDebug() << __FILE__ << __func__ << __LINE__;
		if (rootDir == currentDir){
			close();
		} else {
			currentDir.cdUp();
			updateDir(currentDir);
		}
	}

	FileItem::FileItem(QFileInfo info, QWidget *parent)
		:QPushButton(parent)
		,mFileInfo(info)
	{
		QHBoxLayout *layout  = new QHBoxLayout(this);
		//QTextCodec *codec = QTextCodec::codecForName("GBK");
		//QString name = codec->toUnicode(info.fileName().toStdString().data());
		//qDebug() << "name: " << info.fileName();
		//qDebug() << "utf8 name: " << info.fileName().toUtf8();
		//qDebug() << "unicode name: " << name;
		mFileName = new QLabel(info.fileName(), this);
		//QFont font = mFileName->font();
		//font.setFamily("wenquanyi");
		//mFileName->setFont(font);
		//mFileName->setText(QString::fromUtf8(qPrintable(name)));
		//mFileName->setText(info.fileName());
		//mFileName = new QLabel(name, this);
		mFileType = new QLabel(info.isDir() ? "directory" : "file", this);
		Util::setFontSize(mFileName, 10);
		Util::setFontSize(mFileType, 10);
		mFileName->setStyleSheet("color:white");
		mFileType->setStyleSheet("color:white");

		layout->addWidget(mFileName);
		layout->addStretch(1);
		layout->addWidget(mFileType);
		setContentsMargins(20, 0, 20, 0);
		//setFixedHeight(60);

		setFlat(true);
	}
	
	FileItem::~FileItem()
	{
	}

	void FileItem::paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);
		painter.drawPixmap(0, 0, QPixmap(":/icons/wifibutton_background.png").scaled(size()));
		QPushButton::paintEvent(event);
	}
}
