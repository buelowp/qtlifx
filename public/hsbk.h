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

class HSBK
{
public:
    HSBK(uint16_t hue = 6242, uint16_t saturation = 0, uint16_t brightness = 65535, uint16_t kelvin = 3000);
    HSBK(QString color);
    ~HSBK();
    HSBK(const HSBK &color);
    HSBK& operator=(HSBK& color);

    uint16_t h(float v) { m_hsbk.hue = v; return v; }
    uint16_t s(float v) { m_hsbk.saturation = v; return v; }
    uint16_t b(float v) { m_hsbk.brightness = v; return v; }
    uint16_t k(float v) { m_hsbk.kelvin = v; return v; }
    
    uint16_t h() const { return m_hsbk.hue; }
    uint16_t s() const { return m_hsbk.saturation; }
    uint16_t b() const { return m_hsbk.brightness; }
    uint16_t k() const { return m_hsbk.kelvin; }
    
    lx_dev_color_t getHSBK() const { return m_hsbk; }
    QColor getQColor();
    static QStringList colors();
    
private:
    lx_dev_color_t m_hsbk;
};

