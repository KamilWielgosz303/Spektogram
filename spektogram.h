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

    void on_actionNewFile_triggered();

    void on_actionDrawSpectogram_triggered();

    void on_fftComboBox_activated(const QString &arg1);

    void on_oknoComboBox_activated(int index);

    void on_true_radioButton_clicked();

    void on_false_radioButton_clicked();

private:
    Ui::Spektogram *ui;
    WavFile _file;
    arma::cx_vec _fftData;
    QVector<qreal> _magnitudeData;
    QVector<qreal> _tempMagn;
    QVector<qreal> _fftWin;
    QVector<QVector<qreal>> _magnitudes;
    qint32 _Fs;                                         //Liczba próbek pliku wav
    int _windowsY;                                  //Dokladnosc w Hz , z jaka mozemy wyswietlac punkty spektogramu
    int _windowsX{0};                               //Podział osi X
    qint64 _soundLength;
    quint16 _numberChannels;
    quint32 _samplesPerSec;
    quint32 _bytesPerSec;
    quint16 _blockAlign;
    quint16 _bitsPerSample;
    QVector<qreal> _sampleData;

    bool _interpol{false};
    QCPColorMap *_colorMap;
    QCPColorScale *_amplitudy;
    int _fftSize;                                //Przy czestotliwosci 8000 rozdzielczosc czestotliwosciowa = 15,625 Hz        (8000/512)
    void loadFile();
    void calculateFFT();
    void makePlot();
    void chooseWindow(int i);
    void readDataFile();
    void setDesc();

};
#endif // SPEKTOGRAM_H
