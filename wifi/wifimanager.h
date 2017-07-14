#ifndef  WIFIMANAGER_H_
#define  WIFIMANAGER_H_

#include "launchertypes.h"

#include <QObject>
#include <QVector>
#include <QTimer>

class QSocketNotifier;
struct wpa_ctrl;

namespace ui {

	class WiFiManager : public QObject {
		Q_OBJECT

		public :
			enum WifiSectionType
			{           
				ScanResultsBBSID = 0,
				ScanResultsFREQUENCY,
				ScanResultsSIGNAL,
				ScanResultsFLAGS,
				ScanResultsSSID,

				ScanResultsTotal
			};


			~WiFiManager();

			static WiFiManager &instance();


			void writeWepKey(int network_id, QString pwd, int id);
			int setNetworkParam(int id, const char *field,
									   const char *value, bool quote);
			int ctrlRequest(const char *cmd, char *buf, size_t *buflen);
			int openCtrlConnection(const char *ifname);
			bool disconnect();
			bool start();
			bool stop();
			bool scan();
			bool isWlanOn();
			bool connectToAp(WifiInfo info);

			QVector<WifiInfo> &scanResults() {return mResults;}
			QString connectedSSID();

		Q_SIGNALS:
			void wpaEvent(WpaEventType type);
			void updateResults();
			void apConnected(QString ssid);
			void obtainIpStatus(QString ssid, bool success);
			void statusSSID(QString);
			//void updateResults(QVector<WifiInfo> &vector);

		public slots :
			void receiveMsgs();
		
		private slots :
			void onReadReady();
			void onConnectingTimeout();

		private :
			explicit WiFiManager(QObject *parent = 0);
			//explicit WiFiManager(WiFiManager &manager);
			void processMsg(char *msg);
			void processResults();
			void processBss();
			QString parseStatusString(QString buf);
			int genSignalStrength(int signal);
			AuthType genAuthType(const char *flags);

			struct wpa_ctrl *ctrl_conn;
			struct wpa_ctrl *monitor_conn;
			QSocketNotifier *msgNotifier;
			QVector<WifiInfo> mResults;

			char *ctrl_iface;
			char *ctrl_iface_dir;

			WifiInfo current;
			int id;
			bool isConnecting;

			QTimer mConnectTimer;
	};
}




#endif  /*WIFIMANAGER_H*/
