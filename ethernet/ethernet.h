#ifndef  ETHERNET_H_
#define  ETHERNET_H_

#include "launchertypes.h"

#include <QObject>

namespace ui {
	class EtherNet : public QObject {
		Q_OBJECT

		public :
			explicit EtherNet(QString ifaceName, QObject *parent = 0);
			~EtherNet();

			QString ifaceName() { return mIface; }
			QString mac() { return mMac; }
			QString ip() { return mIp; }
			QString netmask() { return mNetmask; }
			QString gateway() { return mGateWay; }
			QString dns() { return mDns; }

			static int setConfig(const char *netinter, const char *ipaddr, 
					const char *netmask, const char *dns, const char *gateway);
			static int setDefaultRoute(char *netinter, char * route_addr);

			void parseConfiguation();
			static bool isEthernetOn(QString iface = QString(ETHERNET_IFACE));
			static bool isEthernetUp(QString iface = QString(ETHERNET_IFACE));
			static bool isEthernetRunning(QString iface = QString(ETHERNET_IFACE));

		private :
			QString interDns();
			QString interGateway();

			QString mIface;
			QString mMac;
			QString mIp;
			QString mNetmask;
			QString mGateWay;
			QString mDns;
	};
}




#endif  /*ETHERNET_H*/
