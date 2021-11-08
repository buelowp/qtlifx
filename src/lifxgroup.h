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

#ifndef LIFXGROUP_H
#define LIFXGROUP_H

#include <QtCore/QtCore>

#include "lifxbulb.h"

/**
 * \class LifxGroup
 * 
 * This is a container for a LIFX group. It is able to store and retrieve any
 * bulb in the group and provides group details.
 */
class LifxGroup
{
public:
    LifxGroup(QString label, QByteArray uuid, uint64_t timestamp);
    ~LifxGroup();
    
    void addBulb(LifxBulb *bulb) { m_bulbs.push_back(bulb); }
    QString label() const { return m_label; }
    QByteArray uuid() const { return m_uuid; }
    uint64_t timestamp() const { return m_timestamp; }
    int size() const { return m_bulbs.size(); }
    bool contains(LifxBulb *bulb) { return m_bulbs.contains(bulb); }
    
private:
    QVector<LifxBulb*> m_bulbs;
    QString m_label;
    QByteArray m_uuid;
    uint64_t m_timestamp;
};

QDebug operator<<(QDebug debug, const LifxGroup &group);
QDebug operator<<(QDebug debug, const LifxGroup *group);

#endif // LIFXGROUP_H
