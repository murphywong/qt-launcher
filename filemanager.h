#ifndef  FILEMANAGER_H_
#define  FILEMANAGER_H_

#include "launchertypes.h"
#include "basewidget.h"

#include <QLabel>
#include <QFileInfo>
#include <QDir>
#include <QVector>
#include <QPushButton>

class QPaintEvent;
class QVBoxLayout;

namespace ui {
	class FileItem;
	class FileManager : public BaseWidget {
		Q_OBJECT

		public :
			explicit FileManager(QDir dir, QWidget *parent = 0);
			~FileManager();

		protected slots :
			void onToolBackClicked();

		private slots :
			void onItemClicked();

		private :
			void updateDir(QDir dir);
			QDir rootDir;
			QDir currentDir;
			QVector<FileItem *> mFileItems;
			QVBoxLayout *mItemLayout;
	};

	class FileItem : public QPushButton {
		Q_OBJECT

		public :
			explicit FileItem(QFileInfo info, QWidget *parent = 0);
			~FileItem();

			QFileInfo fileInfo() { return mFileInfo; }

		protected :
			void paintEvent(QPaintEvent *event);

		private :
			QLabel *mFileName;
			QLabel *mFileType;
			QFileInfo mFileInfo;
	};
}




#endif  /*FILEMANAGER_H*/
