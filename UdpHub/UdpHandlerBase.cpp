#include "UdpHandlerBase.h"

UdpHandlerBase::UdpHandlerBase(std::string host, int port)
{
    _socket.bind(QHostAddress(QString::fromStdString(host)), port);
    _host = host;
    _port = port;
}

void UdpHandlerBase::SignInterruptedDataHandler(void(*handler)(QByteArray data))
{
    _interruptedDataHandler = handler;
}

void UdpHandlerBase::Send(std::string host, int port, QByteArray data, int blockSize = 60000)
{
    qDebug() << "Cient started" << "\n";

    int dataSize = data.size();
    int numBlocks = dataSize / blockSize + ((dataSize % blockSize) ? 1 : 0);

    for (int i = 0; i < numBlocks; i++)
    {
        int size = qMin(blockSize, dataSize - i * blockSize);
        QString prefix = QString::number(i) + "/" + QString::number(numBlocks) + ":";
        QByteArray block = prefix.toUtf8() + data.mid(i * blockSize, size);

        qDebug() << "count: " << numBlocks - i << "\n";

        _socket.writeDatagram(block.data(), size,
            QHostAddress(QString::fromStdString(host)), port);
    }

    qDebug() << "Cient finished" << "\n";
}

QByteArray UdpHandlerBase::processPendingDatagrams()
{
    QByteArray datagram;
    datagram.resize(_socket.pendingDatagramSize());
    QHostAddress sender;
    quint16 senderPort;

    _socket.readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

    if (datagram.contains("@Conntect"))
    {
        _listenersHosts.push_back(sender.toString().toStdString());
        return;
    }

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
        return _data;
    }
}

int UdpHandlerBase::takeCountRemainingDatagrams(QByteArray& datagram)
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
