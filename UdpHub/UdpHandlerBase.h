#pragma once
#include <qudpsocket.h>
#include <QObject>
#include <stdexcept>


class UdpHandlerBase
{
public:
	UdpHandlerBase(std::string host, int port = 60000);
	void SignInterruptedDataHandler(void(*handler)(QByteArray data));
	void Send(std::string host, int port, QByteArray data, int blockSize);

protected:
	std::list<std::string> _listenersHosts;
	QUdpSocket _socket;
	void(*_interruptedDataHandler)(QByteArray data) = NULL;
	int _prevCountRemainingDatagrams = 1;
	QByteArray _data;
	std::string _host;
	int _port;

	QByteArray processPendingDatagrams();
	static int takeCountRemainingDatagrams(QByteArray& datagram);
};

