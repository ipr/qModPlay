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


class CReadBuffer;

class ArchiveHandler : public QObject
{
    Q_OBJECT
public:
    explicit ArchiveHandler(QObject *parent = 0);

	bool openArchive(QString &archiveFile);
	bool openArchive(QFile *pFile);

	// decrunch next file to buffer from archive
	CReadBuffer *decrunchToBuffer();

signals:

public slots:

private:
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
