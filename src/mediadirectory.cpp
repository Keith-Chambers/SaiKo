#include "mediadirectory.h"

MediaDirectory::MediaDirectory(const QDir& directory, const QString& image_path)
    : m_directory {directory}, m_image_path {image_path}
{

}
