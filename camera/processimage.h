#ifndef PROCESSIMAGE_H
#define PROCESSIMAGE_H

//#include <QtGui>
#include "videodevice.h"

#include <QWidget>

class QTimer;
class QPaintEvent;

class ProcessImage : public QWidget
{
    Q_OBJECT
public:
    ProcessImage(QWidget *parent=0);
    ~ProcessImage();
	
protected :
    void paintEvent(QPaintEvent *);

private:
    QTimer *timer;
    int rs;
    uchar *pp;
    uchar * p;
    size_t len;
    int convert_yuv_to_rgb_pixel(int y, int u, int v);
    int convert_yuv_to_rgb_buffer(unsigned char *yuv, 
			unsigned char *rgb, unsigned int width, unsigned int height);
    VideoDevice *vd;
	

private slots:
    void display_error(QString err);
	//int onGetimage(void);


};

#endif
