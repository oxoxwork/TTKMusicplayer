#ifndef MUSICSONGSEARCHONLINEWIDGET_H
#define MUSICSONGSEARCHONLINEWIDGET_H

#include "core/musicdownloadquerythread.h"
#include "core/musictablewidgetabstract.h"

class MusicDataDownloadThread;
class MusicTextDownLoadThread;

class MUSIC_EXPORT MusicSongSearchOnlineWidget : public MusicTableWidgetAbstract
{
    Q_OBJECT
public:
    explicit MusicSongSearchOnlineWidget(QWidget *parent = 0);
    virtual ~MusicSongSearchOnlineWidget();

    void startSearchSong(const QString&);
    void addSearchMusicToPlayList(int);
    void musicDownloadLocal(int);

signals:
    void musicBgDownloadFinished();
    void showDownLoadInfoFinished(const QString&);
    void MuiscSongToPlayListChanged(const QString& name);
    void showDownLoadInfoFor(DownLoadType);

public slots:
    void listCellClicked(int,int);
    void clearAllItems();
    void creatSearchedItems(const QString&, const QString&, const QString&);
    void itemDoubleClicked(int row, int column);

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
    MusicDownLoadQueryThread* m_downLoadManager;

};

#endif // MUSICSONGSEARCHONLINEWIDGET_H
