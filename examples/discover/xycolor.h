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

#ifndef XYCOLOR_H
#define XYCOLOR_H

#include <QtCore/QtCore>
#include <hueplusplus/ColorUnits.h>

class XYColor
{
public:
    static constexpr float x_default = 0.4573;
    static constexpr float y_default = .41;
    static constexpr float brightness_default = 1;
                                    
    XYColor(float x = x_default, float y = y_default, float b = brightness_default);
    XYColor(QString color);
    ~XYColor();
    XYColor(const XYColor &color);
    XYColor& operator=(XYColor& color);
    
    void setColor(QString color);
    static QStringList colors();

    float x(float v) { m_xyb.xy.x = v; return v; }
    float y(float v) { m_xyb.xy.y = v; return v; }
    float brightness(float v) { m_xyb.brightness = v; return v; }
    
    float x() const { return m_xyb.xy.x; }
    float y() const { return m_xyb.xy.y; }
    float brightness() const { return m_xyb.brightness; }
    
    hueplusplus::XYBrightness getXYB() const { return m_xyb; }
    
private:
    hueplusplus::XYBrightness m_xyb;
};

#endif // XYCOLOR_H
