#include "hsbk.h"

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

HSBK::HSBK(const HSBK& color)
{
    memcpy(&m_hsbk, &color.m_hsbk, sizeof(lx_dev_color_t));
}

HSBK::HSBK(QString color)
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
}

HSBK & HSBK::operator=(HSBK& color)
{
    memcpy(&m_hsbk, &color.m_hsbk, sizeof(lx_dev_color_t));
    return *this;
}

QColor HSBK::getQColor()
{
    QColor c;
    uint16_t max = std::numeric_limits<uint16_t>::max();
    
    c.setHsvF(m_hsbk.hue / max, m_hsbk.saturation / max, m_hsbk.brightness / max);
    return c;
}

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
