#include "ArchiveHandler.h"

// for CReadBuffer
#include "AnsiFile.h"



ArchiveHandler::ArchiveHandler(QObject *parent) :
    QObject(parent),
    m_type(),
    m_xpklib(nullptr)
{
	m_xpklib = new QXpkLib(this);
}

ArchiveHandler::~ArchiveHandler(void)
{
	if (m_xpklib != nullptr)
	{
		delete m_xpklib;
	}
}

bool ArchiveHandler::openArchive(QString &archiveFile)
{
	QXpkLib::CArchiveInfo info; // -> keep as member

	m_xpklib->setInputFile(archiveFile);
	if (m_xpklib->xpkInfo(info) == false)
	{
		// not supported
		return false;
	}

	/*
    QFile *pFile = new QFile(filename);
    qint64 nSize = m_pFile->size();
    uchar *pView = m_pFile->map(0, nSize);
	m_type.DetermineFileType(pView, nSize);
	
	if (m_type.m_enFileCategory != HEADERCAT_ARCHIVE
		&& m_type.m_enFileCategory != HEADERCAT_PACKER)
	{
		// not compressed ?
		return false;
	}

	if (m_type.m_enFileType == HEADERTYPE_UNKNOWN)
	{
		// unknown/unsupported
		return false;
	}
	*/
	
}

/*
bool ArchiveHandler::openArchive(QFile *pFile)
{
	m_type.DetermineFileType(pFile->
}
*/

bool ArchiveHandler::openArchive(uchar *pView, qint64 nSize)
{
	/*
	m_type.DetermineFileType(pView, nSize);
	if (m_type.m_enFileCategory != HEADERCAT_ARCHIVE
		&& m_type.m_enFileCategory != HEADERCAT_PACKER)
	{
		// not compressed ?
		return false;
	}
	*/
}

// single compressed file (XPK, PowerPacker, Imploder, GZip..)
// -> just decrunch
CReadBuffer *ArchiveHandler::decrunchToBuffer()
{
	m_xpklib->getToBuffer(temp);
}

// multi-file archive: need to know which file to decrunch
// (Lha, LZX, 7-zip..)
CReadBuffer *ArchiveHandler::decrunchToBuffer(QString &filename)
{
	CReadBuffer *temp;
	m_xpklib->getToBuffer(filename, temp);
}

