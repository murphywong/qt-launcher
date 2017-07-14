#ifndef  UTIL_H_
#define  UTIL_H_

class QWidget;

class Util {
	public : 
		static void setFontSize(QWidget *, int size);
		static void setLeftMargins(QWidget *, int left);
		static void setRightMargins(QWidget *, int right);
		static void setTopMargins(QWidget *, int top);
		static void setBottomMargins(QWidget *, int top);
};




#endif  /*UTIL_H*/
