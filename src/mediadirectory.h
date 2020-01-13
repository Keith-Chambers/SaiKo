#ifndef MEDIADIRECTORY_H
#define MEDIADIRECTORY_H

#include <QDir>

class MediaDirectory
{
public:
    MediaDirectory(const QDir& directory, const QString& image_path);

    const QDir& directory() const { return m_directory; }
    const QString& imagePath() const { return m_image_path; }

private:
    QDir m_directory;
    QString m_image_path;
};

#endif // MEDIADIRECTORY_H
