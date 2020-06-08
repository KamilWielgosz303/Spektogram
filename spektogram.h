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

private:
    Ui::Spektogram *ui;
    arma::cx_vec fftData;
    QVector<double> magnitudeData;
    QVector<double> phaseData;
    QVector<double> fftWin;
    void paintEvent(QPaintEvent *event);
    QVector<double> tempPlot;

    Chart chart;

};
#endif // SPEKTOGRAM_H
