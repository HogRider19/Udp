#pragma once
#include <qudpsocket.h>
#include <QObject>
#include <stdexcept>

class Server
{
public:
	Server(std::string host, int port);
	~Server();
	void Start();
	void SignInterruptedDataHandler(void(*handler)(QByteArray data));
	

private:
	QUdpSocket _socket;
	void(*_interruptedDataHandler)(QByteArray data) = NULL;
	int _prevCountRemainingDatagrams = 1;
	QByteArray _data;

	void processPendingDatagrams();
	static int takeCountRemainingDatagrams(QByteArray& datagram);
};

