#include "spektogram.h"
#include "ui_spektogram.h"


Spektogram::Spektogram(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Spektogram)
{
    ui->setupUi(this);
    ui->fftComboBox->addItems(QStringList{"128",
                                          "256",
                                          "512",
                                          "1024",
                                          "2048",
                                          "4096",
                                          "8192"});

    ui->oknoComboBox->addItems(QStringList{"Prostokątne",
                                           "Hanna",
                                           "Hamminga",
                                           "Barletta",
                                           "Trójkątne",
                                           "Barletta-Hanna",
                                           "Blackmana"});




    interpol = false;                                                        //Domyslnie wylaczamy interpolacje
    ui->fftComboBox->setCurrentIndex(2);
    ui->oknoComboBox->setCurrentIndex(2);
    QString a = ui->fftComboBox->currentText();
    _fftSize = a.toInt();
    chooseWindow(ui->oknoComboBox->currentIndex());
    colorMap = new QCPColorMap(ui->customPlot->xAxis, ui->customPlot->yAxis);
    amplitudy = new QCPColorScale(ui->customPlot);

    ui->customPlot->xAxis->setLabel("Time [ms]");
    ui->customPlot->yAxis->setLabel("Frequency [Hz]");

    ui->customPlot->plotLayout()->addElement(0, 1, amplitudy);
    amplitudy->setType(QCPAxis::atRight);
    amplitudy->axis()->setLabel("Amplituda [dB]");
    colorMap->setColorScale(amplitudy); // associate the color map with the color scale
}


Spektogram::~Spektogram()
{
    delete colorMap;
    delete amplitudy;
    delete ui;
}

void Spektogram::chooseWindow(int i){
    fftWin.resize(_fftSize);
    switch(i){
    case 0:                          // prostokątne
    {
        fftWin.fill(1);
        break;
    }
    case 1:                          // Hanna
    {
        for(int i=0;i<_fftSize;i++){
            fftWin[i] = 0.5*(1-cos((2*M_PI*i)/_fftSize-1));
        }
        break;
    }
    case 2:                          // Hamminga
    {
        double alph = 0.53834;
        double bet = 0.46164;
        for(int i=0;i<_fftSize;i++){
            fftWin[i] = alph-(bet*cos((2*M_PI*i)/(_fftSize-1)));
        }
        break;
    }
    case 3:                         // Barletta
    {
        for(int i=0;i<_fftSize;i++){
            fftWin[i]=1-qFabs((i-(_fftSize-1)/2)/((_fftSize-1)/2));
        }
        break;
    }
    case 4:                         //Trójkątne
    {
        for(int i=0;i<_fftSize;i++){
            fftWin[i]=1-qFabs((i-(_fftSize-1)/2)/(_fftSize/2));
        }
        break;
    }
    case 5:                          // Barletta-Hanna
    {
        double a0 = 0.62;
        double a1 = 0.48;
        double a2 = 0.38;
        for(int i=0;i<_fftSize;i++){
            fftWin[i]=a0-a1*qFabs(i/(_fftSize-1)-(1/2))-a2*cos((2*M_PI*i)/(_fftSize-1));
        }
        break;
    }
    case 6:                         // Blackmana
    {
        double a0 = 0.42;
        double a1 = 0.5;
        double a2 = 0.08;
        for(int i=0;i<_fftSize;i++){
            fftWin[i]=a0-a1*cos((2*M_PI*i)/(_fftSize-1))+a2*cos((4*M_PI*i)/(_fftSize-1));
        }
        break;
    }

    case 11:                         // flat-top o malej rozdzielczosci
    {
        double a0 = 1;
        double a1 = 1.93;
        double a2 = 1.29;
        double a3 = 0.388;
        double a4 = 0.028;
        for(int i=0;i<_fftSize;i++){
            fftWin[i] = a0-a1*cos((2*M_PI*i)/(_fftSize-1))+a2*cos((4*M_PI*i)/(_fftSize-1))-a3*cos((6*M_PI*i)/(_fftSize-1))+a4*cos((8*M_PI*i)/(_fftSize-1));
        }
        break;
    }
    default:
        break;
    }
}



void Spektogram::loadFile(){
    file.seek(file.headerLength());
    _numberChannels = file.header.NumOfChan;
    _samplesPerSec = file.header.SamplesPerSec;
    _bytesPerSec = file.header.bytesPerSec;
    _blockAlign = file.header.blockAlign;
    _bitsPerSample = file.header.bitsPerSample;
    soundLength = ((file.size()-44)*1000)/file.header.bytesPerSec;
    qDebug() << file.header.AudioFormat << endl
             << _numberChannels << endl
             << _samplesPerSec << endl
             << _bytesPerSec << endl
             << _blockAlign << endl
             << _bitsPerSample << endl
             << file.header.Subchunk2ID<< endl
             << file.header.Subchunk2Size << endl
             << file.bytesAvailable() << endl
             << file.size();
    qDebug() << "Dlugosc:" << soundLength;
    _Fs = _samplesPerSec/2;
    tempMagn.resize(_Fs);
    tempMagn.fill(1);
    readDataFile();
    calculateFFT();
    makePlot();

    if(!ui->fftComboBox->isEnabled())
        ui->fftComboBox->setEnabled(true);
    if(!ui->oknoComboBox->isEnabled())
        ui->oknoComboBox->setEnabled(true);

    if(!ui->true_radioButton->isEnabled())
        ui->true_radioButton->setEnabled(true);
    if(!ui->false_radioButton->isEnabled())
        ui->false_radioButton->setEnabled(true);
}

void Spektogram::on_actionDrawSpectogram_triggered()
{
    calculateFFT();
    makePlot();
}

void Spektogram::readDataFile(){

    quint16 byteToRead = _bitsPerSample/8;
    magnitudes.clear();

    QByteArray buffer;
    sampleData.resize(static_cast<int>(file.bytesAvailable()));
    sampleData.fill(0);

    file.seek(file.headerLength());
        for(int i = 0; !file.atEnd(); i++){
                file.read(buffer.data(),byteToRead);
                switch(byteToRead){
                    case 1:
                        quint8 *sample8;
                        sample8 = reinterpret_cast<quint8*>(buffer.data());
                        sampleData[i]=*sample8;///65536.0;
                        break;
                    case 2:
                        quint16 *sample16;
                        sample16 = reinterpret_cast<quint16*>(buffer.data());
                        sampleData[i]=*sample16;///65536.0;
                        break;
                    case 3:
                        quint32 *sample32;
                        sample32 = reinterpret_cast<quint32*>(buffer.data());
                        sampleData[i]=*sample32;///65536.0;
                        break;
                }
                qDebug() << i;
        }
}

void Spektogram::calculateFFT(){

    _windowsX = 0;
    magnitudes.clear();
    fftData.resize(_fftSize);
    fftData.fill(1);
    magnitudeData.resize(_fftSize/2);
    phaseData.resize(_fftSize/2);
    qint32 fftSizeHalf = _fftSize/2;
    file.seek(file.headerLength());
    QVector<qreal> sampleFFTData;
    qDebug() << "FFF";
        for(QVector<qreal>::iterator wsk = sampleData.begin(); wsk != sampleData.end();){
            _windowsX++;
            sampleFFTData.clear();
            sampleFFTData.resize(_fftSize);
            for(int i = 0; ((wsk != sampleData.end() && i<_fftSize)); i++){
                sampleFFTData[i] = *wsk;
                wsk++;
            }

            for(int i=0;i<_fftSize; i++){
                fftData[static_cast<uint>(i)].real(sampleFFTData[i]*fftWin[i]);
                fftData[static_cast<uint>(i)].imag(0);
            }

            fftData=arma::fft(fftData);

            for(int i=0;i<fftSizeHalf; i++){
                magnitudeData[i]=abs(fftData[static_cast<uint>(i)]);
                phaseData[i]=arg(fftData[static_cast<uint>(i)]);
            }


            double max=*std::max_element(magnitudeData.begin(), magnitudeData.end());

            for(int i=0;i<fftSizeHalf; i++){

                magnitudeData[i]/=max;
                magnitudeData[i]=20*log(magnitudeData[i]);  //skala decybelowa
            }


            magnitudes.append(magnitudeData);

        }

        qDebug()<<2*_Fs;
        qDebug()<<_fftSize;
        qreal chujmnietrafia = (2*_Fs)/static_cast<qreal>(_fftSize);
        qDebug()<<chujmnietrafia<<" <- rozdzielczosc czasowa";
        qreal temp_liczba_okienY = (_Fs)/chujmnietrafia;
        qDebug()<<temp_liczba_okienY<< " <- liczba okien double";
        _windowsY = static_cast<int>(temp_liczba_okienY);
        qDebug()<<_windowsY<< " <- liczba okien int";
}
void Spektogram::makePlot(){

    qDebug()<<"Liczba okien X "<<_windowsX;
    qDebug()<<"Czas trwania utowru "<<soundLength;
    qDebug()<<"Liczba okien Y "<<_windowsY;
    qDebug()<<"Liczba probek "<<_Fs;
    qDebug()<<"Liczba okien X fft"<<magnitudes.length();
    qDebug()<<"Liczba okien Y fft"<<magnitudes.at(0).length();
//    ui->customPlot->clearGraphs();
//    ui->customPlot->clearPlottables();
//    ui->customPlot->clearItems();


qDebug()<<"hhh";
    colorMap->data()->setSize(_windowsX,_windowsY );
      colorMap->data()->setRange(QCPRange(0, soundLength), QCPRange(0, _Fs));
      colorMap->setInterpolate(interpol);


                          //Legenda kolorow oznaczajacych amplitudy




      double x,y;
      for (int xInd=0; xInd<_windowsX; ++xInd)
        for (int yInd=0; yInd<_windowsY; ++yInd){
            colorMap->data()->cellToCoord(xInd, yInd, &x, &y);
            colorMap->data()->setCell(xInd, yInd, magnitudes.at(xInd).at(yInd));
            //qDebug()<<z;
        }

      colorMap->setGradient(QCPColorGradient::gpPolar);
      colorMap->rescaleDataRange(true);
      ui->customPlot->rescaleAxes();
      ui->customPlot->replot();

      qDebug()<<"Narysowane";


}



void Spektogram::on_oknoComboBox_activated(int index)
{
    qDebug()<<"Index okna"<<index;
    chooseWindow(index);
    calculateFFT();
    makePlot();
}

void Spektogram::on_fftComboBox_activated(const QString &arg1)
{
    QString temp = arg1;
    _fftSize = temp.toInt();
    qDebug()<<"Wybieram okno";
    chooseWindow(ui->oknoComboBox->currentIndex());
    qDebug()<<"Po wyborze okna rozmiar"<<_fftSize;
    calculateFFT();
    qDebug()<<"Po liczeniu fft";
    makePlot();
}


void Spektogram::on_true_radioButton_clicked()
{
    interpol = true;
    makePlot();
}

void Spektogram::on_false_radioButton_clicked()
{
    interpol = false;
    makePlot();
}

void Spektogram::on_actionNewFile_triggered()
{
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open WAV file"), "*.wav");

    if(file.open(fileName)){
        loadFile();
    } else{
        QMessageBox::warning(this,"Błąd","Błąd odczytu pliku.");
    }
}
