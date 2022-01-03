#include "playlistsview.h"
#include "ui_playlistsview.h"

Playlistsview::Playlistsview(QWidget *parent, QString uname) :
    QWidget(parent),
    ui(new Ui::Playlistsview),
    uname(uname)
{
    ui->setupUi(this);

    delPlaylistAct = new QAction(tr("&Delete laylist"));
    connect(delPlaylistAct, &QAction::triggered, this, &Playlistsview::deletePlaylist);
    playlists = new QSqlQueryModel;
    ui->playlistsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->playlistsTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->playlistsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->playlistsTable->setModel(playlists);
    ui->playlistsTable->setFocusPolicy(Qt::NoFocus);
    ui->playlistsTable->setFocusPolicy(Qt::NoFocus);
    ui->playlistsTable->verticalHeader()->hide();
    ui->playlistsTable->setFrameStyle(QFrame::NoFrame);
    ui->playlistsTable->setContextMenuPolicy(Qt::CustomContextMenu);

    createplistdialogue = new createPlaylistDialogue(this);
    createplistdialogue->setWindowTitle("Create a playlist");

    connect(createplistdialogue, SIGNAL(createPlaylist(QString, QString)), this, SLOT(create(QString, QString)));
    update();
}

void Playlistsview::update(){
    QSqlQuery q;
    q.prepare("SELECT playlist_id, playlist_name FROM playlist WHERE user_id = ?");
    q.bindValue(0, uname);
    q.exec();
    playlists->setQuery(std::move(q));
    playlists->setHeaderData(1, Qt::Horizontal, tr("Name"));
    ui->playlistsTable->setColumnHidden(0, true);
}

Playlistsview::~Playlistsview()
{
    delete ui;
}

void Playlistsview::create(QString name, QString descript){
    QSqlQuery q;
    q.prepare("INSERT INTO playlist VALUES (?, null, ?, ?)");
    q.bindValue(0, uname);
    q.bindValue(1, name);
    q.bindValue(2, descript);
    q.exec();
    update();
    emit playlistChange();
}


void Playlistsview::on_create_clicked()
{
    createplistdialogue->show();
}


void Playlistsview::on_playlistsTable_clicked(const QModelIndex &index)
{
    ui->playlistsTable->clearSelection();
    emit showPlaylist(playlists->record(index.row()).value(0).toInt());
}


void Playlistsview::on_playlistsTable_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    clickedRow = ui->playlistsTable->indexAt(pos).row();
    menu.addAction(delPlaylistAct);
    menu.exec(ui->playlistsTable->viewport()->mapToGlobal(pos));
}

void Playlistsview::deletePlaylist(){
    QSqlQuery q;
    q.prepare("DELETE FROM playlist WHERE playlist_id = ?");
    q.bindValue(0, playlists->record(clickedRow).value(0).toInt());
    q.exec();
    update();
}
