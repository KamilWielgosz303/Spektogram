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
private:
    Ui::Spektogram *ui;
    arma::cx_vec fftData;
    QVector<double> magnitudeData;
    QVector<double> tempMagn;
    QVector<double> phaseData;
    QVector<double> fftWin;
    void paintEvent(QPaintEvent *event);
    QVector<QVector<double>> magnitudes;
    Chart chart;
    int Fs;                                         //Liczba próbek pliku wav
    int liczba_okienY;                             //Dokladnosc w Hz , z jaka mozemy wyswietlac punkty spektogramu
    int liczba_okienX;                               //Wynik dzielenia całego pliku na mniejsze okna
    int czas;                                   //Pelny czas trwania utworu

    void drawSpektogram(int Fs,int timeWindows);
    int takeRightFreq(int freq,int fftsize,int F_s);

};
#endif // SPEKTOGRAM_H
