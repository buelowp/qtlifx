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

#include "lightbulb.h"

LightBulb::LightBulb(QWidget *parent) : QWidget(parent)
{
    m_state = false;
    connect(this, &LightBulb::openColorDialog, this, &LightBulb::showColorDialog);
}

LightBulb::~LightBulb()
{

}

QSize LightBulb::minimumSizeHint() const
{
    int min = qMin(width(), height());
    return QSize(min, min);
}

QSize LightBulb::sizeHint() const
{
    int min = qMin(width(), height());
    return QSize(min, min);
}

void LightBulb::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        emit stateChangeEvent(m_label, !m_state);
    }
    if (e->button() == Qt::RightButton)
        emit openColorDialog();
}

void LightBulb::showColorDialog()
{
    const QColor color = QColorDialog::getColor(m_color, this, "Select Color");
    emit newColorChosen(m_label, color);
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
    painter.setPen(QColor("black"));
    painter.drawEllipse(circleX, 0, height(), height());
    if (m_text.size()) {
        if (!m_state)
            painter.setPen(Qt::white);
        else
            painter.setPen(Qt::black);
        
        QFont font = painter.font();
        font.setPixelSize(14);
        QFontMetrics fm(font);
        painter.setFont(font);
        QRect fontrect = fm.boundingRect(rect(), Qt::TextWordWrap, m_text);
        int xoffset = width() / 2 - fontrect.width() / 2;
        int yoffset = height() / 2 - fontrect.height() / 2;
        QRect drect(xoffset, yoffset, width() / 2, height() / 2);
        painter.drawText(drect, Qt::TextWordWrap, m_text);
    }    
}

void LightBulb::setPower(uint16_t p)
{
    if (p == 0)
        m_state = false;

    if (p == 65535)
        m_state = true;
}
