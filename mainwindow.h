#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QMainWindow>
#include <QtMultimedia>
#include <QInputDialog>
#include <QAudioOutput>
#include <QStandardPaths>
#include <QStringListModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QTime>
#include <QChar>
#include <QStyleFactory>
#include <QStyle>
#include <QBrush>
#include <QPen>
#include <QAudioInput>
#include <QAudioDevice>

#include <iostream>
#include <string>
#include <vector>

#include "playlist.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void loadPlaylist();
    void setNextTrack();

    Playlist *findPlaylist(QString &);
    Track *getTrack(Playlist *, QString);
    Track *getNextTrack(Playlist *, QString);
    Track *getPrevTrack(Playlist *, QString);
    void DisplayPlaylistTrack(Playlist &);
    std::vector<Playlist>::iterator getPlaylistIterator(Playlist*);
    void deleteFolder(QString path);
    void deleteFile(QString path);
    void setIcons();
    void InitializeGraphics();
    void DrawEqualizer(int);
    void InitializeEqualizer();


private slots:
    //SLIDERS
    void VolumeSliderChangeValue(int);
    void TimeSliderMove(int);
    void TimeSliderChangeValue(int);
    void setTrackTime();

    //PLAYLIST BUTTONS
    void ButtonNewPlaylist();
    void ButtonRemovePlaylist();
    void ButtonPlay();
    void ButtonStop();
    void ButtonAddTrack();
    void ButtonRemoveTrack();
    void ButtonNextTrack();
    void ButtonPrevTrack();

    //MEDIA BUTTONS
    void ButtonStart();
    void ButtonVolume();
    void ButtonForward();
    void ButtonBackward();

    //LIST WIDGET
    void PlaylistList(QModelIndex);
    void TrackList(QModelIndex);

    //MEDIA PLAYER
    void MediaPlayerPositionChange(qint64);
    void MediaPlayerTrackChange(bool);
    void MediaStatus(QMediaPlayer::PlaybackState);
    //void MediaTest();


    //OTHER
    void scrollText();
    void dec1();
    void dec2();
private:
    Ui::MainWindow *ui;

    std::vector<Playlist> m_playlist;

    QMediaPlayer *m_mediaPlayer;

    QAudioOutput *m_audio;

    QAudioDecoder m_decoder;

    QAudioDevice m_device;

    QTimer *m_timer;

    QGraphicsScene *m_scene;
    QGraphicsTextItem *m_text1;
    QGraphicsTextItem *m_text2;
    QGraphicsTextItem *m_text_time;
    QGraphicsTextItem *m_text_mute;
    QGraphicsTextItem *m_text_volume;
    QTextDocument *m_document;
    qreal textX;
    bool running_text1{true};
    bool running_text2{false};
    QFont *m_font;
    QPainter *m_painter;

    QGraphicsScene *m_scene_equalizer;
    std::vector<QGraphicsRectItem *> m_rect;

    QDir m_dir;

    bool slider_clicked{false};
    bool slider_changing{false};
    bool slider_forward{false};
    bool slider_backward{false};
    bool button_play{false};


    QString selected_playlist{""};
    QString playing_playlist{""};
    QString playing_track{""};
};
#endif // MAINWINDOW_H
