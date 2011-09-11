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

#include "AnsiFile.h"
#include "FileType.h"

#include "ModPlayer.h"
#include "DigiBoosterPlayer.h"
#include "DigiBoosterProPlayer.h"
#include "SymphoniePlayer.h"
#include "TfmxPlayer.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pFileBuffer(nullptr),
    m_pModPlayer(nullptr),
    m_pDecodeBuffer(nullptr),
	m_pAudioOut(nullptr)
{
    ui->setupUi(this);
    
    // we can adjust size later,
    // also reusable with suffcient buffer size
    m_pDecodeBuffer = new CReadBuffer();
    m_pFileBuffer = new CReadBuffer();
}

MainWindow::~MainWindow()
{
    // TODO: deleted automatically by output-device?
	m_pDevOut = nullptr;
    
    if (m_pAudioOut != nullptr)
	{
		m_pAudioOut->stop();
		delete m_pAudioOut;
	}

    if (m_pModPlayer != nullptr)
    {
        delete m_pModPlayer;
        m_pModPlayer = nullptr;
    }
    delete m_pFileBuffer;
    delete m_pDecodeBuffer;
    
    delete ui;
}

CModPlayer *MainWindow::GetPlayer(CReadBuffer *fileBuffer) const
{
    if (fileBuffer->GetSize() < 16)
    {
        // nothing useful in the file..
        return nullptr;
    }
    
    CModPlayer *pModPlayer = nullptr;
    CFileType type;
    type.DetermineFileType(fileBuffer->GetBegin(), 16);
    switch (type.m_enFileType)
    {
    case HEADERTYPE_MOD:
        //pModPlayer = new CProTrackerPlayer(fileBuffer);
        break;
        
    case HEADERTYPE_OCTAMED:
        //pModPlayer = new COctaMedPlayer(fileBuffer);
        break;
    case HEADERTYPE_OCTAMED_OSS:
        //pModPlayer = new COctaMedOSSPlayer(fileBuffer);
        break;
        
    case HEADERTYPE_SYMMOD:
        pModPlayer = new CSymphoniePlayer(fileBuffer);
        break;

    case HEADERTYPE_DIGIBOOSTER:
        pModPlayer = new CDigiBoosterPlayer(fileBuffer);
        break;
    case HEADERTYPE_DBMPRO:
        pModPlayer = new CDigiBoosterProPlayer(fileBuffer);
        break;
        
    case HEADERTYPE_AHX:
        //pModPlayer = new CAhxPlayer(fileBuffer);
        break;
        
    case HEADERTYPE_TFMX:
        pModPlayer = new CTfmxPlayer(fileBuffer);
        break;
        
    case HEADERTYPE_OKTALYZER:
        //pModPlayer = new COktalyzerPlayer(fileBuffer);
        break;
    
        /*
    case HEADERTYPE_NOISETRACKER:
        //pModPlayer = new CNoisetrackerPlayer(fileBuffer);
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

    // note: change to memory-mapped files later..
    CAnsiFile file(filename.toStdString());
    if (file.IsOk() == false)
    {
        ui->statusBar->showMessage("Failed to open file: " + filename);
        return;
    }

    // clear&reuse or allocate new if necessary
    m_pFileBuffer->PrepareBuffer(file.GetSize(), false);
    if (file.Read(m_pFileBuffer->GetBegin(), file.GetSize()) == false)
    {
        ui->statusBar->showMessage("Failed to read file: " + filename);
        return;
    }
    file.Close(); // can close already

    // keep as member also..
    m_pModPlayer = GetPlayer(m_pFileBuffer);
    if (m_pModPlayer == nullptr)
    {
        ui->statusBar->showMessage("Failed to create player");
        return;
    }
    
    if (m_pModPlayer->ParseFileInfo() == false)
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

    /*
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
    //CReadBuffer decodeBuffer(nBuffer);
    m_pDecodeBuffer->PrepareBuffer(nBuffer, false);
    
    // placeholder..
    //size_t nInBuf = m_pModPlayer->Decode(m_pDecodeBuffer->GetBegin(), m_pDecodeBuffer->GetSize());
    //qint64 nWritten = m_pDevOut->write(m_pDecodeBuffer->GetBegin(), nInBuf);
    */
}


void MainWindow::on_actionPlay_triggered()
{
    // debug
    //QString filename = "c:/tmpmods/Nexus7-Theme";
    //QString filename = "C:/tmpmods/Breathless.SymMOD";
    //QString filename = "C:/tmpmods/plastic elements.digi";
    //QString filename = "C:/tmpmods/the_falling_log.dbm";
    //QString filename = "C:/tmpmods/AHX.BH 90210";
    //QString filename = "C:/tmpmods/mdat.encounter";
    //QString filename = "C:/tmpmods/smpl.encounter";
    //QString filename = "C:/tmpmods/mod.3DDemoTune";
    //QString filename = "C:/tmpmods/OKT.Tekknomania";
    //QString filename = "C:/tmpmods/WalkingInTheAir.oss.med";
    
    // get selection from list
    QListWidgetItem *pItem = ui->listWidget->currentItem();
    if (pItem == NULL)
    {
        return;
    }
    
    PlayFile(pItem->text());
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
    
    if (m_pModPlayer != nullptr)
    {
        delete m_pModPlayer;
        m_pModPlayer = nullptr;
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
