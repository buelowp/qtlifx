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
    
    /**
     * \fn void addBulb(LifxBulb *bulb)
     * \param bulb Pointer to a LifxBulb class
     * \brief adds a bulb to this group
     */
    void addBulb(LifxBulb *bulb) { m_bulbs.push_back(bulb); }
    /**
     * \fn QString label() const
     * \return QString group name
     * \brief Returns the group name
     */
    QString label() const { return m_label; }
    /**
     * \fn QByteArray uuid() const
     * \return Returns the QByteArray UUID assigned to this group
     * \brief This is the UUID as returned by the query and is mostly used for MAP keys in this library
     */
    QByteArray uuid() const { return m_uuid; }
    /**
     * \fn uint64_t timestamp() const
     * \return uint64_t field containing the last modified timestamp from the bulb
     * \brief This is the last time the group name was modified by someone as reported by the bulb
     */
    uint64_t timestamp() const { return m_timestamp; }
    /**
     * \fn int size() const
     * \return An int size which is the number of bulbs in the group
     * \brief Returns the number of bulbs in the group which is just the QVector size in this case
     */
    int size() const { return m_bulbs.size(); }
    /**
     * \fn bool contains(LifxBulb *bulb)
     * \param LifxBulb pointer to a bulb
     * \return A bool indicating whether a bulb is in the vector
     * \brief Checks the group for a bulb, returns true if the bulb is in the group, false otherwise
     */
    bool contains(LifxBulb *bulb) { return m_bulbs.contains(bulb); }
    /**
     * \fn QVector<LifxBulb*>& bulbs()
     * \return The vector of bulbs in this group
     * \brief Returns the bulbs list this group owns as a reference
     */
    QVector<LifxBulb*>& bulbs() { return m_bulbs; }
    
private:
    QVector<LifxBulb*> m_bulbs;     //!< Vector with all bulbs that exist in this group
    QString m_label;                //!< The group name/label
    QByteArray m_uuid;              //!< Gropu assigned UUID, used for map storage later
    uint64_t m_timestamp;           //!< Assigned timestamp of last time group was modified, currently not used
};

QDebug operator<<(QDebug debug, const LifxGroup &group);
QDebug operator<<(QDebug debug, const LifxGroup *group);

#endif // LIFXGROUP_H
