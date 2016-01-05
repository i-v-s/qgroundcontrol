#include "rosmanager.h"
#include "AutoPilotPlugin.h"
#include "MAVLinkProtocol.h"
#include "UAS.h"
#include "QGCApplication.h"

//QGC_LOGGING_CATEGORY(MultiVehicleManagerLog, "MultiVehicleManagerLog")

ROSManager::ROSManager(QGCApplication* app)
    : QGCTool(app), _rosState(ROS_DISC)
{

}

void ROSManager::launch(const QString &name)
{

}

void ROSManager::setToolbox(QGCToolbox *toolbox)
{
   QGCTool::setToolbox(toolbox);

   QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
   qmlRegisterUncreatableType<ROSManager>("QGroundControl.ROSManager", 1, 0, "ROSManager", "Reference only");

   //connect(_toolbox->linkManager(), &LinkManager::linkActive, this, &MultiVehicleManager::_linkActive);
}
