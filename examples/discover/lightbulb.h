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
#include "lifxbulb.h"

class LightBulb : public QWidget
{
    Q_OBJECT

public:
    LightBulb(LifxBulb *bulb, QWidget *parent = nullptr);
    ~LightBulb();

    void setText(QString text) { m_text = text; update(); }
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    bool localState() { return m_bulb->isOn(); }
    QString label() { return m_bulb->label(); }
    LifxBulb* bulb() { return m_bulb; }

public slots:
    void showColorDialog();
    void stateTimeout();
    void updated();

protected:
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;

signals:
    void stateChangeEvent(LifxBulb *bulb, bool state);
    void colorChangeEvent(LifxBulb *bulb);
    void openColorDialog();
    void newColorChosen(LifxBulb *bulb, QColor color);
    void requestStatus(LifxBulb *bulb);

private:
    LifxBulb *m_bulb;
    QTimer *m_updateTimer;
    QString m_text;
};

#endif // LIGHTBULB_H
