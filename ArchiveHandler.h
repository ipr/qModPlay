#ifndef ARCHIVEHANDLER_H
#define ARCHIVEHANDLER_H

#include <QObject>
#include <QFile>
#include <QString>
#include <QList>

// LZX-archive decompression
#include "qlzxlib.h"

// LhA-archive decompression
#include "qlhalib.h"

// generic cruncher decompression
// (XPK crunchers, PowerPacker, Imploder, GZip..)
#include "qxpklib.h"

// detect what kind of file there is in the archive
#include "FileType.h"


class CReadBuffer;

class ArchiveHandler : public QObject
{
    Q_OBJECT
public:
    explicit ArchiveHandler(QObject *parent = 0);

	bool openArchive(QString &archiveFile);
	//bool openArchive(QFile *pFile);
	bool openArchive(uchar *pView, qint64 nSize);

	// decrunch file to buffer (single-file compression only)
	CReadBuffer *decrunchToBuffer();
	//CReadBuffer *decrunchToBuffer(uchar *pView, qint64 nSize);

	// decrunch given file from archive to buffer (multi-file archive)
	CReadBuffer *decrunchToBuffer(QString &filename);

signals:

public slots:

private:
	CFileType m_type;
	
	// files in archive..
	class FileInfo
	{
	public:
		FileInfo()
			: m_name()
			, m_uncompressedSize(0)
		{}
		QString m_name;
		size_t m_uncompressedSize;
	};
	QList<FileInfo> m_Files;
};

#endif // ARCHIVEHANDLER_H
