#include "spektogram.h"
#include "ui_spektogram.h"

Spektogram::Spektogram(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Spektogram)
{
    ui->setupUi(this);
    const QString dir;
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open WAV file"), "*.wav");
    WavFile file;
    if(file.open(fileName)){
        file.seek(file.headerLength());
        qDebug()<<file.lengthData<<" "<<file.channelsNumber;
        //file.read(reinterpret_cast<char*>(dataPtr),fileWav.Subchunk2Size);
    }
}

Spektogram::~Spektogram()
{
    delete ui;
}

