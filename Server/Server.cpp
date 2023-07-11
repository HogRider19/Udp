#include "Server.h"


void Server::Start()
{
    if (_interruptedDataHandler == NULL)
        throw std::runtime_error("Incorrect initialization of the Server object");

    while (_socket.waitForReadyRead(-1) && _socket.hasPendingDatagrams())
    {
        processPendingDatagrams();
    }
}

void Server::SignInterruptedDataHandler(void(*handler)(QByteArray data))
{
	_interruptedDataHandler = handler;
}

void Server::processPendingDatagrams()
{
    QByteArray datagram;
    datagram.resize(_socket.pendingDatagramSize());
    QHostAddress sender;
    quint16 senderPort;

    _socket.readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

    int countRemainingDatagrams = takeCountRemainingDatagrams(datagram);
    
    qDebug() << "Datagram number: " << countRemainingDatagrams << "\n";

    if (_data.size() == 0)
        _prevCountRemainingDatagrams = countRemainingDatagrams + 1;

    if (countRemainingDatagrams == _prevCountRemainingDatagrams - 1) 
    {
        _data += datagram;
        _prevCountRemainingDatagrams -= 1;
    }
    else 
    {
        qDebug() << "Error accepting package. Package: " << countRemainingDatagrams << "\n";
        _data.clear();
        _prevCountRemainingDatagrams = 1;
        return;
    }

    if (_prevCountRemainingDatagrams == 1) 
    {
        _interruptedDataHandler(_data);
        _prevCountRemainingDatagrams = 1;
        _data.clear();
    }
}

int Server::takeCountRemainingDatagrams(QByteArray& datagram)
{
    QByteArray num1Bytes, num2Bytes;
    bool foundFirstNumber = false;
    for (int i = 0; i < datagram.size(); i++)
    {
        if (!foundFirstNumber) 
        {
            if (datagram[i] != '/')
                num1Bytes.append(datagram[i]);
            else
                foundFirstNumber = true;
            continue;
        }

        if (datagram[i] != ':') 
        {
            num2Bytes.append(datagram[i]);
            continue;
        }
        
        datagram = datagram.mid(i + 1, datagram.size());
        break;
    }
    int first = num1Bytes.toInt();
    int second = num2Bytes.toInt();
    return second - first;
}

Server::Server(std::string host, int port)
{
    _socket.bind(QHostAddress("127.0.0.1"), 1234);
}

Server::~Server()
{

}
