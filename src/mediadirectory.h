#ifndef MEDIADIRECTORY_H
#define MEDIADIRECTORY_H

#include <QDir>

class MediaDirectory
{
public:
    MediaDirectory(const QDir& directory, const QString& image_path);

    const QDir& directory(){ return m_directory; }
    const QString& imagePath(){ return m_image_path; }

private:
    QDir m_directory;
    QString m_image_path;
};

#endif // MEDIADIRECTORY_H
