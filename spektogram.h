#ifndef SPEKTOGRAM_H
#define SPEKTOGRAM_H

#include <QMainWindow>

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
};
#endif // SPEKTOGRAM_H
