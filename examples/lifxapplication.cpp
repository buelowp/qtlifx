/*
 * Example of a simple LIFX controller application
 * 
 * Copyright (C) 2021  Peter Buelow <goballstate at gmail>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "lifxapplication.h"

LifxApplication::LifxApplication() : QMainWindow()
{
    m_x = 0;
    m_y = 0;
    m_uniqueId = 1;

    m_stateCheckInterval = new QTimer(this);
    m_stateCheckInterval->setInterval(2000);
    connect(m_stateCheckInterval, &QTimer::timeout, this, &LifxApplication::runStateCheck);

    m_manager = new LifxManager(this);
    connect(m_manager, &LifxManager::bulbDiscoveryFinished, this, &LifxApplication::bulbDiscoveryFinished);
    connect(m_manager, &LifxManager::bulbStateChange, this, &LifxApplication::bulbStateChange);
    connect(m_manager, &LifxManager::bulbLabelChange, this, &LifxApplication::bulbStateChange);
    connect(m_manager, &LifxManager::bulbPowerChange, this, &LifxApplication::bulbStateChange);

    m_layout = new QGridLayout();
    QWidget *central = new QWidget();
    central->setLayout(m_layout);
    
    QPalette pal(QColor(0,0,0));
    setBackgroundRole(QPalette::Window);
    pal.setColor(QPalette::Window, QColor(0xf0, 0xf0, 0xf0));
    setAutoFillBackground(true);
    setPalette(pal);
    
    setCentralWidget(central);
}

LifxApplication::~LifxApplication()
{
}

void LifxApplication::runStateCheck()
{
    QList<QString> keys = m_widgets.keys();

    foreach(auto key, keys) {
        LifxBulb *bulb = m_manager->getBulbByName(key);
        if (bulb) {
            BulbMessageHandler *handler = createHandler();
            handler->getColorForBulb(bulb);
        }
    }
}

void LifxApplication::bulbStateChange(LifxBulb* bulb)
{
    LightBulb *lb = m_widgets[bulb->label()];
    QHostAddress ip4(bulb->address().toIPv4Address());
    QString label = QString("%1\n%2\nRSSI: %3\nr:%4 g:%5 b:%6").arg(bulb->label()).arg(ip4.toString()).arg(bulb->rssi()).arg(bulb->color().red()).arg(bulb->color().green()).arg(bulb->color().blue());
    if (lb != nullptr) {
        lb->setColor(bulb->color());
        lb->setPower(bulb->power());
        lb->setText(label);
        lb->setLabel(bulb->label());
    }
}

void LifxApplication::setProductsJsonFile(QString path)
{
    QFile file(path);
    QJsonParseError error;
    
    if (file.open(QIODevice::ReadOnly)) {
        qDebug() << __PRETTY_FUNCTION__ << ": found" << file;
        QByteArray json = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        qDebug() << __PRETTY_FUNCTION__ << ":" << error.errorString();
        if (doc.isArray()) {
            qDebug() << __PRETTY_FUNCTION__ << ": Successfully parsed" << file;
            m_manager->setProductCapabilities(doc);
        }
    }
}

void LifxApplication::showEvent(QShowEvent *e)
{
    Q_UNUSED(e)
}

void LifxApplication::go()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "lifxtest", "lifxtest");
    QStringList list = settings.childGroups();

    if (!list.contains("Bulbs")) {
        settings.beginGroup("Bulbs");
        settings.setValue("exists", true);
        settings.endGroup();
        qDebug() << __PRETTY_FUNCTION__ << "Created bulbs group";
    }
    else {
        settings.beginGroup("Bulbs");
        QStringList groups = settings.childGroups();
        for (const auto &child : groups) {
            settings.beginGroup(child);
            QHostAddress address(settings.value("address").toString());
            int port = settings.value("port").toInt();
            BulbMessageHandler *handler = createHandler(address, port);
            handler->discover();
            settings.endGroup();
        }
        settings.endGroup();
    }
}

void LifxApplication::newColorForBulb(QString label, QColor color)
{
    qDebug() << __PRETTY_FUNCTION__;
    LifxBulb *bulb = m_manager->getBulbByName(label);
    if (bulb) {
        m_manager->changeBulbColor(bulb, color);
    }
}

void LifxApplication::bulbDiscoveryFinished(LifxBulb *bulb)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "lifxtest", "lifxtest");
    QStringList groups = settings.childGroups();

    if (groups.contains("Bulbs")) {
        settings.beginGroup("Bulbs");
        QStringList children = settings.childKeys();

        for (const auto &child : children) {
            QString label = QString("%1/label").arg(bulb->label());
            QString address = QString("%1/address").arg(bulb->label());
            QString port = QString("%1/port").arg(bulb->label());
            if (!child.contains(label)) {
                settings.setValue(label, bulb->label());
                settings.setValue(address, bulb->address().toString());
                settings.setValue(port, bulb->port());
            }
        }
        settings.endGroup();
    }
    createDisplayObject(bulb);
}

void LifxApplication::createDisplayObject(LifxBulb* bulb)
{
    LightBulb *widget = new LightBulb();
    connect(widget, &LightBulb::stateChangeEvent, this, &LifxApplication::widgetStateChange);
    connect(widget, &LightBulb::newColorChosen, this, &LifxApplication::newColorForBulb);
    QHostAddress ip4(bulb->address().toIPv4Address());
    QString label = QString("%1\n%2\nRSSI: %3\nr:%4 g:%5 b:%6").arg(bulb->label()).arg(ip4.toString()).arg(bulb->rssi()).arg(bulb->color().red()).arg(bulb->color().green()).arg(bulb->color().blue());
    if (widget != nullptr) {
        widget->setColor(bulb->color());
        widget->setPower(bulb->power());
        widget->setText(label);
        widget->setLabel(bulb->label());
    }
//    qDebug() << __PRETTY_FUNCTION__ << ": Creating" << bulb->label() << ": color" << bulb->color() << ", power" << bulb->power();
    m_layout->addWidget(widget, m_x, m_y);
    m_widgets.insert(bulb->label(), widget);
    m_y++;
    if (m_y > 2) {
        m_x++;
        m_y = 0;
    }
}

void LifxApplication::widgetStateChange(QString label, bool state)
{
    qDebug() << __PRETTY_FUNCTION__ << ": Handling state change for" << label << "to state" << state;
    LifxBulb *bulb = m_manager->getBulbByName(label);

    if (bulb) {
        BulbMessageHandler *handler = createHandler();
        handler->changeBulbState(bulb, state);
    }
}

void LifxApplication::handlerTimeout()
{
}

void LifxApplication::handlerComplete(uint32_t id)
{
    BulbMessageHandler *handler = m_handlers[id];

    if (handler) {
//        qDebug() << __PRETTY_FUNCTION__ << ": Cleaning up handler" << id;
        handler->deleteLater();
        m_handlers.remove(id);
    }
}

void LifxApplication::ackReceived(QHostAddress address, int port, uint64_t target)
{
    LifxBulb *bulb = m_manager->getBulbByMac(target);
    if (bulb) {
        BulbMessageHandler *handler = createHandler(address, port);
        handler->getColorForBulb(bulb);
    }
}

BulbMessageHandler * LifxApplication::createHandler()
{
    BulbMessageHandler *handler = new BulbMessageHandler(this);
    handler->setUniqueId(m_uniqueId++);
    while (m_handlers.contains(handler->uniqueId())) {
        handler->setUniqueId(m_uniqueId++);
    }
    m_handlers[handler->uniqueId()] = handler;
    connect(handler, &BulbMessageHandler::newPacket, m_manager, &LifxManager::newPacket);
    connect(handler, &BulbMessageHandler::bulbMessageTimeout, this, &LifxApplication::handlerTimeout);
    connect(handler, &BulbMessageHandler::complete, this, &LifxApplication::handlerComplete);
    connect(handler, &BulbMessageHandler::packetAck, this, &LifxApplication::ackReceived);
    return handler;
}

BulbMessageHandler * LifxApplication::createHandler(QHostAddress address, int port)
{
    BulbMessageHandler *handler = new BulbMessageHandler(address, port, this);
    handler->setUniqueId(m_uniqueId++);
    while (m_handlers.contains(handler->uniqueId())) {
        handler->setUniqueId(m_uniqueId++);
    }
    m_handlers[handler->uniqueId()] = handler;
    connect(handler, &BulbMessageHandler::newPacket, m_manager, &LifxManager::newPacket);
    connect(handler, &BulbMessageHandler::bulbMessageTimeout, this, &LifxApplication::handlerTimeout);
    connect(handler, &BulbMessageHandler::complete, this, &LifxApplication::handlerComplete);
    connect(handler, &BulbMessageHandler::packetAck, this, &LifxApplication::ackReceived);
    return handler;
}
