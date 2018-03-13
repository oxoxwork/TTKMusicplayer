#include "musicwebmusicradioplaywidget.h"
#include "ui_musicwebmusicradioplaywidget.h"
#include "musiccoremplayer.h"
#include "musicuiobject.h"
#include "musicradioplaylistthread.h"
#include "musicradiosongsthread.h"
#include "musictextdownloadthread.h"
#include "musicdatadownloadthread.h"
#include "musiclrcanalysis.h"
#include "musictime.h"
#include "musiccoreutils.h"
#include "musicwidgetutils.h"
#include "musicfunctionuiobject.h"
#include "musicdownloadwidget.h"

MusicWebMusicRadioPlayWidget::MusicWebMusicRadioPlayWidget(QWidget *parent)
    : MusicAbstractMoveWidget(parent),
      m_ui(new Ui::MusicWebMusicRadioPlayWidget), m_playListThread(nullptr),
      m_songsThread(nullptr)
{
    m_ui->setupUi(this);

    m_currentPlayListIndex = 0;
    m_isPlaying = false;
    m_analysis = new MusicLrcAnalysis(this);
    m_analysis->setLineMax(9);

    m_autoNextTimer.setInterval(3*MT_S2MS);
    connect(&m_autoNextTimer, SIGNAL(timeout()), SLOT(radioNext()));
    connect(m_ui->volumeSlider, SIGNAL(sliderMoved(int)), &m_autoNextTimer, SLOT(stop()));
    connect(m_ui->volumeSlider, SIGNAL(sliderReleased()), &m_autoNextTimer, SLOT(start()));

    m_ui->topTitleCloseButton->setIcon(QIcon(":/functions/btn_close_hover"));
    m_ui->topTitleCloseButton->setStyleSheet(MusicUIObject::MToolButtonStyle04);
    m_ui->topTitleCloseButton->setCursor(QCursor(Qt::PointingHandCursor));
    m_ui->topTitleCloseButton->setToolTip(tr("Close"));
    connect(m_ui->topTitleCloseButton, SIGNAL(clicked()), SLOT(close()));

    m_ui->playButton->setIcon(QIcon(":/functions/btn_pause_hover"));
    m_ui->previousButton->setIcon(QIcon(":/functions/btn_previous_hover"));
    m_ui->nextButton->setIcon(QIcon(":/functions/btn_next_hover"));
    m_ui->downloadButton->setStyleSheet(MusicUIObject::MKGBtnUnDownload);
    m_ui->shareButton->setStyleSheet(MusicUIObject::MKGBtnMore);

    m_ui->playButton->setStyleSheet(MusicUIObject::MBackgroundStyle01);
    m_ui->previousButton->setStyleSheet(MusicUIObject::MBackgroundStyle01);
    m_ui->nextButton->setStyleSheet(MusicUIObject::MBackgroundStyle01);

#ifdef Q_OS_UNIX
    m_ui->playButton->setFocusPolicy(Qt::NoFocus);
    m_ui->previousButton->setFocusPolicy(Qt::NoFocus);
    m_ui->nextButton->setFocusPolicy(Qt::NoFocus);
#endif

    m_ui->playButton->setIconSize(QSize(31, 31));
    m_ui->previousButton->setIconSize(QSize(31, 31));
    m_ui->nextButton->setIconSize(QSize(31, 31));

    m_ui->playButton->setCursor(QCursor(Qt::PointingHandCursor));
    m_ui->previousButton->setCursor(QCursor(Qt::PointingHandCursor));
    m_ui->nextButton->setCursor(QCursor(Qt::PointingHandCursor));
    m_ui->downloadButton->setCursor(QCursor(Qt::PointingHandCursor));
    m_ui->shareButton->setCursor(QCursor(Qt::PointingHandCursor));

    m_ui->volumeSlider->setStyleSheet(MusicUIObject::MSliderStyle01);
    m_ui->volumeSlider->setRange(0, 100);
    m_ui->volumeSlider->setValue(100);

    createCoreModule();

    connect(m_ui->playButton, SIGNAL(clicked()), SLOT(radioPlay()));
    connect(m_ui->previousButton, SIGNAL(clicked()), SLOT(radioPrevious()));
    connect(m_ui->nextButton, SIGNAL(clicked()), SLOT(radioNext()));
    connect(m_ui->downloadButton, SIGNAL(clicked()), SLOT(radioResourceDownload()));
    connect(m_ui->volumeSlider, SIGNAL(valueChanged(int)), SLOT(radioVolume(int)));
}

MusicWebMusicRadioPlayWidget::~MusicWebMusicRadioPlayWidget()
{
    m_autoNextTimer.stop();
    delete m_analysis;
    delete m_radio;
    delete m_songsThread;
    delete m_playListThread;
    delete m_ui;
}

QString MusicWebMusicRadioPlayWidget::getClassName()
{
    return staticMetaObject.className();
}

void MusicWebMusicRadioPlayWidget::closeEvent(QCloseEvent *event)
{
    m_autoNextTimer.stop();
    delete m_radio;
    m_radio = nullptr;
    QWidget::closeEvent(event);
}

void MusicWebMusicRadioPlayWidget::setNetworkCookie(QNetworkCookieJar *jar)
{
    if(m_songsThread == nullptr || m_playListThread == nullptr)
    {
        m_playListThread = new MusicRadioPlayListThread(this, jar);
        connect(m_playListThread, SIGNAL(downLoadDataChanged(QString)), SLOT(getPlayListFinished()));
        m_songsThread = new MusicRadioSongsThread(this, jar);
        connect(m_songsThread, SIGNAL(downLoadDataChanged(QString)), SLOT(getSongInfoFinished()));
    }
}

void MusicWebMusicRadioPlayWidget::updateRadioList(const QString &category)
{
    if(m_playListThread)
    {
        m_playListThread->startToDownload(category);
    }
}

void MusicWebMusicRadioPlayWidget::radioPlay()
{
    if(!m_radio)
    {
        return;
    }

    m_isPlaying = !m_isPlaying;
    m_ui->playButton->setIcon(QIcon(m_isPlaying ? ":/functions/btn_pause_hover" : ":/functions/btn_play_hover"));
    m_radio->play();
}

void MusicWebMusicRadioPlayWidget::radioPrevious()
{
    if(m_playListIds.isEmpty())
    {
        return;
    }

    --m_currentPlayListIndex;
    if(m_currentPlayListIndex > m_playListIds.count())
    {
        m_currentPlayListIndex = m_playListIds.count() - 1;
    }
    else if(m_currentPlayListIndex < 0)
    {
        m_currentPlayListIndex = 0;
    }

    m_songsThread->startToDownload(m_playListIds[m_currentPlayListIndex]);

    if(!m_isPlaying)
    {
        m_ui->playButton->setIcon(QIcon(":/functions/btn_pause_hover"));
    }
}

void MusicWebMusicRadioPlayWidget::radioNext()
{
    if(m_playListIds.isEmpty())
    {
        return;
    }

    ++m_currentPlayListIndex;
    if(m_currentPlayListIndex > m_playListIds.count())
    {
        m_currentPlayListIndex = m_playListIds.count() - 1;
    }
    else if(m_currentPlayListIndex < 0)
    {
        m_currentPlayListIndex = 0;
    }

    m_songsThread->startToDownload(m_playListIds[m_currentPlayListIndex]);

    if(!m_isPlaying)
    {
        m_ui->playButton->setIcon(QIcon(":/functions/btn_pause_hover"));
    }
}

void MusicWebMusicRadioPlayWidget::radioVolume(int num)
{
    if(m_radio)
    {
        m_radio->setVolume(num);
    }
}

void MusicWebMusicRadioPlayWidget::radioResourceDownload()
{
    MusicObject::MusicSongInformation info;
    if(m_songsThread)
    {
        info = m_songsThread->getMusicSongInfo();
    }

    if(info.m_songAttrs.isEmpty())
    {
        return;
    }

    MusicDownloadWidget *download = new MusicDownloadWidget(this);
    download->setSongName(info, MusicDownLoadQueryThreadAbstract::MusicQuery);
    download->show();
}

void MusicWebMusicRadioPlayWidget::getPlayListFinished()
{
    m_playListIds = m_playListThread->getMusicPlayList();
    m_currentPlayListIndex = 0;
    if(m_songsThread && !m_playListIds.isEmpty())
    {
        m_songsThread->startToDownload(m_playListIds.first());
    }
}

void MusicWebMusicRadioPlayWidget::getSongInfoFinished()
{
    m_isPlaying = true;
    startToPlay();
}

void MusicWebMusicRadioPlayWidget::createCoreModule()
{
    m_radio = new MusicCoreMPlayer(this);
    connect(m_radio, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
    connect(m_radio, SIGNAL(durationChanged(qint64)), SLOT(durationChanged(qint64)));
}

void MusicWebMusicRadioPlayWidget::startToPlay()
{
    MusicObject::MusicSongInformation info;
    if(m_songsThread)
    {
        info = m_songsThread->getMusicSongInfo();
    }

    if(info.m_songAttrs.isEmpty())
    {
        return;
    }

    if(!m_radio)
    {
        createCoreModule();
    }
    m_radio->setMedia(MusicCoreMPlayer::MusicCategory, info.m_songAttrs.first().m_url);
    m_radio->play();

    /// fix current play volume temporary
    int v = m_ui->volumeSlider->value();
    m_ui->volumeSlider->setValue(0);
    m_ui->volumeSlider->setValue(v);

    QString name = MusicUtils::Core::lrcPrefix() + info.m_singerName + " - " + info.m_songName + LRC_FILE;
    if(!QFile::exists(name))
    {
        MusicTextDownLoadThread* lrcDownload = new MusicTextDownLoadThread(info.m_lrcUrl, name,
                                 MusicDownLoadThreadAbstract::Download_Lrc, this);
        connect(lrcDownload, SIGNAL(downLoadDataChanged(QString)), SLOT(lrcDownloadStateChanged()));
        lrcDownload->startToDownload();
    }
    else
    {
        lrcDownloadStateChanged();
    }

    name = ART_DIR_FULL + info.m_singerName + SKN_FILE;
    if(!QFile::exists(name))
    {
        MusicDataDownloadThread *picDwonload = new MusicDataDownloadThread(info.m_smallPicUrl, name,
                                 MusicDownLoadThreadAbstract::Download_SmlBG, this);
        connect(picDwonload, SIGNAL(downLoadDataChanged(QString)), SLOT(picDownloadStateChanged()));
        picDwonload->startToDownload();
    }
    else
    {
        picDownloadStateChanged();
    }
}

void MusicWebMusicRadioPlayWidget::lrcDownloadStateChanged()
{
    MusicObject::MusicSongInformation info;
    if(m_songsThread)
    {
        info = m_songsThread->getMusicSongInfo();
    }

    if(info.m_songAttrs.isEmpty())
    {
        return;
    }

    QString name = info.m_singerName + " - " + info.m_songName;
    name = name.trimmed();
    m_ui->titleWidget->setText(name);
    m_analysis->transLrcFileToTime(MusicUtils::Core::lrcPrefix() + name + LRC_FILE);
}

void MusicWebMusicRadioPlayWidget::picDownloadStateChanged()
{
    MusicObject::MusicSongInformation info;
    if(m_songsThread)
    {
        info = m_songsThread->getMusicSongInfo();
    }

    if(info.m_songAttrs.isEmpty())
    {
        return;
    }

    QPixmap pix(ART_DIR_FULL + info.m_singerName + SKN_FILE);
    if(pix.isNull())
    {
        pix.load(":/image/lb_defaultArt");
    }
    pix = MusicUtils::Widget::pixmapToRound(pix, QSize(150, 150), 100, 100);
    m_ui->artistLabel->setPixmap(pix);
    m_ui->artistLabel->start();
}

void MusicWebMusicRadioPlayWidget::positionChanged(qint64 position)
{
    if(!m_radio)
    {
        return;
    }

    m_autoNextTimer.stop();
    m_autoNextTimer.start();
    m_ui->positionLabel->setText(QString("%1").arg(MusicTime::msecTime2LabelJustified(position*MT_S2MS)));

    if(m_analysis->isEmpty())
    {
        QString lrc = QString("<p style='font-weight:600;' align='center'>%1</p>").arg(tr("unFoundLrc"));
        m_ui->lrcLabel->setText(lrc);
        return;
    }

    int index = m_analysis->getCurrentIndex();
    qint64 time = m_analysis->findTime(index);

    if(time < position*MT_S2MS && time != -1)
    {
        QString lrc;
        for(int i=0; i<m_analysis->getLineMax(); ++i)
        {
            if(i == m_analysis->getMiddle())
            {
                lrc += QString("<p style='font-weight:600;' align='center'>");
            }
            else
            {
                lrc += QString("<p align='center'>");
            }
            lrc += m_analysis->getText(i);
            lrc += QString("</p>");
        }
        m_ui->lrcLabel->setText(lrc);
        m_analysis->setCurrentIndex(++index);
    }
}

void MusicWebMusicRadioPlayWidget::durationChanged(qint64 duration)
{
    if(!m_radio)
    {
        return;
    }
    m_ui->durationLabel->setText(QString("/%1").arg(MusicTime::msecTime2LabelJustified(duration*MT_S2MS)));
}

void MusicWebMusicRadioPlayWidget::show()
{
    setBackgroundPixmap(m_ui->background, size());
    MusicAbstractMoveWidget::show();
}