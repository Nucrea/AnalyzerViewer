#ifndef ANALYZER_H
#define ANALYZER_H


#include <QProcess>
#include <QSerialPort>
#include <QThread>
#include <QTimer>
#include "model.h"
#include "analyzermodule.h"


enum class DisconnectStatus {
    ASKED,
    CONNECTION_LOST,
    PORT_ERROR,
    COMMAND_ERROR,
};


enum class ConnectionStatus {
    OK,
    ERROR
};


enum class MeasuringStatus {
    STATUS_OK,
    STATUS_ERROR,
    STATUS_STOPPED
};


enum class WorkingTask {
    NONE,
    MEASURE,
    CONNECT,
    DISCONNECT
};


struct MeasurePointData {
    float radius[8];
    float tenzo_delta[8];
    float emg_value[8];
};


Q_DECLARE_METATYPE(DisconnectStatus)
Q_DECLARE_METATYPE(ConnectionStatus)
Q_DECLARE_METATYPE(MeasuringStatus)
Q_DECLARE_METATYPE(MeasurePointData)
Q_DECLARE_METATYPE(QVector<Vertex>)


class AnalyzerWorker : public QObject
{
    Q_OBJECT

    AnalyzerScript* mScript = nullptr;
    QProcess* mScriptProcess = nullptr;

    //State machine
    ConnectionStatus mConnectionStatus;
    MeasuringStatus mMeasuringStatus;
    WorkingTask mWorkingTask = WorkingTask::NONE;
    AnalyzerMeasureSettings mSettings;
    bool mStoppedFlag = false;
    bool mConnectedFlag = false;

    void delay(int msecs, const bool& stopFlag);

public:
    AnalyzerWorker();
    ~AnalyzerWorker();

    void stopAll();
    void setTask(WorkingTask task);
    void setModel(Model* model);
    void setMeasureSettings(AnalyzerMeasureSettings& settings);

    ConnectionStatus getConnectionStatus() const;
    MeasuringStatus getMeasuringStatus() const;
    bool isTaskFinished() const;
    bool isConnected() const;

private:
    void startConnect(const bool& stopFlag);
    void startDisconnect(const bool& stopFlag);
    void startMeasure(AnalyzerMeasureSettings& settings, const bool& stopFlag);

public slots:
    void run();

signals:
    void disconnected(DisconnectStatus status);

    //connect
    void connectionFinished(ConnectionStatus status);

    //measure
    void measureFinished(MeasuringStatus status);
    void measureNewLayer(QVector<Vertex> layer);
};


class Analyzer : public QObject
{
    Q_OBJECT

    AnalyzerWorker* mWorker;
    QThread* mWorkerThread;

    bool mIsConnected = false;

public:
    Analyzer();
    ~Analyzer();

    void stopAll();
    void startConnect();
    void startDisconnect();
    void startMeasure(AnalyzerMeasureSettings& settings);

    bool isConnected();

signals:
    void disconnected(DisconnectStatus status);

    //connect
    void connectionFinished(ConnectionStatus status);

    //measure
    void measureFinished(MeasuringStatus status);
    void measureNewLayer(QVector<Vertex> layer);
};


#endif // ANALYZER_H
