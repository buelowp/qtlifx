/*
 * HSBK Color helper
 * 
 * Copyright (C) 2021  Peter Buelow <goballstate at gmail>
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

#include "hsbk.h"

/**
 * \fn HSBK::HSBK(uint16_t hue, uint16_t saturation, uint16_t brightness, uint16_t kelvin)
 * \param hue The H for the hsbk value
 * \param saturation The S for the hsbk value
 * \param brightness The B for the hsbk value
 * \param kelvin The K for the hsbk value
 * 
 * Constructs a new HSBK object with the given values. It will default to a warm white
 * if no parameters are sent
 */
HSBK::HSBK(uint16_t hue, uint16_t saturation, uint16_t brightness, uint16_t kelvin)
{
    m_hsbk.kelvin = kelvin;
    m_hsbk.hue = hue;
    m_hsbk.saturation = saturation;
    m_hsbk.brightness = brightness;
}

HSBK::~HSBK()
{
}

/**
 * \fn HSBK::HSBK(const HSBK& color)
 * 
 * Copy constructor
 */
HSBK::HSBK(const HSBK& color)
{
    memcpy(&m_hsbk, &color.m_hsbk, sizeof(lx_dev_color_t));
}

/**
 * \fn HSBK::HSBK(QString color)
 * 
 * Constructs an HSBK based on a color name
 */
HSBK::HSBK(QString color)
{
    setColor(color);
}

/**
 * \fn void HSBK::setColor(QString color)
 * \param color
 * 
 * Sets this HSBK object to the color identified by "color"
 */
void HSBK::setColor(QString color)
{
    if (color.toLower() == "candlelight") {
        m_hsbk.kelvin = 1500;
        m_hsbk.hue = 6242;
        m_hsbk.saturation = 0;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "sunset") {
        m_hsbk.kelvin = 2000;
        m_hsbk.hue = 6242;
        m_hsbk.saturation = 0;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "ultrawarm") {
        m_hsbk.kelvin = 2500;
        m_hsbk.hue = 6242;
        m_hsbk.saturation = 0;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "incandescent") {
        m_hsbk.kelvin = 2700;
        m_hsbk.hue = 6242;
        m_hsbk.saturation = 0;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "warm") {
        m_hsbk.kelvin = 3000;
        m_hsbk.hue = 6242;
        m_hsbk.saturation = 0;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "neutral") {
        m_hsbk.kelvin = 3500;
        m_hsbk.hue = 6242;
        m_hsbk.saturation = 0;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "cool") {
        m_hsbk.kelvin = 4000;
        m_hsbk.hue = 6242;
        m_hsbk.saturation = 0;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "cooldaylight") {
        m_hsbk.kelvin = 4500;
        m_hsbk.hue = 6242;
        m_hsbk.saturation = 0;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "softdaylight") {
        m_hsbk.kelvin = 5000;
        m_hsbk.hue = 6242;
        m_hsbk.saturation = 0;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "daylight") {
        m_hsbk.kelvin = 5600;
        m_hsbk.hue = 6242;
        m_hsbk.saturation = 0;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "orange") {
        m_hsbk.kelvin = 1500;
        m_hsbk.hue = 4550;
        m_hsbk.saturation = 65535;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "red") {
        m_hsbk.kelvin = 3000;
        m_hsbk.hue = 65535;
        m_hsbk.saturation = 65535;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "green") {
        m_hsbk.kelvin = 3000;
        m_hsbk.hue = 21840;
        m_hsbk.saturation = 65535;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "yellow") {
        m_hsbk.kelvin = 3000;
        m_hsbk.hue = 10920;
        m_hsbk.saturation = 65535;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "blue") {
        m_hsbk.kelvin = 3000;
        m_hsbk.hue = 43680;
        m_hsbk.saturation = 65535;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "purple") {
        m_hsbk.kelvin = 3000;
        m_hsbk.hue = 50870;
        m_hsbk.saturation = 65535;
        m_hsbk.brightness = 65535;
    }
    else {
        m_hsbk.kelvin = 3000;
        m_hsbk.hue = 6242;
        m_hsbk.saturation = 0;
        m_hsbk.brightness = 65535;
    }    
}

/**
 * \fn HSBK & HSBK::operator=(HSBK& color)
 * 
 * Assignment constructor
 */
HSBK & HSBK::operator=(HSBK& color)
{
    memcpy(&m_hsbk, &color.m_hsbk, sizeof(lx_dev_color_t));
    return *this;
}

/**
 * \fn QColor HSBK::getQColor()
 * \return Returns a QColor representation of this object
 * 
 * Attempts to map an HSBK to a QColor
 */
QColor HSBK::getQColor()
{
    QColor c;
    uint16_t max = std::numeric_limits<uint16_t>::max();
    
    c.setHsvF(m_hsbk.hue / max, m_hsbk.saturation / max, m_hsbk.brightness / max);
    return c;
}

/**
 * \fn uint16_t HSBK::b(float v)
 * \param v The brightness expressed as a percentage, between 0 and 1
 * \return Returns the newly assigned brightness
 * 
 * Will calculate the uint16_t value for brightness based on the percentage
 * of max brightness.
 */
uint16_t HSBK::b(float v)
{
    uint16_t max = std::numeric_limits<uint16_t>::max();
    
    if (v > 1)
        v = 1;
    if (v < 0)
        v = .01;
    
    m_hsbk.brightness = max * v;
    return m_hsbk.brightness;
}

/**
 * \fn uint16_t HSBK::h(float v)
 * \param v The 360 degree color wheel to convert from
 * \return Returns the new uint16_t h value after conversion
 * 
 * This takes the 360 degree color wheel and converts it to a full
 * uint16_t. I cheated and precalc'd the multiplier to make it easier
 * though it's to some extent an approximation.
 */
uint16_t HSBK::h(float v)
{
    float multiplier = 182.042;
    
    if (v > 360.0)
        v = 0;
    if (v < 0.0)
        v = 0;
    
    m_hsbk.hue = v * multiplier;
    return m_hsbk.hue;
}


/**
 * \fn QStringList HSBK::colors()
 * \return Returns a QStringList
 * 
 * A static function. This is the list of color names the class supports
 */
QStringList HSBK::colors()
{
    QStringList colorList = {"candlelight",
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
