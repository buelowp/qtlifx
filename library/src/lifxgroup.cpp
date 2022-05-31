/*
 * LIFX groups as reported by the bulbs
 * 
 * Copyright (C) 2021 Peter Buelow <goballstate at gmail>
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
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(group.timestamp() / 1000);
    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << "Group: " << group.label() << " [" << group.uuid().toHex() << "] created at " << dt.toString();

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
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(group->timestamp() / 1000);
    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << "Group: " << group->label() << " [" << group->uuid().toHex() << "] created at " << dt.toString() << " has " << group->size() << " bulbs";
    return debug;
}
