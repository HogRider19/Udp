#include <QtCore/QCoreApplication>
#include <qudpsocket.h>
#include "Server.h"
#include <fstream>
#include <string>
#include <map>

void interruptedDataHandler(QByteArray data);
std::map<std::string, std::string> readConfigFile(std::string filename);

int main(int argc, char* argv[]) 
{
	QCoreApplication a(argc, argv);

    auto config = readConfigFile("config.conf");
	Server server(config["host"], std::stoi(config["port"]));
	server.SignInterruptedDataHandler(interruptedDataHandler);
	server.Start();

	return a.exec();
}

void interruptedDataHandler(QByteArray data)
{
	qDebug() << "Received datagram:\n\n" << QString::fromUtf8(data.constData(), data.size());
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
