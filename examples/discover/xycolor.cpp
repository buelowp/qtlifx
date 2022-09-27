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

#include "xycolor.h"

XYColor::XYColor(float x, float y, float b)
{
    m_xyb.brightness = b;
    m_xyb.xy.x = x;
    m_xyb.xy.y = y;
}

XYColor::XYColor(QString color)
{
    setColor(color);
}

XYColor::~XYColor()
{
}

XYColor & XYColor::operator=(XYColor& color)
{
    memcpy(&m_xyb, &color.m_xyb, sizeof(hueplusplus::XYBrightness));
    return *this;
}

XYColor::XYColor(const XYColor& color)
{
    memcpy(&m_xyb, &color.m_xyb, sizeof(hueplusplus::XYBrightness));  
}

void XYColor::setColor(QString color)
{
    if (color.toLower() == "candlelight") {
        m_xyb.brightness = brightness_default;
        m_xyb.xy.x = 0.585725205;
        m_xyb.xy.y = 0.39311745;
    }
    else if (color.toLower() == "sunset") {
        m_xyb.brightness = brightness_default;
        m_xyb.xy.x = 0.526685851;
        m_xyb.xy.y = 0.413295716;
    }
    else if (color.toLower() == "ultrawarm") {
        m_xyb.brightness = brightness_default;
        m_xyb.xy.x = 0.477003132;
        m_xyb.xy.y = 0.413676523;
    }
    else if (color.toLower() == "incandescent") {
        m_xyb.brightness = brightness_default;
        m_xyb.xy.x = 0.459867518;
        m_xyb.xy.y = 0.410600974;
    }
    else if (color.toLower() == "warm") {
        m_xyb.brightness = brightness_default;
        m_xyb.xy.x = 0.436939256;
        m_xyb.xy.y = 0.404076823;
    }
    else if (color.toLower() == "neutral") {
        m_xyb.brightness = brightness_default;
        m_xyb.xy.x = 0.405311084;
        m_xyb.xy.y = 0.390719511;
    }
    else if (color.toLower() == "cool") {
        m_xyb.brightness = brightness_default;
        m_xyb.xy.x = 0.380446356;
        m_xyb.xy.y = 0.376751159;
    }
    else if (color.toLower() == "cooldaylight") {
        m_xyb.brightness = brightness_default;
        m_xyb.xy.x = 0.360793649;
        m_xyb.xy.y = 0.363548975;
    }
    else if (color.toLower() == "softdaylight") {
        m_xyb.brightness = brightness_default;
        m_xyb.xy.x = 0.345106666;
        m_xyb.xy.y = 0.351612579;
    }
    else if (color.toLower() == "daylight") {
        m_xyb.brightness = brightness_default;
        m_xyb.xy.x = 0.330202691;
        m_xyb.xy.y = 0.339083802;
    }
    else if (color == "orange") {
        m_xyb.brightness = brightness_default;
        m_xyb.xy.x = .6454;
        m_xyb.xy.y = .3429;
    }
    else if (color == "red") {
        m_xyb.brightness = brightness_default;
        m_xyb.xy.x = .6632;
        m_xyb.xy.y = .2946;
    }
    else if (color == "blue") {
        m_xyb.brightness = brightness_default;
        m_xyb.xy.x = .1545;
        m_xyb.xy.y = .0998;
    }
    else if (color == "yellow") {
        m_xyb.brightness = brightness_default;
        m_xyb.xy.x = .479;
        m_xyb.xy.y = .4679;
    }
    else if (color == "green") {
        m_xyb.brightness = brightness_default;
        m_xyb.xy.x = .17;
        m_xyb.xy.y = .7;
    }
    else if (color == "purple") {
        m_xyb.brightness = brightness_default;
        m_xyb.xy.x = .1915;
        m_xyb.xy.y = .0661;
    }
    else if (color == "nightlight") {
        m_xyb.brightness = .00393701;
        m_xyb.xy.x = 0.561;
        m_xyb.xy.y = 0.4042;
    }
    else {
        m_xyb.brightness = brightness_default;
        m_xyb.xy.x = 0.436939256;
        m_xyb.xy.y = 0.404076823;
    }
}

QStringList XYColor::colors()
{
    QStringList colorList = {"nightlight",
                            "candlelight",
                            "sunset",
                            "ultrawarm",
                            "incandescent",
                            "warm",
                            "neutral",
                            "cool",
                            "cooldaylight",
                            "softdaylight",
                            "daylight",
                            "orange",
                            "yellow",
                            "red",
                            "green",
                            "blue",
                            "purple"};
                            
    return colorList;
}
