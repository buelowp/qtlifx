#pragma once

#include <QtCore/QtCore>

class OnState : public QState
{
    Q_OBJECT
    
public:
    OnState(QState *parent = Q_NULLPTR);
    
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
};
