#pragma once

#include <QtCore/QtCore>

class MorningState : public QState
{
    Q_OBJECT
    
public:
    MorningState(QState *parent = Q_NULLPTR);
    
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
};
