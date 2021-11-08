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

#include "lifxgroup.h"

LifxGroup::LifxGroup(QString label, QByteArray uuid, uint64_t timestamp) :
    m_label(label), m_uuid(uuid), m_timestamp(timestamp)
{
}

LifxGroup::~LifxGroup()
{
}

/**
 * \fn QDebug operator<<(QDebug debug, const LifxGroup &bulb)
 * \brief Pretty print the LifxGroup object
 * 
 * For use with qDebug() only
 * 
 * Only prints the ipv4 address at this time
 */
QDebug operator<<(QDebug debug, const LifxGroup &group)
{
    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << "Group: " << group.label() << " [" << group.uuid().toHex() << "] created at " << group.timestamp();

    return debug;
}

/**
 * \fn QDebug operator<<(QDebug debug, const LifxGroup *bulb)
 * \brief Pretty print the LifxGroup object
 * 
 * For use with qDebug() only
 * 
 * Only prints the ipv4 address at this time
 */
QDebug operator<<(QDebug debug, const LifxGroup *group)
{
    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << "Group: " << group->label() << " [" << group->uuid().toHex() << "] created at " << group->timestamp() << " has " << group->size() << " bulbs";
    return debug;
}
