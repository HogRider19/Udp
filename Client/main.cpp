#include <QtCore/QCoreApplication>
#include <qudpsocket.h>
#include <QFile>
#include <stdexcept>
#include <fstream>
#include <string>
#include <map>

void readCurrentDirectoryFile(const QString& fileName, QByteArray& data);
std::map<std::string, std::string> readConfigFile(std::string filename);

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    printf("Client started \n");

    auto config = readConfigFile("config.conf");

    QUdpSocket udpSocket;
    QByteArray datagram;
    readCurrentDirectoryFile(QString::fromStdString(config["dataPath"]), datagram);

    int dataSize = datagram.size();
    int blockSize = std::stoi(config["blockSize"]);
    int numBlocks = dataSize / blockSize + ((dataSize % blockSize) ? 1 : 0);

    for (int i = 0; i < numBlocks; i++) 
    {
        int size = qMin(blockSize, dataSize - i * blockSize);
        QString prefix = QString::number(i) + "/" + QString::number(numBlocks) + ":";
        QByteArray block = prefix.toUtf8() + datagram.mid(i * blockSize, size);
        
        qDebug() << "count: " << numBlocks - i << "\n";

        udpSocket.writeDatagram(block.data(), size,
            QHostAddress(QString::fromStdString(config["host"])), std::stoi(config["port"]));
    }

    printf("Cient finished");

    return a.exec();
}

void readCurrentDirectoryFile(const QString& fileName, QByteArray& data) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("File read error...");
    }
    data = file.readAll();
    file.close();
}

std::map<std::string, std::string> readConfigFile(std::string filename)
{
    std::map<std::string, std::string> config;
    std::ifstream file(filename);

    if (file.is_open()) 
    {
        std::string line;
        while (std::getline(file, line)) 
        {
            if (line[0] != '#' && line != "")
            {
                size_t equals_pos = line.find("=");
                std::string key = line.substr(0, equals_pos);
                std::string value = line.substr(equals_pos + 1);
                config[key] = value;
            }
        }
        file.close();
    }
    else
        throw std::runtime_error("Configuration read error...");

    return config;
}


