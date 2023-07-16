#include "Hub.h"

Hub::Hub(std::string host, int port) : UdpHandlerBase(host, port) {}

Hub::~Hub() {}

void Hub::Start()
{
    if (_interruptedDataHandler == NULL)
        throw std::runtime_error("Incorrect initialization of the Server object");

    while (_socket.waitForReadyRead(-1) && _socket.hasPendingDatagrams())
    {
        QByteArray data = processPendingDatagrams();
        if (!data.isEmpty()) 
        {
            _sendDataToAllClients(_data);
        }
    }
}

void Hub::SignInterruptedDataHandler(void(*handler)(QByteArray data))
{
    _interruptedDataHandler = handler;
}

void Hub::_sendDataToAllClients(QByteArray data)
{
    for (auto it = _listenersHosts.begin(); it != _listenersHosts.end(); ++it) 
    {
        Send(*it, _port, data, 60000);
    }
}