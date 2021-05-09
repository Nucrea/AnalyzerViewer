#include "analyzermodule.h"

#include <QSerialPortInfo>
#include <QElapsedTimer>
#include <QDebug>
#include <QMutex>


//-----------------------------------------------------------------------
//---- ANALYZER MODULE --------------------------------------------------
//-----------------------------------------------------------------------
AnalyzerScript::AnalyzerScript()
{
    bind(14001);
}


AnalyzerScript::~AnalyzerScript()
{
}


int recursion_level = 0;


QByteArray AnalyzerScript::sendPacket(QByteArray arr)
{
    write(arr);
    waitForBytesWritten(200);
    waitForReadyRead(200);

    QByteArray ret_data = read(65565);
    if (ret_data.size() == 0)
    {
        if (recursion_level == 3) {
            recursion_level = 0;
            return QByteArray();
        }

        recursion_level++;
        return sendPacket(arr);
    }
    \
    recursion_level = 0;
    return ret_data;
}


bool AnalyzerScript::startMeasure(AnalyzerMeasureSettings& settings)
{
    QByteArray array;
    array.append( (char)AnalyzerScriptCommand::MEASURE );
    array.append( settings.flag_delta? 1:0 );
    array.append( settings.flag_emg? 1:0 );
    array.append( QByteArray(reinterpret_cast<const char *>(&settings.layers_count), sizeof(int)) );
    array.append( QByteArray(reinterpret_cast<const char *>(&settings.layer_step), sizeof(float)) );

    QByteArray data = sendPacket(array);

    return (data.length()==1 && data.at(0)==1);
}


bool AnalyzerScript::stop()
{
    QByteArray data = sendPacket( QByteArray(1, (char)AnalyzerScriptCommand::STOP) );

    return (data.length()==1 && data.at(0)==1);
}


bool AnalyzerScript::readEcho()
{
    QByteArray data = sendPacket( QByteArray(1, (char)AnalyzerScriptCommand::READ_ECHO) );

    return (data.length()==1 && data.at(0)==1);
}


bool AnalyzerScript::readLastLayer(QVector<Vertex>& layer)
{
    QByteArray data = sendPacket( QByteArray(1, (char)AnalyzerScriptCommand::READ_LAST_LAYER) );

    if (data.length() != 20*8) {
        return false;
    }

    QMutex mutex;

    mutex.lock();

    layer.clear();
    for (int i=0; i<8; ++i)
    {
        layer.push_back( Vertex() );
        const char* bytes = data.constData();
        float tmp;

        memcpy(&tmp, &bytes[i*20 + 0], 4); layer[i].x = tmp;
        memcpy(&tmp, &bytes[i*20 + 4], 4); layer[i].y = tmp;
        memcpy(&tmp, &bytes[i*20 + 8], 4); layer[i].z = tmp;
        memcpy(&tmp, &bytes[i*20 + 12], 4); layer[i].radius = tmp;
        memcpy(&tmp, &bytes[i*20 + 16], 4); layer[i].delta = tmp;
    }

    mutex.unlock();

    return true;
}


bool AnalyzerScript::readModel(Model& model)
{
    QByteArray data = sendPacket( QByteArray(1, (char)AnalyzerScriptCommand::READ_MODEL) );

    return false;
}


bool AnalyzerScript::readStatus(AnalyzerStatus& status)
{
    QByteArray data = sendPacket( QByteArray(1, (char)AnalyzerScriptCommand::READ_STATUS) );

    if (data.length() != 6) {
        return false;
    }

    QMutex mutex;

    mutex.lock();
    status.flag_calibrated = data.at(0);
    status.flag_error = data.at(1);
    status.flag_busy = data.at(2);
    status.flag_layer_ready = data.at(3);
    status.modules_count = data.at(4);
    status.layers_count = data.at(5);
    mutex.unlock();

    return true;
}

