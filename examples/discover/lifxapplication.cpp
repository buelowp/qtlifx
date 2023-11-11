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

    m_protocol = new LifxProtocol();
    m_protocol->enableDebug(false);
    connect(m_protocol, &LifxProtocol::newBulbFound, this, &LifxApplication::newBulb);
    connect(m_protocol, &LifxProtocol::newGroupFound, this, &LifxApplication::newGroup);

    m_discoverInterval = new QTimer();
    m_discoverInterval->setInterval(60000);
    connect(m_discoverInterval, &QTimer::timeout, m_protocol, &LifxProtocol::discover);

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

void LifxApplication::setProductsJsonFile(QString path)
{
/*
    QFile file(path);
    QJsonParseError error;
    
    if (file.open(QIODevice::ReadOnly)) {
        qDebug() << __PRETTY_FUNCTION__ << ": found" << file;
        QByteArray json = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        qDebug() << __PRETTY_FUNCTION__ << ":" << error.errorString();
        if (doc.isArray()) {
            qDebug() << __PRETTY_FUNCTION__ << ": Successfully parsed" << file;
            m_protocol->setProductCapabilities(doc);
        }
    }
*/
}

void LifxApplication::showEvent(QShowEvent *e)
{
    Q_UNUSED(e)
}

void LifxApplication::go()
{
    m_protocol->discover();
}

void LifxApplication::newGroup(LifxGroup* group)
{
    Q_UNUSED(group)
}

void LifxApplication::newBulb(LifxBulb *bulb)
{
    qDebug() << __PRETTY_FUNCTION__ << ":" << bulb;
    createDisplayObject(bulb);
    bulb->setUpdateInterval(2000);
}

void LifxApplication::createDisplayObject(LifxBulb* bulb)
{
    LightBulb *widget = new LightBulb(bulb);
    m_widgets[bulb] = widget;
    m_layout->addWidget(widget, m_x, m_y);
    m_y++;
    if (m_y > 2) {
        m_x++;
        m_y = 0;
    }
}
