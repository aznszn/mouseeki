#include "artistview.h"
#include "ui_artistview.h"

ArtistView::ArtistView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArtistView)
{
    ui->setupUi(this);
    albums_table_model = new QSqlQueryModel;
    ui->albums->setModel(albums_table_model);
    ui->albums->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->albums->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->albums->setSelectionBehavior(QTableView::SelectRows);
    ui->albums->setFocusPolicy(Qt::NoFocus);
    ui->albums->verticalHeader()->hide();
    ui->albums->setFrameStyle(QFrame::NoFrame);
    ui->albums->setContextMenuPolicy(Qt::CustomContextMenu);

    QFont font(QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(":/fonts/resources/fonts/GothicA1-ExtraBold.ttf")).at(0));
    font.setPointSize(40);
    ui->name->setFont(font);
    font.setPointSize(20);
    ui->descrText->setFont(font);
}

void ArtistView::update(int ID, QString name){
    this->ID = ID;
    this->name = name;
    QSqlQuery q;
    q.prepare("SELECT album_id, album_name, album_year, album_length, no_of_songs from album where artist_id = ?");
    q.bindValue(0, ID);
    q.exec();
    albums_table_model->setQuery(std::move(q));
    ui->name->setText(name);
    albums_table_model->setHeaderData(1,Qt::Horizontal, tr("Name"));
    albums_table_model->setHeaderData(2,Qt::Horizontal, tr("Year"));
    albums_table_model->setHeaderData(3,Qt::Horizontal, tr("Length"));
    albums_table_model->setHeaderData(4,Qt::Horizontal, tr("Number of songs"));
    ui->albums->setColumnHidden(0, true);
    ui->albums->setFocusPolicy(Qt::NoFocus);

}

ArtistView::~ArtistView()
{
    delete ui;
}

void ArtistView::on_albums_clicked(const QModelIndex &index)
{
    ui->albums->clearSelection();
    int ID = albums_table_model->record(index.row()).value(0).toInt();
    QString name = albums_table_model->record(index.row()).value(1).toString();
    emit showAlbum(ID, name);
}

