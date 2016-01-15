#ifndef ROSMANAGER_H
#define ROSMANAGER_H
#include <mavlink.h>
#include "QGCToolbox.h"
#include "LinkInterface.h"

enum ROSState
{
    ROS_DISC,
    ROS_STOP,
    ROS_RUN
};



class ROSHost : public QObject
{
    Q_OBJECT
};

class ROSManager : public QGCTool
{
    Q_OBJECT
    ROSState _rosState;
    MultiVehicleManager * _multiVehicleManager;
    MAVLinkProtocol * _mavlink;
    LinkManager * _linkManager;
    void _mavlinkMessageReceived(LinkInterface* link, mavlink_message_t message);
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
