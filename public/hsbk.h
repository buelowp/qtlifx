/*
 * HSBK color helper
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

#pragma once

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include "defines.h"

/**
 * \class HSBK
 * \brief (PUBLIC) A container class with helper functions for direct use of the LIFX hsb color scheme
 * 
 * Attempts to abstract the HSBK concept, and allow for easy assignment and management
 * of LIFX specific HSBK colors. LIFX uses HSV, but replaces V with B for brightness. 
 * 
 * HSBK is Hue, Saturation, Brightness, and Kelvin. It maps directly to HSV, but includes
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

    /**
     * \fn uint16_t h(uint16_t v)
     * \param v The H value of the HSBK as a uint16_t
     * \return Returns the newly set value
     * 
     * Sets the H value to the range 0 to 65535
     */
    uint16_t h(uint16_t v) { m_hsbk.hue = v; return v; }
    uint16_t h(float v);
    /**
     * \fn uint16_t s(uint16_t v)
     * \param v The S value of the HSBK as a uint16_t
     * \return Returns the newly set value
     * 
     * Sets the S value to the range 0 to 65535
     */
    uint16_t s(uint16_t v) { m_hsbk.saturation = v; return v; }
    /**
     * \fn uint16_t b(uint16_t v)
     * \param v The B value of the HSBK as a uint16_t
     * \return Returns the newly set value
     * 
     * Sets the B value to the range 0 to 65535
     */
    uint16_t b(uint16_t v) { m_hsbk.brightness = v; return v; }
    uint16_t b(float v);
    /**
     * \fn uint16_t k(uint16_t v)
     * \param v The K value of the HSBK as a uint16_t
     * \return Returns the newly set value
     * 
     * Sets the K value to the range 0 to 65535
     */
    uint16_t k(uint16_t v) { m_hsbk.kelvin = v; return v; }
    
    /**
     * \fn uint16_t h()
     * \return Returns the newly set value
     * 
     * Returns the current H value of the HSBK
     */
    uint16_t h() const { return m_hsbk.hue; }
    /**
     * \fn uint16_t s()
     * \return Returns the newly set value
     * 
     * Returns the current S value of the HSBK
     */
    uint16_t s() const { return m_hsbk.saturation; }
    /**
     * \fn uint16_t b() 
     * \return Returns the newly set value
     * 
     * Returns the current B value of the HSBK
     */
    uint16_t b() const { return m_hsbk.brightness; }
    /**
     * \fn uint16_t k()
     * \return Returns the newly set value
     * 
     * Returns the current K value of the HSBK
     */
    uint16_t k() const { return m_hsbk.kelvin; }
    
    /**
     * \fn lx_dev_color_t getHSBK()
     * \return Returns the contained struct this class wraps
     * 
     * This class just contains the lx_dev_color_t structure, and
     * provides some work to make it useful. This function
     * returns the raw data directly.
     */
    lx_dev_color_t getHSBK() const { return m_hsbk; }
    QColor getQColor();
    void setColor(QString color);
    void hsvColorWheel(uint16_t degrees, uint8_t spct, uint8_t vpct);
    static QStringList colors();
    
private:
    lx_dev_color_t m_hsbk;                                      //!< The color struct used by the LIFX protocol
};

