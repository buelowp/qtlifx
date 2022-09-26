
#include <QtCore/QtCore>
#include "lifxpacket.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    LifxPacket packet;

    if (argc != 2) {
        qWarning().noquote() << "usage:" << app.arguments().at(0) << "<bytestream>";
        return 0;
    }

    QByteArray ba = QByteArray::fromHex(argv[1]);
    QHostAddress ha;
    packet.setDatagram(ba.data(), ba.size(), ha, 57600);
    qDebug() << packet;
}
