#pragma once

#include <QtCore/QtCore>

class OffState : public QState
{
    Q_OBJECT
    
public:
    OffState(QState *parent = Q_NULLPTR);
    
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
};
