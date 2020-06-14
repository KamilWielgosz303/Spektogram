#include "spektogram.h"
#include "ui_spektogram.h"

#define FFT_SIZE 512                      //Przy czestotliwosci 8000 rozdzielczosc czestotliwosciowa = 15,625 Hz        (8000/512)


Spektogram::Spektogram(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Spektogram)
{
    ui->setupUi(this);
    const QString dir;
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open WAV file"), "*.wav");
    WavFile file;

    fftWin.resize(FFT_SIZE);
    //fftWin.fill(1);                                       //Okno prostokątne




    /*for(int i=0;i<FFT_SIZE;i++){
        fftWin[i] = 0.5*(1-cos((2*M_PI*i)/FFT_SIZE-1));       //Okno Hanna
    }*/




    double alph = 0.53834;
    double bet = 0.46164;
    for(int i=0;i<FFT_SIZE;i++){
        fftWin[i] = alph-(bet*cos((2*M_PI*i)/(FFT_SIZE-1)));             //Okno Hamminga
    }




    /*
    double a0 = 1;
    double a1 = 1.93;
    double a2 = 1.29;                                                   //Okno flat-top o malej rozdzielczosci
    double a3 = 0.388;
    double a4 = 0.028;
    for(int i=0;i<FFT_SIZE;i++){
        fftWin[i] = a0-a1*cos((2*M_PI*i)/(FFT_SIZE-1))+a2*cos((4*M_PI*i)/(FFT_SIZE-1))-a3*cos((6*M_PI*i)/(FFT_SIZE-1))+a4*cos((8*M_PI*i)/(FFT_SIZE-1));
    }*/
    fftData.resize(FFT_SIZE);
    fftData.fill(1);
    magnitudeData.resize(FFT_SIZE/2);
    phaseData.resize(FFT_SIZE/2);
    liczba_okienX = 0;

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
            liczba_okienX++;                                                      //Obliczam na ile czesci podzieli sie utwór
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
                //magnitudeData[i]+=0.01; //saturate -40 dB

                magnitudeData[i]=10*log(magnitudeData[i]);  //skala decybelowa

                //qDebug()<<"Elo"<<magnitudeData[i];
            }

            /*for(int j=1;j<FFT_SIZE;j++){
               tempMagn[takeRightFreq(j,FFT_SIZE,Fs)] = magnitudeData[j-1];
               qDebug()<<tempMagn[j-1];
            }*/

            //qDebug()<<sampleData.length();
            //sss++;
            magnitudes.append(magnitudeData);
        }
        czas = (FFT_SIZE*liczba_okienX)/10;

        int fftsize = FFT_SIZE;
        qDebug()<<2*Fs;
        qDebug()<<fftsize;
        float chujmnietrafia = (2*Fs)/static_cast<float>(fftsize);
        qDebug()<<chujmnietrafia<<" <- rozdzielczosc czasowa";
        float temp_liczba_okienY = (Fs)/chujmnietrafia;
        qDebug()<<temp_liczba_okienY<< " <- liczba okien double";
        liczba_okienY = static_cast<int>(temp_liczba_okienY);
        qDebug()<<liczba_okienY<< " <- liczba okien int";
    }

    Spektogram::makePlot();

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

    qDebug()<<"Liczba okien X "<<liczba_okienX;
    qDebug()<<"Czas trwania utowru "<<czas;
    qDebug()<<"Liczba okien Y "<<liczba_okienY;
    qDebug()<<"Liczba probek "<<Fs;
    qDebug()<<"Liczba okien X fft"<<magnitudes.length();
    qDebug()<<"Liczba okien Y fft"<<magnitudes.at(0).length();

    QCPColorMap *colorMap = new QCPColorMap(ui->customPlot->xAxis, ui->customPlot->yAxis);
    colorMap->data()->setSize(liczba_okienX,liczba_okienY );
    ui->customPlot->xAxis->setLabel("Time [ms]");
    ui->customPlot->yAxis->setLabel("Frequency [Hz]");
      colorMap->data()->setRange(QCPRange(0, czas), QCPRange(0, Fs));
      colorMap->setInterpolate(true);

      QCPColorScale *colorScale = new QCPColorScale(ui->customPlot);
      ui->customPlot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
      colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
      colorMap->setColorScale(colorScale); // associate the color map with the color scale
      colorScale->axis()->setLabel("Amplituda [dB]");


      double x,y,z;
      for (int xInd=0; xInd<liczba_okienX; ++xInd)
        for (int yInd=0; yInd<liczba_okienY; ++yInd){
            colorMap->data()->cellToCoord(xInd, yInd, &x, &y);


            colorMap->data()->setCell(xInd, yInd, magnitudes.at(xInd).at(yInd));
            //qDebug()<<z;
        }

      colorMap->setGradient(QCPColorGradient::gpPolar);
      colorMap->rescaleDataRange(true);



      ui->customPlot->rescaleAxes();

}


