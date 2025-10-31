#include "playwav.h"

PlayWav *PlayWav::playWav = 0;
void PlayWav::play(const QString &fileName, int playCount)
{
    //没有实例化过则先实例化
    if (playWav == 0) {
        playWav = new PlayWav;
    }

    playWav->play1(fileName, playCount);
}

void PlayWav::stop()
{
    //没有实例化过则先实例化
    if (playWav == 0) {
        playWav = new PlayWav;
    }

    playWav->stop1();
}

PlayWav::PlayWav(QObject *parent) : QObject(parent)
{
    playCount = 1;
    isPlayCount = 0;

    //启动定时器循环播放
    timer = new QTimer(this);
    timer->setInterval(10 * 1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(play()));

    //嵌入式板子上一般QSound类歇菜,需要用执行命令去播放声音文件
#ifdef __arm__
    process = new QProcess(this);
#else
    player = 0;
#endif
}

void PlayWav::play()
{
    if (isPlayCount > playCount) {
        isPlayCount = 0;
        timer->stop();
        return;
    }

    isPlayCount++;
#ifdef __arm__
    if (process->state() == QProcess::NotRunning) {
#ifdef arma9
        process->start(QString("aplay %1").arg(fileName));
#else
        process->start(QString("mpv %1").arg(fileName));
#endif
    }
#endif
}

void PlayWav::play1(const QString &fileName, int playCount)
{
    this->fileName = fileName;
    this->playCount = playCount;
    this->isPlayCount = 0;

#ifdef __arm__
    play();
    if (playCount > 1) {
        timer->start();
    }
#else
#ifdef playsound
    if (player != 0) {
        player->deleteLater();
        player = 0;
    }

    player = new QSoundEffect(this);
    player->setSource(QUrl::fromLocalFile(fileName));
    player->setLoopCount(playCount);
    player->play();
#endif
#endif
}

void PlayWav::stop1()
{
    if (this->fileName.isEmpty() || this->fileName.contains("无")) {
        return;
    }
    isPlayCount = 0;
    timer->stop();

#ifdef __arm__
    process->close();
#else
#ifdef playsound
    player->stop();
#endif
#endif
}
