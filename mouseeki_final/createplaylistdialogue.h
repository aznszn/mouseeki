#ifndef CREATEPLAYLISTDIALOGUE_H
#define CREATEPLAYLISTDIALOGUE_H

#include <QDialog>

namespace Ui {
class createPlaylistDialogue;
}

class createPlaylistDialogue : public QDialog
{
    Q_OBJECT

public:
    explicit createPlaylistDialogue(QWidget *parent = nullptr);
    ~createPlaylistDialogue();

signals:
    void createPlaylist(QString name, QString decription);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::createPlaylistDialogue *ui;
};

#endif // CREATEPLAYLISTDIALOGUE_H
