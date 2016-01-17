#include "rosmanager.h"
#include "AutoPilotPlugin.h"
#include "MAVLinkProtocol.h"
#include "UAS.h"
#include "QGCApplication.h"
#include <string.h>
//#include "mavlink/ros/mavlink.h"
//#include "mavlink/ros/mavlink_msg_rpg_svo_key.h"

//QGC_LOGGING_CATEGORY(MultiVehicleManagerLog, "MultiVehicleManagerLog")

ROSManager::ROSManager(QGCApplication* app)
    : QGCTool(app), _rosState(ROS_DISC), _svoInfo(app), _multiVehicleManager(NULL)
{
}

void _sendMessage(LinkInterface* link, mavlink_message_t message, MAVLinkProtocol * mavlink)
{
    // Create buffer
    static uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    // Rewriting header to ensure correct link ID is set
    static uint8_t messageKeys[256] = MAVLINK_MESSAGE_CRCS;
    mavlink_finalize_message_chan(&message, mavlink->getSystemId(), mavlink->getComponentId(), link->getMavlinkChannel(), message.len, messageKeys[message.msgid]);
    // Write message into buffer, prepending start sign
    int len = mavlink_msg_to_send_buffer(buffer, &message);
    // If link is connected
    if (link->isConnected())
    {
        // Send the portion of the buffer now occupied by the message
        link->writeBytes((const char*)buffer, len);
    }
}

void ROSManager::launch(const QString &name)
{
    //uint16_t mavlink_msg_rpg_svo_key_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
    mavlink_message_t msg;
    char buf[sizeof(mavlink_ros_cmd_launch_t::launch_file)] = {0};
    strncpy(buf, name.toStdString().c_str(), sizeof(buf) - 1);

    QmlObjectListModel* links = _linkManager->links();

    for (int i = links->count() - 1; i >= 0; i--)
    {
        LinkInterface * link = links->value<LinkInterface*>(i);
        if(link->isConnected())
        {
            mavlink_msg_ros_cmd_launch_pack_chan(_mavlink->getSystemId(), _mavlink->getComponentId(), link->getMavlinkChannel(), &msg, buf);
            _sendMessage(link, msg, _mavlink);
        }
    }
}

void ROSManager::svoKey(const QString &key)
{
    //uint16_t mavlink_msg_rpg_svo_key_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
    mavlink_message_t msg;

    QmlObjectListModel* links = _linkManager->links();

    for (int i = links->count() - 1; i >= 0; i--)
    {
        LinkInterface * link = links->value<LinkInterface*>(i);
        if(link->isConnected())
        {
            mavlink_msg_rpg_svo_key_pack_chan(_mavlink->getSystemId(), _mavlink->getComponentId(), link->getMavlinkChannel(), &msg, key.at(0).toLatin1());
            _sendMessage(link, msg, _mavlink);
        }
    }
}

void ROSManager::_mavlinkMessageReceived(LinkInterface* link, mavlink_message_t message)
{
    switch(message.msgid)
    {
    case MAVLINK_MSG_ID_ROS_INFO:
        break;
    case MAVLINK_MSG_ID_RPG_SVO_INFO:
        {
            mavlink_rpg_svo_info_t info;
            mavlink_msg_rpg_svo_info_decode(&message, &info);
            _svoInfo.onMessage(info);
        }
        break;
    }
}

void ROSManager::setToolbox(QGCToolbox *toolbox)
{
    QGCTool::setToolbox(toolbox);
    _multiVehicleManager = toolbox->multiVehicleManager();
    _mavlink = toolbox->mavlinkProtocol();
    _linkManager = toolbox->linkManager();
    connect(_mavlink, &MAVLinkProtocol::messageReceived, this, &ROSManager::_mavlinkMessageReceived);

   QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
   qmlRegisterUncreatableType<ROSManager>("QGroundControl.ROSManager", 1, 0, "ROSManager", "Reference only");
   qmlRegisterUncreatableType<SVOInfo>("QGroundControl.ROSManager.SVOInfo", 1, 0, "SVOInfo", "Reference only");

   //connect(_toolbox->linkManager(), &LinkManager::linkActive, this, &MultiVehicleManager::_linkActive);
}

SVOInfo::SVOInfo(QGCApplication *app): QObject(app)
{
    memset(&_info, 0, sizeof(_info));
}

void SVOInfo::onMessage(const mavlink_rpg_svo_info_t &info)
{
    _receivedTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    emit receivedTimeChanged(_receivedTime);
    if(_info.num_matches != info.num_matches)
        emit numMatchesChanged(info.num_matches);
    if(_info.processing_time != info.processing_time)
        emit processingTimeChanged(info.processing_time);
    if(_info.stage != info.stage)
        emit stageChanged(info.stage);
    if(_info.tracking_quality != info.tracking_quality)
        emit trackingQualityChanged(info.tracking_quality);
    _info = info;
}
