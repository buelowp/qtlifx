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
#include "lifxgroup.h"

/**
 * \class LifxManager
 * \brief Public interface to the LIFX bulbs
 */
class LifxManager : public QObject
{
    Q_OBJECT

public:
    LifxManager(QObject *parent = nullptr);
    ~LifxManager();
    
    void initialize();
    LifxBulb* getBulbByName(QString &name);
    LifxBulb* getBulbByMac(uint64_t target);
    LifxGroup* getGroupByName(QString &name);
    LifxGroup* getGroupByUUID(QByteArray &uuid);
    
public slots:
    void discoveryFailed();
    void newPacket(LifxPacket *packet);
    void changeBulbColor(QString &name, QColor color);
    void changeBulbColor(uint64_t target, QColor color);
    void changeGroupColor(QByteArray &uuid, QColor color);
    void changeGroupState(QByteArray &uuid, bool state);

signals:
    void finished();
    void newBulbAvailable(QString, uint64_t);
    void bulbStateChanged(QString, uint64_t);
    void newGroupAvailable(QString, QByteArray);

private:
    LifxProtocol *m_protocol;
    QMap<uint64_t, LifxBulb*> m_bulbs;
    QMap<QString, LifxBulb*> m_bulbsByName;
    QMap<QByteArray, LifxGroup*> m_groups;
};

#endif // LIGHTMANAGER_H
