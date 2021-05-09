#include "analyzer.h"

#include <QSerialPortInfo>
#include <QElapsedTimer>
#include <QDebug>
#include <QMutex>


//-----------------------------------------------------------------------
//---- ANALYZER CLASS ---------------------------------------------------
//-----------------------------------------------------------------------

Analyzer::Analyzer()
{
    qRegisterMetaType<DisconnectStatus>();
    qRegisterMetaType<ConnectionStatus>();
    qRegisterMetaType<MeasuringStatus>();
    qRegisterMetaType<QVector<Vertex>>();

    mWorkerThread = new QThread();
    mWorker = new AnalyzerWorker();
    mWorker->moveToThread(mWorkerThread);

    connect(mWorkerThread, SIGNAL(started()), mWorker, SLOT(run()));
    connect(mWorker, SIGNAL(disconnected(DisconnectStatus)), this, SIGNAL(disconnected(DisconnectStatus)));
    connect(mWorker, SIGNAL(connectionFinished(ConnectionStatus)), this, SIGNAL(connectionFinished(ConnectionStatus)));
    connect(mWorker, SIGNAL(measureNewLayer(QVector<Vertex>)), this, SIGNAL(measureNewLayer(QVector<Vertex>)));
    connect(mWorker, SIGNAL(measureFinished(MeasuringStatus)), this, SIGNAL(measureFinished(MeasuringStatus)));

    connect(mWorker, &AnalyzerWorker::disconnected, [=](DisconnectStatus status) { mIsConnected = false; } );
    connect(mWorker, &AnalyzerWorker::connectionFinished, [=](ConnectionStatus status) {
        mIsConnected = status==ConnectionStatus::OK;
    } );

    mWorkerThread->start();
}


Analyzer::~Analyzer()
{
}


void Analyzer::stopAll() {
    mWorker->stopAll();
}

void Analyzer::startConnect() {
    mWorker->setTask(WorkingTask::CONNECT);
}

void Analyzer::startDisconnect() {
    mWorker->setTask(WorkingTask::DISCONNECT);
}

void Analyzer::startMeasure(AnalyzerMeasureSettings& settings) {
    mWorker->setMeasureSettings(settings);
    mWorker->setTask(WorkingTask::MEASURE);
}

bool Analyzer::isConnected() {
    return mIsConnected;
}


//-----------------------------------------------------------------------
//---- ANALYZER WORKER -------------------------------===----------------
//-----------------------------------------------------------------------

AnalyzerWorker::AnalyzerWorker()
{
}


AnalyzerWorker::~AnalyzerWorker()
{
    if (mScript != nullptr) {
        delete mScript;
    }

    if (mScriptProcess != nullptr) {
        delete mScriptProcess;
    }
}


void AnalyzerWorker::stopAll() {
    mStoppedFlag = true;
}

void AnalyzerWorker::setTask(WorkingTask task) {
    mWorkingTask = task;
}

void AnalyzerWorker::setMeasureSettings(AnalyzerMeasureSettings& settings) {
    mSettings = settings;
}


ConnectionStatus AnalyzerWorker::getConnectionStatus() const {
    return mConnectionStatus;
}

MeasuringStatus AnalyzerWorker::getMeasuringStatus() const {
    return mMeasuringStatus;
}

bool AnalyzerWorker::isTaskFinished() const {
    return mWorkingTask == WorkingTask::NONE;
}

bool AnalyzerWorker::isConnected() const {
    return mConnectedFlag;
}


//Worker thread func
void AnalyzerWorker::run()
{
    mScript = new AnalyzerScript();
    AnalyzerStatus status;

    while(true)
    {
        //Check, if modules is connected
        if (mConnectedFlag)
        {
            if ( !mScript->readStatus(status) || status.flag_error ) {
                mConnectedFlag = false;
                mScript->disconnectFromHost();
                emit disconnected(DisconnectStatus::CONNECTION_LOST);
            }
        }

        //If there is a task
        if (mWorkingTask != WorkingTask::NONE)
        {
            WorkingTask tsk = mWorkingTask;

            //Perform task
            switch(tsk) {
                case WorkingTask::DISCONNECT: startDisconnect(mStoppedFlag); break;
                case WorkingTask::CONNECT: startConnect(mStoppedFlag); break;
                case WorkingTask::MEASURE: startMeasure(mSettings, mStoppedFlag); break;
                default: break;
            }

            //Callback via signal
            if (!mStoppedFlag)
            {
                switch(tsk) {
                    case WorkingTask::DISCONNECT: emit disconnected(DisconnectStatus::ASKED); break;
                    case WorkingTask::CONNECT: emit connectionFinished(mConnectionStatus); break;
                    case WorkingTask::MEASURE: emit measureFinished(mMeasuringStatus); break;
                    default: break;
                }
            }

            mWorkingTask = WorkingTask::NONE;
        }

        //Clear flags
        mStoppedFlag = false;

        thread()->msleep(500);
    }
}


void AnalyzerWorker::startMeasure(AnalyzerMeasureSettings& settings, const bool& stopFlag)
{
    if (!mConnectedFlag) {
        mMeasuringStatus = MeasuringStatus::STATUS_ERROR;
    }

    AnalyzerStatus status;
    QVector<Vertex> layer;
    QElapsedTimer timer;

    mScript->startMeasure(settings);
    timer.start();
    while (timer.elapsed() <= 500) {
        if (stopFlag) {
            return;
        }
    }

    while(true)
    {
        if (stopFlag)
        {
            timer.start();
            while (timer.elapsed() <= 500);

            if ( !mScript->readStatus(status) ) {
                mMeasuringStatus = MeasuringStatus::STATUS_ERROR;
                return;
            }

            if (!status.flag_busy) {
                mMeasuringStatus = MeasuringStatus::STATUS_STOPPED;
                mStoppedFlag = false;
                return;
            }
        }

        if ( !mScript->readStatus(status) ) {
            mMeasuringStatus = MeasuringStatus::STATUS_ERROR;
            return;
        }

        if (status.flag_layer_ready)
        {
            if ( !mScript->readLastLayer(layer) ) {
                mMeasuringStatus = MeasuringStatus::STATUS_ERROR;
                return;
            }

            emit measureNewLayer(layer);
        }

        if (!status.flag_busy) {
            break;
        }

        timer.start();
        while (timer.elapsed() <= 500) {
            if (stopFlag) {
                mScript->stop();
                //mMeasuringStatus = MeasuringStatus::STATUS_STOPPED;
                //return;
            }
        }
    }

    mMeasuringStatus = MeasuringStatus::STATUS_OK;
}


void AnalyzerWorker::startConnect(const bool& stopFlag)
{
    if (mConnectedFlag) {
        return;
    }

    AnalyzerStatus status;
    QElapsedTimer timer;

    timer.start();
    while(timer.elapsed() <= 4000) {
        if (stopFlag) {
            return;
        }
    }

    mScript->connectToHost("127.0.0.1", 14002);

    if ( !mScript->readEcho() ) {
        mConnectionStatus = ConnectionStatus::ERROR;
        mScript->disconnectFromHost();
    }
    else if ( !mScript->readStatus(status) || status.flag_error ) {
        mConnectionStatus = ConnectionStatus::ERROR;
        mScript->disconnectFromHost();
    }
    else
    {
        if (status.flag_busy) {
            mScript->stop();
        }

        mConnectionStatus = ConnectionStatus::OK;
        mConnectedFlag = true;
    }
}


void AnalyzerWorker::startDisconnect(const bool &stopFlag)
{
    if ( !mConnectedFlag ) {
        return;
    }

    mScript->stop();
    mScript->disconnectFromHost();

    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed() <= 500);

    mConnectedFlag = false;
}


//------------------------------------------------------------------------------------
//Modules collective functions

void AnalyzerWorker::delay(int msecs, const bool& stopFlag)
{
    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed()<=msecs && !stopFlag);
}

