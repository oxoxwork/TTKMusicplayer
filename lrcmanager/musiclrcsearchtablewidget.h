#ifndef MUSICLRCSEARCHTABLEWIDGET_H
#define MUSICLRCSEARCHTABLEWIDGET_H

#include "core/musicdownloadquerythread.h"
#include "core/musictablewidgetabstract.h"

class MUSIC_EXPORT MusicLrcSearchTableWidget : public MusicTableWidgetAbstract
{
    Q_OBJECT
public:
    explicit MusicLrcSearchTableWidget(QWidget *parent = 0);
    virtual ~MusicLrcSearchTableWidget();

    void startSearchLrc(const QString&);
    void musicDownloadLocal(int);
    inline void setCurrentSongName(const QString& name){ m_currentSongName = name;}

signals:
    void lrcDownloadStateChanged(const QString&);

public slots:
    void clearAllItems();
    void creatSearchedItems(const QString&, const QString&, const QString&);
    void itemDoubleClicked(int row, int column);

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
    MusicDownLoadQueryThread* m_downLoadManager;
    QString m_currentSongName;

};

#endif // MUSICLRCSEARCHTABLEWIDGET_H
