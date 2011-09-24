#include "ArchiveHandler.h"

// for CReadBuffer
#include "AnsiFile.h"



ArchiveHandler::ArchiveHandler(QObject *parent) :
    QObject(parent),
    m_type()
{
}

bool ArchiveHandler::openArchive(QString &archiveFile)
{
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
	
	if (m_type.m_enFileType == HEADERTYPE_LHA)
	{
		QLhALib *pLhaLib = new QLhALib(this);
		pLhaLib->SetArchive(archiveFile);
		//pLhaLib->List(); // -> file list for playlist
	}
	else if (m_type.m_enFileType == HEADERTYPE_LZX)
	{
		QLZXLib *pLzxLib = new QLZXLib(this);
		pLzxLib->SetArchive(archiveFile);
		//pLzxLib->List(); // -> file list for playlist
	}
	/*
	else if (m_type.m_enFileType == HEADERTYPE_PP20)
	{
		// PowerPacker crunched file
		// -> handle with XPK library
	}
	else if (m_type.m_enFileType == HEADERTYPE_IMPLODER)
	{
		// Imploder crunched file
		// -> handle with XPK library
	}
	*/
	/*
	else if (m_type.m_enFileType == HEADERTYPE_7Z)
	{
		// 7-zip, not yet supported
	}
	else if (m_type.m_enFileType == HEADERTYPE_XZ)
	{
		// XZ, not yet supported
	}
	*/
	else
	{
		// just use XPK-library for rest,
		// it should know how to decompress whatever it is packed with..
		QXpkLib *pXpkLib = new QXpkLib(this);
		pXpkLib->setInputFile(archiveFile);
	}
	
}

/*
bool ArchiveHandler::openArchive(QFile *pFile)
{
	m_type.DetermineFileType(pFile->
}
*/

bool ArchiveHandler::openArchive(uchar *pView, qint64 nSize)
{
	m_type.DetermineFileType(pView, nSize);
	if (m_type.m_enFileCategory != HEADERCAT_ARCHIVE
		&& m_type.m_enFileCategory != HEADERCAT_PACKER)
	{
		// not compressed ?
		return false;
	}
}

// single compressed file (XPK, PowerPacker, Imploder, GZip..)
// -> just decrunch
CReadBuffer *ArchiveHandler::decrunchToBuffer()
{
	// should have this already..
	//QXpkLib *pXpkLib = new QXpkLib(this);
	//pXpkLib->setInputFile(archiveFile);
	
	// unpack to our buffer
	//pXpkLib->xpkUnpack();
	
}

// multi-file archive: need to know which file to decrunch
// (Lha, LZX, 7-zip..)
CReadBuffer *ArchiveHandler::decrunchToBuffer(QString &filename)
{
	// should have either of these already
	//QLhALib *pLhaLib = new QLhALib(this);
	//QLZXLib *pLzxLib = new QLZXLib(this);
	
	//pLhaLib->ExtractToBuffer(filename, buffer);
	
	// TODO: should add similar extract-to-buffer
	// method but becomes tricky when in merged-group
	// in compressed file 
	// -> need to decompress other files in same group also..
	//
	//pLzxLib->Extract(filename);
}

