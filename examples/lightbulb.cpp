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

#include "lightbulb.h"

LightBulb::LightBulb(QWidget *parent) : QWidget(parent)
{
    m_state = false;
}

LightBulb::~LightBulb()
{

}

void LightBulb::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e)
    
    QPainter painter(this);
    int circleX = width() / 2 - height() / 2;
    painter.setRenderHint(QPainter::Antialiasing);
    if (!m_state)
        m_color = QColor("black");
        
    painter.setBrush(m_color);
    painter.setPen(m_color);
    painter.drawEllipse(circleX, 0, height(), height());
    if (m_text.size()) {
        if (!m_state)
            painter.setPen(Qt::white);
        else
            painter.setPen(Qt::black);
        
        QFont font = painter.font();
        font.setPixelSize(12);
        QFontMetrics fm(font);
        painter.setFont(font);
        int xoffset = fm.boundingRect(m_text).width() / 2;
        int yoffset = fm.boundingRect(m_text).height() / 2;
        painter.drawText(width() / 2 - xoffset, (height() + yoffset) / 2, m_text);
    }    
}
