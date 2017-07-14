#include "wifimanager.h"
#include "wpa_ctrl.h"
#include "includes.h"

#include <sys/types.h>
#include <dirent.h>
#include <QList>
#include <QtDebug>
#include <QProcess>
#include <QStringList>
#include <QSocketNotifier>
#include <QNetworkInterface>

#define WPA_GUI_KEY_DATA "[key is configured]"
#define WPA_EVENT_PWD_INCORRECT  "WPA: 4-Way Handshake failed - pre-shared key may be incorrect"

namespace ui {

	static int str_match(const char *a, const char *b)
	{
		return strncmp(a, b, strlen(b)) == 0;
	}

	WiFiManager &WiFiManager::instance()
	{
		static WiFiManager manager;
		return manager;
	}

	WiFiManager::WiFiManager(QObject *parent)
		:QObject(parent)
		,ctrl_conn(0)
		,monitor_conn(0)
		,ctrl_iface(0)
		,ctrl_iface_dir(strdup("/var/run/wpa_supplicant"))
		,isConnecting(false)
	{
		//qDebug() << __FILE__ << __func__ << __LINE__;
		connect(&mConnectTimer, SIGNAL(timeout()), SLOT(onConnectingTimeout()));
	}


	WiFiManager::~WiFiManager()
	{
		free(ctrl_iface_dir);
		ctrl_iface_dir = 0;

		if (ctrl_iface){
			free(ctrl_iface);
			ctrl_iface = 0;
		}

		if (ctrl_conn) {
			wpa_ctrl_close(ctrl_conn);
			ctrl_conn = NULL;
		}

		if (monitor_conn) {
			delete msgNotifier;
			msgNotifier = NULL;
			wpa_ctrl_detach(monitor_conn);
			wpa_ctrl_close(monitor_conn);
			monitor_conn = NULL;
		}
	}

	bool WiFiManager::disconnect()
	{
		char reply[10] = {0};
		size_t len;
		bool ret = true;

		if (ctrlRequest("REMOVE_NETWORK ALL", reply, &len) < 0){
			qDebug() << "Failed to remove netowrk";
			ret = false;
		}

		isConnecting = false;

		return ret;
	}

	bool WiFiManager::start()
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		if (openCtrlConnection(ctrl_iface) < 0) {
			printf("Failed to open control connection to wpa_supplicant\n");
			return false;
		}

		scan();
		processBss();

		return true;
	}

	bool WiFiManager::stop()
	{
		//char reply[2048] = {0};
		//size_t len;
		bool ret = true;

		mResults.clear();

		//if (ctrlRequest(qPrintable(QString("REMOVE_NETWORK %1").arg(id)), reply, &len) < 0){
		if (!disconnect()){
			qDebug() << "Failed to remove netowrk";
			ret = false;
		}

		if (ctrl_conn) {
			wpa_ctrl_close(ctrl_conn);
			ctrl_conn = NULL;
		}

		if (monitor_conn) {
			delete msgNotifier;
			msgNotifier = NULL;
			wpa_ctrl_detach(monitor_conn);
			wpa_ctrl_close(monitor_conn);
			monitor_conn = NULL;
		}
		return ret;
	}

	int WiFiManager::openCtrlConnection(const char *ifname)
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		char *cfile;
		int flen;
		//char buf[2048], *pos, *pos2;
		//size_t len;

		if (ifname) {
			if (ifname != ctrl_iface) {
				free(ctrl_iface);
				ctrl_iface = strdup(ifname);
			}
		} else {
#ifdef CONFIG_CTRL_IFACE_UDP
			free(ctrl_iface);
			ctrl_iface = strdup("udp");
#endif /* CONFIG_CTRL_IFACE_UDP */
#ifdef CONFIG_CTRL_IFACE_UNIX
			struct dirent *dent;
			DIR *dir = opendir(ctrl_iface_dir);
			free(ctrl_iface);
			ctrl_iface = NULL;
			if (dir) {
				while ((dent = readdir(dir))) {
#ifdef _DIRENT_HAVE_D_TYPE
					/* Skip the file if it is not a socket.
					 * Also accept DT_UNKNOWN (0) in case
					 * the C library or underlying file
					 * system does not support d_type. */
					if (dent->d_type != DT_SOCK &&
						dent->d_type != DT_UNKNOWN)
						continue;
#endif /* _DIRENT_HAVE_D_TYPE */

					if (strcmp(dent->d_name, ".") == 0 ||
						strcmp(dent->d_name, "..") == 0)
						continue;
					printf("Selected interface '%s'\n",
						   dent->d_name);
					ctrl_iface = strdup(dent->d_name);
					break;
				}
				closedir(dir);
			}
#endif /* CONFIG_CTRL_IFACE_UNIX */
#ifdef CONFIG_CTRL_IFACE_NAMED_PIPE
			struct wpa_ctrl *ctrl;
			int ret;

			free(ctrl_iface);
			ctrl_iface = NULL;

			ctrl = wpa_ctrl_open(NULL);
			if (ctrl) {
				len = sizeof(buf) - 1;
				ret = wpa_ctrl_request(ctrl, "INTERFACES", 10, buf,
							   &len, NULL);
				if (ret >= 0) {
					connectedToService = true;
					buf[len] = '\0';
					pos = strchr(buf, '\n');
					if (pos)
						*pos = '\0';
					ctrl_iface = strdup(buf);
				}
				wpa_ctrl_close(ctrl);
			}
#endif /* CONFIG_CTRL_IFACE_NAMED_PIPE */
		}

		if (ctrl_iface == NULL) {
#ifdef CONFIG_NATIVE_WINDOWS
			static bool first = true;
			if (first && !serviceRunning()) {
				first = false;
				if (QMessageBox::warning(
						this, qAppName(),
						tr("wpa_supplicant service is not "
						   "running.\n"
						   "Do you want to start it?"),
						QMessageBox::Yes | QMessageBox::No) ==
					QMessageBox::Yes)
					startService();
			}
#endif /* CONFIG_NATIVE_WINDOWS */
			return -1;
		}

#ifdef CONFIG_CTRL_IFACE_UNIX
		flen = strlen(ctrl_iface_dir) + strlen(ctrl_iface) + 2;
		cfile = (char *) malloc(flen);
		if (cfile == NULL)
			return -1;
		snprintf(cfile, flen, "%s/%s", ctrl_iface_dir, ctrl_iface);
#else /* CONFIG_CTRL_IFACE_UNIX */
		flen = strlen(ctrl_iface) + 1;
		cfile = (char *) malloc(flen);
		if (cfile == NULL)
			return -1;
		snprintf(cfile, flen, "%s", ctrl_iface);
#endif /* CONFIG_CTRL_IFACE_UNIX */

		if (ctrl_conn) {
			wpa_ctrl_close(ctrl_conn);
			ctrl_conn = NULL;
		}

		if (monitor_conn) {
			delete msgNotifier;
			msgNotifier = NULL;
			wpa_ctrl_detach(monitor_conn);
			wpa_ctrl_close(monitor_conn);
			monitor_conn = NULL;
		}

		printf("Trying to connect to '%s'\n", cfile);
		ctrl_conn = wpa_ctrl_open(cfile);
		if (ctrl_conn == NULL) {
			free(cfile);
			return -1;
		}
		monitor_conn = wpa_ctrl_open(cfile);
		free(cfile);
		if (monitor_conn == NULL) {
			wpa_ctrl_close(ctrl_conn);
			return -1;
		}
		if (wpa_ctrl_attach(monitor_conn)) {
			printf("Failed to attach to wpa_supplicant\n");
			wpa_ctrl_close(monitor_conn);
			monitor_conn = NULL;
			wpa_ctrl_close(ctrl_conn);
			ctrl_conn = NULL;
			return -1;
		}

#if defined(CONFIG_CTRL_IFACE_UNIX) || defined(CONFIG_CTRL_IFACE_UDP)
		msgNotifier = new QSocketNotifier(wpa_ctrl_get_fd(monitor_conn),
						  QSocketNotifier::Read, this);
		connect(msgNotifier, SIGNAL(activated(int)), SLOT(receiveMsgs()));
#endif

		return 0;
	}


	int WiFiManager::ctrlRequest(const char *cmd, char *buf, size_t *buflen)
	{
		int ret;

		if (ctrl_conn == NULL)
			return -3;
		ret = wpa_ctrl_request(ctrl_conn, cmd, strlen(cmd), buf, buflen, NULL);
		if (ret == -2)
			printf("'%s' command timed out.\n", cmd);
		else if (ret < 0)
			printf("'%s' command failed.\n", cmd);

		return ret;
	}

	void WiFiManager::receiveMsgs()
	{
		//qDebug() << __FILE__ << __func__ << __LINE__;
		char buf[256];
		size_t len;

		while (monitor_conn && wpa_ctrl_pending(monitor_conn) > 0) {
			len = sizeof(buf) - 1;
			if (wpa_ctrl_recv(monitor_conn, buf, &len) == 0) {
				buf[len] = '\0';
				processMsg(buf);
			}
		}
	}

	void WiFiManager::processMsg(char *msg)
	{
		//qDebug() << __FILE__ << __func__ << __LINE__;
		char *pos = msg, *pos2;
		//int priority = 2;

		if (*pos == '<') {
			/* skip priority */
			pos++;
			//priority = atoi(pos);
			pos = strchr(pos, '>');
			if (pos)
				pos++;
			else
				pos = msg;
		}

		/* Update last message with truncated version of the event */
		if (strncmp(pos, "CTRL-", 5) == 0) {
			pos2 = strchr(pos, str_match(pos, WPA_CTRL_REQ) ? ':' : ' ');
			if (pos2)
				pos2++;
			else
				pos2 = pos;
		} else
			pos2 = pos;

		if (str_match(pos, WPA_EVENT_PWD_INCORRECT)){
			disconnect();
			emit obtainIpStatus("pwderror", false);
		} else if (str_match(pos, WPA_EVENT_SCAN_RESULTS)) {
			emit wpaEvent(WpaEventScanResults);
			//processResults();
			processBss();
			qDebug() << "WPA_EVENT_SCAN_RESULTS";
		} else if (str_match(pos, WPA_EVENT_DISCONNECTED)) {
			emit wpaEvent(WpaEventDisconnected);
			qDebug() << "WPA_EVENT_DISCONNECTED";
		} else if (str_match(pos, WPA_EVENT_CONNECTED)) {
			emit wpaEvent(WpaEventConnected);
			qDebug() << "catch WPA_EVENT_CONNECTED";

			QString cmd;
			cmd.append("udhcpc -i ");
			cmd.append(ctrl_iface ? ctrl_iface : WIRELESS_IFACE);
			cmd.append(" -n -q -t 6");
			qDebug() << cmd;
			if (::system(qPrintable(cmd))){
			qDebug() << "@@@@@@@@@@@@@@@@@@ Failed to exec udhcpc @@@@@@@@@@@@@@@@@@@@";
				isConnecting = false;
				disconnect();
				emit obtainIpStatus(current.ssid, false);
			} else {
			qDebug() << "@@@@@@@@@@@@@@@@@@ Exec udhcpc success @@@@@@@@@@@@@@@@@@@@";
				emit obtainIpStatus(current.ssid, true);
			}
			mConnectTimer.stop();
		}
	}

	void WiFiManager::processResults()
	{
		char reply[2048] = {0};
		size_t len;

		mResults.clear();

		if (ctrlRequest("SCAN_RESULT", reply, &len) < 0){
			return ;
		}

		reply[len] = '\0';

		QString results(reply);
		qDebug() << results;

		QStringList items = results.split(QRegExp("\\n"));
		foreach(QString item, items)
		{   
			WifiInfo info;
			QStringList ap = item.split(QRegExp("\\t"));

			if(ap.size() < 5 || ap.at(ScanResultsSSID).isEmpty())
			{   
				qDebug() << "size isn't enough:" << ap.size();
				continue;
			}   

			info.bbsid = ap.at(ScanResultsBBSID);
			info.freq = ap.at(ScanResultsFREQUENCY);
			info.signal = ap.at(ScanResultsSIGNAL);
			info.flags = ap.at(ScanResultsFLAGS);
			info.ssid = ap.at(ScanResultsSSID);

			info.auth = genAuthType(qPrintable(info.flags));
			info.level = genSignalStrength(info.signal.toInt());

			qDebug()<<info.bbsid<<info.freq<<info.ssid<<info.auth<<info.signal;
			if (info.auth == AUTH_NONE_OPEN){
				mResults.insert(0, info);
			} else {
				mResults.append(info);
			}   
		}   

		qDebug() << "\n\n\t Results size:" << mResults.size();

		if (mResults.size() > 0){
			emit updateResults();
		}
	}

	void WiFiManager::processBss()
	{
		char reply[2048];
		size_t reply_len;
		int index;
		char cmd[20];

		mResults.clear();

		index = 0;
		while (1) {
			snprintf(cmd, sizeof(cmd), "BSS %d", index++);
			if (index > 1000)
				break;

			reply_len = sizeof(reply) - 1;
			if (ctrlRequest(cmd, reply, &reply_len) < 0)
				break;
			reply[reply_len] = '\0';

			QString bss(reply);
			if (bss.isEmpty() || bss.startsWith("FAIL"))
				break;

			//qDebug() << "BSS " + index << ":" << bss;
			QString ssid, bssid, freq, signal, flags;

			QStringList lines = bss.split(QRegExp("\\n"));
			for (QStringList::Iterator it = lines.begin();
				 it != lines.end(); it++) {
				int pos = (*it).indexOf('=') + 1;
				if (pos < 1)
					continue;

				if ((*it).startsWith("bssid="))
					bssid = (*it).mid(pos);
				else if ((*it).startsWith("freq="))
					freq = (*it).mid(pos);
				else if ((*it).startsWith("level="))
					signal = (*it).mid(pos);
				else if ((*it).startsWith("flags="))
					flags = (*it).mid(pos);
				else if ((*it).startsWith("ssid="))
					ssid = (*it).mid(pos);
			}

			if (ssid.isEmpty()){
				qDebug() << "size isn't enough:" << mResults.size();
				continue;
			}   

			WifiInfo info;
			info.bbsid = bssid;
			info.freq = freq;
			info.signal = signal;
			info.flags = flags;
			info.ssid = ssid;

			info.auth = genAuthType(qPrintable(info.flags));
			info.level = genSignalStrength(info.signal.toInt());

			//qDebug()<<info.bbsid<<info.freq<<info.ssid<<info.auth<<info.signal;
			if (info.auth == AUTH_NONE_OPEN){
				mResults.insert(0, info);
			} else {
				mResults.append(info);
			}   


			if (bssid.isEmpty())
				break;
		}

		qDebug() << "\n\n\t Results size:" << mResults.size();

		emit updateResults();
	}

	AuthType WiFiManager::genAuthType(const char *flags)
	{
		AuthType auth = AUTH_NONE_OPEN;

		if (strstr(flags, "[WPA2-EAP"))
			auth = AUTH_WPA2_EAP;
		else if (strstr(flags, "[WPA-EAP"))
			auth = AUTH_WPA_EAP;
		else if (strstr(flags, "[WPA2-PSK"))
			auth = AUTH_WPA2_PSK;
		else if (strstr(flags, "[WPA-PSK"))
			auth = AUTH_WPA_PSK;
		else if (strstr(flags, "[WEP"))
			auth = AUTH_NONE_WEP;
		else
			auth = AUTH_NONE_OPEN;

		return auth;

	}

	int WiFiManager::genSignalStrength(int signal)
	{
		int qual = 0;

		if(signal >= 200)
			qual = 3;
		else if( (signal >= 185) && (signal < 200) )
			qual = 3;
		else if( (signal >= 175) && (signal < 185) )
			qual = 2;
		else
			qual = 1;

		return qual;
	}

	bool WiFiManager::isWlanOn()
	{
		QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
		for (int i = 0; i < interfaces.size(); i++)
		{
			QNetworkInterface::InterfaceFlags flag = interfaces.at(i).flags();
			QString name = interfaces.at(i).humanReadableName();
			if (name == WIRELESS_IFACE && 
					flag & QNetworkInterface::IsUp && flag & QNetworkInterface::IsRunning){
				return true;
			}
		}

		return false;
	}

	bool WiFiManager::scan()
	{
		char reply[1024] = {0};
		size_t reply_len = sizeof(reply) - 1;
		if (ctrlRequest("SCAN", reply, &reply_len) < 0) {
			return false;
		}
		reply[reply_len] = '\0';
		return true;
	}
	
	bool WiFiManager::connectToAp(WifiInfo info)
	{
		qDebug() << __FILE__ << __func__ << __LINE__;

		char reply[10], cmd[256];
		size_t reply_len;
		int psklen = info.pwd.length();

		if (!disconnect()){
			qDebug() << "Failed to remove netowrk";
		}

		isConnecting = true;

		current = info;

		memset(reply, 0, sizeof(reply));
		reply_len = sizeof(reply) - 1;

		ctrlRequest("ADD_NETWORK", reply, &reply_len);
		if (reply[0] == 'F') {
			qDebug() << "Failed to add network to wpa_supplicant";
			return false;
		}
		id = atoi(reply);

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
		setNetworkParam(id, "ssid", info.ssid.toAscii().constData(), true);
#else
		setNetworkParam(id, "ssid", info.ssid.toLatin1().constData(), true);
#endif

		const char *key_mgmt = NULL, *proto = NULL, *pairwise = NULL;
		switch (info.auth) {
		case AUTH_NONE_OPEN:
		case AUTH_NONE_WEP:
		case AUTH_NONE_WEP_SHARED:
			key_mgmt = "NONE";
			break;
		case AUTH_IEEE8021X:
			key_mgmt = "IEEE8021X";
			break;
		case AUTH_WPA_PSK:
			key_mgmt = "WPA-PSK";
			proto = "WPA";
			break;
		case AUTH_WPA2_PSK:
			key_mgmt = "WPA-PSK";
			proto = "WPA2";
			break;
		}

		if (info.auth == AUTH_NONE_WEP_SHARED)
			setNetworkParam(id, "auth_alg", "SHARED", false);
		else
			setNetworkParam(id, "auth_alg", "OPEN", false);

#if 0
		if (info.auth == AUTH_WPA_PSK || info.auth == AUTH_WPA_EAP ||
			info.auth == AUTH_WPA2_PSK || info.auth == AUTH_WPA2_EAP) {
			int encr = encrSelect->currentIndex();
			if (encr == 0)
				pairwise = "TKIP";
			else
				pairwise = "CCMP";
		}
#endif

		if (proto)
			setNetworkParam(id, "proto", proto, false);
		if (key_mgmt)
			setNetworkParam(id, "key_mgmt", key_mgmt, false);
		if (pairwise) {
			setNetworkParam(id, "pairwise", pairwise, false);
			setNetworkParam(id, "group", "TKIP CCMP WEP104 WEP40", false);
		}

		switch (info.auth){
			case AUTH_WPA_PSK :
			case AUTH_WPA2_PSK :
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
				setNetworkParam(id, "psk",
						info.pwd.toAscii().constData(),
						psklen != 64);
#else
				setNetworkParam(id, "psk",
						info.pwd.toLatin1().constData(),
						psklen != 64);
#endif
				break;
			case AUTH_NONE_OPEN :
			case AUTH_NONE_WEP :
			case AUTH_NONE_WEP_SHARED :
			{
				writeWepKey(id, info.pwd, 0);
				writeWepKey(id, "", 1);
				writeWepKey(id, "", 2);
				writeWepKey(id, "", 3);
			}
				break;
			default :
				break;
		}

		snprintf(cmd, sizeof(cmd), "ENABLE_NETWORK %d", id);
		reply_len = sizeof(reply);
		ctrlRequest(cmd, reply, &reply_len);
		if (strncmp(reply, "OK", 2) != 0) {
			qDebug() << "Failed to enable network in wpa_supplicant";
			return false;
			/* Network was added, so continue anyway */
		}

		mConnectTimer.start(60 * 1000);

		return true;
	}

	int WiFiManager::setNetworkParam(int id, const char *field,
					   const char *value, bool quote)
	{
		char reply[10], cmd[256];
		size_t reply_len;
		snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s %s%s%s",
			 id, field, quote ? "\"" : "", value, quote ? "\"" : "");
		reply_len = sizeof(reply);
		ctrlRequest(cmd, reply, &reply_len);
		return strncmp(reply, "OK", 2) == 0 ? 0 : -1;
	}
	void WiFiManager::writeWepKey(int network_id, QString pwd, int id)
	{
		char buf[10];
		bool hex;
		const char *txt, *pos;
		size_t len;

		if (pwd.isEmpty())
			return;

		/*
		 * Assume hex key if only hex characters are present and length matches
		 * with 40, 104, or 128-bit key
		 */
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
		txt = pwd.toAscii().constData();
#else
		txt = pwd.toLatin1().constData();
#endif
		if (strcmp(txt, WPA_GUI_KEY_DATA) == 0)
			return;
		len = strlen(txt);
		if (len == 0)
			return;
		pos = txt;
		hex = true;
		while (*pos) {
			if (!((*pos >= '0' && *pos <= '9') ||
				  (*pos >= 'a' && *pos <= 'f') ||
				  (*pos >= 'A' && *pos <= 'F'))) {
				hex = false;
				break;
			}
			pos++;
		}
		if (hex && len != 10 && len != 26 && len != 32)
			hex = false;
		snprintf(buf, sizeof(buf), "wep_key%d", id);
		setNetworkParam(network_id, buf, txt, !hex);
	}

	void WiFiManager::onReadReady()
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		QProcess *process = qobject_cast<QProcess *>(sender());
		if (process){
			char buf[2048] = {0};
			size_t len;
			QString result;
			while((len = process->read(buf, 2048)) > 0){
				buf[len] = '\0';
				result.append(buf);
			}

			emit statusSSID(parseStatusString(result));
			process->deleteLater();
		}
	}

	QString WiFiManager::connectedSSID()
	{
#if 0
		QProcess *process = new QProcess;
		connect(process, SIGNAL(readyRead()), SLOT(onReadReady()));
		process->start("wpa_cli status");
#endif
#if 1
		qDebug() << __FILE__ << __func__ << __LINE__;
		char reply[2048] = {0};
		size_t len;

		if (ctrlRequest("STATUS", reply, &len) < 0){
			return QString();
		}
		reply[len] = '\0';

		return parseStatusString(QString(reply));
#endif
	}

	QString WiFiManager::parseStatusString(QString result)
	{
		qDebug() << result;
		QString wpaStatus;
		QString ssid;
		if (!result.isEmpty()){
			foreach(QString line, result.split(QRegExp("\\n")))
			{
				int pos = line.indexOf('=') + 1;
				if(pos < 1)
					continue;

				qDebug() << "line-->" << line;

				if(line.startsWith("ssid="))
					ssid = line.mid(pos);
				else if (line.startsWith("wpa_state="))
					wpaStatus = line.mid(pos);
			}
		}

		if(wpaStatus == "COMPLETED")
			return ssid;

		return QString();

	}

	void WiFiManager::onConnectingTimeout()
	{
		qDebug() << __FILE__ << __func__ << __LINE__;
		if (isConnecting){
			disconnect();
			emit obtainIpStatus(current.ssid, false);
		}

		mConnectTimer.stop();
	}
} //end of namespace
