#include "spektogram.h"
#include "ui_spektogram.h"

#define FFT_SIZE 512              //Przy czestotliwosci 8000 rozdzielczosc czestotliwosciowa = 15,625 Hz        (8000/512)

Spektogram::Spektogram(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Spektogram)
{
    ui->setupUi(this);
    Spektogram::makePlot();
    const QString dir;
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open WAV file"), "*.wav");
    WavFile file;

    fftWin.resize(FFT_SIZE);
    fftWin.fill(1);
    fftData.resize(FFT_SIZE);
    fftData.fill(1);
    magnitudeData.resize(FFT_SIZE/2);
    phaseData.resize(FFT_SIZE/2);
    timeWindows = 0;
    /*
    if(file.open(fileName)){
        file.seek(file.headerLength());
        //quint64 length = file.lengthData;
        QByteArray buffer;
        quint16 *s;
        QVector<double> sampleData;
        qDebug() << sizeof(WAV_HEADER) << endl
                 << file.header.AudioFormat << endl
                 << file.header.NumOfChan << endl
                 << file.header.SamplesPerSec << endl
                 << file.header.bytesPerSec << endl
                 << file.header.blockAlign << endl
                 << file.header.bitsPerSample << endl
                 << file.header.Subchunk2ID<< endl
                 << file.header.Subchunk2Size << endl
                 << file.bytesAvailable() << endl
                 << file.size();
        qDebug() << "Dlugosc:" << " " << ((file.size()-44)*1000)/file.header.bytesPerSec;
        Fs = file.header.SamplesPerSec/2;
        tempMagn.resize(Fs);
        tempMagn.fill(1);
        for(int l = 0; !file.atEnd(); l++){
            timeWindows++;

            sampleData.resize(FFT_SIZE);
            sampleData.fill(0);
            for(int i = 0; i<FFT_SIZE; i++){
                if (file.atEnd()){
                    break;
                }

                file.read(buffer.data(),2);
                s = reinterpret_cast<quint16*>(buffer.data());
                sampleData[i]=*s/65536.0;
                //qDebug() << sampleData[i];
            }
            // tutaj wywolac fft, bedzie dla n-tej sekundy
            for(int i=0;i<FFT_SIZE; i++){
                fftData[static_cast<uint>(i)].real(sampleData[i]*fftWin[i]);
                fftData[static_cast<uint>(i)].imag(0);
            }
            fftData=arma::fft(fftData);
            //fftData.print();
            for(int i=0;i<FFT_SIZE/2; i++){

                magnitudeData[i]=abs(fftData[static_cast<uint>(i)]);
                phaseData[i]=arg(fftData[static_cast<uint>(i)]);
            }
            double max=*std::max_element(magnitudeData.begin(), magnitudeData.end());
            for(int i=0;i<FFT_SIZE/2; i++){

                magnitudeData[i]/=max;  //normalise
                magnitudeData[i]+=0.01; //saturate -40 dB

                magnitudeData[i]=20*log(magnitudeData[i]);  //skala decybelowa

                //qDebug()<<"Elo"<<magnitudeData[i];
            }

            for(int j=1;j<FFT_SIZE;j++){
               tempMagn[takeRightFreq(j,FFT_SIZE,Fs)] = magnitudeData[j-1];
               qDebug()<<tempMagn[j-1];
            }

            //qDebug()<<sampleData.length();
            //sss++;
            magnitudes.append(magnitudeData);
            qDebug()<<timeWindows;
        }



    }
    */
}


Spektogram::~Spektogram()
{
    delete ui;
}

void Spektogram::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    /*
    //qDebug()<<magnitudes.at(1).at(4000);
    chart.drawSpectGrid(painter, centralWidget()->geometry(),timeWindows,Fs);
    //qDebug()<<centralWidget()->geometry();
    if(ui->actiondrawSpect->isChecked()){
        chart.drawSpectData(painter,magnitudes);
        if(ui->selectInput1->isChecked()){
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
        }
    }
    */
}


void Spektogram::drawSpektogram(int Fs,int timeWindows){

}

int Spektogram::takeRightFreq(int freq,int fftsize,int F_s){
    int rightFreq;
    rightFreq = (freq*F_s)/fftsize;
    return rightFreq;
}


void Spektogram::makePlot(){

    // configure axis rect:
    ui->customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    ui->customPlot->axisRect()->setupFullAxesBox(true);
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");

    // set up the QCPColorMap:
    QCPColorMap *colorMap = new QCPColorMap(ui->customPlot->xAxis, ui->customPlot->yAxis);
    int nx = 200;
    int ny = 200;
    colorMap->data()->setSize(nx, ny); // we want the color map to have nx * ny data points
    colorMap->data()->setRange(QCPRange(-4, 4), QCPRange(-4, 4)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions
    // now we assign some data, by accessing the QCPColorMapData instance of the color map:
    double x, y, z;
    for (int xIndex=0; xIndex<nx; ++xIndex)
    {
      for (int yIndex=0; yIndex<ny; ++yIndex)
      {
        colorMap->data()->cellToCoord(xIndex, yIndex, &x, &y);
        double r = 3*qSqrt(x*x+y*y)+1e-2;
        z = 2*x*(qCos(r+2)/r-qSin(r+2)/r); // the B field strength of dipole radiation (modulo physical constants)
        colorMap->data()->setCell(xIndex, yIndex, z);
      }
    }

    // add a color scale:
    QCPColorScale *colorScale = new QCPColorScale(ui->customPlot);
    ui->customPlot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
    colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    colorMap->setColorScale(colorScale); // associate the color map with the color scale
    colorScale->axis()->setLabel("Magnetic Field Strength");

    // set the color gradient of the color map to one of the presets:
    colorMap->setGradient(QCPColorGradient::gpPolar);
    // we could have also created a QCPColorGradient instance and added own colors to
    // the gradient, see the documentation of QCPColorGradient for what's possible.

    // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
    colorMap->rescaleDataRange();

    // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
    QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->customPlot);
    ui->customPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    // rescale the key (x) and value (y) axes so the whole color map is visible:
    ui->customPlot->rescaleAxes();

}


