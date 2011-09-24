#include "ArchiveHandler.h"

// for CReadBuffer
#include "AnsiFile.h"

// detect what kind of file there is in the archive
#include "FileType.h"


ArchiveHandler::ArchiveHandler(QObject *parent) :
    QObject(parent)
{
}

bool ArchiveHandler::openArchive(QString &archiveFile)
{
}

bool ArchiveHandler::openArchive(QFile *pFile)
{
}

CReadBuffer *ArchiveHandler::decrunchToBuffer()
{
}

