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

    case HEADERTYPE_DIGIBOOSTER:
        pModPlayer = new CDigiBoosterPlayer(&fileBuffer);
        break;
        
    case HEADERTYPE_DBPRO:
        // TODO: may need different for this..
        break;
        
    case HEADERTYPE_AHX:
        //pModPlayer = new CAhxPlayer();
        break;
        
    case HEADERTYPE_OKTALYZER:
        //pModPlayer = new COktalyzerPlayer();
        break;
    
        /*
    case HEADERTYPE_NOISETRACKER:
        //pModPlayer = new CNoisetrackerPlayer();
        break;
        */

    default:
        // just to silence GCC..
        break;
    }
    return pModPlayer;
}

void MainWindow::PlayFile(QString &filename)
{
    // TODO: different thread for module handling?
    
    CAnsiFile file(filename.toStdString());
    if (file.IsOk() == false)
    {
        ui->statusBar->showMessage("Failed to open file: " + filename);
        return;
    }

    // TODO: keep as member..
    // while debugging, release when exiting..
    CReadBuffer filebuf(file.GetSize());
    if (file.Read(filebuf.GetBegin(), filebuf.GetSize()) == false)
    {
        ui->statusBar->showMessage("Failed to read file: " + filename);
        return;
    }

    // keep as member also..
    CModPlayer *pModPlayer = GetPlayer(filebuf);
    if (pModPlayer == nullptr)
    {
        ui->statusBar->showMessage("Failed to create player");
        return;
    }
    
    if (pModPlayer->ParseFileInfo() == false)
    {
        ui->statusBar->showMessage("File could not be handled");
        return;
    }
    
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (info.isNull() == true)
    {
        ui->statusBar->showMessage("Failed to get default audio output");
		return;
    }
    
    // TODO: get info on what is suitable format for playing..
    // (what is supported by player/format/module..)
    //QAudioFormat format = pModPlayer->GetOutputFormat();

    // placeholder..
    QAudioFormat format;
    m_pAudioOut = new QAudioOutput(format, this);
    
    // TODO: change&notify handlers (push new data when enough has played)
	//connect(m_pAudioOut, SIGNAL(stateChanged(QAudio::State)), this, SLOT(onAudioState(QAudio::State)));
	//connect(m_pAudioOut, SIGNAL(notify()), this, SLOT(onPlayNotify()));
    
    // count size of buffer for decoding:
    // channels, samplesize, frequency etc.
    // just guess for now: 
    // expect 2-channels and 16-bit samples at 44.1kHz,
    // get proper values later
    int64_t nBuffer = ( 2 * 2 * 44100);

    int iBufSize = m_pAudioOut->bufferSize();
	if (iBufSize < nBuffer)
	{
		m_pAudioOut->setBufferSize(nBuffer);
	}
    m_pAudioOut->setNotifyInterval(250); // 250ms
    
    // create buffer for decoding 
    //QByteArray decodeBuffer();
    CReadBuffer decodeBuffer(nBuffer);
    
    // placeholder..
    size_t nInBuf = pModPlayer->Decode(decodeBuffer.GetBegin(), decodeBuffer.GetSize());
    qint64 nWritten = m_pDevOut->write(decodeBuffer.GetBegin(), nInBuf);
}


void MainWindow::on_actionPlay_triggered()
{
    // TODO: get selection from list..
    
    //QString filename = "c:/tmpmods/Nexus7-Theme";
    QString filename = "C:/tmpmods/Breathless.SymMOD";


    PlayFile(filename);    
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
