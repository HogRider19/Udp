#include "HubClient.h"


HubClient::HubClient(std::string host, int port) : UdpHandlerBase(host, port) {}

HubClient::~HubClient() {}

void HubClient::Listen()
{
    _isListen = true;
    while(_isListen) 
    {
        if (_socket.waitForReadyRead(-1)) 
        {
            if (_socket.hasPendingDatagrams())
                processPendingDatagrams();
            else
                _isListen = false;
        }
    }
}
