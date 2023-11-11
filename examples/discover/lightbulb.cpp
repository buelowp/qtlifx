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

LightBulb::LightBulb(LifxBulb *bulb, QWidget *parent) : QWidget(parent)
{
    if (bulb) {
        m_bulb = bulb;
        m_updateTimer = new QTimer();
        m_updateTimer->setInterval(10000);
        m_updateTimer->start();
        connect(m_updateTimer, &QTimer::timeout, this, &LightBulb::stateTimeout);
        connect(this, &LightBulb::openColorDialog, this, &LightBulb::showColorDialog);
        connect(bulb, &LifxBulb::updated, this, &LightBulb::updated);
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": Bulb was NULL";
    }
}

LightBulb::~LightBulb()
{
}

void LightBulb::updated()
{
    update();
}

void LightBulb::stateTimeout()
{
    m_bulb->getRSSI();
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
        emit m_bulb->toggleState();
    }
    if (e->button() == Qt::RightButton)
        emit openColorDialog();
}

void LightBulb::showColorDialog()
{
    QColor color = QColorDialog::getColor(m_bulb->color(), this, "Select Color");
    m_bulb->changeColor(color);
}

void LightBulb::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e)
    
    QPainter painter(this);
    int circleX = width() / 2 - height() / 2;
    painter.setRenderHint(QPainter::Antialiasing);
    m_text =  QString("%1\n%2\nRSSI: %3\nr:%4 g:%5 b:%6").arg(m_bulb->label()).arg(m_bulb->address().toString().mid(7)).arg(m_bulb->rssi()).arg(m_bulb->color().red()).arg(m_bulb->color().green()).arg(m_bulb->color().blue());

    if (m_bulb->isOn())
        painter.setBrush(m_bulb->color());
    else
        painter.setBrush(QColor("black"));
    painter.setPen(QColor("black"));
    painter.drawEllipse(circleX, 0, height(), height());
    if (m_text.size()) {
        if (!m_bulb->isOn())
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
