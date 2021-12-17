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
        m_hsbk.hue = 4631;
        m_hsbk.saturation = 65535;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "red") {
        m_hsbk.kelvin = 1500;
        m_hsbk.hue = 63963;
        m_hsbk.saturation = 65535;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "green") {
        m_hsbk.kelvin = 1500;
        m_hsbk.hue = 23398;
        m_hsbk.saturation = 65535;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "yellow") {
        m_hsbk.kelvin = 1500;
        m_hsbk.hue = 11002;
        m_hsbk.saturation = 65535;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "blue") {
        m_hsbk.kelvin = 1500;
        m_hsbk.hue = 44392;
        m_hsbk.saturation = 65535;
        m_hsbk.brightness = 65535;
    }
    else if (color.toLower() == "purple") {
        m_hsbk.kelvin = 1500;
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
