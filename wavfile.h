#ifndef WAVFILE_H
#define WAVFILE_H

#include <QObject>
#include <QFile>
#include <QAudioFormat>
#include <QtEndian>

class WavFile : public QFile
{
public:
    WavFile(QObject *parent = nullptr);

    using QFile::open;
    bool open(const QString &fileName);
    const QAudioFormat &fileFormat() const;
    qint64 headerLength() const;
    quint64 lengthData;
    quint16 channelsNumber;

private:
    bool readHeader();

private:
    QAudioFormat m_fileFormat;
    qint64 m_headerLength;
};

#endif // WAVFILE_H
