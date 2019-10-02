#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

#include <opencv2/core/mat.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio.hpp"

#include <sstream> // remove later

#include <QObject>
#include <QContiguousCache>
#include <QDebug>

#define RECORD_TIME_MS 3000
#define HOWLONGISTHESNAKE 5

using namespace cv;

class ImageBuffer : public QObject
{
    Q_OBJECT
private:
    mutable QContiguousCache<Mat> m_imgbuffer;
    const QString m_debug_name = "ImageBuffer: ";
    Mat *m_readPtr; // tail
    Mat *m_writePtr; // head
    Size m_stdMatSize;

    int m_writeIdx;
    int m_readIdx;

    double m_stdFps;
    int m_capacity;

public:
    explicit ImageBuffer(QObject *parent = nullptr)
    {
        Q_UNUSED(parent)

    }

    int getBufferSize()
    {
        return m_imgbuffer.size();
    }

    void setStdResolution(int width, int height)
    {
        m_stdMatSize = Size(width,height);
    }

    Size getStdResolution()
    {
        return m_stdMatSize;
    }

    void setBufferCapByFps(double fps)
    {
        int size = static_cast<int>((fps/1000)*RECORD_TIME_MS);
        m_stdFps = fps;

        m_imgbuffer.clear();
        m_imgbuffer.setCapacity(size);
        m_capacity = m_imgbuffer.capacity();

        m_writeIdx = 0;
        m_readIdx = 0;
        // set ref to element for both pointers
        m_writePtr = &m_imgbuffer[m_writeIdx];
        m_readPtr = &m_imgbuffer[m_readIdx];

        qDebug()<<m_debug_name+"buffer capacity set to:"<<m_capacity;
    }

    int getBufferCap()
    {
        return m_imgbuffer.size();
    }

    double getStdFps()
    {
        return m_stdFps;
    }

    void writeImgToBuffer(Mat img)
    {
        m_writePtr = &m_imgbuffer[m_writeIdx];
        *m_writePtr = img;
        qDebug()<<"Image written to index:"<<m_writeIdx<<m_writePtr->total()<<"buffer size:"<<m_imgbuffer.size();
        emit imageWritten();
        if (m_writeIdx < m_capacity-1)
            m_writeIdx++;
        else
            m_writeIdx = 0;
    }

    //    void writeImgToBuffer(Mat img)
    //    {
    //        *m_writePtr = img;
    //        qDebug()<<"Image written to index:"<<m_writeIdx<<m_writePtr->total()<<"buffer size:"<<m_imgbuffer.size();
    //        if (m_writeIdx < m_imgbuffer.capacity()-1)
    //            m_writeIdx++;
    //        else
    //            m_writeIdx = 0;
    //        m_writePtr = &m_imgbuffer[m_writeIdx];
    //    }

    Mat getImgFromBuffer()
    {
        Mat tmp;
        if ( m_readIdx > (m_writeIdx - HOWLONGISTHESNAKE) )
//                ||
//             ( (m_readIdx > m_capacity - HOWLONGISTHESNAKE) &&
//               (m_writeIdx < HOWLONGISTHESNAKE) &&
//               (m_readIdx - m_writeIdx > m_capacity - HOWLONGISTHESNAKE ) ) )
        {
            qDebug()<<"Snake is too young for mutilation.";
            return tmp;
        }
        qDebug()<<"Grab that little fuck by the tail!";

        m_readPtr = &m_imgbuffer[m_readIdx];
        tmp = *m_readPtr;
        qDebug()<<"Image read from index:"<<m_readIdx<<tmp.total();
        emit imageRead();

        if (m_readIdx < m_capacity-1)
            m_readIdx++;
        else
            m_readIdx = 0;

        return tmp;
    }

    //    Mat getImgFromBuffer()
    //    {
    //        qDebug()<<"getImgFromBuffer";
    //        Mat tmp = Mat::zeros(m_stdMatSize, CV_8UC3);

    //        if (m_imgbuffer.size() > 0)
    //        {
    //            if (m_imgbuffer.size() == m_imgbuffer.capacity())
    //            {
    //                // buffer was filled at least once
    //                if ((m_writeIdx - m_lookAhead) > 0)
    //                {
    //                    qDebug()<<"wp index from lookAhead to capacity";
    //                    tmp = *m_readPtr;
    //                    m_readIdx++;
    //                    m_readPtr = &m_imgbuffer[m_readIdx];
    //                    // wp index from lookAhead to capacity
    //                    // rp index from 0 to (capacity-lookAhead)
    //                }
    //                else if (m_writeIdx < m_lookAhead) {
    //                    qDebug()<<"wp index from 0 to (lookAhead-1)";
    //                    tmp = *m_readPtr;
    //                    m_readIdx++;
    //                    m_readPtr = &m_imgbuffer[m_readIdx];
    //                    // wp index from 0 to (lookAhead-1)
    //                    // rp index from (capacity-lookAhead+1) to (capacity-1)
    //                }
    //            }
    //            else {
    //                // there are less elements than capacity
    //                if ((m_writeIdx - m_lookAhead) > 0)
    //                {
    //                    // start reading when there is enough space to the writer
    //                    tmp = *m_readPtr;
    //                    qDebug()<<"Image read from index:"<<m_readIdx;
    //                    m_readIdx++;
    //                    m_readPtr = &m_imgbuffer[m_readIdx];
    //                }
    //                else
    //                {
    //                    qDebug()<<m_debug_name+"Imagebuffer has not enough elements to read.";
    //                }
    //            }
    //        }
    //        else {
    //            qDebug()<<m_debug_name+"Nothing to read from imagebuffer";
    //        }
    //        return tmp;
    //    }




signals:
    void imageWritten();
    void imageRead();

public slots:


};

#endif // IMAGEBUFFER_H
