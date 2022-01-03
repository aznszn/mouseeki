#include "createplaylistdialogue.h"
#include "ui_createplaylistdialogue.h"

createPlaylistDialogue::createPlaylistDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::createPlaylistDialogue)
{
    ui->setupUi(this);
}

createPlaylistDialogue::~createPlaylistDialogue()
{
    delete ui;
}

void createPlaylistDialogue::on_buttonBox_accepted()
{
    if(!ui->Name->text().isEmpty())
       emit createPlaylist(ui->Name->text(), ui->description->toPlainText());
}


void createPlaylistDialogue::on_buttonBox_rejected()
{
    this->close();
}

