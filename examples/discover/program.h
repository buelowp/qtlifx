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

#ifndef PROGRAM_H
#define PROGRAM_H

#include <vector>

#include <lifxbulb.h>
#include <lifxmanager.h>

#include <hueplusplus/LinHttpHandler.h>
#include <hueplusplus/Bridge.h>

class Program : public QObject {
    Q_OBJECT
public:
    Program(LifxManager *lifx, std::vector<hueplusplus::Light> *hue, QObject *parent = nullptr);
    ~Program();

    virtual void runPlugin() = 0;
    virtual void stopPlugin() = 0;
    virtual void setHueBulbNames(QStringList &names) = 0;
    virtual void setLifxBulbNames(QStringList &names) = 0;
    virtual bool running() { return m_running; }
    
    bool m_running;
    std::vector<hueplusplus::Light> *m_hue;
    LifxManager *m_lifx;
};

#endif // PROGRAM_H
