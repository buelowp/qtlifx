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

#pragma once

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include "defines.h"

/**
 * \class HSBK
 * 
 * Attempts to abstract the HSBK concept, and allow for easy assignment and management
 * of LIFX specific HSBK colors.
 * 
 * HSBK is Hue, Saturation, Brightness, and Kelvin. It roughly maps to HSV, but includes
 * the Kelvin argument. When assigning the HSBK to a QColor, Kelving is ignored
 * unfortunately. Therefore, it's not possible to map 1-1. I'm still looking into how
 * to "fix" this.
 */
class HSBK
{
public:
    HSBK(uint16_t hue = 6242, uint16_t saturation = 0, uint16_t brightness = 65535, uint16_t kelvin = 3000);
    HSBK(QString color);
    ~HSBK();
    HSBK(const HSBK &color);
    HSBK& operator=(HSBK& color);

    uint16_t h(uint16_t v) { m_hsbk.hue = v; return v; }            //!< Sets and returns the H from the HSBK
    uint16_t h(float v);                                            //!< Sets H to a percentage of the 360 color wheel
    uint16_t s(uint16_t v) { m_hsbk.saturation = v; return v; }     //!< Sets and returns the S from the HSBK
    uint16_t b(uint16_t v) { m_hsbk.brightness = v; return v; }     //!< Sets and returns the B from the HSBK
    uint16_t b(float v);                                            //!< Sets brightness by percent, where v > 0 && v < 1
    uint16_t k(uint16_t v) { m_hsbk.kelvin = v; return v; }         //!< Sets and returns the K from the HSBK
    
    uint16_t h() const { return m_hsbk.hue; }                   //!< Returns the H from the HSBK
    uint16_t s() const { return m_hsbk.saturation; }            //!< Returns the S from the HSBK
    uint16_t b() const { return m_hsbk.brightness; }            //!< Returns the B from the HSBK
    uint16_t k() const { return m_hsbk.kelvin; }                //!< Returns the K from the HSBK
    
    lx_dev_color_t getHSBK() const { return m_hsbk; }           //!< Returns the HSBK structure used by the LIFX protocol
    QColor getQColor();
    void setColor(QString color);
    static QStringList colors();
    
private:
    lx_dev_color_t m_hsbk;                                      //!< The color struct used by the LIFX protocol
};

