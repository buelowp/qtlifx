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

#ifndef LIFXPRODUCT_H
#define LIFXPRODUCT_H

#include <QtCore/QtCore>

/**
 * \class LifxProduct
 * \brief (PRIVATE) Contains product details for a bulb from products.json
 * 
 * This class is internal to a LifxBulb object, and any data in may be referenced
 * directly from the bulb. This class is not available to external users.
 */
class LifxProduct
{
public:
    LifxProduct();
    ~LifxProduct();

    void setName(QString name) { m_name = name; }
    void setColor(bool color) { m_color = color; }
    void setChain(bool chain) { m_chain = chain; }
    void setMatrix(bool matrix) { m_matrix = matrix; }
    void setIR(bool ir) { m_infrared = ir; }
    void setMultiZone(bool mz) { m_multizone = mz; }
    void setRange(int low, int high) { m_lowKelvin = low; m_highKelvin = high; }
    
    QString name() const { return m_name; }
    bool colorCapable() const { return m_color; }
    bool matrixCapable() const { return m_matrix; }
    bool canChain() const { return m_chain; }
    bool irCapable() const { return m_infrared; }
    bool multizoneCapable() const { return m_multizone; }
    int highKelvin() const { return m_highKelvin; }
    int lowKelvin() const { return m_lowKelvin; }
    
private:
    float toKey(int major, int minor) const;
    
    QString m_name;
    bool m_color;
    bool m_chain;
    bool m_matrix;
    bool m_infrared;
    bool m_multizone;
    int m_lowKelvin;
    int m_highKelvin;
};

QDebug operator<<(QDebug debug, const LifxProduct &product);
QDebug operator<<(QDebug debug, const LifxProduct *product);
#endif // LIFXPRODUCT_H
