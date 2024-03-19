#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("background-color: black;");
    this->m_timer = new QTimer(this);

    //Initialize Audio
    this->m_audio = new QAudioOutput{this};

    //Initialize QMediaPlayer
    this->m_mediaPlayer = new QMediaPlayer{this};

    //Setting audio
    this->m_audio->setVolume(0.5f);
    this->m_mediaPlayer->setAudioOutput(this->m_audio);

    //SLOTS AND SIGNALS

    //BUTTONS
    connect(ui->m_playButton, SIGNAL(clicked()), this, SLOT(ButtonPlay()));
    connect(ui->m_stopButton, SIGNAL(clicked()), this, SLOT(ButtonStop()));
    connect(ui->m_newPlayListButton, SIGNAL(clicked()), this, SLOT(ButtonNewPlaylist()));
    connect(ui->m_addTrackButton, SIGNAL(clicked()), this, SLOT(ButtonAddTrack()));
    connect(ui->m_removePlaylistButton, SIGNAL(clicked()), this, SLOT(ButtonRemovePlaylist()));
    connect(ui->m_removeTrackButton, SIGNAL(clicked()), this, SLOT(ButtonRemoveTrack()));
    connect(ui->m_nextButton, SIGNAL(clicked()), this, SLOT(ButtonNextTrack()));
    connect(ui->m_prevButton, SIGNAL(clicked()), this, SLOT(ButtonPrevTrack()));

    //MEDIA BUTTONS
    connect(ui->m_start, SIGNAL(clicked()), this, SLOT(ButtonStart()));
    connect(ui->m_mute, SIGNAL(clicked()), this, SLOT(ButtonVolume()));
    connect(ui->m_forward, SIGNAL(clicked()), this, SLOT(ButtonForward()));
    connect(ui->m_backward, SIGNAL(clicked()), this, SLOT(ButtonBackward()));

    //EQUALIZER SIGNALS
    //connect(&this->m_decoder, SIGNAL(bufferReady()), this, SLOT(dec1()));
    //connect(&this->m_decoder, SIGNAL(finished()), this, SLOT(dec2()));


    //LIST WIDGET
    connect(ui->m_PlaylistList, SIGNAL(clicked(QModelIndex)), this, SLOT(PlaylistList(QModelIndex)));
    ui->m_TrackList->setFocusPolicy(Qt::NoFocus);
    ui->m_PlaylistList->setFocusPolicy(Qt::NoFocus);

    //SLIDERS
    connect(ui->m_volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(VolumeSliderChangeValue(int)));
    connect(ui->m_timeSlider, SIGNAL(sliderMoved(int)), this, SLOT(TimeSliderMove(int)));
    connect(ui->m_timeSlider, SIGNAL(valueChanged(int)), this, SLOT(TimeSliderChangeValue(int)));

    //MEDIA PLAYER
    connect(this->m_mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(MediaPlayerPositionChange(qint64)));
    connect(this->m_mediaPlayer, SIGNAL(playingChanged(bool)), this, SLOT(MediaPlayerTrackChange(bool)));
    connect(this->m_mediaPlayer, SIGNAL(playbackStateChanged(QMediaPlayer::PlaybackState)), this, SLOT(MediaStatus(QMediaPlayer::PlaybackState)));

    //LOAD PLAYLISTS AND TRACKS
    this->loadPlaylist();

    //OTHER
    this->InitializeGraphics();
    this->InitializeEqualizer();

    //NEW BUTTONS
    ui->m_start->setCheckable(true);
    ui->m_start->setAutoRepeat(false);

    ui->m_mute->setCheckable(true);
    ui->m_mute->setAutoRepeat(false);

    this->setIcons();

}

MainWindow::~MainWindow()
{
    delete ui;
    delete this->m_timer;
    delete this->m_audio;
    delete this->m_mediaPlayer;
    delete this->m_scene;
    delete this->m_text1;
    delete this->m_text2;
    delete this->m_font;
}
void MainWindow::dec1(){
    QAudioBuffer buf = this->m_decoder.read();

    // Pobierz dane audio

    const qint16 *data = buf.constData<qint16>();
    int numFrames = buf.frameCount();
    int numChannels = buf.format().channelCount();


    // Parametry wizualizacji
    int numBars = 20; // Ilość pasków
    int barWidth = 10; // Szerokość paska
    int barSpacing = 2; // Odstęp między paskami
    int maxHeight = ui->m_equalizer->height() - 20; // Maksymalna wysokość paska

    // Oblicz średnią amplitudę dla każdego paska
    int framesPerBar = numFrames / numBars;
    QVector<int> barAmplitudes(20, 0);

    for (int i = 0; i < numFrames; ++i) {
        int barIndex = i / framesPerBar;
        if(barIndex < 20){
            barAmplitudes[barIndex] += qAbs(data[i]);
        }
    }

    // Normalizuj amplitudy do zakresu [0, maxHeight]
    int max = barAmplitudes[0];
    for(int i = 1 ; i < 20; i++){
        if(barAmplitudes[i] > max){
            max = barAmplitudes[i];
        }
    }
    for (int i = 0; i < numBars; i++) {
        if(max > 0){
            int barHeight = (maxHeight * barAmplitudes[i]) / max;
            this->m_rect[i]->setRect(0, 0, 15, barHeight);
            this->m_rect[i]->setPos(i * 15, 110 - barHeight);
        }
    }
}
void MainWindow::dec2(){

}
void MainWindow::InitializeEqualizer(){

    this->m_scene_equalizer = new QGraphicsScene(this);
    this->m_scene_equalizer->setSceneRect(0, 0 , ui->m_equalizer->width(), ui->m_equalizer->height());
    ui->m_equalizer->setScene(this->m_scene_equalizer);
    ui->m_equalizer->setRenderHint(QPainter::Antialiasing);
    ui->m_equalizer->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->m_equalizer->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    for(int i = 0; i < 20; i++){
        //490 10 301 111
        QRect r(0, 0, 15, i*4 + 1);
        QGraphicsRectItem *rectItem = this->m_scene_equalizer->addRect(r);
        rectItem->setPos(i * 15, 110 - (i * 4));
        QColor color(0,255,0);
        rectItem->setBrush(color);
        this->m_rect.push_back(rectItem);
    }

    ui->m_equalizer->setVisible(true);
    ui->m_equalizer->show();
}
void MainWindow::DrawEqualizer(int volumeLevel){

}
void MainWindow::InitializeGraphics(){
    this->m_scene = new QGraphicsScene(this);
    this->m_scene->setSceneRect(0, 0, ui->m_view->width(), ui->m_view->height());
    ui->m_view->setScene(this->m_scene);
    ui->m_view->setRenderHint(QPainter::Antialiasing);
    ui->m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->m_font = new QFont("Retro Gaming", 18);

    this->m_text1 = this->m_scene->addText("HELLO IN TRACKPIX!", *this->m_font);
    this->m_text2 = this->m_scene->addText("HELLO IN TRACKPIX!", *this->m_font);
    this->m_font->setPointSize(15);
    this->m_text_time = this->m_scene->addText("TIME", *this->m_font);
    this->m_text_mute = this->m_scene->addText("MUTE: FALSE", *this->m_font);
    this->m_text_volume = this->m_scene->addText("VOLUME:50%", *this->m_font);

    this->m_text1->setDefaultTextColor(QColor(0,255,0));
    this->m_text1->setPos(451,0);
    this->m_text2->setDefaultTextColor(QColor(0,255,0));
    this->m_text2->setPos(451,0);
    this->m_text_time->setDefaultTextColor(QColor(0,255,0));
    this->m_text_time->setPos(20,40);
    this->m_text_mute->setDefaultTextColor(QColor(255,0,0));
    this->m_text_mute->setPos(20,60);
    this->m_text_volume->setDefaultTextColor(QColor(0,255,0));
    this->m_text_volume->setPos(20,80);
    std::cout << this->m_scene->width() << "  " << this->m_scene->height() << std::endl;

    textX = ui->m_view->width();
    ui->m_view->setVisible(true);
    ui->m_view->show();
    connect(this->m_timer, SIGNAL(timeout()), this, SLOT(scrollText()));
    this->m_timer->start(10);
}
void MainWindow::scrollText(){


    int text1_length = this->m_text1->boundingRect().width();
    float text1_posX = this->m_text1->pos().x();

    int text2_length = this->m_text2->boundingRect().width();
    float text2_posX = this->m_text2->pos().x();

    if(this->running_text1){
        this->m_text1->setPos((text1_posX - 0.8), this->m_text1->pos().y());
        if((text1_posX + text1_length) < (this->m_scene->width()/2)){
            this->running_text2 = true;
        }
    }
    if(this->running_text2){
        this->m_text2->setPos((text2_posX - 0.8), this->m_text2->pos().y());
        if((text2_posX + text2_length) < (this->m_scene->width()/2)){
            this->running_text1 = true;
        }
    }
    if( (text1_posX + text1_length) <= 0){
        this->running_text1 = false;
        this->m_text1->setPos(451,0);
    }
    if( (text2_posX + text1_length) <= 0){
        this->running_text2 = false;
        this->m_text2->setPos(451,0);
    }
    //std::cout << "POS 1 " << this->m_text1->pos().x() << "  " << this->m_text1->pos().y() << std::endl;
    //std::cout << "POS 2 " << this->m_text2->pos().x() << "  " << this->m_text2->pos().y() << std::endl;

}
void MainWindow::setIcons(){
    QIcon playIcon = style()->standardIcon(QStyle::SP_MediaPause);
    QPixmap pixmap = playIcon.pixmap(QSize(30,30));

    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), QColor(Qt::green));
    ui->m_start->setIcon(QIcon(pixmap));

    QIcon forwardIcon = style()->standardIcon(QStyle::SP_MediaSeekForward);
    QPixmap forward_pixmap = forwardIcon.pixmap(QSize(30,30));

    QPainter forward_painter(&forward_pixmap);
    forward_painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    forward_painter.fillRect(forward_pixmap.rect(), QColor(Qt::green));
    ui->m_forward->setIcon(QIcon(forward_pixmap));

    QIcon backwardIcon = style()->standardIcon(QStyle::SP_MediaSeekBackward);
    QPixmap backward_pixmap = backwardIcon.pixmap(QSize(30,30));

    QPainter backward_painter(&backward_pixmap);
    backward_painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    backward_painter.fillRect(backward_pixmap.rect(), QColor(Qt::green));
    ui->m_backward->setIcon(QIcon(backward_pixmap));

    QIcon muteIcon = style()->standardIcon(QStyle::SP_MediaVolume);
    QPixmap mute_pixmap = muteIcon.pixmap(QSize(30,30));

    QPainter mute_painter(&mute_pixmap);
    mute_painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    mute_painter.fillRect(mute_pixmap.rect(), QColor(Qt::green));
    ui->m_mute->setIcon(QIcon(mute_pixmap));

}
void MainWindow::ButtonStart(){
    if(ui->m_start->isChecked()){
        QIcon playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
        QPixmap pixmap = playIcon.pixmap(QSize(30,30));

        QPainter painter(&pixmap);
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(pixmap.rect(), QColor("#1B8719"));
        ui->m_start->setIcon(QIcon(pixmap));
        this->m_mediaPlayer->pause();
    }else{
        QIcon playIcon = style()->standardIcon(QStyle::SP_MediaPause);
        QPixmap pixmap = playIcon.pixmap(QSize(30,30));

        QPainter painter(&pixmap);
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(pixmap.rect(), QColor(Qt::green));
        ui->m_start->setIcon(QIcon(pixmap));
        this->m_mediaPlayer->play();
    }
}
void MainWindow::ButtonVolume(){
    if(ui->m_mute->isChecked()){
        QIcon muteIcon = style()->standardIcon(QStyle::SP_MediaVolumeMuted);
        QPixmap pixmap = muteIcon.pixmap(QSize(30,30));

        QPainter painter(&pixmap);
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(pixmap.rect(), QColor(Qt::red));
        ui->m_mute->setIcon(QIcon(pixmap));
        this->m_audio->setMuted(true);
    }else{
        QIcon muteIcon = style()->standardIcon(QStyle::SP_MediaVolume);
        QPixmap pixmap = muteIcon.pixmap(QSize(30,30));

        QPainter painter(&pixmap);
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(pixmap.rect(), QColor(Qt::green));
        ui->m_mute->setIcon(QIcon(pixmap));
        this->m_audio->setMuted(false);
    }
}

void MainWindow::loadPlaylist()
{
    QString playlist_path(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Playlists");
    this->m_dir.setPath(playlist_path);

    if(this->m_dir.isEmpty()){
        return;
    }
    QStringList playlist_directoryList = this->m_dir.entryList();
    for(int i = 0; i < playlist_directoryList.size(); i++){
        QString playlist_name = playlist_directoryList[i];
        if(playlist_name == ".." || playlist_name == "."){
            continue;
        }

        this->m_playlist.push_back(Playlist{playlist_name});
        ui->m_PlaylistList->addItem(playlist_name);
        this->m_dir.setPath(playlist_path + "/" + playlist_name);
        if(this->m_dir.isEmpty()){
            continue;
        }
        QStringList track_directoryList = this->m_dir.entryList();

        for(int j = 0; j < track_directoryList.size(); j++){
            QString track_name = track_directoryList[j];

            if(track_name == ".." || track_name == "."){
                continue;
            }

            QString path = this->m_dir.path() + "/";
            QString name = track_name;
            QString format = ".mp3";

            for(int k = 0; k < 4; k++){
                name.removeLast();
            }
            auto &current_playlist = this->m_playlist.back();
            current_playlist.addTrack(Track{name, path, format});
        }
    }
}
void MainWindow::PlaylistList(QModelIndex)
{
    if(ui->m_PlaylistList->selectedItems().empty()){
        return; //IF THERE IS NO SELECTED ITEM LEAVE
    }
    if(selected_playlist == ui->m_PlaylistList->currentItem()->text()){
        return; //IF SELECTED ITEM IS THE SAME THAT NEW SELECTED ITEM LEAVE
    }
    //GET NAME OF SELECTED PLAYLIST
    ui->m_TrackList->clear();
    selected_playlist = ui->m_PlaylistList->currentItem()->text();
    //FIND PLAYLIST
    Playlist *playlist = this->findPlaylist(selected_playlist);
    if(playlist == nullptr){
        std::cout << "NIE ZNALEZIONO PLAYLISTY" << std::endl;
    }
    //DISPALY ALL TRACKS OF SELECTED PLAYLIST
    this->DisplayPlaylistTrack(*playlist);
    //SELECT PLAYING TRACK IN TRACK LIST
    if(this->selected_playlist == this->playing_playlist){
        QList<QListWidgetItem*> list = ui->m_TrackList->findItems(this->playing_track, Qt::MatchWildcard);
        ui->m_TrackList->setCurrentItem(list[0]);
    }
}
void MainWindow::setNextTrack()
{
    if(this->playing_playlist == ""){
        std::cout << "THERE IS NO SELECTED PLAYLIST" << std::endl;
        return; //THERE IS NO SELECTED PLAYLIST
    }
    if(this->playing_track == ""){
        std::cout << "INT TRACK LIST IS NO SELECTED TRACK" << std::endl;
        return; //IN TRACK LIST IS NO SELECTED TRACK
    }

    //GET TRACK WHO WAS PLAYLING
    QString current_track = this->playing_track;
    //FIND PLAYLIST OF TRACK
    Playlist *playlist = this->findPlaylist(playing_playlist);
    //SET URL OF TRACK IN MEDIA PLAYER
    this->m_mediaPlayer->setSource(this->getNextTrack(playlist, current_track)->getUrl());
    //SET ACTUALL PLAYING TRACK
    this->playing_track = this->getNextTrack(playlist, current_track)->getName();
    //SET LABEL
    this->m_text1->setPlainText(this->playing_track);
    this->m_text2->setPlainText(this->playing_track);
    //PLAY TRACK
    this->m_mediaPlayer->play();

    //SET TRACK IN TRACK LIST
    if(this->selected_playlist == this->playing_playlist){
        int current_row = ui->m_TrackList->currentRow();
        if(current_row == ui->m_TrackList->count() - 1){
            ui->m_TrackList->setCurrentRow(0);
        }else{
            ui->m_TrackList->setCurrentRow(++current_row);
        }
    }
    //SET TIME OF TRACK
    this->setTrackTime();
}

void MainWindow::VolumeSliderChangeValue(int value)
{
    float fvolume = static_cast<float>(value) / 100.0f;
    this->m_audio->setVolume(fvolume);
    QString svolume = QString::fromStdString(std::to_string(value));
    this->m_text_volume->setPlainText("VOLUME: " + svolume + "%");
}
void MainWindow::ButtonForward(){
    if(!this->slider_changing){
        ui->m_timeSlider->setSliderDown(true);
        this->slider_forward= true;
        int val = ui->m_timeSlider->value();
        ui->m_timeSlider->setValue(val + 20000);
        ui->m_timeSlider->setSliderDown(false);
        this->slider_forward= false;
    }
}
void MainWindow::ButtonBackward(){
    this->slider_backward= true;
    ui->m_timeSlider->setSliderDown(true);
    int val = ui->m_timeSlider->value();
    ui->m_timeSlider->setValue(val - 20000);
    ui->m_timeSlider->setSliderDown(false);
    this->slider_backward= false;
}
void MainWindow::TimeSliderMove(int value)
{
    if(!this->slider_clicked){
        this->m_mediaPlayer->setPosition(value);

        QAudioOutput *a = this->m_mediaPlayer->audioOutput();
        this->m_mediaPlayer->setAudioOutput(a);
        QAudioDevice d = a->device();
        QByteArray b = d.id();

        std::cout << "INFO: " << b.size() << std::endl;
        std::cout << "INFO: " << b.data() << std::endl;
    }
}

void MainWindow::TimeSliderChangeValue(int value)
{
    if(!this->slider_changing && !this->slider_forward && !this->slider_backward){
        ui->m_timeSlider->setSliderDown(true);
        QPoint mousePos = QCursor::pos();
        QPoint sliderPos = ui->m_timeSlider->mapToGlobal(QPoint(0, 0));
        int relativePos = mousePos.x() - sliderPos.x();

        int sliderWidth = ui->m_timeSlider->width();
        int newValue = (static_cast<double>(relativePos) / sliderWidth) * ui->m_timeSlider->maximum();
        ui->m_timeSlider->setValue(newValue);
        ui->m_timeSlider->setSliderDown(false);
    }
}

void MainWindow::ButtonNewPlaylist()
{
    QString playlist_name = QInputDialog::getText(this, "Create Playlist", "Playlist Name:");
    if(playlist_name == ""){
        QMessageBox::information(this, "WRONG NAME", "TRY AGAIN");
        return;
    }
    if(playlist_name.size() > 20){
        QMessageBox::information(this, "TO LONG NAME", "TRY AGAIN");
    }
    for(auto playlist : this->m_playlist){
        if(playlist_name == playlist.getName()){
            QMessageBox::information(this, "PLAYLIST WITH THIS NAME EXIST", "TRY AGAIN");
            return;
        }
    }
    this->m_playlist.push_back(Playlist{playlist_name});
    ui->m_PlaylistList->addItem(playlist_name);

    //Create Playlist Folder
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString destinationFolder = QDir(appDataPath).filePath("Playlists/" + playlist_name);
    if(!destinationFolder.isEmpty()){
        if(QDir().mkpath(destinationFolder)){
        }else{
            std::cout << "NIE UDALO SIE UTORZYC FOLDERU: " << destinationFolder.toStdString() << std::endl;
        }
    }
}

void MainWindow::ButtonRemovePlaylist()
{
    if(ui->m_PlaylistList->selectedItems().empty()){
        return; //PLAYLIST ARE NO SELECTED
    }
    QString playlist_name = ui->m_PlaylistList->currentItem()->text();
    std::cout << "USUWANA PLAYLISTA: " << playlist_name.toStdString() << std::endl;
    Playlist *playlist = this->findPlaylist(playlist_name);
    auto p_it = this->getPlaylistIterator(playlist);
    this->m_playlist.erase(p_it);
    if(ui->m_PlaylistList->currentItem()->text() == this->playing_playlist){
        this->m_mediaPlayer->stop();
        this->playing_playlist = "";
        this->selected_playlist = "";
        this->playing_track = "";
    }else{
        this->selected_playlist = "";
    }
    QString playlist_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    playlist_path += "/Playlists/" + playlist_name + "/";
    this->deleteFolder(playlist_path);
    int row = ui->m_PlaylistList->currentRow();
    ui->m_PlaylistList->takeItem(row);
}

void MainWindow::ButtonPlay()
{
    if(ui->m_PlaylistList->selectedItems().empty()){
        return; //IF THERE IS NO SELECTED PLAYLIST
    }
    if(ui->m_TrackList->selectedItems().empty()){
        //IF THERE IS NO SELECTED TRACK PLAY FIRST

        //GET PLAYLIST NAME
        Playlist *playlist_name = this->findPlaylist(selected_playlist);
        //SELECT TRACK ON TRACK LIST
        ui->m_TrackList->setCurrentRow(0);
        //SET URL TRACK IN MEDIA PLAYER
        this->m_mediaPlayer->setSource(((*playlist_name)[0]).getUrl());
        //SET ACTUAL PLAYING PLAYLIST
        this->playing_playlist = playlist_name->getName();
        //SET ACTUAL PLAYING TRACK
        this->playing_track = ((*playlist_name)[0]).getName();
        //PLAY TRACK
        this->m_mediaPlayer->play();
        //SET TRACK TIME
        this->setTrackTime();

    }else{
        this->button_play = true;
        //GET TRACK NAME
        QString track_name = ui->m_TrackList->currentItem()->text();
        //GET SELECTED PLAYLIST
        Playlist *playlist_name = findPlaylist(this->selected_playlist);
        //SET URL TRACK IN MEDIA PLAYER
        this->m_mediaPlayer->setSource(this->getTrack(playlist_name, track_name)->getUrl());
        //SET ACTUAL PLAYING PLAYLIST
        this->playing_playlist = playlist_name->getName();
        //SET ACTUAL PLAYING TRACK
        this->playing_track = this->getTrack(playlist_name, track_name)->getName();
        //PLAY TRACK
        this->m_mediaPlayer->play();
        //SET TRACK TIME
        this->setTrackTime();
        this->button_play = false;

        //this->m_decoder.setSource(this->getTrack(playlist_name, track_name)->getUrl());
        //this->m_decoder.start();


    }
    //SET LABEL
    this->m_text1->setPlainText(this->playing_track);
    this->m_text2->setPlainText(this->playing_track);





}

void MainWindow::ButtonStop()
{
    if(this->m_mediaPlayer->isPlaying()){
        this->m_mediaPlayer->pause();
        ui->m_stopButton->setText("CONTINUE");
    }else{
        this->m_mediaPlayer->play();
        ui->m_stopButton->setText("PAUSE");
    }
}

void MainWindow::ButtonAddTrack()
{
    if(ui->m_PlaylistList->selectedItems().empty()){
        return; //THERE IS NO SELECTED PLAYLIST
    }
    //OPEN QFILE DIALOG TO CHOOSE A TRACKS
    QFileDialog file_dialog;
    file_dialog.setFileMode(QFileDialog::ExistingFiles);
    QStringList files = file_dialog.getOpenFileNames(this,"select mp3 tracks", "C:/");

    if(files.isEmpty()){
        QMessageBox::information(this, "ERROR", "NO FILES SELECTED");
        return;
    }

    for(int i = 0; i < files.size(); i++){
        QString file = files[i];
        QString path;
        QString name;
        QString format = ".mp3";
        std::cout << "FILE[i] " << file.toStdString() << std::endl;
        if(!file.indexOf(".mp3")){
            QMessageBox::information(this, "ERROR", "WRONG FORMAT ONLY MP3!");
            continue;
        }
        int last_slash;
        //FIND END OF THE PATH OF TRACK
        for(int j = 0; j < file.size() - 1; j++){
            if(file[j] == QChar('/')){
                last_slash = j;
            }
        }
        //GET PATH OF TRACK
        for(int j = 0; j <= last_slash; j++){
            path.push_back(file[j]);
        }
        //GET NAME OF TRACK
        for(int j = last_slash + 1; j < file.size() - 4; j++){
            name.push_back(file[j]);
        }
        //CHECK IF THE TRACK EXIST IN SELECTED PLAYLIST
        Playlist *playlist = this->findPlaylist(this->selected_playlist);
        if(this->getTrack(playlist, name) != nullptr){
            continue;
        }
        //ADD TRACK TO SELECTED PLAYLIST AND TRACK LIST
        playlist->addTrack(Track{name, path, format});
        ui->m_TrackList->addItem(name);

        //ADD TRACK TO THE PLAYLIST FOLDER
        QString playlist_path(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                              + "/Playlists/" + playlist->getName() + "/" + name + format);
        std::cout << path.toStdString() << name.toStdString() << format.toStdString() << std::endl;
        std::cout << file.toStdString() << std::endl;
        std::cout << playlist_path.toStdString() << std::endl;
        this->m_dir.setPath(playlist_path);
        if(QFile::copy(file, this->m_dir.path())){
            std::cout << "SUCCES OF ADDING TRACK TO PLAYLIST FOLDER" << std::endl;
        }
    }
}

void MainWindow::ButtonRemoveTrack()
{
    if(ui->m_TrackList->selectedItems().empty()){
        return; //THERE ARE NO SELECTED TRACK
    }
    QString track_name = ui->m_TrackList->currentItem()->text();
    if(this->playing_track == ui->m_TrackList->currentItem()->text()
        && ui->m_PlaylistList->currentItem()->text() == playing_playlist){
        this->playing_track = "";
    }

    int row = ui->m_TrackList->currentRow();
    ui->m_TrackList->takeItem(row);
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    path += "/Playlists/" +  this->selected_playlist + "/" + track_name + ".mp3";
    std::cout << "USUNIETO TRACK: " << path.toStdString() << std::endl;
    this->deleteFile(path);
}

void MainWindow::ButtonNextTrack()
{
    if(this->playing_track.isEmpty()){
        return; //THERE ARE NO PLAYING TRACK
    }
    this->button_play = true;
    //FINDING PLAYLIST
    Playlist *playlist = this->findPlaylist(playing_playlist);
    //GET ACTUAL PLAYING TRACK
    QString track_name = this->playing_track;
    //GET NEXT TRACK
    Track *track = this->getNextTrack(playlist, track_name);
    //SET TRACK URL IN MEDIA PLAYER
    this->m_mediaPlayer->setSource(track->getUrl());
    //SET ACTUAL PLAYING TRACK
    this->playing_track = track->getName();
    //PLAY TRACK
    this->m_mediaPlayer->play();
    //SET TRACK TIME
    this->setTrackTime();
    if(selected_playlist == playing_playlist){
        //SET TRACK IN TRACK LIST
        int current_row = ui->m_TrackList->currentRow();
        if(current_row == ui->m_TrackList->count() - 1){
            ui->m_TrackList->setCurrentRow(0);
        }else{
            ui->m_TrackList->setCurrentRow(++current_row);
        }

    }
    this->button_play = false;
    //SET LABEL
    this->m_text1->setPlainText(this->playing_track);
    this->m_text2->setPlainText(this->playing_track);
}

void MainWindow::ButtonPrevTrack()
{
    if(this->playing_track.isEmpty()){
        return; //THERE ARE NO PLAYING TRACK
    }
    this->button_play = true;
    //FINDING PLAYLIST
    Playlist *playlist = this->findPlaylist(playing_playlist);
    //GET ACTUAL PLAYING TRACK
    QString track_name = this->playing_track;
    //GET NEXT TRACK
    Track *track = this->getPrevTrack(playlist, track_name);
    //SET TRACK URL IN MEDIA PLAYER
    this->m_mediaPlayer->setSource(track->getUrl());
    //SET ACTUAL PLAYING TRACK
    this->playing_track = track->getName();
    //PLAY TRACK
    this->m_mediaPlayer->play();
    //SET TRACK TIME
    this->setTrackTime();
    if(selected_playlist == playing_playlist){
        //SET TRACK IN TRACK LIST
        int current_row = ui->m_TrackList->currentRow();
        if(current_row == 0){
            ui->m_TrackList->setCurrentRow(ui->m_TrackList->count() - 1);
        }else{
            ui->m_TrackList->setCurrentRow(--current_row);
        }

    }
    this->button_play = false;
    //SET LABEL
    this->m_text1->setPlainText(this->playing_track);
    this->m_text2->setPlainText(this->playing_track);
}

void MainWindow::TrackList(QModelIndex)
{

}

void MainWindow::MediaPlayerPositionChange(qint64 value)
{
    if(!ui->m_timeSlider->isSliderDown() ){
        this->slider_changing = true;
        ui->m_timeSlider->setValue(value);
        Playlist * playlist = this->findPlaylist(playing_playlist);
        //this->m_decoder.setSource(this->getTrack(playlist, playing_track)->getUrl());
        //this->m_decoder.start();

        this->slider_changing = false;
    }
    QTime  mediaTime (0, (this->m_mediaPlayer->position() / 60000) % 60,
                    (this->m_mediaPlayer->position() / 1000) % 60);
    QString formattedTime = mediaTime.toString("mm:ss");
    this->m_text_time->setPlainText(formattedTime);

}

void MainWindow::MediaPlayerTrackChange(bool flag)
{

}

void MainWindow::MediaStatus(QMediaPlayer::PlaybackState status)
{
    if(status == QMediaPlayer::PlaybackState::StoppedState){
        if(!this->button_play){
            std::cout << "NEXT TRACK" << std::endl;
            this->setNextTrack();
        }
    }
}
Playlist *MainWindow::findPlaylist(QString &name)
{
    for(auto &playlist : this->m_playlist){
        if(playlist.getName() == name){
            return &playlist;
        }
    }
    return nullptr;
}

void MainWindow::DisplayPlaylistTrack(Playlist &playlist)
{
    for(auto &track : playlist){
        ui->m_TrackList->addItem(track.getName());
    }
}
Track *MainWindow::getTrack(Playlist *playlist, QString track_name){
    for(auto &track : *playlist){
        if(track.getName() == track_name){
            return &track;
        }
    }
    return nullptr;
}
Track *MainWindow::getNextTrack(Playlist *playlist, QString track_name){
    for(auto t_it = playlist->begin(); t_it != playlist->end(); t_it++){
        if(t_it->getName() == track_name){
            if(t_it == playlist->end() - 1){
                t_it = playlist->begin();
                return &(*t_it);
            }else{
                t_it++;
                return &(*t_it);
            }
        }
    }
    return nullptr;
}
Track *MainWindow::getPrevTrack(Playlist *playlist, QString track_name){
    for(auto t_it = playlist->begin(); t_it != playlist->end(); t_it++){
        if(t_it->getName() == track_name){
            if(t_it == playlist->begin()){
                t_it = playlist->last();
                return &(*t_it);
            }else{
                t_it--;
                return &(*t_it);
            }
        }
    }
    return nullptr;
}
void MainWindow::setTrackTime(){
    qint64 duration = this->m_mediaPlayer->duration();
    QTime mediaTime(0, (duration / 60000) % 60, (duration / 1000) % 60);
    QString formattedTime = mediaTime.toString("hh:mm:ss");
    //ui->m_totalTimeLabel->setText(formattedTime);
    ui->m_timeSlider->setRange(0, this->m_mediaPlayer->duration());
}
std::vector<Playlist>::iterator MainWindow::getPlaylistIterator(Playlist* playlist){
    auto p_it = this->m_playlist.begin();
    for(; p_it != this->m_playlist.end(); p_it++){
        if(p_it->getName() == playlist->getName()){
            return p_it;
        }
    }
    return this->m_playlist.end();
}
void MainWindow::deleteFolder(QString path){
    this->m_dir.setPath(path);
    QStringList files = this->m_dir.entryList();
    QString delete_file_path = "";
    std::cout << "SIZE: " << files.size() << std::endl;
    for(int i = 0; i < files.size(); i++){
        QString file = files[i];
        if(file != "." && file != ".."){
            std::cout << "REMOVING FILE: " << file.toStdString() << std::endl;
            delete_file_path = path + "/" + file;
            std::cout << "REMOVING FILE: " << delete_file_path.toStdString() << std::endl;
            this->m_dir.remove(delete_file_path);

        }
    }
    this->m_dir.rmpath(path);
}
void MainWindow::deleteFile(QString path){
    this->m_dir.setPath(path);
    this->m_dir.remove(path);
}
