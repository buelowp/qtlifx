/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2021  <copyright holder> <email>
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
    
    m_x = 0;
    m_y = 0;
    
    m_layout = new QGridLayout();
    QWidget *central = new QWidget();
    central->setLayout(m_layout);
    
    QPalette pal(QColor(0,0,0));
    setBackgroundRole(QPalette::Window);
    pal.setColor(QPalette::Window, Qt::white);
    setAutoFillBackground(true);
    setPalette(pal);
    
    setCentralWidget(central);
}

LifxApplication::~LifxApplication()
{

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
    qDebug() << __PRETTY_FUNCTION__;
}

void LifxApplication::go()
{
    m_manager->initialize();
}

void LifxApplication::bulbDiscovered(LifxBulb *bulb)
{
    LightBulb *widget = new LightBulb();
    widget->setText(bulb->label());
    widget->setColor(bulb->color());
    widget->setPower(bulb->power());
    m_layout->addWidget(widget, m_x, m_y);
    m_y++;
    if (m_y > 2) {
        m_x++;
        m_y = 0;
    }
}
