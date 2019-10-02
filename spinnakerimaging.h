#ifndef SPINNAKERIMAGING_H
#define SPINNAKERIMAGING_H

#include "imagebuffer.h"

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QFileInfo>
#include <QDir>

// we dont use fucking manufacturer specific namespaces
//using namespace Spinnaker;
//using namespace Spinnaker::GenApi;
//using namespace Spinnaker::GenICam;


class SpinnakerImaging : public QThread
{
    Q_OBJECT
private:
    QString m_pixFmt = "Mono8";// Mono8 Mono16 BayerRG16 YUV444Packed BGR8 YCbCr8
    double m_exposureTime = 3000;
    bool m_exposureTimeAuto = true;
    const QString m_trigger = "SOFTWARE"; // "NONE" "HARDWARE"
    double m_fps;
    bool m_print = false;

    int64_t m_imgWidth;
    int64_t m_imgHeight;

    QString m_targetLocation;
    QString m_spinnakerVersion;
    bool m_acquisition = false;
    bool m_initialized = false;
    uint64_t m_imageCnt = 0;
    QString m_debug_name = "SpinnakerImaging: ";

    ImageBuffer *m_refSpinnakerToBuffer;


    // Spinnaker stuff
    Spinnaker::SystemPtr m_system;
    Spinnaker::CameraList m_camList;
    Spinnaker::CameraPtr m_pCam;
    Spinnaker::GenApi::INodeMap *m_nodeMapTLDevice;
    Spinnaker::GenApi::INodeMap *m_nodeMap;

    Spinnaker::GenApi::CIntegerPtr m_ptrOffsetX;
    Spinnaker::GenApi::CIntegerPtr m_ptrOffsetY;
    Spinnaker::GenApi::CIntegerPtr m_ptrWidth;
    Spinnaker::GenApi::CIntegerPtr m_ptrHeight;
    Spinnaker::GenApi::CFloatPtr   m_ptrExposureTime;

    Spinnaker::GenApi::CEnumerationPtr m_ptrPixelFormat;
    Spinnaker::GenApi::CEnumerationPtr m_ptrAcquisitionMode;
    Spinnaker::GenApi::CEnumerationPtr m_ptrAutoExposure;
    Spinnaker::GenApi::CEnumerationPtr m_ptrAutoWhiteBalance;
    Spinnaker::GenApi::CEnumerationPtr m_ptrAutoGain;
    Spinnaker::GenApi::CEnumerationPtr m_ptrAdcBitDepth;


public:
    SpinnakerImaging(QObject *parent);

    bool getAcquisition();
    bool getInitialized();
    double getFrameRateMax();
    double getCurrentFrameRate();

    void setAcquisition(bool acqu);
    void setPixelFormat();
    void setOffset(int x, int y);
    void setAcquisitionMode(QString mode);
    void setResolution(int width, int height);
    void setResolutionMax();
    void setExposureTimeManual(double microsecs);
    void setExposureTimeAuto();
    void setTrigger(const QString trigger);

    void printDeviceInfo();
    void printCamSettings();
    void printAvailableFormats();
    void printAllNodeFeatures(Spinnaker::CameraPtr pCam);

    int initBlackflyS();
    int deinitBlackflyS();

    void run() override;

    // *** PUBLIC MANUFACTURER FUNCTIONS ***
    enum readType
    {
        VALUE,
        INDIVIDUAL
    };
    const readType chosenRead = VALUE;
    const unsigned int maxChars = 35;
    int PrintCategoryNodeAndAllFeatures(Spinnaker::GenApi::CNodePtr node, unsigned int level);
    int PrintNode(Spinnaker::GenApi::CNodePtr node, unsigned int level);
    int PrintValueNode(Spinnaker::GenApi::CNodePtr node, unsigned int level);
    int PrintStringNode(Spinnaker::GenApi::CNodePtr node, unsigned int level);
    int PrintIntegerNode(Spinnaker::GenApi::CNodePtr node, unsigned int level);
    int PrintFloatNode(Spinnaker::GenApi::CNodePtr node, unsigned int level);
    int PrintBooleanNode(Spinnaker::GenApi::CNodePtr node, unsigned int level);
    int PrintCommandNode(Spinnaker::GenApi::CNodePtr node, unsigned int level);
    int PrintEnumerationNodeAndCurrentEntry(Spinnaker::GenApi::CNodePtr node, unsigned int level);
    int PrintEnumerationSelector(Spinnaker::GenApi::CNodePtr node, unsigned int level);
    void Indent(unsigned int level);

signals:
    void acquisitionStarted();


public slots:

    void imagerFinished()
    {
        qDebug()<<m_debug_name<<"finished.";
        if (m_initialized) deinitBlackflyS();
        //this->deleteLater();
    }

    // m_debug_name+"
    // m_debug_name+"

};

#endif // SPINNAKERIMAGING_H
