#pragma once
#include <qudpsocket.h>
#include <QObject>
#include <stdexcept>
#include "UdpHandlerBase.h"


class HubClient : public UdpHandlerBase
{
public:
	HubClient(std::string host, int port);
	~HubClient();
	void Listen();

private:
	bool _isListen = false;
};

