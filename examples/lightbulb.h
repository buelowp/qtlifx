/*
 * Allows one to interact with a bulb as an example
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

#ifndef LIGHTBULB_H
#define LIGHTBULB_H

#include <QtWidgets/QtWidgets>

class LightBulb : public QWidget
{
public:
    LightBulb(QWidget *parent = nullptr);
    ~LightBulb();

    void setText(QString text) { m_text = text; update(); }
    void setColor(QColor c) { m_color = c; update(); }
    void setPower(uint16_t p) { m_state = (p != 0); }
    
protected:
    void paintEvent(QPaintEvent *e) override;
    
private:
    QString m_text;
    QColor m_color;
    bool m_state;
};

#endif // LIGHTBULB_H
