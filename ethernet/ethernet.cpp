#include "ethernet.h"

#include <QNetworkInterface>
#include <QStringList>
#include <QtDebug>
#include <QList>
#include <QFile>
#include <dirent.h>
#include <string.h>
#include <netinet/if_ether.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <linux/route.h>
#include <errno.h>

namespace ui {
	EtherNet::EtherNet(QString iface, QObject *parent)
		:QObject(parent)
		,mIface(iface)
	{
		parseConfiguation();
	}

	EtherNet::~EtherNet()
	{
	}

	void EtherNet::parseConfiguation()
	{
		QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
		for (int i = 0; i < interfaces.size(); i++)
		{
			QNetworkInterface iface = interfaces.at(i);
			if (iface.humanReadableName() == mIface){
				QList<QNetworkAddressEntry> addrEntries = iface.addressEntries();
				for (int j = 0; j < addrEntries.size(); j++)
				{
					QNetworkAddressEntry entry = addrEntries.at(j);
					QHostAddress ip = entry.ip();
					if (ip.protocol() == QAbstractSocket::IPv4Protocol){
						mIp = entry.ip().toString();
						mNetmask = entry.netmask().toString();
						qDebug() << "ip: " << entry.ip().toString();
						qDebug() << "netmask: " << entry.netmask().toString();
						qDebug() << "\n";
					}
				}

				mMac = iface.hardwareAddress();
				
				break;
			}
		}

		mDns = interDns();
		mGateWay = interGateway();
	}

	QString EtherNet::interDns()
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		QFile file("/etc/resolv.conf");

		if (!file.exists()){
			qDebug() << "file \"/etc/resolv.conf\" do not exists";
			return 0;
		}

		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			return 0;

		QTextStream in(&file);
		QStringList list;
		QString all = in.readAll();

		QStringList lines = all.split(QRegExp("\\n"));
		for (QStringList::Iterator it = lines.begin();
				 it != lines.end(); it++) {
			if ( (*it).startsWith("#")){ //|| (!(*it).contains("nameserver"))){
				continue;
			}

			if ((*it).contains("nameserver")){
				int pos = (*it).indexOf(' ') + 1;
				if (pos < 1)
				{
					continue;
				}

				list << (*it).mid(pos);
			}
		}


		file.close();

		qDebug() << __FILE__ << __func__ << __LINE__ << "lists-->" << list;
		return QString(list.join(","));
	}

	QString EtherNet::interGateway()
	{
		QString gateway;
		char buff[256];
		int nl = 0;
		struct in_addr dest;
		struct in_addr gw;
		int flgs, ref, use, metric;
		unsigned long int d, g, m;
		int find_default_flag = 0;
		char sgw[16];

		FILE *fp = fopen("/proc/net/route", "r");

		while (fgets(buff, sizeof (buff), fp) != NULL)
		{
			if (nl) {
				int ifl = 0;
				while (buff[ifl] != ' ' && buff[ifl] != '\t' && buff[ifl] != '\0')
					ifl++;
				buff[ifl] = 0; /* interface */
				if (sscanf(buff + ifl + 1, "%lx%lx%X%d%d%d%lx",
							&d, &g, &flgs, &ref, &use, &metric, &m) != 7) {
					fclose(fp);
					return 0;
				}

				if (flgs & RTF_UP) {
					dest.s_addr = d;
					gw.s_addr = g;
					strcpy(sgw, (gw.s_addr == 0 ? "" : (char *) inet_ntoa(gw)));

					if (dest.s_addr == 0) {
						find_default_flag = 1;
						gateway.clear();
						gateway.sprintf("%s", sgw);
						//memcpy(gateway, sgw, strlen(sgw));
						//printf("gateway:%s\n", gateway);
						break;
					}
				}
			}
			nl++;
		}
		fclose(fp);

		if (find_default_flag == 1)
			return gateway;
		else
			return QString();
	}

	int EtherNet::setConfig(const char *netinter, const char *ipaddr, 
			const char *netmask, const char *dns, const char *gateway)
	{

		int s_SocketHandle;
		FILE *f;

		const char *s_MyDevice;
		struct ifreq ifr;
		struct rtentry	rtent;
		struct sockaddr_in*p = (struct sockaddr_in *)&(ifr.ifr_addr);
		
		s_MyDevice=netinter;
		s_SocketHandle = socket(AF_INET,SOCK_PACKET,htons(ETH_P_ALL));

		if(s_SocketHandle<0) {
			printf("staticonf::setConfig:socket error\n");
			return -1;
		}
		
		memset(&ifr,0,sizeof(struct ifreq));
		memset(&rtent,0,sizeof(struct rtentry));
	   
		p->sin_family = AF_INET;


		memcpy(ifr.ifr_name,s_MyDevice,strlen(s_MyDevice));
		
		p->sin_addr.s_addr = inet_addr(ipaddr);
		/* setting IP address */
		if ( ioctl(s_SocketHandle,SIOCSIFADDR,&ifr) == -1 )  {
			printf("staticonf::setConfig:ioctl SIOCSIFADDR fail!\n");
			close(s_SocketHandle);
			return -1;
		}

		p->sin_addr.s_addr = inet_addr(netmask);
		/* setting netmask */
		if ( ioctl(s_SocketHandle,SIOCSIFNETMASK,&ifr) == -1 )	{
			printf("staticonf::setConfig:ioctl SIOCSIFNETMASK fail!\n");
			close(s_SocketHandle);
			return -1;
		}

		f=fopen("/etc/resolv.conf","w");/*setting dns server*/
		if ( f ) {
			fprintf(f,"nameserver %s\n\
		   search localdomain\n",dns);
			fclose(f);
		} else {
			printf("staticonf::setConfig:dns fopen fail!\n");
			close(s_SocketHandle);
			return -1;
		}
		/*setting gateway*/
		if (0 != setDefaultRoute((char *)netinter, (char *)gateway)) {
			close(s_SocketHandle);
			return -1;
		}

		close(s_SocketHandle);

		return 0;

	}


	int EtherNet::setDefaultRoute(char *netinter, char * route_addr)
	{
		struct	rtentry 	rtent;
		struct	sockaddr_in *p;
		int dhcpSocket = socket(AF_INET, SOCK_DGRAM, 0);

		memset(&rtent,0,sizeof(struct rtentry));
		p			=	(struct sockaddr_in *)&rtent.rt_dst;
		p->sin_family		=	AF_INET;
		p->sin_addr.s_addr	=	0;
		p			=	(struct sockaddr_in *)&rtent.rt_gateway;
		p->sin_family		=	AF_INET;
		/*memcpy(&p->sin_addr.s_addr,inet_addr(route_addr),4);*/
		p->sin_addr.s_addr=inet_addr(route_addr);
		p			=	(struct sockaddr_in *)&rtent.rt_genmask;
		p->sin_family		=	AF_INET;
		p->sin_addr.s_addr	=	0;
		rtent.rt_dev		=	netinter;

		rtent.rt_metric 		=	0;
		rtent.rt_window 	=	1;
		rtent.rt_flags			=	RTF_UP|RTF_GATEWAY;

		/*failed to configure the gateway*/
		if ( ioctl(dhcpSocket,SIOCADDRT,&rtent) == -1 ) {
			/* possibly gateway is over the bridge */
			if ( errno == ENETUNREACH )    { 						   
				/* try adding a route to gateway first */
				memset(&rtent,0,sizeof(struct rtentry));
				p			=	(struct sockaddr_in *)&rtent.rt_dst;
				p->sin_family		=	AF_INET;
				/*memcpy(&p->sin_addr.s_addr,inet_addr(route_addr),4);*/
				p->sin_addr.s_addr=inet_addr(route_addr);

				p			=	(struct sockaddr_in *)&rtent.rt_gateway;
				p->sin_family		=	AF_INET;
				p->sin_addr.s_addr	=	0;
				p			=	(struct sockaddr_in *)&rtent.rt_genmask;
				p->sin_family		=	AF_INET;
				p->sin_addr.s_addr	=	0xffffffff;


				rtent.rt_dev		=	netinter;

				rtent.rt_metric 	=	  0;
				rtent.rt_flags		=	  RTF_UP|RTF_HOST;
				if ( ioctl(dhcpSocket,SIOCADDRT,&rtent) == 0 ) {
					memset(&rtent,0,sizeof(struct rtentry));
					p			=	(struct sockaddr_in *)&rtent.rt_dst;
					p->sin_family	=	AF_INET;
					p->sin_addr.s_addr	=	0;
					p			=	(struct sockaddr_in *)&rtent.rt_gateway;
					p->sin_family	=	AF_INET;
					/* memcpy(&p->sin_addr.s_addr,inet_addr(route_addr),4);*/
					p->sin_addr.s_addr=inet_addr(route_addr);

					p			=	(struct sockaddr_in *)&rtent.rt_genmask;
					p->sin_family	=	AF_INET;
					p->sin_addr.s_addr	=	0;

					rtent.rt_dev	=	netinter;

					rtent.rt_metric =	1;
					rtent.rt_window =	0;
					rtent.rt_flags	=	RTF_UP|RTF_GATEWAY;
					if ( ioctl(dhcpSocket,SIOCADDRT,&rtent) == -1 ) {
						printf("staticonf::setConfig: ioctl SIOCADDRT: %m\n");
						return -1;
					} else {
						return 0;
					}
				} else {
					return -1;
				}
			} else {
				printf("staticonf::setConfig:ioctl SIOCADDRT: %m\n");
				return -1;
			}
		} else {
			return 0;
		}

		return 0;
	}

	bool EtherNet::isEthernetOn(QString iface)
	{
		QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
		for (int i = 0; i < interfaces.size(); i++)
		{
			QNetworkInterface::InterfaceFlags flag = interfaces.at(i).flags();
			QString name = interfaces.at(i).humanReadableName();
			if (name == iface && flag & QNetworkInterface::IsUp && flag & QNetworkInterface::IsRunning){
				return true;
			}
		}

		return false;
	}

	bool EtherNet::isEthernetUp(QString iface)
	{
		QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
		for (int i = 0; i < interfaces.size(); i++)
		{
			QNetworkInterface::InterfaceFlags flag = interfaces.at(i).flags();
			QString name = interfaces.at(i).humanReadableName();
			if (name == iface && flag & QNetworkInterface::IsUp){
				return true;
			}
		}

		return false;
	}

	bool EtherNet::isEthernetRunning(QString iface)
	{
		QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
		for (int i = 0; i < interfaces.size(); i++)
		{
			QNetworkInterface::InterfaceFlags flag = interfaces.at(i).flags();
			QString name = interfaces.at(i).humanReadableName();
			if (name == iface && flag & QNetworkInterface::IsRunning){
				return true;
			}
		}

		return false;
	}
}
