#ifndef AQUISITIONBFS_H
#define AQUISITIONBFS_H

#include <QObject>
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include <iostream>
#include <sstream>

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;

// This class implements the Spinnaker API
class AquisitionBFS : public QObject
{
    Q_OBJECT
public:
    explicit AquisitionBFS(QObject *parent = nullptr);

    int RunSingleCamera(CameraPtr pCam);
    int PrintDeviceInfo(INodeMap& nodeMap);
    int AcquireImages(CameraPtr pCam, INodeMap& nodeMap, INodeMap& nodeMapTLDevice);
    int DisableHeartbeat(INodeMap& nodeMap, INodeMap& nodeMapTLDevice);


signals:

public slots:
};

#endif // AQUISITIONBFS_H
