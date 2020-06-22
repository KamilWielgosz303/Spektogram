#ifndef SPEKTOGRAM_H
#define SPEKTOGRAM_H

#include <QMainWindow>
#include <QFileDialog>
#include <QObject>
#include <QDebug>
#include <armadillo>
#include "wavfile.h"
#include <QPainter>
#include <chart.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Spektogram; }
QT_END_NAMESPACE

class Spektogram : public QMainWindow
{
    Q_OBJECT

public:
    Spektogram(QWidget *parent = nullptr);
    ~Spektogram();

private slots:
    void on_actiondrawSpect_triggered();

    void makePlot();
    void chooseWindow(int i);
    void on_verticalSlider_valueChanged(int value);

    //void on_dial_valueChanged(int value);

    void on_pushButton_clicked();

    void on_actionNewFile_triggered();

private:
    Ui::Spektogram *ui;
    WavFile file;
    arma::cx_vec fftData;
    QVector<double> magnitudeData;
    QVector<double> tempMagn;
    QVector<double> phaseData;
    QVector<double> fftWin;
    QVector<QVector<double>> magnitudes;
    Chart chart;
    qint32 _Fs;                                         //Liczba próbek pliku wav
    quint32 liczba_okienY;                             //Dokladnosc w Hz , z jaka mozemy wyswietlac punkty spektogramu
    quint32 liczba_okienX;                               //Wynik dzielenia całego pliku na mniejsze okna
    qint64 soundLength;
    quint16 _numberChannels;
    quint32 _samplesPerSec;
    quint32 _bytesPerSec;
    quint16 _blockAlign;
    quint16 _bitsPerSample;
    void loadFile();
};
#endif // SPEKTOGRAM_H
