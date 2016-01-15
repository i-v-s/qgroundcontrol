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
    : QGCTool(app), _rosState(ROS_DISC), _multiVehicleManager(NULL)
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


    //_multiVehicleManager->activeVehicle()->sendMessage(msg);
}

void ROSManager::_mavlinkMessageReceived(LinkInterface* link, mavlink_message_t message)
{


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

   //connect(_toolbox->linkManager(), &LinkManager::linkActive, this, &MultiVehicleManager::_linkActive);
}
