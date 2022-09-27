#pragma once

#include <QtCore/QtCore>

class DimState : public QState
{
    Q_OBJECT
    
public:
    DimState(QState *parent = Q_NULLPTR);
    
protected:
    void onEntry(QEvent*) override;
    void onExit(QEvent*) override;
};
