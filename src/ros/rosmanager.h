#ifndef ROSMANAGER_H
#define ROSMANAGER_H

#include "QGCToolbox.h"

enum ROSState
{
    ROS_DISC,
    ROS_STOP,
    ROS_RUN
};


class ROSManager : public QGCTool
{
    Q_OBJECT
    ROSState _rosState;
public:
    virtual void setToolbox(QGCToolbox *toolbox);
    ROSManager(QGCApplication* app);
    Q_PROPERTY(ROSState rosState          READ rosState   NOTIFY rosStateChanged)
    Q_INVOKABLE void launch(const QString &name);
    ROSState rosState(void) { return _rosState; }
signals:
    void rosStateChanged(ROSState rosState);
};



#endif // ROSMANAGER_H
