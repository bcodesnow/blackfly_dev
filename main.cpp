#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "aquisitionbfs.h" // spinnaker api class
#include "spinnakerimaging.h"



int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    SpinnakerImaging blackfly(nullptr);
    if (blackfly.initBlackflyS() == 0) // GUI blocking!!!
    {
        blackfly.setAcquisition(true);
        // fire!
        blackfly.start();
        // wait!
        QThread::msleep(1000);
        // surrender!
        blackfly.setAcquisition(false);
        blackfly.wait();

        // try again
        //        qDebug()<<"try again!";
        //        QThread::msleep(1000);
        //        blackfly.setAcquisition(true);
        //        blackfly.start();
        //        QThread::msleep(500);
        //        blackfly.setAcquisition(false);
        //        blackfly.wait();
    }
    else {
        qDebug()<<"Camera initialization failed.";
    }




    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);









    return app.exec();
}
