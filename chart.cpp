#include "chart.h"
#include <QDebug>


Chart::Chart()
{
    for(int i=0; i<LOGMAX; i++)
        logTable[i]=(i/9)+log10((i%9)+1);

}
void Chart::drawSpectGrid(QPainter &painter, QRect geometry , int timeWindows , int Fs)
{
    gw=geometry.width()-(2*MX);
    gh=geometry.height()-(2*MY);
    gx=geometry.x()+MX;
    gy=geometry.y()+MY;
    gxstart = MX+2;
    gystart = MY+gh+24;
    this->Fs = Fs;

    qDebug()<<gh;
    qDebug()<<"Punkt poczatkowy osi x to "<<geometry.x();
    qDebug()<<"Punkt poczatkowy osi y to "<<geometry.y();

    dx=gw/static_cast<double>(gridNumX);
    dy=gh/static_cast<double>(gridNumY);


    maxValueX = timeWindows*1000;
    maxValueY = Fs;

    QPen pen;
    pen.setWidth(1);
    pen.setColor(backgroundColor);
    painter.setPen(pen);
    painter.setBrush(backgroundColor);
    painter.drawRect(geometry);

    // -----  frame ------------
    pen.setStyle(Qt::SolidLine);
    pen.setColor(gridColor);
    pen.setWidth(2);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(pen);
    painter.drawRect(gx, gy, gw, gh);
    // ----- grid ------------
    //pen.setStyle(Qt::DotLine);
    //pen.setWidth(1);
    //painter.setPen(pen);
    //for(int px=1; px<gridNumX; px++)
    //    painter.drawLine(QLineF(gx+(px*dx), gy, gx+(px*dx), gy+gh));
    //for(int py=1; py<gridNumY; py++)
    //    painter.drawLine(QLineF(gx, gy+(py*dy), gx+gw, gy+(py*dy)));
    // ------ desc ----------
    QFont font;
    font.setPointSize(8);
    painter.setFont(font);
    pen.setColor(textColor);
    painter.setPen(pen);
    dvx=maxValueX-minValueX;
    dvy=maxValueY-minValueY;
    for(int px=0; px<=gridNumX; px++)
        painter.drawText(QPointF(gx-(font.pointSize()/4)+px*dx, gy+gh+(font.pointSize()*2)), QString().sprintf("%d",static_cast<int>(minValueX+(dvx*px)/gridNumX)));
    for(int py=0; py<=gridNumY; py++)
        painter.drawText(QPointF(gx-(font.pointSize()*4), gy+(font.pointSize()/2)+py*dy), QString().sprintf("%d",static_cast<int>(maxValueY-(dvy*py)/gridNumY)));

}
void Chart::drawSpectData(QPainter &painter, QVector<QVector<double>> &magnitudes)
{
    int freqMax = magnitudes.at(1).size();
    int timeWindows = magnitudes.size();
    QVector<double> temp;
    for(int i=1;i<=Fs;i++){

    }
    dx=gw/timeWindows;
    dy=gh/freqMax;
    //gmy=gy+(gh/2);
    //int k = 0;

    //qDebug() << data.size();
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(pixColor);
    pen.setWidth(1);
    painter.setPen(pen);

    /*for(int i=1; i<1000; i++){
        for(int j=100;j<1000;j++){
            painter.drawPoint(gx+(i-1),gx+i*dx);
        }
        //painter.drawLine(QLineF(gx+(i-1)*dx, gmy-(data[i-1]*dy), gx+i*dx, gmy-(data[i]*dy)));
    }*/
    for(int i=0;i<timeWindows;i++){
        for(int j=0;j<freqMax;j++){
            for(int k=0;k<dx;k++){
                pixelColor(magnitudes.at(i).at(j));
                painter.drawPoint(static_cast<int>(gxstart+k+(k*dx)),gystart-j);
            }
        }
    }

}

void Chart::pixelColor(double magnitude){
    if(magnitude >= 100)
        pixColor = Qt::black;
    if(magnitude >= 80 && magnitude < 100)
        pixColor = Qt::darkBlue;
    if(magnitude >= 60 && magnitude < 80)
        pixColor.setRgb(184,3,225);
    if(magnitude >= 40 && magnitude < 60)
        pixColor = Qt::red;
    if(magnitude >= 20 && magnitude < 40)
        pixColor = Qt::yellow;
    if(magnitude >= 0 && magnitude < 20)
        pixColor = Qt::white;
}

