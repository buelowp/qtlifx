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

    m_manager = new LifxManager();
    m_manager->enableDebug(false);
    connect(m_manager, &LifxManager::bulbDiscoveryFinished, this, &LifxApplication::bulbDiscoveryFinished);
    connect(m_manager, &LifxManager::bulbStateChange, this, &LifxApplication::bulbStateChange);
    connect(m_manager, &LifxManager::bulbLabelChange, this, &LifxApplication::bulbStateChange);
    connect(m_manager, &LifxManager::bulbPowerChange, this, &LifxApplication::bulbStateChange);
    connect(m_manager, &LifxManager::ack, this, &LifxApplication::ackReceived);
    connect(m_manager, &LifxManager::bulbDiscoveryFailed, this, &LifxApplication::discoveryFailed);

    m_discoverInterval = new QTimer(this);
    m_discoverInterval->setInterval(60000);
    connect(m_discoverInterval, &QTimer::timeout, m_manager, &LifxManager::discover);

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

void LifxApplication::discoveryFailed()
{
    qWarning() << __PRETTY_FUNCTION__;
}

void LifxApplication::runStateCheck(LifxBulb *bulb)
{
    m_manager->getColorForBulb(bulb);
}

void LifxApplication::bulbStateChange(LifxBulb* bulb)
{
    if (m_widgets.size()) {
        LightBulb *b = m_widgets[bulb];
        if (b)
            b->update();
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
    m_manager->discover();
}

void LifxApplication::newColorForBulb(LifxBulb *bulb, QColor color)
{
    m_manager->changeBulbColor(bulb, color);
}

void LifxApplication::bulbDiscoveryFinished(LifxBulb *bulb)
{
    qDebug() << __PRETTY_FUNCTION__ << ":" << bulb;
    createDisplayObject(bulb);
}

void LifxApplication::createDisplayObject(LifxBulb* bulb)
{
    LightBulb *widget = new LightBulb(bulb);
    m_widgets[bulb] = widget;
    connect(widget, &LightBulb::stateChangeEvent, this, &LifxApplication::widgetStateChange);
    connect(widget, &LightBulb::newColorChosen, this, &LifxApplication::newColorForBulb);
    connect(widget, &LightBulb::requestStatus, this, &LifxApplication::runStateCheck);
    m_layout->addWidget(widget, m_x, m_y);
    qDebug() << __PRETTY_FUNCTION__ << ":" << bulb;
    m_y++;
    if (m_y > 2) {
        m_x++;
        m_y = 0;
    }
}

void LifxApplication::widgetStateChange(LifxBulb *bulb, bool state)
{
    m_manager->changeBulbState(bulb, state, 0, true);
}

void LifxApplication::handlerTimeout(uint32_t uniqueId)
{
    qWarning() << __PRETTY_FUNCTION__ << ": Operation handled by" << uniqueId << "timed out";
}

void LifxApplication::ackReceived(uint32_t uniqueId)
{
}
