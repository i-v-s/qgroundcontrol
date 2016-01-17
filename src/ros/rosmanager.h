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

enum SVOStage {
  STAGE_PAUSED,
  STAGE_FIRST_FRAME,
  STAGE_SECOND_FRAME,
  STAGE_DEFAULT_FRAME,
  STAGE_RELOCALIZING
};

class ROSManager;

class ROSHost : public QObject
{
    Q_OBJECT
};

class SVOInfo : public QObject
{
    Q_OBJECT
    mavlink_rpg_svo_info_t _info;
    qint64 _receivedTime;
public:
    SVOInfo(QGCApplication* app);
    void onMessage(const mavlink_rpg_svo_info_t & info);
    Q_PROPERTY(float processingTime READ  processingTime NOTIFY  processingTimeChanged)
    Q_PROPERTY(int       numMatches READ      numMatches NOTIFY      numMatchesChanged)
    Q_PROPERTY(int  trackingQuality READ trackingQuality NOTIFY trackingQualityChanged)
    Q_PROPERTY(int            stage READ           stage NOTIFY           stageChanged)
    Q_PROPERTY(qint64  receivedTime READ    receivedTime NOTIFY    receivedTimeChanged)
    float processingTime() const {return _info.processing_time;}
    int       numMatches() const {return _info.num_matches;}
    int  trackingQuality() const {return _info.tracking_quality;}
    int            stage() const {return _info.stage;}
    qint64  receivedTime() const {return _receivedTime;}
signals:
    void  processingTimeChanged(float processingTime);
    void      numMatchesChanged(int numMatches);
    void trackingQualityChanged(int trackingQuality);
    void           stageChanged(int stage);
    void    receivedTimeChanged(qint64 receivedTime);
};

class ROSManager : public QGCTool
{
    Q_OBJECT
    ROSState _rosState;
    SVOInfo _svoInfo;
    MultiVehicleManager * _multiVehicleManager;
    MAVLinkProtocol * _mavlink;
    LinkManager * _linkManager;
    void _mavlinkMessageReceived(LinkInterface* link, mavlink_message_t message);
public:
    virtual void setToolbox(QGCToolbox *toolbox);
    ROSManager(QGCApplication* app);
    virtual ~ROSManager(){}
    Q_PROPERTY(ROSState rosState          READ rosState   NOTIFY rosStateChanged)
    Q_PROPERTY(SVOInfo * svoInfo READ svoInfo CONSTANT)
    Q_INVOKABLE void launch(const QString &name);
    Q_INVOKABLE void svoKey(const QString &key);
    ROSState rosState(void) { return _rosState; }
    SVOInfo * svoInfo(void) { return &_svoInfo; }
signals:
    void rosStateChanged(ROSState rosState);
};



#endif // ROSMANAGER_H
