#include "spinnakerimaging.h"

SpinnakerImaging::SpinnakerImaging(QObject *parent)
{
    Q_UNUSED(parent)
    connect(this, &SpinnakerImaging::finished, this, &SpinnakerImaging::videoThreadFinished);
}

void SpinnakerImaging::setAcquisition(bool acqu)
{
    m_acquisition = acqu;
}

bool SpinnakerImaging::getAcquisition()
{
    return m_acquisition;
}

bool SpinnakerImaging::getInitialized()
{
    return m_initialized;
}

double SpinnakerImaging::getFrameRateMax()
{
    Spinnaker::GenApi::CFloatPtr ptrAcquisitionFrameRate = m_nodeMap->GetNode("AcquisitionFrameRate");
    if (!IsAvailable(ptrAcquisitionFrameRate) || !IsReadable(ptrAcquisitionFrameRate))
    {
        qDebug()<<m_debug_name+"Unable to retrieve maximum frame rate.";
        return 0;
    }
    return ptrAcquisitionFrameRate->GetMax();
}

double SpinnakerImaging::getCurrentFrameRate()
{
    Spinnaker::GenApi::CFloatPtr ptrAcquisitionFrameRate = m_nodeMap->GetNode("AcquisitionFrameRate");
    if (!IsAvailable(ptrAcquisitionFrameRate) || !IsReadable(ptrAcquisitionFrameRate))
    {
        qDebug()<<m_debug_name+"Unable to retrieve current frame rate.";
        return 0;
    }
    return ptrAcquisitionFrameRate->GetValue();
}

void SpinnakerImaging::setPixelFormat()
{
     m_ptrPixelFormat = m_nodeMap->GetNode("PixelFormat");
    if (IsAvailable(m_ptrPixelFormat) && IsWritable(m_ptrPixelFormat))
    {
        Spinnaker::GenApi::CEnumEntryPtr ptrPixelFormatEntry = m_ptrPixelFormat->GetEntryByName(m_pixFmt.toStdString().c_str());
        if (IsAvailable(ptrPixelFormatEntry) && IsReadable(ptrPixelFormatEntry))
        {
            int64_t pixelFormatMono8 = ptrPixelFormatEntry->GetValue();
            m_ptrPixelFormat->SetIntValue(pixelFormatMono8);
            qDebug()<<m_debug_name+"Pixel format set to"<<m_ptrPixelFormat->GetCurrentEntry()->GetSymbolic();
        }
        else { qDebug()<<m_debug_name+"Pixel format entry not available:"<<m_pixFmt;
            m_pixFmt = "";
        }

    }
    else {
        qDebug()<<m_debug_name+"Pixel format not available"<<m_pixFmt;
        m_pixFmt = "";
    }
}

void SpinnakerImaging::setOffset(int x, int y)
{
    if (IsAvailable(m_ptrOffsetX) && IsWritable(m_ptrOffsetX))
    {
        m_ptrOffsetX->SetValue(x);
        qDebug()<<m_debug_name+"Offset X set to"<<m_ptrOffsetX->GetValue();
    }
    else
    {
        qDebug()<<m_debug_name+"Offset X not available";
    }

    if (IsAvailable(m_ptrOffsetY) && IsWritable(m_ptrOffsetY))
    {
        m_ptrOffsetY->SetValue(y);
        qDebug()<<m_debug_name+"Offset Y set to"<<m_ptrOffsetY->GetValue();
    }
    else
    {
        qDebug()<<m_debug_name+"Offset Y not available";
    }
}

void SpinnakerImaging::setAcquisitionMode(QString mode)
{
    if (!IsAvailable(m_ptrAcquisitionMode) || !IsWritable(m_ptrAcquisitionMode))
    {
        qDebug() << "Unable to set acquisition mode to"<<mode;
        return;
    }
    Spinnaker::GenApi::CEnumEntryPtr ptrAcquisitionModeEntry = m_ptrAcquisitionMode->GetEntryByName(mode.toStdString().c_str());
    if (!IsAvailable(ptrAcquisitionModeEntry) || !IsReadable(ptrAcquisitionModeEntry))
    {
        qDebug() << "Unable to set acquisition mode entry to"<<mode;
        return;
    }
    m_ptrAcquisitionMode->SetIntValue(ptrAcquisitionModeEntry->GetValue());
}

void SpinnakerImaging::setResolution(int width, int height)
{
    if (IsAvailable(m_ptrWidth) && IsWritable(m_ptrWidth))
    {
        m_ptrWidth->SetValue(width);
        qDebug()<<m_debug_name+"Width set to"<<m_ptrWidth->GetValue();
    }
    else qDebug()<<m_debug_name+"Width not available";

    if (IsAvailable(m_ptrHeight) && IsWritable(m_ptrHeight))
    {
        m_ptrHeight->SetValue(height);
        qDebug()<<m_debug_name+"Height set to " << m_ptrHeight->GetValue();
    }
    else qDebug()<<m_debug_name+"Height not available";
}

void SpinnakerImaging::setResolutionMax()
{
    if (IsAvailable(m_ptrWidth) && IsWritable(m_ptrWidth))
    {
        m_ptrWidth->SetValue(m_ptrWidth->GetMax());
        qDebug()<<m_debug_name+"Width set to maximum"<<m_ptrWidth->GetValue();
    }
    else qDebug()<<m_debug_name+"Width not available";

    if (IsAvailable(m_ptrHeight) && IsWritable(m_ptrHeight))
    {
        m_ptrHeight->SetValue(m_ptrHeight->GetMax());
        qDebug()<<m_debug_name+"Height set to maximum"<<m_ptrHeight->GetValue();
    }
    else qDebug()<<m_debug_name+"Height not available";
}

void SpinnakerImaging::setExposureTimeManual(double microsecs)
{
    // Disable automatic exposure
    if (!IsAvailable(m_ptrAutoExposure) || !IsWritable(m_ptrAutoExposure))
    {
        qDebug()<<m_debug_name+"Unable to disable automatic exposure.";
        return;
    }
    Spinnaker::GenApi::CEnumEntryPtr ptrExposureAutoOff = m_ptrAutoExposure->GetEntryByName("Off");
    if (!IsAvailable(ptrExposureAutoOff) || !IsReadable(ptrExposureAutoOff))
    {
        qDebug()<<m_debug_name+"Unable to disable automatic exposure (enum entry).";
        return;
    }
    m_ptrAutoExposure->SetIntValue(ptrExposureAutoOff->GetValue());

    // Set manual exposure time
    if (!IsAvailable(m_ptrExposureTime) || !IsWritable(m_ptrExposureTime))
    {
        qDebug()<<m_debug_name+"Unable to set exposure time.";
        return;
    }
    double exposureTimeMax = m_ptrExposureTime->GetMax();
    if (microsecs > exposureTimeMax)
    {
        microsecs = exposureTimeMax;
    }
    m_ptrExposureTime->SetValue(microsecs);
    qDebug()<<m_debug_name+"Manual exposure time set to"<<m_ptrExposureTime->GetValue()<<"us";
}

void SpinnakerImaging::setExposureTimeAuto()
{
    // Enable automatic exposure
    if (!IsAvailable(m_ptrAutoExposure) || !IsWritable(m_ptrAutoExposure))
    {
        qDebug()<<m_debug_name+"Unable to enable automatic exposure.";
        return;
    }
    Spinnaker::GenApi::CEnumEntryPtr ptrExposureAutoContinuous = m_ptrAutoExposure->GetEntryByName("Continuous");
    if (!IsAvailable(ptrExposureAutoContinuous) || !IsReadable(ptrExposureAutoContinuous))
    {
        qDebug()<<m_debug_name+"Unable to enable automatic exposure (enum entry).";
        return;
    }
    m_ptrAutoExposure->SetIntValue(ptrExposureAutoContinuous->GetValue());
    qDebug()<<m_debug_name+"Auto exposure enabled:"<<m_ptrAutoExposure->GetIntValue();
}

void SpinnakerImaging::setTrigger(const QString trigger)
{
    // http://softwareservices.ptgrey.com/Spinnaker/latest/_trigger_8cpp-example.html
    qDebug()<<"Implementation of Trigger needed. Currently testing with manual exposure times.";
    if (trigger == "SOFTWARE") {

    }
    else if ("HARDWARE") {

    }
    else {

    }
}

void SpinnakerImaging::printDeviceInfo()
{
    qDebug()<<endl<<m_debug_name+"- Device Info -";
    m_nodeMapTLDevice = &m_pCam->GetTLDeviceNodeMap();
    // Get device information
    Spinnaker::GenApi::FeatureList_t features;
    const Spinnaker::GenApi::CCategoryPtr category = m_nodeMapTLDevice->GetNode("DeviceInformation");
    if (IsAvailable(category) && IsReadable(category))
    {
        category->GetFeatures(features);
        for (auto it = features.begin(); it != features.end(); ++it)
        {
            const Spinnaker::GenApi::CNodePtr pfeatureNode = *it;
            Spinnaker::GenApi::CValuePtr pValue = static_cast<Spinnaker::GenApi::CValuePtr>(pfeatureNode);
            qDebug()<<"VideoThread:"<<pfeatureNode->GetName()<<":"<<(IsReadable(pValue) ? pValue->ToString() : "Node not readable");
            //qDebug() << (IsReadable(pValue) ? pValue->ToString() : "Node not readable");
        }
    }
    else qDebug() << m_debug_name+"Device control information not available.";
    Spinnaker::GenApi::CStringPtr ptrStringSerial = m_nodeMapTLDevice->GetNode("DeviceSerialNumber");
    if (IsAvailable(ptrStringSerial) && IsReadable(ptrStringSerial))
    {
        qDebug()<<m_debug_name+"Device serial number:"<<ptrStringSerial->GetValue();
    }
    else qDebug()<<m_debug_name+"Device serial number not available";
    qDebug()<<endl;
}

void SpinnakerImaging::printCamSettings()
{
    qDebug()<<endl<<m_debug_name+"- Camera Settings -";
    qDebug()<<m_debug_name+"Acquisition mode:"<< m_ptrAcquisitionMode->GetCurrentEntry()->GetSymbolic();
    qDebug()<<m_debug_name+"Pixel format:"<< m_ptrPixelFormat->GetCurrentEntry()->GetSymbolic();
    qDebug()<<m_debug_name+"ADC bit depth:"<< m_ptrAdcBitDepth->GetCurrentEntry()->GetSymbolic();
    qDebug()<<m_debug_name+"Auto white balance:"<< m_ptrAutoWhiteBalance->GetCurrentEntry()->GetSymbolic();
    qDebug()<<m_debug_name+"Auto gain:"<< m_ptrAutoGain->GetCurrentEntry()->GetSymbolic();
    qDebug()<<m_debug_name+"Auto exposure:"<< m_ptrAutoExposure->GetCurrentEntry()->GetSymbolic();
    qDebug()<<m_debug_name+"Exposure time:"<< m_ptrExposureTime->GetValue();
    qDebug()<<m_debug_name+"Width:"<< m_ptrWidth->GetValue();
    qDebug()<<m_debug_name+"Height:"<< m_ptrHeight->GetValue();
    qDebug()<<m_debug_name+"Offset X:"<< m_ptrOffsetX->GetValue();
    qDebug()<<m_debug_name+"Offset Y:"<< m_ptrOffsetY->GetValue()<<endl;
}

void SpinnakerImaging::printAvailableFormats()
{
    qDebug()<<endl<<m_debug_name+"Reading available pixel formats";
    // Read available entries
    Spinnaker::GenApi::NodeList_t entries;
    m_ptrPixelFormat->GetEntries(entries);

    for (unsigned int i=0;i<entries.size();i++) {
        Spinnaker::GenApi::CEnumEntryPtr ptrEnumEntry = entries.at(i);
        if (IsAvailable(ptrEnumEntry) && IsReadable(ptrEnumEntry))
            qDebug()<<ptrEnumEntry->GetDisplayName();
    }
    qDebug()<<endl;
}

int SpinnakerImaging::initBlackflyS()
{
    if (!m_initialized)
    {
        qDebug()<<m_debug_name+"Initialize Blackfly camera";
        m_targetLocation = QDir::currentPath();
        qDebug()<<m_debug_name+"Video target location:"<<m_targetLocation;
        // check write permission
        QFileInfo my_dir(m_targetLocation);
        if(!(my_dir.isDir() && my_dir.isWritable()))
        {
            qDebug()<<m_debug_name+"Error: no write permission";
            return -1;
        }
        // Retrieve singleton reference to system object
        m_system = Spinnaker::System::GetInstance();

        // Print out current library version
        const Spinnaker::LibraryVersion spinnakerLibraryVersion = m_system->GetLibraryVersion();
        m_spinnakerVersion = QString::number(spinnakerLibraryVersion.major)+"."+
                QString::number(spinnakerLibraryVersion.minor)+"."+
                QString::number(spinnakerLibraryVersion.type)+"."+
                QString::number(spinnakerLibraryVersion.build);
        qDebug()<<m_debug_name+"Spinnaker library version:"<<m_spinnakerVersion;

        // Retrieve list of cameras from the system
        m_camList = m_system->GetCameras();
        unsigned int camCount = m_camList.GetSize();
        if (camCount < 1)
        {
            qDebug()<<m_debug_name+"No Blackfly cameras found.";
            return -1;
        }
        else if (camCount == 1) {
            qDebug()<<m_debug_name+"Found 1 camera.";
        }
        else qDebug()<<m_debug_name+"Found"<<camCount<<"cameras. Only using one of them.";

        // Creating camera object, using only 1 camera
        m_pCam = m_camList.GetByIndex(0);

        try {
            // Initialize cam
            m_pCam->Init();

            // Read node map and settings. Retrieved private objects will be modified by public functions.
            m_nodeMap = &m_pCam->GetNodeMap();
            m_ptrWidth = m_nodeMap->GetNode("Width");
            m_ptrHeight = m_nodeMap->GetNode("Height");
            m_ptrOffsetX = m_nodeMap->GetNode("OffsetX");
            m_ptrOffsetY = m_nodeMap->GetNode("OffsetY");
            m_ptrExposureTime = m_nodeMap->GetNode("ExposureTime");
            m_ptrPixelFormat = m_nodeMap->GetNode("PixelFormat");
            m_ptrAcquisitionMode = m_nodeMap->GetNode("AcquisitionMode");
            m_ptrAutoExposure = m_nodeMap->GetNode("ExposureAuto");
            m_ptrAutoWhiteBalance = m_nodeMap->GetNode("BalanceWhiteAuto");
            m_ptrAutoGain = m_nodeMap->GetNode("GainAuto");
            m_ptrAdcBitDepth = m_nodeMap->GetNode("AdcBitDepth");

            // Set trigger
            setTrigger(m_trigger); // implementation needed
            // Read pixel formats
            printAvailableFormats();
            // Pixel Format
            // had the issue that pixel format could not be changed anymore
            setPixelFormat(); // BayerRG16 BayerRG12p YUV444Packed BGR8 YCbCr8
            // Offset X + Y
            setOffset(0,0);
            // Resolution
            setResolutionMax(); //setResolution(640,512);
            // Frame rate
            qDebug()<<m_debug_name+"Maximum frame rate:"<<getFrameRateMax();
            m_fps = getCurrentFrameRate();
            qDebug()<<m_debug_name+"Current frame rate:"<<m_fps;
            // Acquisition Mode
            setAcquisitionMode("Continuous");
            // Exposure Time
            if (!m_exposureTimeAuto)
                setExposureTimeManual(m_exposureTime);
            else setExposureTimeAuto();
            // Manufacturer printing functions
            printAllNodeFeatures(m_pCam);
            // Start acquisition. Note that the camera needs some time to adjust its brightness.
            m_pCam->BeginAcquisition();
        }
        catch (Spinnaker::Exception& e)
        {
            qDebug()<<m_debug_name+"initBlackflyS: Error:"<<e.what()<<e.GetError()<<e.GetErrorMessage();
            return -1;
        }
        qDebug()<<"Blackfly Init complete";
        m_initialized = true;
        return 0;
    }
    else {
        qDebug()<<"Camera already initialized.";
        return 0;
    }
}

int SpinnakerImaging::deinitBlackflyS()
{
    if (m_initialized)
    {
        m_pCam->EndAcquisition();
        try {
            // Deinitialize cam
            m_pCam->EndAcquisition();
            m_pCam->DeInit();
            // Release reference to the camera
            qDebug()<<m_debug_name+"Release reference to camera";
            m_pCam = nullptr;
            m_camList.Clear();
            m_system->ReleaseInstance();
        }
        catch (Spinnaker::Exception& e)
        {
            qDebug()<<m_debug_name+"deinitBlackflyS: Error:"<<e.what()<<e.GetError()<<e.GetErrorMessage();
            return -1;
        }
        qDebug()<<"Blackfly Deinit complete";
        return 0;
    }
    else {
        qDebug()<<"Camera already deinitialized";
        return 0;
    }
}

void SpinnakerImaging::run() {
    qDebug()<<m_debug_name+"Acquisition started.";

    while (m_acquisition) {
        try
        {
            Spinnaker::ImagePtr pResultImage = m_pCam->GetNextImage();

            if (pResultImage->IsIncomplete())
            {
                // Retrieve and print the image status description
                qDebug()<<m_debug_name+"Image incomplete:"<<
                          Spinnaker::Image::GetImageStatusDescription(pResultImage->GetImageStatus());
            }
            else
            {
                const size_t width = pResultImage->GetWidth();
                const size_t height = pResultImage->GetHeight();

                m_imageCnt++;

                // convert not used right now
                //Spinnaker::ImagePtr convertedImage = pResultImage->Convert(Spinnaker::PixelFormatEnums::PixelFormat_Mono8,
                //                                                           Spinnaker::ColorProcessingAlgorithm::HQ_LINEAR);

                // Create a unique filename
                QString exposure;
                if (m_exposureTimeAuto)
                    exposure = "auto";
                else
                    exposure = QString::number(m_exposureTime,'g',2);
                QString filename = "Img-"+QString::number(m_imageCnt)+"_fmt-"+m_pixFmt
                        +"_fps-"+QString::number(m_fps,'g',2)+"_exp-"+exposure;

                // Save image
                pResultImage->Save(filename.toStdString().c_str(),Spinnaker::ImageFileFormat::BMP); // RAW PPM BMP JPEG PNG
                qDebug()<<"SAVED:"<<filename;
            }

            // Release image
            pResultImage->Release();
        }
        catch (Spinnaker::Exception& e)
        {
            qDebug()<<m_debug_name+"Error (Acquisition):"<<e.what();
            return;
        }
    }
}




// *** PUBLIC MANUFACTURER FUNCTIONS ***

// this function calls all manufacturer printing functions
void SpinnakerImaging::printAllNodeFeatures(Spinnaker::CameraPtr pCam)
{
    unsigned int level = 0;
    // cam init not needed
    qDebug()<<endl<<m_debug_name+"*** Printing Transport Layer Device NodeMap ***";
    Spinnaker::GenApi::INodeMap &tlDeviceNodeMap = pCam->GetTLDeviceNodeMap();
    PrintCategoryNodeAndAllFeatures(tlDeviceNodeMap.GetNode("Root"), level);
    qDebug()<<m_debug_name+"*** Printed Transport Layer Device NodeMap ***"<<endl;
    // cam init not needed
    qDebug()<<endl<<m_debug_name+"*** Printing Transport Layer Stream NodeMap ***";
    Spinnaker::GenApi::INodeMap &tlStreamNodeMap = pCam->GetTLStreamNodeMap();
    PrintCategoryNodeAndAllFeatures(tlStreamNodeMap.GetNode("Root"), level);
    qDebug()<<m_debug_name+"*** Printed Transport Layer Stream NodeMap ***"<<endl;
    // cam must be initialized
    qDebug()<<endl<<m_debug_name+"*** Printing GeniCam NodeMap ***";
    Spinnaker::GenApi::INodeMap &appLayerNodeMap = pCam->GetNodeMap();
    PrintCategoryNodeAndAllFeatures(appLayerNodeMap.GetNode("Root"), level);
    qDebug()<<m_debug_name+"*** Printed GeniCam NodeMap ***"<<endl;
}

// This function retrieves and prints out the display name of a category node
// before printing all child nodes. Child nodes that are also category nodes are
// printed recursively.
int SpinnakerImaging::PrintCategoryNodeAndAllFeatures(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    int result = 0;
    try
    {
        // Cast as category node
        Spinnaker::GenApi::CCategoryPtr ptrCategoryNode = static_cast<Spinnaker::GenApi::CCategoryPtr>(node);
        // Retrieve display name
        Spinnaker::GenICam::gcstring displayName = ptrCategoryNode->GetDisplayName();
        // Print display name
        Indent(level);
        qDebug()<<displayName;
        //
        // Retrieve children
        //
        // *** NOTES ***
        // The two nodes that typically have children are category nodes and
        // enumeration nodes. Throughout the examples, the children of category
        // nodes are referred to as features while the children of enumeration
        // nodes are referred to as entries. Keep in mind that enumeration
        // nodes can be cast as category nodes, but category nodes cannot be
        // cast as enumerations.
        //
        Spinnaker::GenApi::FeatureList_t features;
        ptrCategoryNode->GetFeatures(features);
        //
        // Iterate through all children
        //
        // *** NOTES ***
        // If dealing with a variety of node types and their values, it may be
        // simpler to cast them as value nodes rather than as their individual
        // types. However, with this increased ease-of-use, functionality is
        // sacrificed.
        //
        Spinnaker::GenApi::FeatureList_t::const_iterator it;
        for (it = features.begin(); it != features.end(); ++it)
        {
            Spinnaker::GenApi::CNodePtr ptrFeatureNode = *it;
            // Ensure node is available and readable
            if (!IsAvailable(ptrFeatureNode) || !IsReadable(ptrFeatureNode))
            {
                continue;
            }
            // Category nodes must be dealt with separately in order to
            // retrieve subnodes recursively.
            if (ptrFeatureNode->GetPrincipalInterfaceType() == Spinnaker::GenApi::intfICategory)
            {
                result = result | PrintCategoryNodeAndAllFeatures(ptrFeatureNode, level + 1);
            }
            // Print the node
            else
            {
                result = result | PrintNode(ptrFeatureNode, level + 1);
            }
        }
    }
    catch (Spinnaker::Exception& e)
    {
        qDebug()<<"Error:"<<e.what();
        result = -1;
    }
    return result;
}

// Based on the read type specified, print the node using the correct print function.
int SpinnakerImaging::PrintNode(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    switch (chosenRead)
    {
    case VALUE:
    {
        return PrintValueNode(node, level);
    }
    case INDIVIDUAL: // Cast all non-category nodes as actual types
    {
        switch (node->GetPrincipalInterfaceType())
        {
        case Spinnaker::GenApi::intfIString:
        {
            return PrintStringNode(node, level);
        }
        case Spinnaker::GenApi::intfIInteger:
        {
            return PrintIntegerNode(node, level);
        }
        case Spinnaker::GenApi::intfIFloat:
        {
            return PrintFloatNode(node, level);
        }
        case Spinnaker::GenApi::intfIBoolean:
        {
            return PrintBooleanNode(node, level);
        }
        case Spinnaker::GenApi::intfICommand:
        {
            return PrintCommandNode(node, level);
        }
        case Spinnaker::GenApi::intfIEnumeration:
        {
            return PrintEnumerationNodeAndCurrentEntry(node, level);
        }
        default:
        {
            qDebug()<<"Unexpected interface type.";
            return -1;
        }
        }
    }
    default:
    {
        qDebug()<<"Unexpected read type.";
        return -1;
    }
    }
}

// This function retrieves and prints the display name and value of all node
// types as value nodes. A value node is a general node type that allows for
// the reading and writing of any node type as a string.
int SpinnakerImaging::PrintValueNode(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    int result = 0;
    try
    {
        // If this node is a selector and is an enumeration node print out its entries and selected features
        if (node->IsSelector() && (node->GetPrincipalInterfaceType() == Spinnaker::GenApi::intfIEnumeration))
        {
            return PrintEnumerationSelector(node, level);
        }

        // Cast as value node
        Spinnaker::GenApi::CValuePtr ptrValueNode = static_cast<Spinnaker::GenApi::CValuePtr>(node);
        //
        // Retrieve display name
        //
        // *** NOTES ***
        // A node's 'display name' is generally more appropriate for output and
        // user interaction whereas its 'name' is what the camera understands.
        // Generally, its name is the same as its display name but without
        // spaces - for instance, the name of the node that houses a camera's
        // serial number is 'DeviceSerialNumber' while its display name is
        // 'Device Serial Number'.
        //
        Spinnaker::GenICam::gcstring displayName = ptrValueNode->GetDisplayName();
        //
        // Retrieve value of any node type as string
        //
        // *** NOTES ***
        // Because value nodes return any node type as a string, it can be much
        // easier to deal with nodes as value nodes rather than their actual
        // individual types.
        //
        Spinnaker::GenICam::gcstring value = ptrValueNode->ToString();
        // Ensure that the value length is not excessive for printing
        if (value.size() > maxChars)
        {
            value = value.substr(0, maxChars) + "...";
        }
        // Print value
        Indent(level);
        qDebug() << displayName << ": " << value;
    }
    catch (Spinnaker::Exception& e)
    {
        qDebug() << "Error: " << e.what();
        result = -1;
    }
    return result;
}

// This function retrieves and prints the display name and value of a string
// node, limiting the number of printed characters to a maximum defined by the
// maxChars global variable. Level parameter determines the indentation level
// for the output.
int SpinnakerImaging::PrintStringNode(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    int result = 0;
    try
    {
        // Cast as string node
        Spinnaker::GenApi::CStringPtr ptrStringNode = static_cast<Spinnaker::GenApi::CStringPtr>(node);
        // Retrieve display name
        Spinnaker::GenICam::gcstring displayName = ptrStringNode->GetDisplayName();
        //
        // Retrieve string node value
        //
        // *** NOTES ***
        // The Spinnaker SDK includes its own wrapped string class, gcstring. As
        // such, there is no need to import the 'string' library when using the
        // SDK. If a standard string object is preferred, simply use a c-style
        // or static cast on the gcstring object.
        //
        Spinnaker::GenICam::gcstring value = ptrStringNode->GetValue();
        // Ensure that the value length is not excessive for printing
        if (value.size() > maxChars)
        {
            value = value.substr(0, maxChars) + "...";
        }
        // Print value; 'level' determines the indentation level of output
        Indent(level);
        qDebug() << displayName << ": " << value;
    }
    catch (Spinnaker::Exception& e)
    {
        qDebug() << "Error: " << e.what();
        result = -1;
    }
    return result;
}

// This function retrieves and prints the display name and value of an integer
// node.
int SpinnakerImaging::PrintIntegerNode(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    int result = 0;
    try
    {
        // Cast node as integer node
        Spinnaker::GenApi::CIntegerPtr ptrIntegerNode = static_cast<Spinnaker::GenApi::CIntegerPtr>(node);
        // Retrieve display name
        Spinnaker::GenICam::gcstring displayName = ptrIntegerNode->GetDisplayName();
        //
        // Retrieve integer node value
        //
        // *** NOTES ***
        // Keep in mind that the data type of an integer node value is an
        // int64_t as opposed to a standard int. While it is true that the two
        // are often interchangeable, it is recommended to use the int64_t
        // to avoid the introduction of bugs.
        //
        // All node types except for base and port nodes include a handy
        // ToString() method which returns a value as a gcstring.
        //
        int64_t value = ptrIntegerNode->GetValue();
        // Print value
        Indent(level);
        qDebug() << displayName << ": " << value;
    }
    catch (Spinnaker::Exception& e)
    {
        qDebug() << "Error: " << e.what();
        result = -1;
    }
    return result;
}

// This function retrieves and prints the display name and value of a float
// node.
int SpinnakerImaging::PrintFloatNode(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    int result = 0;
    try
    {
        // Cast as float node
        Spinnaker::GenApi::CFloatPtr ptrFloatNode = static_cast<Spinnaker::GenApi::CFloatPtr>(node);
        // Retrieve display name
        Spinnaker::GenICam::gcstring displayName = ptrFloatNode->GetDisplayName();
        //
        // Retrieve float node value
        //
        // *** NOTES ***
        // Please take note that floating point numbers in the Spinnaker SDK are
        // almost always represented by the larger data type double rather than
        // float.
        //
        double value = ptrFloatNode->GetValue();
        // Print value
        Indent(level);
        qDebug() << displayName << ": " << value;
    }
    catch (Spinnaker::Exception& e)
    {
        qDebug() << "Error: " << e.what();
        result = -1;
    }
    return result;
}

// This function retrieves and prints the display name and value of a boolean,
// printing "true" for true and "false" for false rather than the corresponding
// integer value ('1' and '0', respectively).
int SpinnakerImaging::PrintBooleanNode(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    int result = 0;
    try
    {
        // Cast as boolean node
        Spinnaker::GenApi::CBooleanPtr ptrBooleanNode = static_cast<Spinnaker::GenApi::CBooleanPtr>(node);
        // Retrieve display name
        Spinnaker::GenICam::gcstring displayName = ptrBooleanNode->GetDisplayName();
        //
        // Retrieve value as a string representation
        //
        // *** NOTES ***
        // Boolean node type values are represented by the standard bool data
        // type. The boolean ToString() method returns either a '1' or '0' as
        // a string rather than a more descriptive word like 'true' or 'false'.
        //
        Spinnaker::GenICam::gcstring value = (ptrBooleanNode->GetValue() ? "true" : "false");
        // Print value
        Indent(level);
        qDebug() << displayName << ": " << value;
    }
    catch (Spinnaker::Exception& e)
    {
        qDebug() << "Error: " << e.what();
        result = -1;
    }
    return result;
}

// This function retrieves and prints the display name and tooltip of a command
// node, limiting the number of printed characters to a defined maximum.
// The tooltip is printed below because command nodes do not have an intelligible
// value.
int SpinnakerImaging::PrintCommandNode(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    try
    {
        // Cast as command node
        Spinnaker::GenApi::CCommandPtr ptrCommandNode = static_cast<Spinnaker::GenApi::CCommandPtr>(node);
        // Retrieve display name
        Spinnaker::GenICam::gcstring displayName = ptrCommandNode->GetDisplayName();
        //
        // Retrieve tooltip
        //
        // *** NOTES ***
        // All node types have a tooltip available. Tooltips provide useful
        // information about nodes. Command nodes do not have a method to
        // retrieve values as their is no intelligible value to retrieve.
        //
        Spinnaker::GenICam::gcstring tooltip = ptrCommandNode->GetToolTip();
        // Ensure that the value length is not excessive for printing
        if (tooltip.size() > maxChars)
        {
            tooltip = tooltip.substr(0, maxChars) + "...";
        }
        // Print tooltip
        Indent(level);
        qDebug() << displayName << ": " << tooltip;
        return 0;
    }
    catch (Spinnaker::Exception& e)
    {
        qDebug() << "Error: " << e.what();
        return -1;
    }
}

// This function retrieves and prints the display names of an enumeration node
// and its current entry (which is actually housed in another node unto itself).
int SpinnakerImaging::PrintEnumerationNodeAndCurrentEntry(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    try
    {
        // If this enumeration node is a selector, cycle through its entries and selected features
        if (node->IsSelector())
        {
            return PrintEnumerationSelector(node, level);
        }
        // Cast as enumeration node
        Spinnaker::GenApi::CEnumerationPtr ptrEnumerationNode = static_cast<Spinnaker::GenApi::CEnumerationPtr>(node);
        //
        // Retrieve current entry as enumeration node
        //
        // *** NOTES ***
        // Enumeration nodes have three methods to differentiate between: first,
        // GetIntValue() returns the integer value of the current entry node;
        // second, GetCurrentEntry() returns the entry node itself; and third,
        // ToString() returns the symbolic of the current entry.
        //
        Spinnaker::GenApi::CEnumEntryPtr ptrEnumEntryNode = ptrEnumerationNode->GetCurrentEntry();
        // Retrieve display name
        Spinnaker::GenICam::gcstring displayName = ptrEnumerationNode->GetDisplayName();
        //
        // Retrieve current symbolic
        //
        // *** NOTES ***
        // Rather than retrieving the current entry node and then retrieving its
        // symbolic, this could have been taken care of in one step by using the
        // enumeration node's ToString() method.
        //
        Spinnaker::GenICam::gcstring currentEntrySymbolic = ptrEnumEntryNode->GetSymbolic();
        // Print current entry symbolic
        Indent(level);
        qDebug() << displayName << ": " << currentEntrySymbolic;
        return 0;
    }
    catch (Spinnaker::Exception& e)
    {
        qDebug() << "Error: " << e.what();
        return -1;
    }
}

// This function retrieves and prints the display names of enumeration selector nodes.
// The selector will cycle through every selector entry and print out all the selected
// features for that selector entry. It is possible for integer nodes to be selector
// nodes as well, but this function will only cycle through Enumeration nodes.
int SpinnakerImaging::PrintEnumerationSelector(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    int result = 0;
    try
    {
        Spinnaker::GenApi::FeatureList_t selectedFeatures;
        node->GetSelectedFeatures(selectedFeatures);
        // Cast as an enumeration node
        Spinnaker::GenApi::CEnumerationPtr ptrSelectorNode = static_cast<Spinnaker::GenApi::CEnumerationPtr>(node);
        Spinnaker::GenApi::StringList_t entries;
        ptrSelectorNode->GetSymbolics(entries);
        // Note current selector node entry
        Spinnaker::GenApi::CEnumEntryPtr ptrCurrentEntry = ptrSelectorNode->GetCurrentEntry();
        // Retrieve display name
        Spinnaker::GenICam::gcstring displayName = ptrSelectorNode->GetDisplayName();
        // Retrieve current symbolic
        Spinnaker::GenICam::gcstring currentEntrySymbolic = ptrSelectorNode->ToString();
        // Print current entry symbolic
        Indent(level);
        qDebug() << displayName << ": " << currentEntrySymbolic;
        // For every selector node entry
        for (size_t i = 0; i < entries.size(); i++)
        {
            Spinnaker::GenApi::CEnumEntryPtr selectorEntry = ptrSelectorNode->GetEntryByName(entries[i]);
            Spinnaker::GenApi::FeatureList_t::const_iterator it;
            // Go through each enum entry of the selector node
            if (IsWritable(ptrSelectorNode))
            {
                if (IsAvailable(selectorEntry) && IsReadable(selectorEntry))
                {
                    ptrSelectorNode->SetIntValue(selectorEntry->GetValue());
                    Indent(level + 1);
                    qDebug() << displayName << ": " << ptrSelectorNode->ToString();
                }
            }
            // Look at every node that is affected by the selector node
            for (it = selectedFeatures.begin(); it != selectedFeatures.end(); ++it)
            {
                Spinnaker::GenApi::CNodePtr ptrFeatureNode = *it;
                if (!IsAvailable(ptrFeatureNode) || !IsReadable(ptrFeatureNode))
                {
                    continue;
                }
                // Print the selected feature
                else
                {
                    result = result | PrintNode(ptrFeatureNode, level + 2);
                }
            }
        }
        // Restore the selector to its original value
        if (IsWritable(ptrSelectorNode))
        {
            ptrSelectorNode->SetIntValue(ptrCurrentEntry->GetValue());
        }
    }
    catch (Spinnaker::Exception& e)
    {
        qDebug() << "Error: " << e.what();
        result = -1;
    }
    return result;
}

// This helper function deals with output indentation, of which there is a lot.
void SpinnakerImaging::Indent(unsigned int level)
{
    for (unsigned int i = 0; i < level; i++)
    {
        //qDebug() << "   ";
    }
}


