#ifndef MODEL_H
#define MODEL_H


#include <QVector>
#include <QDateTime>


struct Vertex
{
    double x, y, z;
    float radius, delta;
    double emg, toughness;
};


class Model
{
private:
    QVector<QVector<Vertex>> mVertexes;
    QDateTime mDateTime;
    QString mName;
    int mLayers;
    int mPoints;
    int mColor;

public:
    Model();

    const QVector<QVector<Vertex>>& getVertexes() const;
    const QDateTime& getDateTime() const;
    const QString& getName() const;
    int getLayers();
    int getPoints();
    int getColor();

    void clear();

    void setVertexes(const QVector<QVector<Vertex>>& vertexes);
    void setDateTime(const QDateTime& date);
    void setName(const QString& name);
    void setColor(int color);

    bool loadFromCSV(QString path);
    bool loadFromCLT(QString path);

    bool interpolateLinear(int mul_x, int mul_z);
    bool interpolatePoly(int order, int mul_x, int mul_z);
    bool interpolateFEA(int mul_x, int mul_z);
};


#endif // MODELTOOLS_H
