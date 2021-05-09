#include "model.h"
#include <QFile>
#include <QMessageBox>
#include <QRandomGenerator>
#include "math.h"
#include "spline.h"


Model::Model() {
    mLayers = 0;
    mPoints = 0;
    mColor = 0xFACE8D;
}


const QVector<QVector<Vertex>>& Model::getVertexes() const {
    return mVertexes;
}

int Model::getLayers() {
    return mLayers;
}

int Model::getPoints() {
    return mPoints;
}

int Model::getColor() {
    return mColor;
}

const QDateTime& Model::getDateTime() const {
    return mDateTime;
}


void Model::clear() {
    mVertexes.clear();
    mLayers = 0;
    mPoints = 0;
}


void Model::setVertexes(const QVector<QVector<Vertex>>& vertexes)
{
    mVertexes = vertexes;
    mLayers = vertexes.count();

    if (mLayers != 0) {
        mPoints = vertexes[0].count();
    }

    /*for (auto layer: vertexes)
    {
        mVertexes.push_back(QVector<Vertex>() );
        auto& vector = mVertexes.back();

        for (Vertex vertex: layer)
        {
            vector.push_back( Vertex() );
            vector.last() = vertex;
        }
    }*/
}

void Model::setDateTime(const QDateTime& date) {
    mDateTime = date;
}

void Model::setColor(int color) {
    mColor = color;
}


bool Model::loadFromCSV(QString path)
{
    mVertexes.clear();

    QFile file(path);
    if ( !file.open(QFile::ReadOnly) ) {
        return false;
    }

    QVector<QStringList> lines;
    QRandomGenerator random(QDateTime::currentMSecsSinceEpoch());

    while( !file.atEnd() )
        lines.push_back( QString( file.readLine() ).split(';') );

    for(auto line: lines)
    {
        int number = line[4].toInt();

        if (number==1) {
            mLayers++;
            mVertexes.push_back( QVector<Vertex>() );
        }

        if (number > mPoints)
            mPoints = number;

        mVertexes[mLayers-1].push_back( Vertex() );
        mVertexes[mLayers-1][number-1].x = line[0].toDouble();
        mVertexes[mLayers-1][number-1].y = line[1].toDouble();
        mVertexes[mLayers-1][number-1].z = line[2].toDouble();
        mVertexes[mLayers-1][number-1].emg = 0x8F + (random.generate() % (0xFF - 0x8F));
        mVertexes[mLayers-1][number-1].delta = line[3].toDouble();
    }

    file.close();

    return true;
}


bool Model::loadFromCLT(QString path)
{
    return false;
}


bool Model::interpolateLinear(int mul_x, int mul_z)
{

}


bool Model::interpolatePoly(int order, int mul_points, int mul_layers)
{
    if (mVertexes.size()==0 || mVertexes[0].size()==0) {
        return false;
    }

    QVector<QVector<Vertex>> vertexes1, vertexes2, vertexes3;

    int layers = mVertexes.size();
    int points = mVertexes[0].size();

    //Interpolate circles
    for (int l=0; l<layers; ++l)
    {
        //Fill radiuses
        std::vector<double> N, R, D;
        for (int p=0; p<points; ++p) {
            double r = sqrt( pow(mVertexes[l][p].x, 2) + pow(mVertexes[l][p].y, 2) );
            R.push_back(r);
            N.push_back(p);
            D.push_back(mVertexes[l][p].delta);
        }
        R.push_back(R[0]);
        N.push_back(points);
        D.push_back(mVertexes[l][0].delta);

        //Create spline
        tk::spline splineForm;
        tk::spline splineDelta;
        splineForm.set_points(N, R);
        splineDelta.set_points(N, D);

        //Push spline values
        QVector<Vertex> layer;
        float new_points = points*mul_points;
        for (int i=0; i<new_points; ++i)
        {
            float x = ((float)i) / ((float)mul_points);
            float value = splineForm(x);
            float angle = M_PI_2 + i*(2*M_PI/new_points);

            Vertex v;
            v.x = value*cos(angle);
            v.y = value*sin(angle);
            v.z = mVertexes[l][0].z;
            v.delta = splineDelta(x);
            layer.push_back(v);
        }

        vertexes1.push_back(layer);
    }

    setVertexes(vertexes1);

    return true;
}

bool Model::interpolateFEA(int mul_x, int mul_z)
{
    return false;
}


