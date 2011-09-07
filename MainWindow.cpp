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
    
    //CModPlayer *pModPlayer = nullptr;
    CFileType type;
    type.DetermineFileType(filebuf.GetBegin(), 16);
    
    switch (type.m_enFileType)
    {
    case HEADERTYPE_MOD:
        //pModPlayer = new CProTrackerPlayer();
        break;
        
    case HEADERTYPE_OCTAMED:
        //pModPlayer = new COctaMedPlayer();
        break;
        
    case HEADERTYPE_SYMMOD:
        //pModPlayer = new CSymphoniePlayer();
        break;

    case HEADERTYPE_DBM:
    case HEADERTYPE_DBPRO:
        //pModPlayer = new CDigiBoosterPlayer();
        break;

        /*        
    case HEADERTYPE_XM:
        // fast tracker
        break;
        
    case HEADERTYPE_S3M:
        // scream tracker 3
        break;
        
    case HEADERTYPE_IT:
        //pModPlayer = new CImpulseTrackerPlayer();
        break;
        */
        
    default:
        ui->statusBar->showMessage("Unsupported format");
        break;
    }

    /*
    if (pModPlayer == nullptr)
    {
        ui->statusBar->showMessage("Failed to create player");
        return;
    }
    */
    
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (info.isNull() == true)
    {
        ui->statusBar->showMessage("Failed to get default audio output");
		return;
    }

    //m_pAudioOut = new QAudioOutput(format, this);
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
