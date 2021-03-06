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
    m_manager = new LifxManager();
    connect(m_manager, &LifxManager::bulbDiscoveryFinished, this, &LifxApplication::bulbDiscovered);
    connect(m_manager, &LifxManager::bulbStateChange, this, &LifxApplication::bulbStateChanged);
    
    m_x = 0;
    m_y = 0;
    
    m_stateCheckInterval = new QTimer();
    connect(m_stateCheckInterval, &QTimer::timeout, this, &LifxApplication::timeout);
    m_stateCheckInterval->setInterval(5000);
    m_stateCheckInterval->start();

    m_discoverInterval = new QTimer();
    connect(m_discoverInterval, &QTimer::timeout, this, &LifxApplication::discoverTimeout);
    m_discoverInterval->setInterval(60000);
    m_discoverInterval->start();

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

void LifxApplication::bulbStateChanged(LifxBulb* bulb)
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

void LifxApplication::timeout()
{
    m_manager->updateState();
}

void LifxApplication::discoverTimeout()
{
    m_manager->initialize();
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
            qDebug() << __PRETTY_FUNCTION__ << ": checking" << child;
            QHostAddress address(settings.value("address").toString());
            int port = settings.value("port").toInt();
            m_manager->discoverBulb(address, port);
            settings.endGroup();
        }
        settings.endGroup();
    }
    if (list.size())
        QTimer::singleShot(10000, this, &LifxApplication::runDiscovery);
    else
        runDiscovery();
}

void LifxApplication::newColorForBulb(QString label, QColor color)
{
    if (label.size()) {
        LifxBulb *bulb = m_manager->getBulbByName(label);
        if (bulb) {
            m_manager->changeBulbColor(bulb, color);
        }
    }
}

void LifxApplication::runDiscovery()
{
    m_manager->initialize();
}

void LifxApplication::bulbDiscovered(LifxBulb *bulb)
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

    qDebug().nospace().noquote() << "Found new bulb: " << bulb;
    LightBulb *widget = new LightBulb();
    connect(widget, &LightBulb::stateChangeEvent, this, &LifxApplication::widgetStateChange);
    connect(widget, &LightBulb::newColorChosen, this, &LifxApplication::newColorForBulb);
    widget->setText(bulb->label());
    widget->setColor(bulb->color());
    widget->setPower(bulb->power());
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
    LifxBulb *bulb = m_manager->getBulbByName(label);
    if (bulb) {
        m_manager->changeBulbState(bulb, !state);
    }
}
