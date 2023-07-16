#pragma once
#include <qudpsocket.h>
#include <QObject>
#include <stdexcept>
#include "UdpHandlerBase.h"

class Hub : UdpHandlerBase
{
public:
	Hub(std::string host, int port);
	~Hub();
	void Start();
	void SignInterruptedDataHandler(void(*handler)(QByteArray data));

private:
	void _sendDataToAllClients(QByteArray data);

};