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

// use for archive/compressed file information
#include "ArchiveHandler.h"

// moved audio device and module file handling
// to separate from GUI, simplifies later..
//
#include "PlaybackHandler.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_playbackHandler(nullptr),
    m_lastPath(),
	m_baseTitle()
{
    ui->setupUi(this);
	
	m_baseTitle = this->windowTitle();
    
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
    QTreeWidgetItem *pItem = ui->treeWidget->currentItem();
    //QListWidgetItem *pItem = ui->listWidget->currentItem();
    if (pItem == NULL
        && ui->treeWidget->count() > 0)
    {
        // get first
        pItem = ui->treeWidget->item(0);
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
		// TODO: make better playlist later..

		// fix pathname, even Windows supports correct way now
		// -> no reason to use that old shit anywhere
		//
		file.replace('\\', "/");
		
		QTreeWidgetItem *pTopItem = new QTreeWidgetItem((QTreeWidgetItem*)0);
		pTopItem->setText(0, file);
		
		// TODO: check file type:
		// - show files in archive if multi-file archive
		// - show single-file compression type if XPK/PP20/IMPL..
		// - otherwise show as-is
		
		ui->treeWidget->addTopLevelItem(pTopItem);
    }
}

void MainWindow::on_actionPath_triggered()
{
    QStringList path = QFileDialog::getExistingDirectory(this, tr("Select dir"), m_lastPath);
    if (path == NULL)
    {
		return;
    }
    
    // keep this
    m_lastPath = path;
    
	// list recursively supported files in selected path
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
	setWindowTitle(m_baseTitle + " - " + message);
	//ui->statusBar->showMessage(message);
}

void MainWindow::onError(QString message)
{
    ui->statusBar->showMessage("Error: " + message);
}

