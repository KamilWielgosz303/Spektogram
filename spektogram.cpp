#include "spektogram.h"
#include "ui_spektogram.h"

Spektogram::Spektogram(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Spektogram)
{
    ui->setupUi(this);
    const QString dir;
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open WAV file"), "*.wav");
    WavFile file;
    if(file.open(fileName)){
        file.seek(file.headerLength());
        qDebug()<<file.lengthData<<" "<<file.channelsNumber;
        qDebug()<<file.size();
        //file.read(reinterpret_cast<char*>(dataPtr),fileWav.Subchunk2Size);
    }
    for(int i=0;i<2048;i++)
        tempPlot.append(i);
}

Spektogram::~Spektogram()
{
    delete ui;
}

void Spektogram::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    chart.drawLinearGrid(painter, centralWidget()->geometry());
    qDebug()<<centralWidget()->geometry();
    if(ui->actiondrawSpect->isChecked()){
        chart.plotColor=Qt::red;
        chart.drawLinearData(painter, tempPlot);
        chart.plotColor=Qt::green;
        /*if(ui->selectInput1->isChecked()){
            chart.plotColor=Qt::red;
            chart.drawLinearData(painter, timeDataCh1);
        }
        if(ui->selectInput2->isChecked()){
            chart.plotColor=Qt::green;
            chart.drawLinearData(painter, timeDataCh2);
        }
        if(ui->selectInput3->isChecked()){
            chart.plotColor=Qt::yellow;
            chart.drawLinearData(painter, timeDataCh3);
        }*/
    }
}



