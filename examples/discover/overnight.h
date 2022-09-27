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

#pragma once

#include <cmath>

#include <sunset.h>
#include <hsbk.h>

#include "program.h"
#include "xycolor.h"

#include "statemachines/offstate.h"
#include "statemachines/onstate.h"
#include "statemachines/dimstate.h"
#include "statemachines/morningstate.h"

#define LATITUDE        42.0039
#define LONGITUDE       -87.9703

class Overnight : public Program
{
    Q_OBJECT
public:
    Overnight(LifxManager *lifx, std::vector<hueplusplus::Light> *hue, QObject *parent = nullptr);
    ~Overnight();

    void runPlugin() override;
    void stopPlugin() override;
    
    void setHueBulbNames(QStringList &names);
    void setLifxBulbNames(QStringList &names);
    void setNightLightNames(QStringList &names);
    
signals:
    void earlymorning();
    void sunrise();
    void sunset();
    void dimmer();
    void pluginReady();
    void startelevn();
    void stairlights();
    void indoorLightsOn();
    void indoorLightsOff();
    
protected slots:
    void timeout();
    void turnOffLights();
    void turnOnLights();
    void dimLights();
    void started();
    void morningLights();
    void turnOnNightLights();
    void turnOffNightLights();
    void setDaysEventTimes();    
    
private:
    void indoorStateMachine();
    void outdoorStateMachine();
    
    QVector<LifxBulb*> m_lifxBulbs;
    QVector<int> m_hueBulbsOutdoor;
    QVector<int> m_hueBulbsIndoor;
    QMap<QString, int> m_events;
    QTimer *m_nextEventTimer;
    QTimer *m_setEventTimes;
    QStateMachine *m_outdoorLights;
    QStateMachine *m_indoorLightsSM;
    QMutex m_mutex;
    SunSet m_sun;
    QState *m_offstate;
    QState *m_onstate;
    QState *m_dimstate;
    QState *m_earlystate;
    int m_startCount;
};

