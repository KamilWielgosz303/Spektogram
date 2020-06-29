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

    ui->oknoComboBox->addItems(QStringList{"Rectangular",
                                           "Hann",
                                           "Hamming",
                                           "Barlett",
                                           "Triangular",
                                           "Barlett-Hann",
                                           "Blackman",
                                           "Flat-Top"});

    ui->fftComboBox->setCurrentIndex(2);
    ui->oknoComboBox->setCurrentIndex(2);
    QString a = ui->fftComboBox->currentText();
    _fftSize = a.toInt();
    chooseWindow(ui->oknoComboBox->currentIndex());
    _colorMap = new QCPColorMap(ui->customPlot->xAxis, ui->customPlot->yAxis);
    _amplitudy = new QCPColorScale(ui->customPlot);

    ui->customPlot->xAxis->setLabel("Time [ms]");
    ui->customPlot->yAxis->setLabel("Frequency [Hz]");

    ui->customPlot->plotLayout()->addElement(0, 1, _amplitudy);
    _amplitudy->setType(QCPAxis::atRight);
    _amplitudy->axis()->setLabel("Magnitude [dB]");
    _colorMap->setColorScale(_amplitudy); // associate the color map with the color scale
}


Spektogram::~Spektogram()
{
    delete _colorMap;
    delete _amplitudy;
    delete ui;
}

void Spektogram::chooseWindow(int i){
    _fftWin.resize(_fftSize);
    switch(i){
    case 0:                          // prostokątne
    {
        _fftWin.fill(1);
        break;
    }
    case 1:                          // Hanna
    {
        for(int i=0;i<_fftSize;i++){
            _fftWin[i] = 0.5*(1-cos((2*M_PI*i)/_fftSize-1));
        }
        break;
    }
    case 2:                          // Hamminga
    {
        double alph = 0.53834;
        double bet = 0.46164;
        for(int i=0;i<_fftSize;i++){
            _fftWin[i] = alph-(bet*cos((2*M_PI*i)/(_fftSize-1)));
        }
        break;
    }
    case 3:                         // Barletta
    {
        for(int i=0;i<_fftSize;i++){
            _fftWin[i]=1-qFabs((i-(_fftSize-1)/2)/((_fftSize-1)/2));
        }
        break;
    }
    case 4:                         //Trójkątne
    {
        for(int i=0;i<_fftSize;i++){
            _fftWin[i]=1-qFabs((i-(_fftSize-1)/2)/(_fftSize/2));
        }
        break;
    }
    case 5:                          // Barletta-Hanna
    {
        double a0 = 0.62;
        double a1 = 0.48;
        double a2 = 0.38;
        for(int i=0;i<_fftSize;i++){
            _fftWin[i]=a0-a1*qFabs(i/(_fftSize-1)-(1/2))-a2*cos((2*M_PI*i)/(_fftSize-1));
        }
        break;
    }
    case 6:                         // Blackmana
    {
        double a0 = 0.42;
        double a1 = 0.5;
        double a2 = 0.08;
        for(int i=0;i<_fftSize;i++){
            _fftWin[i]=a0-a1*cos((2*M_PI*i)/(_fftSize-1))+a2*cos((4*M_PI*i)/(_fftSize-1));
        }
        break;
    }

    case 7:                         // flat-top o malej rozdzielczosci
    {
        double a0 = 1;
        double a1 = 1.93;
        double a2 = 1.29;
        double a3 = 0.388;
        double a4 = 0.028;
        for(int i=0;i<_fftSize;i++){
            _fftWin[i] = a0-a1*cos((2*M_PI*i)/(_fftSize-1))+a2*cos((4*M_PI*i)/(_fftSize-1))-a3*cos((6*M_PI*i)/(_fftSize-1))+a4*cos((8*M_PI*i)/(_fftSize-1));
        }
        break;
    }
    default:
        break;
    }
}



void Spektogram::loadFile(){
    _file.seek(_file.headerLength());
    _numberChannels = _file.header.NumOfChan;
    _samplesPerSec = _file.header.SamplesPerSec;
    _bytesPerSec = _file.header.bytesPerSec;
    _blockAlign = _file.header.blockAlign;
    _bitsPerSample = _file.header.bitsPerSample;
    _soundLength = ((_file.size()-44)*1000)/_file.header.bytesPerSec;
    qDebug() << _file.header.AudioFormat << endl
             << _numberChannels << endl
             << _samplesPerSec << endl
             << _bytesPerSec << endl
             << _blockAlign << endl
             << _bitsPerSample << endl
             << _file.header.Subchunk2ID<< endl
             << _file.header.Subchunk2Size << endl
             << _file.bytesAvailable() << endl
             << _file.size();
    qDebug() << "Dlugosc:" << _soundLength;
        _Fs = _samplesPerSec/2;
        _tempMagn.resize(_Fs);
        _tempMagn.fill(1);
        readDataFile();
        calculateFFT();
        makePlot();
        setDesc();


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
    _magnitudes.clear();

    QByteArray buffer;
    _sampleData.resize(static_cast<int>(_file.bytesAvailable()/byteToRead));
    _sampleData.fill(1);

    _file.seek(_file.headerLength());
    for(int i = 0; !_file.atEnd(); i++){
        _file.read(buffer.data(),byteToRead);
        switch(byteToRead){
        case 1:
            quint8 *sample8;
            sample8 = reinterpret_cast<quint8*>(buffer.data());
            _sampleData[i]=*sample8;
            qDebug() << _sampleData[i];
            break;
        case 2:
            quint16 *sample16;
            sample16 = reinterpret_cast<quint16*>(buffer.data());
            _sampleData[i]=*sample16;
            break;
        case 3:
            quint32 *sample32;
            sample32 = reinterpret_cast<quint32*>(buffer.data());
            _sampleData[i]=*sample32;
            break;
        default:
            quint64 *sample64;
            sample64 = reinterpret_cast<quint64*>(buffer.data());
            _sampleData[i]=*sample64;
        }
    }
}

void Spektogram::calculateFFT(){

    _windowsX = 0;
    _magnitudes.clear();
    _fftData.clear();
    _fftData.resize(_fftSize);
    _fftData.fill(1);
    _magnitudeData.resize(_fftSize/2);
    _file.seek(_file.headerLength());
    QVector<qreal> sampleFFTData;
    qint32 fftSizeHalf = _fftSize/2;
    for(QVector<qreal>::iterator wsk = _sampleData.begin(); wsk != _sampleData.end();){
        _windowsX++;
        sampleFFTData.clear();
        sampleFFTData.resize(_fftSize);
        for(int i = 0; ((wsk != _sampleData.end() && i<_fftSize)); i++){
            sampleFFTData[i] = *wsk;
            wsk++;
        }

        for(int i=0;i<_fftSize; i++){
            _fftData[static_cast<uint>(i)].real(sampleFFTData[i]*_fftWin[i]);
            _fftData[static_cast<uint>(i)].imag(0);
        }

        _fftData=arma::fft(_fftData);

        for(int i=0;i<fftSizeHalf; i++){
            _magnitudeData[i]=abs(_fftData[static_cast<uint>(i)]);
        }


        double max=*std::max_element(_magnitudeData.begin(), _magnitudeData.end());

        for(int i=0;i<fftSizeHalf; i++){

            _magnitudeData[i]/=max;
            _magnitudeData[i]=20*log(_magnitudeData[i]);  //skala decybelowa
        }


        _magnitudes.append(_magnitudeData);

    }

    qDebug()<<2*_Fs;
    qDebug()<<_fftSize;
    qreal r_time = (2*_Fs)/static_cast<qreal>(_fftSize);
    qDebug()<<r_time<<" <- rozdzielczosc czasowa";
    qreal temp_liczba_okienY = (_Fs)/r_time;
    qDebug()<<temp_liczba_okienY<< " <- liczba okien double";
    _windowsY = static_cast<int>(temp_liczba_okienY);
    qDebug()<<_windowsY<< " <- liczba okien int";
}
void Spektogram::makePlot(){

    qDebug()<<"Liczba okien X "<<_windowsX;
    qDebug()<<"Czas trwania utowru "<<_soundLength;
    qDebug()<<"Liczba okien Y "<<_windowsY;
    qDebug()<<"Liczba probek "<<_Fs;
    qDebug()<<"Liczba okien X fft"<<_magnitudes.length();
    qDebug()<<"Liczba okien Y fft"<<_magnitudes.at(0).length();

    _colorMap->data()->setSize(_windowsX,_windowsY );
    _colorMap->data()->setRange(QCPRange(0, _soundLength), QCPRange(0, _Fs));
    _colorMap->setInterpolate(_interpol);

    double x,y;
    for (int xInd=0; xInd<_windowsX; ++xInd)
        for (int yInd=0; yInd<_windowsY; ++yInd){
            _colorMap->data()->cellToCoord(xInd, yInd, &x, &y);
            _colorMap->data()->setCell(xInd, yInd, _magnitudes.at(xInd).at(yInd));
        }

    _colorMap->setGradient(QCPColorGradient::gpPolar);
    _colorMap->rescaleDataRange(true);
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
}

void Spektogram::setDesc(){
    ui->numChan->setText(QString::number(_numberChannels) + " channel");
    ui->sampSec->setText(QString::number(_samplesPerSec) + " samples/sec");
    ui->bitsSamp->setText(QString::number(_bitsPerSample) + " bits/sample");
    ui->flength->setText(QString::number(_soundLength) + " ms");
}


void Spektogram::on_oknoComboBox_activated(int index)
{
    chooseWindow(index);
    calculateFFT();
    makePlot();
}

void Spektogram::on_fftComboBox_activated(const QString &arg1)
{
    _fftSize = arg1.toInt();
    chooseWindow(ui->oknoComboBox->currentIndex());
    calculateFFT();
    makePlot();
}


void Spektogram::on_true_radioButton_clicked()
{
    _interpol = true;
    makePlot();
}

void Spektogram::on_false_radioButton_clicked()
{
    _interpol = false;
    makePlot();
}

void Spektogram::on_actionNewFile_triggered()
{
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open WAV file"), "*.wav");

    if(_file.open(fileName)){
        loadFile();
    } else{
        QMessageBox::warning(this,"Błąd","Błąd odczytu pliku.");
    }
}
