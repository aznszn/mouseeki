#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, QString uname)
    : QMainWindow(parent)
    , user(uname)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFile file(":/stylesheet/dark");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    this->setStyleSheet(styleSheet);

    auto leftPanelModel = new QStandardItemModel(this);
    ui->leftPanel->setModel(leftPanelModel);
    leftPanelModel->appendRow(new QStandardItem(QIcon(":/icons/search"), "Search"));
    leftPanelModel->appendRow(new QStandardItem(QIcon(":/icons/home-page"), "Home"));
    leftPanelModel->appendRow(new QStandardItem(QIcon(":/icons/playlist"), "Your playlists"));
    leftPanelItems = new QStringList{"Home", "Search", "Your Library", "Playlists"};
    ui->leftPanel->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->leftPanel->setFrameStyle(QFrame::NoFrame);
    ui->leftPanel->setIconSize(QSize(32, 32));
    ui->leftPanel->setSelectionRectVisible(false);
    ui->leftPanel->setFocusPolicy(Qt::NoFocus);

    player = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    audioOutput->setVolume(50);
    player->setAudioOutput(audioOutput);
    playList = new QList<QString>;
    playList_IDs = new QList<int>;

    searchView = new Search(this, playList, playList_IDs, user);
    homeView = new Home(this, user, playList, playList_IDs);
    artistView = new ArtistView(this);
    albumView = new AlbumView(this, playList, playList_IDs, user);
    plview = new Playlistsview(this, user);
    pl = new playlist(this,user, playList, playList_IDs);

    ui->unameText->setText(user);

    ui->center->addWidget(searchView);
    ui->center->addWidget(homeView);
    ui->center->addWidget(artistView);
    ui->center->addWidget(albumView);
    ui->center->addWidget(artistView);
    ui->center->addWidget(plview);
    ui->center->addWidget(pl);

    ui->center->setCurrentIndex(ui->center->indexOf(searchView));
    pages.push_back(ui->center->indexOf(searchView));
    currPage = 0;

    this->showMaximized();
    //connect(searchView, SIGNAL(playSong_(QString)), this, SLOT(playSong(QString)));
    connect(artistView, SIGNAL(showAlbum(int,QString)), this, SLOT(moveToAlbum(int,QString)));

    connect(searchView, SIGNAL(playSong_(int)), this, SLOT(playPlayList(int)));
    connect(searchView, SIGNAL(showArtist(int,QString)), this, SLOT(moveToArtist(int,QString)));
    connect(searchView, SIGNAL(likeUnlike(int)), this, SLOT(toggleLike(int)));
    connect(searchView, SIGNAL(addToQ(int,QString)), this, SLOT(addToQ(int,QString)));
    connect(searchView, SIGNAL(showAlbum(int,QString)), this, SLOT(moveToAlbum(int,QString)));

    connect(homeView, SIGNAL(showAlbum(int,QString)), this, SLOT(moveToAlbum(int,QString)));
    connect(homeView, SIGNAL(play(int)), this, SLOT(playPlayList(int)));
    connect(homeView, SIGNAL(showArtist(int,QString)), this, SLOT(moveToArtist(int,QString)));
    connect(homeView, SIGNAL(likeUnlike(int)), this, SLOT(toggleLike(int)));
    connect(homeView, SIGNAL(addToQ(int,QString)), this, SLOT(addToQ(int,QString)));
    connect(homeView, SIGNAL(showPlaylist(int)), this, SLOT(moveToPlaylist(int)));

    //connect(albumView, SIGNAL(playSong_(int)), this, SLOT(playPlayList(int)));
    connect(albumView, SIGNAL(showArtist(int,QString)), this, SLOT(moveToArtist(int,QString)));
    //connect(albumView, SIGNAL(showAlbum(int,QString)), this, SLOT(moveToAlbum(int,QString)));
    connect(albumView, SIGNAL(likeUnlike(int)), this, SLOT(toggleLike(int)));
    connect(albumView, SIGNAL(addToQ(int,QString)), this, SLOT(addToQ(int,QString)));
    //connect(albumView, SIGNAL(showPlaylist(int)), this, SLOT(moveToPlaylist(int)));

    connect(pl, SIGNAL(showArtist(int,QString)), this, SLOT(moveToArtist(int,QString)));
    connect(pl, SIGNAL(showAlbum(int,QString)), this, SLOT(moveToAlbum(int,QString)));
    connect(pl, SIGNAL(likeUnlike(int)), this, SLOT(toggleLike(int)));
    connect(pl, SIGNAL(addToQ(int,QString)), this, SLOT(addToQ(int,QString)));

    connect(player, &QMediaPlayer::durationChanged,ui->songProgressBar,&QAbstractSlider::setMaximum);
    connect(player, &QMediaPlayer::positionChanged, ui->songProgressBar,&QAbstractSlider::setValue);
    connect(ui->songProgressBar, &QAbstractSlider::sliderMoved, player, &QMediaPlayer::setPosition);
    connect(albumView, SIGNAL(playAlbum(int)), this, SLOT(playPlayList(int)));
    connect(pl, SIGNAL(play(int)), this, SLOT(playPlayList(int)));
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::song_ended);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, [&](QMediaPlayer::MediaStatus status){
        if(status == QMediaPlayer::LoadedMedia){
            ui->playButton->setChecked(false);
        }
    });
    connect(plview, SIGNAL(showPlaylist(int)), this, SLOT(moveToPlaylist(int)));
    connect(player, &QMediaPlayer::durationChanged, this, [&](qint64 dur) {
         ui->endTime->setText(QTime((dur/(1000*60*60))%60,(dur/(1000*60))%60,(dur/1000)%60,0).toString());
    });
    connect(player, &QMediaPlayer::positionChanged, this,[&](qint64 pos){
        ui->currTime->setText(QTime((pos/(1000*60*60))%60,(pos/(1000*60))%60,(pos/1000)%60,0).toString());
    });

    connect(plview, SIGNAL(playlistChange()), this, SLOT(updateHomeView()));

    isPlaying = false;
    setupButtons();
    logoutAct = new QAction(tr("Logout"), this);
    connect(logoutAct, SIGNAL(triggered()), this, SLOT(logout()));
}

void MainWindow::logout(){
    emit loggedOut();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::song_ended(QMediaPlayer::MediaStatus status){
    if(status == QMediaPlayer::EndOfMedia){
        if(loop1){
            player->play();
        }
        else
            emit ui->nextButton->clicked();
    }
}

void MainWindow::on_leftPanel_clicked(const QModelIndex &index)
{
    if(currPage != pages.size() - 1)
        pages.erase(pages.begin() + currPage + 1, pages.end());

    if(index.row() == 0){
        ui->center->setCurrentIndex(ui->center->indexOf(searchView));
        pages.push_back(ui->center->indexOf(searchView));
        currPage++;
    } else if(index.row() == 1) {
        ui->center->setCurrentIndex(ui->center->indexOf(homeView));
        pages.push_back(ui->center->indexOf(homeView));
        currPage++;
    }else if(index.row() == 2){
        ui->center->setCurrentIndex(ui->center->indexOf(plview));
        pages.push_back(ui->center->indexOf(plview));
        currPage++;
    }
}

void MainWindow::moveToArtist(int ID, QString name){
    if(currPage != pages.size() - 1)
        pages.erase(pages.begin() + currPage + 1, pages.end());

    pages.push_back(ui->center->indexOf(artistView));
    currPage++;
    ui->center->setCurrentIndex(ui->center->indexOf(artistView));
    artistView->update(ID, name);
}

void MainWindow::moveToAlbum(int ID, QString name){
    if(currPage != pages.size() - 1)
        pages.erase(pages.begin() + currPage + 1, pages.end());

    pages.push_back(ui->center->indexOf(albumView));
    currPage++;
    ui->center->setCurrentIndex(ui->center->indexOf(albumView));
    albumView->update(ID, name);

}

void MainWindow::moveToPlaylist(int ID){
    if(currPage != pages.size() - 1)
        pages.erase(pages.begin() + currPage + 1, pages.end());

    pages.push_back(ui->center->indexOf(pl));
    currPage++;
    ui->center->setCurrentIndex(ui->center->indexOf(pl));
    pl->update(ID);
}

void MainWindow::playSong(QString URL){
    ui->likeButton->show();
    playList->clear();
    current = 0;
    player->setSource(QUrl::fromLocalFile(URL));
    player->play();
}

void MainWindow::on_playButton_clicked()
{
    if(player->hasAudio()){
        if(player->playbackState() == QMediaPlayer::PlayingState){
            player->pause();
        }
        else if(player->playbackState() == QMediaPlayer::PausedState){
            player->play();
        }
    }
    else
        ui->playButton->setChecked(false);
}

void MainWindow::playPlayList(int curr){
    ui->likeButton->show();
    player->setSource(QUrl::fromLocalFile(playList->at(curr)));
    player->play();
    current = curr;
    updateNowPlaying();
}


void MainWindow::on_nextButton_clicked()
{
    if(playList->empty())
        return;

    if(current != playList->size() - 1){
        player->setSource(QUrl::fromLocalFile(playList->at(++current)));
        player->play();
    }
    else if(loop){
        player->setSource(QUrl::fromLocalFile(playList->at((current = 0))));
        player->play();
    }
    else
        player->stop();
    updateNowPlaying();
}


void MainWindow::on_prevButton_clicked()
{
    if(playList->empty())
        return;

    if(player->position() >= 5000)
        player->setPosition(0);

    else if(current != 0){
        player->setSource(QUrl::fromLocalFile(playList->at(--current )));
        player->play();
    }
    else if(loop){
        player->setSource(QUrl::fromLocalFile(playList->at((current = playList->size() - 1))));
        player->play();
    }
    else
        player->setPosition(0);
    updateNowPlaying();
}


void MainWindow::on_loopButton_clicked()
{
    if(!loop && !loop1){
        loop = true;
        ui->loopButton->setText("Loop");
        ui->loopButton->setIcon(*loopIcon);
    }
    else if(loop){
        loop1 = true;
        loop = false;
        ui->loopButton->setText("Loop1");
        ui->loopButton->setIcon(*loop1Icon);
    }
    else{
        loop1 = false;
        ui->loopButton->setText("NoLoop");
        ui->loopButton->setIcon(*noLoopIcon);
    }
}

void MainWindow::toggleLike(int ID){
    QSqlQuery q;
    q.prepare("SELECT * FROM likes WHERE user_id = ? AND song_id = ?");
    q.bindValue(0, user);
    q.bindValue(1, ID);
    q.exec();

    if(!q.next())
        q.prepare("INSERT INTO likes VALUES (?, ?)");

    else
        q.prepare("DELETE FROM likes WHERE user_id = ? AND song_id = ?");

    q.bindValue(0, user);
    q.bindValue(1, ID);
    q.exec();
    if(!playList_IDs->empty() && playList_IDs->at(current) == ID)
        updateNowPlaying();
    homeView->update();
}

void MainWindow::updateNowPlaying(){
    int currID = playList_IDs->at(current);
    QSqlQuery q;
    QString currName;
    QString currPicUrl;
    q.exec("select pic_url, name, artname from song_info WHERE id = " + QString::number(currID));
    q.next();

    QPixmap p(q.value(0).toString());
    int w = ui->currSongPic->width();
    int h = ui->currSongPic->height();
    ui->currSongPic->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio));

    QString currSongName = q.value(1).toString();
    QString currSongArtist = q.value(2).toString();

    ui->currSongName->setToolTip("");
    if(currSongName.length() > 19){
        ui->currSongName->setToolTip(currSongName);
        currSongName.resize(16);
        currSongName += "...";
    }

    ui->currSongName->setText(currSongName);
    ui->currSongArtist->setText(currSongArtist);

    q.prepare("SELECT * FROM likes WHERE user_id = ? AND song_id = ?");
    q.bindValue(0, user);
    q.bindValue(1, currID);
    q.exec();
    if(q.next())
        ui->likeButton->setChecked(true);
    else
        ui->likeButton->setChecked(false);
}

void MainWindow::addToQ(int ID, QString url){
    if(!playList->empty()){
        if(current != playList->size() - 1){
            playList->insert(current + 1, url);
            playList_IDs->insert(current + 1, ID);
        }
        else{
            playList->append(url);
            playList_IDs->append(ID);
        }
    }
}


void MainWindow::on_likeButton_clicked()
{
    if(player->hasAudio())
        toggleLike(playList_IDs->at(current));
}

void MainWindow::setupButtons(){
    QIcon icon;
    icon.addPixmap(QPixmap(":/icons/play"), QIcon::Normal, QIcon::On);
    icon.addPixmap(QPixmap(":/icons/pause"), QIcon::Normal, QIcon::Off);
    ui->playButton->setIcon(icon);
    ui->playButton->setCheckable(true);
    ui->playButton->setChecked(true);
    ui->playButton->setIconSize(QSize(40,40));


    QIcon icon2;
    icon2.addPixmap(QPixmap(":/icons/hearth"), QIcon::Normal, QIcon::Off);
    icon2.addPixmap(QPixmap(":/icons/hearthed"), QIcon::Normal, QIcon::On);
    ui->likeButton->setIcon(icon2);
    ui->likeButton->setCheckable(true);
    ui->likeButton->setIconSize(QSize(35,35));

    ui->nextButton->setIcon((QIcon(QPixmap(":/icons/next"))));
    ui->nextButton->setIconSize(QSize(35,35));

    ui->prevButton->setIcon((QIcon(QPixmap(":/icons/back"))));
    ui->prevButton->setIconSize(QSize(35,35));

    loopIcon = new QIcon(QPixmap(":/icons/repeat"));
    loop1Icon = new QIcon(QPixmap(":/icons/repeat1"));
    noLoopIcon = new QIcon(QPixmap(":/icons/norepeat"));
    ui->loopButton->setIcon(*noLoopIcon);
    ui->loopButton->setIconSize(QSize(35,35));

    ui->nextPageButton->setIcon(QIcon(QPixmap(":/icons/right-arrow")));
    ui->nextPageButton->setIconSize(QSize(35,35));

    ui->prevPageButton->setIcon(QIcon(QPixmap(":/icons/left-arrow")));
    ui->prevPageButton->setIconSize(QSize(35,35));
}


void MainWindow::on_unameText_clicked()
{
    QMenu menu(this);
    menu.addAction(logoutAct);
    menu.exec(ui->unameText->pos());
}

void MainWindow::on_prevPageButton_clicked()
{
    if(currPage != 0){
        ui->center->setCurrentIndex(pages[--currPage]);
    }
}


void MainWindow::on_nextPageButton_clicked()
{
    if(currPage != pages.size() - 1){
        ui->center->setCurrentIndex(pages[++currPage]);
    }
}

void MainWindow::updateHomeView(){
    homeView->update();
}

