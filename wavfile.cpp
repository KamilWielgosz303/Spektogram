#include "wavfile.h"



WavFile::WavFile(QObject *parent)
    : QFile(parent)
    , m_headerLength(0)
{

}

bool WavFile::open(const QString &fileName)
{
    close();
    setFileName(fileName);
    return QFile::open(QIODevice::ReadOnly) && readHeader();
}

const QAudioFormat &WavFile::fileFormat() const
{
    return m_fileFormat;
}

qint64 WavFile::headerLength() const
{
return m_headerLength;
}

bool WavFile::readHeader()
{
    seek(0);
    bool result = read(reinterpret_cast<char *>(&header), sizeof(WAV_HEADER)) == sizeof(WAV_HEADER);
    if (result) {
        if ((memcmp(&header.RIFF, "RIFF", 4) == 0
            || memcmp(&header.RIFF, "RIFX", 4) == 0)
            && memcmp(&header.WAVE, "WAVE", 4) == 0
            && memcmp(&header.fmt, "fmt ", 4) == 0
            && (header.AudioFormat == 1 || header.AudioFormat == 0)) {
            if (memcmp(&header.RIFF, "RIFF", 4) == 0)
                m_fileFormat.setByteOrder(QAudioFormat::LittleEndian);
            else
                m_fileFormat.setByteOrder(QAudioFormat::BigEndian);

            int bps = qFromLittleEndian<quint16>(header.bitsPerSample);
            m_fileFormat.setChannelCount(qFromLittleEndian<quint16>(header.NumOfChan));
            m_fileFormat.setCodec("audio/pcm");
            m_fileFormat.setSampleRate(qFromLittleEndian<quint32>(header.SamplesPerSec));
            m_fileFormat.setSampleSize(qFromLittleEndian<quint16>(header.bitsPerSample));
            m_fileFormat.setSampleType(bps == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);
        } else {
            result = false;
        }
    }
    m_headerLength = pos();
    return result;
}
