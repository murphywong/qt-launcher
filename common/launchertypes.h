#ifndef  WITSTYPES_H_
#define  WITSTYPES_H_

#include <QString>

#define ETHERNET_IFACE	"eth0"
#define WIRELESS_IFACE	"wlan0"

#define TFCARD_FILE	"/dev/sda4"
#define TFCARD_MOUNT_PATH	"/opt"

#define VIDEO_PATH	"/usr/share/video"
#define VIDEO_FILENAME	"test.mkv"

#define BLUETOOTH_MOUNT_PATH	"/opt"

enum FileManagerType {
	FileTFCard,
	FileBluetooth,
};

enum WpaEventType {
	WpaEventScanResults,
	WpaEventDisconnected,
	WpaEventConnected,
};

enum AuthType{
	AUTH_NONE_OPEN,
	AUTH_NONE_WEP,
	AUTH_NONE_WEP_SHARED,
	AUTH_IEEE8021X,
	AUTH_WPA_PSK,
	AUTH_WPA_EAP,
	AUTH_WPA2_PSK,
	AUTH_WPA2_EAP
};

enum IndicateType {
	IndicateWiFi,
	IndicateBluetooth,
	IndicateEthernet
};


typedef struct
{   
	int auth;
	int level;

	QString bbsid;
	QString freq;
	QString signal;
	QString flags;
	QString ssid;
	QString pwd;
}WifiInfo;



#endif  /*TYPES_H*/
