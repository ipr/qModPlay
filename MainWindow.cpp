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
    m_playbackHandler(nullptr),
    m_lastPath()
{
    ui->setupUi(this);
    
    m_playbackHandler = new PlaybackHandler(this);
    connect(m_playbackHandler, SIGNAL(playbackFinished()), this, SLOT(onPlaybackStopped()));
	connect(m_playbackHandler, SIGNAL(status(QString)), this, SLOT(onStatus(QString)));
    connect(m_playbackHandler, SIGNAL(error(QString)), this, SLOT(onError(QString)));
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
    if (pItem == NULL
        && ui->listWidget->count() > 0)
    {
        // get first
        pItem = ui->listWidget->item(0);
    }
    
    if (pItem != NULL)
    {
        m_playbackHandler->playFile(pItem->text());
    }
}

void MainWindow::on_actionStop_triggered()
{
    m_playbackHandler->stopPlay();
}

void MainWindow::on_actionFiles_triggered()
{
    // get file(s) to playlist
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Open file"), m_lastPath);
    
    if (files.isEmpty() == false)
    {
        // keep path where file(s) were selected
        m_lastPath = files[0];
        m_lastPath.replace('\\', "/");
        m_lastPath = m_lastPath.left(m_lastPath.lastIndexOf('/'));
    }

    // add each selection to playlist
    // TODO: check for duplicates?
    //    
    foreach (QString file, files)
    {
        // (temp)
		ui->listWidget->addItem(new QListWidgetItem(file));
    }
}

void MainWindow::onPlaybackStopped()
{
    // show that we have stopped/move to next file
    // (placeholder)
    int currentRow = ui->listWidget->currentRow() +1;
    if (currentRow < ui->listWidget->count())
    {
        ui->listWidget->setCurrentRow(currentRow);
        
        QListWidgetItem *pItem = ui->listWidget->currentItem();
        if (pItem != NULL)
        {
            m_playbackHandler->playFile(pItem->text());
        }
    }
}

void MainWindow::onStatus(QString message)
{
	ui->statusBar->showMessage(message);
}

void MainWindow::onError(QString message)
{
    ui->statusBar->showMessage("Error: " + message);
}

