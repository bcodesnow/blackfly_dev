#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include "imagebuffer.h"

#include <QObject>
#include <QThread>

class ImageProcessor : public QThread
{
    Q_OBJECT
private:
    VideoWriter m_vidWriter;
    Size m_frame_size;
    QString m_debug_name = "ImageProcessor: ";
    ImageBuffer *m_refProcessorToBuffer;
    double m_writeFps = 0;
    bool m_writing;
    uint16_t m_readDelay = 10000; // us
    uint64_t m_imageWriteCnt = 0;
public:
    ImageProcessor(QObject *bufferref)
    {
        qDebug()<<m_debug_name+"created.";

        m_refProcessorToBuffer = static_cast<ImageBuffer*>(bufferref);
        connect(this, &ImageProcessor::finished, this, &ImageProcessor::writerFinished);
        connect(m_refProcessorToBuffer, &ImageBuffer::imageWritten, this, &ImageProcessor::bufferHasData);


    }

    void run() override {
        static bool test;
        if (test) return;
        m_frame_size = m_refProcessorToBuffer->getStdResolution();
        m_writeFps = m_refProcessorToBuffer->getStdFps();
        m_readDelay = static_cast<uint16_t>( (1/m_writeFps) * 1e6 ); // approximated delay for reading
        qDebug()<<m_debug_name+"running:"<<m_writing<<"Frame size:"<<m_frame_size.width
               <<"x"<<m_frame_size.height<<"FPS:"<<m_writeFps<<"Read delay:"<<m_readDelay;
        if (!m_frame_size.empty() && m_writeFps > 0 && m_refProcessorToBuffer->getBufferSize() > 0)
        {
            m_vidWriter = VideoWriter("output.avi", VideoWriter::fourcc('M','J','P','G'), m_writeFps, m_frame_size);
            while (m_writing)
            {
                Mat frame = m_refProcessorToBuffer->getImgFromBuffer();
                if (frame.empty())
                {
                    qDebug()<<"Read frame is empty.";
                    QThread::usleep(m_readDelay);
                }
                else {
                    qDebug()<<"Write frame to video:"<<m_vidWriter.isOpened()<<frame.total();
                    m_vidWriter.write(frame);
                    m_imageWriteCnt++;
                    if (m_imageWriteCnt >= 200 )
                    {
                        test = true;
                        qDebug()<<"TEST: END AND SAVE VIDEO"<<m_imageWriteCnt;
                        m_writing = false;
                    }

                }
            }
            qDebug()<<"Releasing VideoWriter...";
            m_vidWriter.release();
        }
        else {
            qDebug()<<m_debug_name+"started but nothing to read. Stopping.";
            qDebug()<<"Frame size:"<<m_frame_size.width<<m_frame_size.height<<"Write fps:"<<m_writeFps;
            m_writing = false;
            return;
        }
        return;
    }

signals:
    void newDataInBuffer();

public slots:
    void bufferHasData()
    {
        if (!m_writing && (m_refProcessorToBuffer->getBufferSize() > HOWLONGISTHESNAKE))
        {
            qDebug()<<m_debug_name<<"starting thread.";
            m_writing = true;
            this->start();
        }
    }

    void writerFinished()
    {
        qDebug()<<m_debug_name+"finished.";
    }
};

#endif // IMAGEPROCESSOR_H
