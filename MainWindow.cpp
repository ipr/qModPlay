//////////////////////////////////////
//
// main Qt-app window and framework
// to handle module players and audio-device
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#include "MainWindow.h"
#include "ui_MainWindow.h"


#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QDebug>

// for testing
#include <QThread>

// moved audio device and module file handling
// to separate from GUI, simplifies later..
//
#include "PlaybackHandler.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_playbackHandler(nullptr)
{
    ui->setupUi(this);
    
    m_playbackHandler = new PlaybackHandler(this);
    connect(m_playbackHandler, SIGNAL(playbackFinished()), this, SLOT(onPlaybackStopped()));
}

MainWindow::~MainWindow()
{
    delete m_playbackHandler;
    delete ui;
}

void MainWindow::on_actionPlay_triggered()
{
    // get selection from list
    QListWidgetItem *pItem = ui->listWidget->currentItem();
    if (pItem == NULL)
    {
        return;
    }
    
    m_playbackHandler->playFile(pItem->text());
}

void MainWindow::on_actionStop_triggered()
{
    m_playbackHandler->stopPlay();
}

void MainWindow::on_actionFiles_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Open file"));
    foreach (QString file, files)
    {
        // (temp)
		ui->listWidget->addItem(new QListWidgetItem(file));
    }
}

void MainWindow::onPlaybackStopped()
{
    // show that we have stopped/move to next file
}
