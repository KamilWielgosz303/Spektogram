#ifndef SPEKTOGRAM_H
#define SPEKTOGRAM_H

#include <QMainWindow>
#include <QFileDialog>
#include <QObject>
#include <QDebug>
#include <armadillo>
#include "wavfile.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Spektogram; }
QT_END_NAMESPACE

class Spektogram : public QMainWindow
{
    Q_OBJECT

public:
    Spektogram(QWidget *parent = nullptr);
    ~Spektogram();

private:
    Ui::Spektogram *ui;
    arma::cx_vec fftData;
    QVector<double> magnitudeData;
    QVector<double> phaseData;
    QVector<double> fftWin;

};
#endif // SPEKTOGRAM_H
