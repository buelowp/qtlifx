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

#include "lifxproduct.h"

LifxProduct::LifxProduct()
{
    m_color = true;
    m_chain = false;
    m_matrix = false;
    m_infrared = false;
    m_multizone = false;
}

LifxProduct::~LifxProduct()
{
}

QDebug operator<<(QDebug debug, const LifxProduct& product)
{
    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << product.name() << " [" << product.lowKelvin() << "," << product.highKelvin() << "] color:" << product.colorCapable();
    debug.nospace().noquote() << " chain:" << product.canChain() << " matrix:" << product.matrixCapable() << " IR:" << product.irCapable();
    debug.nospace().noquote() << " multizone:" << product.multizoneCapable();
    return debug;
}

QDebug operator<<(QDebug debug, const LifxProduct *product)
{
    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << product->name() << " [" << product->lowKelvin() << "," << product->highKelvin() << "] color:" << product->colorCapable();
    debug.nospace().noquote() << " chain:" << product->canChain() << " matrix:" << product->matrixCapable() << " IR:" << product->irCapable();
    debug.nospace().noquote() << " multizone:" << product->multizoneCapable();
    return debug;
}
