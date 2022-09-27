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

#include "overnight.h"

Overnight::Overnight(LifxManager *lifx, std::vector<hueplusplus::Light> *hue, QObject *parent) : Program(lifx, hue, parent)
{
    m_nextEventTimer = new QTimer();
    connect(m_nextEventTimer, &QTimer::timeout, this, &Overnight::timeout);
    m_setEventTimes = new QTimer();
    connect(m_setEventTimes, &QTimer::timeout, this, &Overnight::setDaysEventTimes);
    m_startCount = 0;
    
    setDaysEventTimes();
    
    outdoorStateMachine();
    indoorStateMachine();
}

Overnight::~Overnight()
{
    m_outdoorLights->stop();
    m_outdoorLights->deleteLater();
    stopPlugin();
    m_nextEventTimer->deleteLater();
}

void Overnight::outdoorStateMachine()
{
    qDebug() << "Creating outdoor state machine";
    m_outdoorLights = new QStateMachine();
    QState *m_offstate = new QState();
    QState *m_onstate = new QState();
    QState *m_dimstate = new QState();
    QState *m_earlystate = new QState();
    
    m_dimstate->addTransition(this, &Overnight::earlymorning, m_earlystate);
    m_onstate->addTransition(this, &Overnight::dimmer, m_dimstate);
    m_offstate->addTransition(this, &Overnight::sunset, m_onstate);
    m_offstate->addTransition(this, &Overnight::startelevn, m_dimstate);
    m_offstate->addTransition(this, &Overnight::earlymorning, m_earlystate);
    m_offstate->addTransition(this, &Overnight::dimmer, m_dimstate);
    m_earlystate->addTransition(this, &Overnight::sunrise, m_offstate);
    
    connect(m_dimstate, &QState::entered, this, &Overnight::dimLights);
    connect(m_onstate, &QState::entered, this, &Overnight::turnOnLights);
    connect(m_offstate, &QState::entered, this, &Overnight::turnOffLights);
    connect(m_earlystate, &QState::entered, this, &Overnight::morningLights);
    connect(m_outdoorLights, &QStateMachine::started, this, &Overnight::started);
    
    m_outdoorLights->addState(m_offstate);
    m_outdoorLights->addState(m_onstate);
    m_outdoorLights->addState(m_dimstate);
    m_outdoorLights->addState(m_earlystate);
    m_outdoorLights->setInitialState(m_offstate);
    m_outdoorLights->start();
}

void Overnight::indoorStateMachine()
{
    qDebug() << "Creating indoor state machine";
    m_indoorLightsSM = new QStateMachine();
    QState *on = new QState();
    QState *off = new QState();
    
    on->addTransition(this, &Overnight::sunrise, off);
    off->addTransition(this, &Overnight::indoorLightsOn, on);
    connect(on, &QState::entered, this, &Overnight::turnOnNightLights);
    connect(off, &QState::entered, this, &Overnight::turnOffNightLights);
    connect(m_indoorLightsSM, &QStateMachine::started, this, &Overnight::started);
    
    m_indoorLightsSM->addState(on);
    m_indoorLightsSM->addState(off);
    m_indoorLightsSM->setInitialState(off);
    m_indoorLightsSM->start();
}

void Overnight::started()
{
    if (++m_startCount == 2) {
        qDebug() << "Plugin is ready";
        emit pluginReady();
    }
}

void Overnight::setDaysEventTimes()
{
    QDateTime now = QDateTime::currentDateTime();
    QDateTime tomorrow = now.addDays(1);
    int offset = now.offsetFromUtc();
    double sunrise_fract;
    double sunset_fract;
    
    tomorrow.setTime(QTime(2, 1, 0));
    
    offset = offset / 3600;
    m_sun.setCurrentDate(now.date().year(), now.date().month(), now.date().day());
    m_sun.setPosition(LATITUDE, LONGITUDE, offset);
    m_sun.setTZOffset(offset);
    double sunrise = m_sun.calcSunrise();
    double sunset = m_sun.calcSunset();

    if (sunrise <= 300)
        m_events.insert("beforesunrise", (sunrise - 30));
    else
        m_events.insert("beforesunrise", 300);
    
    m_events.insert("sunrise", (int)sunrise);
    m_events.insert("sunset", (int)sunset);
    m_events.insert("nine", 1260);
    m_events.insert("eleven", 1380);
    
    double sunrise_pct = std::modf(sunrise, &sunrise_fract);
    double sunset_pct = std::modf(sunset, &sunset_fract);
    int sunrise_secs = sunrise_pct * 60.0;
    int sunset_secs = sunset_pct * 60.0;
    qDebug() << "Sunrise" << QTime::fromMSecsSinceStartOfDay(((sunrise_fract * 60) + sunrise_secs) * 1000).toString("h:mm:ss A") << ", Sunset" << QTime::fromMSecsSinceStartOfDay(((sunset_fract * 60) + sunset_secs) * 1000).toString("h:mm:ss A");
    m_setEventTimes->setInterval(now.msecsTo(tomorrow));
    m_setEventTimes->setSingleShot(true);
    m_setEventTimes->start();
}

void Overnight::setNightLightNames(QStringList &names)
{
    m_hueBulbsIndoor.clear();
    for (auto name : names) {
        for (int i = 0; i < m_hue->size(); i++) {
            if (name == QString::fromStdString(m_hue->at(i).getName())) {
                m_hueBulbsIndoor.push_back(i);
                break;
            }
        }
    }
}

void Overnight::setHueBulbNames(QStringList &names)
{
    m_hueBulbsOutdoor.clear();
    for (auto name : names) {
        for (int i = 0; i < m_hue->size(); i++) {
            if (name == QString::fromStdString(m_hue->at(i).getName())) {
                m_hueBulbsOutdoor.push_back(i);
                break;
            }
        }
    }
}

void Overnight::setLifxBulbNames(QStringList &names)
{
    m_lifxBulbs.clear();
    for (auto name: names) {
        LifxBulb *bulb = m_lifx->getBulbByName(name);
        if (bulb) {
            m_lifxBulbs.push_back(bulb);
        }
    }
}

/**
 * TODO: Find a way to save old bulb state so we can return to that when this plugin finishes
 */
void Overnight::runPlugin()
{
    if (!m_running) {
        qDebug() << "Starting the standard overnight program";
        m_running = true;
        m_nextEventTimer->start(1000);
    }
}

void Overnight::stopPlugin()
{
    qDebug() << "Stopping standard overnight program";
    m_outdoorLights->stop();
    m_nextEventTimer->stop();
    m_setEventTimes->stop();
    QThread::msleep(100);
    turnOffLights();
    m_hueBulbsOutdoor.clear();
    m_lifxBulbs.clear();
    m_running = false;
}

void Overnight::timeout()
{
    QTime now = QTime::currentTime();
    QTime midnight(0, 0, 0);
    int mpm = midnight.secsTo(now) / 60;
    static int lastmpm = 10000;
    
    if (mpm != lastmpm) {
        if (mpm >= m_events["eleven"]) {
            emit dimmer();
        }
        if (mpm >= m_events["nine"]) {
            emit indoorLightsOn();
        }
        else if (mpm >= m_events["sunset"]) {
            emit sunset();
        }
        else if (mpm >= m_events["sunrise"]) {
            emit sunrise();
        }
        else if (mpm >= m_events["beforesunrise"]) {
            emit earlymorning();
        }
        lastmpm = mpm;
    }
}

void Overnight::dimLights()
{
    qDebug() << "Dimming lights";
    HSBK hsbk("warm");
    XYColor xyc("warm");
    xyc.brightness(.2);
    hsbk.b((float).2);
    
    for (auto bulb : m_lifxBulbs) {
        m_lifx->changeBulbState(bulb, true);
        m_lifx->changeBulbColor(bulb, hsbk);
    }
    
    for (int i = 0; i < m_hueBulbsOutdoor.size(); i++) {
        m_hue->at(m_hueBulbsOutdoor.at(i)).setColorXY(xyc.getXYB());
    }    
}

void Overnight::turnOffLights()
{
    qDebug() << "Turning lights off";
    for (auto bulb : m_lifxBulbs) {
        m_lifx->changeBulbState(bulb, false);
    }
    
    for (int i = 0; i < m_hueBulbsOutdoor.size(); i++) {
        m_hue->at(m_hueBulbsOutdoor.at(i)).off();
    }
}

void Overnight::turnOnLights()
{
    qDebug() << "Turning lights on";
    if (m_hueBulbsOutdoor.size()) {
        XYColor warm("warm");
        XYColor sunset("sunset");

        for (int i = 0; i < m_hueBulbsOutdoor.size(); i++) {
            if (m_hue->at(m_hueBulbsOutdoor.at(i)).getName() == "Patio North") 
                m_hue->at(m_hueBulbsOutdoor.at(i)).setColorXY(sunset.getXYB());
            else    
                m_hue->at(m_hueBulbsOutdoor.at(i)).setColorXY(warm.getXYB());
        }
    }
    
    if (m_lifxBulbs.size()) {
        HSBK color("warm");
        HSBK patio("sunset");
        for (int i = 0; i < m_lifxBulbs.size(); i++) {
            LifxBulb *bulb = m_lifxBulbs.at(i);
            m_lifx->changeBulbState(bulb, true);
            if (bulb->label().contains("office", Qt::CaseInsensitive))
                m_lifx->changeBulbColor(bulb, patio);
            else
                m_lifx->changeBulbColor(bulb, color);
        }
    }
}

void Overnight::morningLights()
{
    qDebug() << "Warming up lights";

    if (m_hueBulbsOutdoor.size()) {
        XYColor color("warm");
        
        for (int i = 0; i < m_hueBulbsOutdoor.size(); i++) {
            if (m_hue->at(m_hueBulbsOutdoor.at(i)).getName() == "West High" || m_hue->at(m_hueBulbsOutdoor.at(i)).getName() == "East High") {
                color.brightness(.2);
                m_hue->at(m_hueBulbsOutdoor.at(i)).setColorXY(color.getXYB());
            }
            else {
                color.brightness(.5);
                m_hue->at(m_hueBulbsOutdoor.at(i)).setColorXY(color.getXYB());
            }
        }
    }
    
    if (m_lifxBulbs.size()) {
        HSBK color("warm");
        color.b((float).5);
        for (int i = 0; i < m_lifxBulbs.size(); i++) {
            LifxBulb *bulb = m_lifxBulbs.at(i);
            m_lifx->changeBulbState(bulb, true);
            m_lifx->changeBulbColor(bulb, color);
        }
    }
}

void Overnight::turnOffNightLights()
{
    qDebug() << "Turning off nightlights";
    if (m_hueBulbsIndoor.size()) {
        for (int i = 0; i < m_hueBulbsIndoor.size(); i++) {
            m_hue->at(m_hueBulbsIndoor.at(i)).off();
        }
    }
}

void Overnight::turnOnNightLights()
{
    qDebug() << "Turning on nightlights";
    
    if (m_hueBulbsIndoor.size()) {
        XYColor color("nightlight");
        for (int i = 0; i < m_hueBulbsIndoor.size(); i++) {
            m_hue->at(m_hueBulbsIndoor.at(i)).setColorXY(color.getXYB());
        }
    }
}
