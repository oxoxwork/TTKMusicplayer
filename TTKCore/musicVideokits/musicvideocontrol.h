#ifndef MUSICVIDEOCONTROL_H
#define MUSICVIDEOCONTROL_H

/* =================================================
 * This file is part of the TTK Music Player project
 * Copyright (c) 2014 - 2016 Greedysky Studio
 * All rights reserved!
 * Redistribution and use of the source code or any derivative
 * works are strictly forbiden.
   =================================================*/

#include <QLabel>
#include "musicobject.h"
#include "musicglobaldefine.h"

class QPushButton;
class MusicVolumePopWidget;
class MusicMovingLabelSlider;
class MusicVideoQualityPopWidget;
class MusicVideoBarrageStylePopWidget;
class MusicLocalSongSearchEdit;

/*! @brief The class of the video controller.
 * @author Greedysky <greedysky@163.com>
 */
class MUSIC_VIDEO_EXPORT MusicVideoControl : public QWidget
{
    Q_OBJECT
public:
    explicit MusicVideoControl(bool popup, QWidget *parent = 0);
    /*!
     * Object contsructor.
     */
    ~MusicVideoControl();

    static QString getClassName();
    /*!
     * Get class object name.
     */
    void setValue(qint64 position) const;
    /*!
     * Set current video play value.
     */
    void durationChanged(qint64 duration) const;
    /*!
     * Set current video duration.
     */
    void setButtonStyle(bool style) const;
    /*!
     * Set current button style.
     */
    void mediaChanged(const QString &url);
    /*!
     * Set current media url.
     */
    inline bool isPopup() const { return m_widgetPopup;}
    /*!
     * Check widget is popup or not.
     */
    void setFixedSize(int w, int h);
    /*!
     * Resize width bound by given width and height.
     */

Q_SIGNALS:
    void mvURLChanged(const QString &data);
    /*!
     * Set current media url by selected quality.
     */
    void sliderValueChanged(int value);
    /*!
     * Slider value changed at value.
     */

    void pushBarrageChanged(bool on);
    /*!
     * Open barrage on or not.
     */
    void addBarrageChanged(const QString &string);
    /*!
     * Add barrage text to mv.
     */

public Q_SLOTS:
    void show();
    /*!
     * Override show function.
     */
    void pushBarrageClicked();
    /*!
     * Push barrage clicked.
     */

protected:
    QWidget *createVideoBarrageWidget();
    /*!
     * Create video barrage widget.
     */

    bool m_widgetPopup;
    QLabel *m_timeLabel;
    QPushButton *m_playButton;
    MusicMovingLabelSlider *m_timeSlider;
    MusicVolumePopWidget *m_volumeButton;
    MusicVideoQualityPopWidget *m_qualityButton;

    bool m_pushBarrageOn;
    QPushButton *m_pushBarrage, *m_barrageSend;
    MusicVideoBarrageStylePopWidget *m_menuBarrage;
    MusicLocalSongSearchEdit *m_lineEditBarrage;

};

#endif // MUSICVIDEOCONTROL_H
