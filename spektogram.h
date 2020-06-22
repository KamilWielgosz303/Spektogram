#ifndef SPEKTOGRAM_H
#define SPEKTOGRAM_H

#include <QMainWindow>
#include <QFileDialog>
#include <QObject>
#include <QDebug>
#include <QtMath>
#include <QMessageBox>
#include <armadillo>
#include "wavfile.h"
#include <QPainter>
#include "qcustomplot.h"

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

    //void on_dial_valueChanged(int value);
    void on_actionNewFile_triggered();

    void on_actionDrawSpectogram_triggered();

    void on_fftComboBox_activated(const QString &arg1);

    void on_fftComboBox_activated(int index);

    void on_oknoComboBox_activated(int index);

    void on_fftComboBox_currentIndexChanged(const QString &arg1);

    void on_true_radioButton_clicked();

    void on_false_radioButton_clicked();

private:
    Ui::Spektogram *ui;
    WavFile file;
    arma::cx_vec fftData;
    QVector<qreal> magnitudeData;
    QVector<qreal> tempMagn;
    QVector<qreal> phaseData;
    QVector<qreal> fftWin;
    QVector<QVector<qreal>> magnitudes;
    qint32 _Fs;                                         //Liczba próbek pliku wav
    int _windowsY;                             //Dokladnosc w Hz , z jaka mozemy wyswietlac punkty spektogramu
    int _windowsX{0};                               //Wynik dzielenia całego pliku na mniejsze okna
    qint64 soundLength;
    quint16 _numberChannels;
    quint32 _samplesPerSec;
    quint32 _bytesPerSec;
    quint16 _blockAlign;
    quint16 _bitsPerSample;

    bool interpol;

    int _fftSize;                                //Przy czestotliwosci 8000 rozdzielczosc czestotliwosciowa = 15,625 Hz        (8000/512)
    void loadFile();
    void calculateFFT();
    void makePlot();
    void chooseWindow(int i);

};
#endif // SPEKTOGRAM_H
