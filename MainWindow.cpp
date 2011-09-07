#include "MainWindow.h"
#include "ui_MainWindow.h"


#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QDebug>

// for testing
#include <QThread>

#include "AnsiFile.h"
#include "FileType.h"

#include "ModPlayer.h"
#include "DigiBoosterPlayer.h"
#include "SymphoniePlayer.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
	m_pAudioOut(nullptr)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    if (m_pAudioOut != nullptr)
	{
		m_pAudioOut->stop();
		delete m_pAudioOut;
	}
    delete ui;
}

CModPlayer *MainWindow::GetPlayer(CReadBuffer &fileBuffer)
{
    if (fileBuffer.GetSize() < 16)
    {
        // nothing useful in the file..
        return nullptr;
    }
    
    CModPlayer *pModPlayer = nullptr;
    CFileType type;
    type.DetermineFileType(fileBuffer.GetBegin(), 16);
    switch (type.m_enFileType)
    {
    case HEADERTYPE_MOD:
        //pModPlayer = new CProTrackerPlayer();
        break;
        
    case HEADERTYPE_OCTAMED:
        //pModPlayer = new COctaMedPlayer();
        break;
        
    case HEADERTYPE_SYMMOD:
        pModPlayer = new CSymphoniePlayer(&fileBuffer);
        break;

    case HEADERTYPE_DBM:
        pModPlayer = new CDigiBoosterPlayer(&fileBuffer);
        break;
        
    case HEADERTYPE_DBPRO:
        // TODO: may need different for this..
        break;
        
    case HEADERTYPE_AHX:
        break;
        
    case HEADERTYPE_OKTALYZER:
        break;

    default:
        // just to silence GCC..
        break;
    }
    return pModPlayer;
}


void MainWindow::on_actionPlay_triggered()
{
    std::string filename = "c:/tmpmods/Nexus7-Theme";

    // TODO: different thread for module handling?
    
    CAnsiFile file(filename);
    if (file.IsOk() == false)
    {
        ui->statusBar->showMessage("Failed to open file: " + QString::fromStdString(filename));
        return;
    }
    
    CReadBuffer filebuf(file.GetSize());
    if (file.Read(filebuf.GetBegin(), filebuf.GetSize()) == false)
    {
        ui->statusBar->showMessage("Failed to read file: " + QString::fromStdString(filename));
        return;
    }
    
    CModPlayer *pModPlayer = GetPlayer(filebuf);
    if (pModPlayer == nullptr)
    {
        ui->statusBar->showMessage("Failed to create player");
        return;
    }
    
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (info.isNull() == true)
    {
        ui->statusBar->showMessage("Failed to get default audio output");
		return;
    }
    
    // 
    //QAudioFormat format = pModPlayer->GetOutputFormat();

    //m_pAudioOut = new QAudioOutput(format, this);
    //
	//connect(m_pAudioOut, SIGNAL(stateChanged(QAudio::State)), this, SLOT(onAudioState(QAudio::State)));
	//connect(m_pAudioOut, SIGNAL(notify()), this, SLOT(onPlayNotify()));
    
    
}

void MainWindow::on_actionStop_triggered()
{
    // TODO: deleted automatically by output-device?
	m_pDevOut = nullptr;
	
	if (m_pAudioOut != nullptr)
	{
		m_pAudioOut->stop();
		delete m_pAudioOut;
		m_pAudioOut = nullptr;
	}
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
