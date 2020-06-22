#include "spektogram.h"
#include "ui_spektogram.h"


Spektogram::Spektogram(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Spektogram)
{
    ui->setupUi(this);
    ui->fftComboBox->addItems(QStringList{"512",
                                          "1024",
                                          "2048"});

    ui->oknoComboBox->addItems(QStringList{"Prostokątne",
                                           "Hanna",
                                           "Hamminga",
                                           "Barletta",
                                           "Trójkątne",
                                           "Barletta-Hanna",
                                           "Blackmana"});
    QString a = ui->fftComboBox->currentText();
    _fftSize = a.toInt();
    qDebug() << _fftSize;
    chooseWindow(ui->oknoComboBox->currentIndex());
    qDebug() << ui->oknoComboBox->currentIndex();
    fftData.resize(_fftSize);
    fftData.fill(1);
    magnitudeData.resize(_fftSize/2);
    phaseData.resize(_fftSize/2);
}


Spektogram::~Spektogram()
{
    delete ui;
}

void Spektogram::chooseWindow(int i){
    fftWin.resize(_fftSize);
    switch(i){
    case 0:                          //Okno prostokątne
    {
        fftWin.fill(1);
        break;
    }
    case 1:                          //Okno Hanna
    {
        for(int i=0;i<_fftSize;i++){
            fftWin[i] = 0.5*(1-cos((2*M_PI*i)/_fftSize-1));
        }
        break;
    }
    case 2:                          //Okno Hamminga
    {
        double alph = 0.53834;
        double bet = 0.46164;
        for(int i=0;i<_fftSize;i++){
            fftWin[i] = alph-(bet*cos((2*M_PI*i)/(_fftSize-1)));
        }
        break;
    }
    case 3:                         //Okno Barletta
    {
        for(int i=0;i<_fftSize;i++){
            fftWin[i]=1-qFabs((i-(_fftSize-1)/2)/((_fftSize-1)/2));
        }
        break;
    }
    case 4:                         //Okno Trójkątne
    {
        for(int i=0;i<_fftSize;i++){
            fftWin[i]=1-qFabs((i-(_fftSize-1)/2)/(_fftSize/2));
        }
        break;
    }
    case 5:                          //Okno Barletta-Hanna
    {
        double a0 = 0.62;
        double a1 = 0.48;
        double a2 = 0.38;
        for(int i=0;i<_fftSize;i++){
            fftWin[i]=a0-a1*qFabs(i/(_fftSize-1)-(1/2))-a2*cos((2*M_PI*i)/(_fftSize-1));
        }
        break;
    }
    case 6:                         //Okno Blackmana
    {
        double a0 = 0.42;
        double a1 = 0.5;
        double a2 = 0.08;
        for(int i=0;i<_fftSize;i++){
            fftWin[i]=a0-a1*cos((2*M_PI*i)/(_fftSize-1))+a2*cos((4*M_PI*i)/(_fftSize-1));
        }
        break;
    }

    case 11:                         //Okno flat-top o malej rozdzielczosci
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

void Spektogram::on_actionNewFile_triggered()
{
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open WAV file"), "*.wav");

    if(file.open(fileName)){
        loadFile();
    } else{
        QMessageBox::warning(this,"Błąd","Błąd odczytu pliku.");
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
}

void Spektogram::on_actionDrawSpectogram_triggered()
{
/*     walidacja fft size oraz okno

    func



    */
    calculateFFT();
    makePlot();
}

void Spektogram::calculateFFT(){

    _windowsX = 0;
    quint16 byteToRead = _bitsPerSample/8;
    qDebug() << byteToRead;
    magnitudes.clear();

    QByteArray buffer;
    quint32 *sample;
    QVector<qreal> sampleData;
    qint32 fftSizeHalf = _fftSize/2;
    sampleData.resize(_fftSize);
    file.seek(file.headerLength());
        for(int l = 0; !file.atEnd(); l++){
            _windowsX++;
            sampleData.fill(0);

            for(int i = 0; i<_fftSize; i++){
                if (file.atEnd()){
                    break;
                }
                file.read(buffer.data(),byteToRead);
                sample = reinterpret_cast<quint32*>(buffer.data());
                sampleData[i]=*sample/65536.0;
            }

            for(int i=0;i<_fftSize; i++){
                fftData[static_cast<uint>(i)].real(sampleData[i]*fftWin[i]);
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
                magnitudeData[i]=10*log(magnitudeData[i]);  //skala decybelowa
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

    QCPColorMap *colorMap = new QCPColorMap(ui->customPlot->xAxis, ui->customPlot->yAxis);
    colorMap->data()->setSize(_windowsX,_windowsY );
    ui->customPlot->xAxis->setLabel("Time [ms]");
    ui->customPlot->yAxis->setLabel("Frequency [Hz]");
      colorMap->data()->setRange(QCPRange(0, soundLength), QCPRange(0, _Fs));
      colorMap->setInterpolate(false);

      QCPColorScale *colorScale = new QCPColorScale(ui->customPlot);
      ui->customPlot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
      colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
      colorMap->setColorScale(colorScale); // associate the color map with the color scale
      colorScale->axis()->setLabel("Amplituda [dB]");


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

}
