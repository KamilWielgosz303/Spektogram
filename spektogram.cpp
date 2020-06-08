#include "spektogram.h"
#include "ui_spektogram.h"

#define FFT_SIZE 4000

Spektogram::Spektogram(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Spektogram)
{
    ui->setupUi(this);
    const QString dir;
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open WAV file"), "*.wav");
    WavFile file;


    fftWin.resize(FFT_SIZE);
    fftWin.fill(1);
    fftData.resize(FFT_SIZE);
    fftData.fill(1);
    magnitudeData.resize(FFT_SIZE);
    phaseData.resize(FFT_SIZE);

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
        for(int l = 0; !file.atEnd(); l++){

            sampleData.resize(FFT_SIZE);
            sampleData.fill(0);
            for(int i = 0; i<FFT_SIZE; i++){
                if (file.atEnd()){
                    break;
                }

                file.read(buffer.data(),4);
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

            }
            qDebug()<< phaseData;
        }



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



