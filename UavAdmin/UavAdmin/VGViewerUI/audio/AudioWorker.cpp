#include "AudioWorker.h"

static QString sStrNum ;

AudioWorker::AudioWorker(QObject *parent) : QObject(parent)
,m_tts(new QTextToSpeech)
{
    sStrNum = tr("0123456789");//"0一二三四五六七八九"
    if(m_tts)
        connect(m_tts, &QTextToSpeech::stateChanged, this, &AudioWorker::onStateChanged);

    connect(this, &AudioWorker::begingSay, this, &AudioWorker::onPrcsSayQue);
}

AudioWorker::~AudioWorker()
{
    m_tts->stop();
    delete m_tts;
}

void AudioWorker::say(const QString &value, const QStringList &param)
{
    emit begingSay(value, param);
}

int AudioWorker::countVoiceQue()
{
    return m_queAudioLoop.size();
}

void AudioWorker::onStateChanged(QTextToSpeech::State state)
{
    if (state == QTextToSpeech::Ready)
        _speak();
}

void AudioWorker::onPrcsSayQue(const QString &value, const QStringList &params)
{
    _addAudioItem(value, params);
    _speak();
}

AudioWorker::AudioItem *AudioWorker::_getAudioItem(const QString &value)
{
    for (AudioItem &item : m_queAudioLoop)
    {
        if (item.first == value)
            return &item;
    }

    return NULL;
}

void AudioWorker::_addAudioItem(const QString &value, const QStringList &params)
{
    if (params.count() == 0)
    {
        if (!m_queAudioMed.contains(value))
            m_queAudioMed.append(value);
        return;
    }

    if (AudioItem *item = _getAudioItem(value))
        item->second = params;
    else
        m_queAudioLoop.append(AudioItem(value, params));
}

void AudioWorker::_speak()
{
    if (m_tts && m_tts->state() == QTextToSpeech::Ready)
    {
        if (m_queAudioMed.count() > 0)
        {
            m_tts->say(_genSpeakString(m_queAudioMed.takeFirst()));
            return;
        }

        if(m_queAudioLoop.count()>0)
        {
            AudioItem item = m_queAudioLoop.takeFirst();
            QString strSpeak = _genSpeakString(item.first);
            for (const QString &itr : item.second)
            {
                strSpeak = strSpeak.arg(_genSpeakString(itr, true));
            }
            m_tts->say(strSpeak);
        }
    }
}

QString AudioWorker::_genSpeakString(const QString &str, bool bParam)
{
    QString ret = str;
    if (bParam)
    {
        if (str.front() == '+')
            ret.remove(0, 1);
        if (str.front() == '-')
            ret.replace(0, 1, tr("negative"));//"负"
        int idx = ret.indexOf(".");
        if (idx>0)
        {
            ret.replace(idx, 1, tr("dot"));//"点"
            for (int i = idx + 1; i < ret.length(); ++i)
            {
                QChar c = ret.at(i);
                if (c >= '0' &&  c <= '9')
                    ret.replace(i, 1, sStrNum.at(c.toLatin1() - '0'));
                else
                    break;
            }
        }
    }
    else
    {
        int idx = str.indexOf('-');
        int count = 1;
        if (idx > 0 && idx+1<str.length())
        {
            if (str.at(idx+1) == '-')
                count = 2;

            if (str.left(idx).lastIndexOf(QRegExp("\\d")) + 1 == idx && str.mid(idx + count).indexOf(QRegExp("\\d")) == 0)
                ret.replace(idx, count, tr("To"));//"至"
        }
        ret.replace(tr("magic correct 1"), tr("magic correct 2"));//校磁;较磁
        ret.replace("\r\n", "");
    }

    return ret;
}

