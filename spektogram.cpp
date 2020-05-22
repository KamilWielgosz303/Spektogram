#include "spektogram.h"
#include "ui_spektogram.h"

Spektogram::Spektogram(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Spektogram)
{
    ui->setupUi(this);
}

Spektogram::~Spektogram()
{
    delete ui;
}

