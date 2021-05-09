#ifndef ANALYZER_MODULE_H
#define ANALYZER_MODULE_H


#include <QSerialPort>
#include <QThread>
#include <QTimer>
#include <QUdpSocket>
#include "model.h"


struct AnalyzerStatus
{
    bool flag_calibrated;
    bool flag_error;
    bool flag_busy;
    bool flag_layer_ready;
    int modules_count;
    int layers_count;
};


enum class AnalyzerScriptCommand
{
    MEASURE = 0x41,
    STOP = 0x42,
    READ_MODEL = 0x43,
    READ_STATUS = 0x44,
    READ_ECHO = 0x45,
    READ_LAST_LAYER = 0x46
};


struct AnalyzerMeasureSettings
{
    bool flag_emg;
    bool flag_delta;
    int layers_count;
    float layer_step;
};


class AnalyzerScript : public QUdpSocket
{
    QByteArray sendPacket(QByteArray arr);

public:
    AnalyzerScript();
    virtual ~AnalyzerScript();

    bool startMeasure(AnalyzerMeasureSettings& settings);
    bool stop();
    bool readEcho();
    bool readModel(Model& model);
    bool readLastLayer(QVector<Vertex>& layer);
    bool readStatus(AnalyzerStatus& status);
};


#endif // ANALYZER_H
