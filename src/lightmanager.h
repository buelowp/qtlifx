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

#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <QtCore/QtCore>

#include "defines.h"
#include "lifxprotocol.h"
#include "lifxbulb.h"
#include "lifxpacket.h"

class LightManager : public QObject
{
    Q_OBJECT

public:
    LightManager(QObject *parent = nullptr);
    ~LightManager();
    
    void initialize();
    
public slots:
    void discoveryFailed();
    void newPacket(LifxPacket *packet);

signals:
    void finished();

private:
    LifxProtocol *m_protocol;
    QMap<uint64_t, LifxBulb*> m_bulbs;
};

#endif // LIGHTMANAGER_H
