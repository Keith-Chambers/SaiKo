#ifndef KPL_AUDIOFILE_H
#define KPL_AUDIOFILE_H

#include <kpl/core.h>
#include <kpl/filesystem.h>

#include <fileref.h>
#include <tag.h>
#include <mpeg/id3v2/id3v2tag.h>
#include <mpeg/id3v2/id3v2header.h>
#include <mpeg/id3v2/id3v2frame.h>
#include <mpeg/mpegfile.h>
#include <mpeg/id3v2/frames/attachedpictureframe.h>

namespace kpl
{
    namespace audio {

        kpl::String extractTitle(kpl::filesystem::FileIdentifier file);
        kpl::String extractArtist(kpl::filesystem::FileIdentifier file);
        kpl::String extractAlbum(kpl::filesystem::FileIdentifier file);
        kpl::String extractGenre(kpl::filesystem::FileIdentifier file);
        i32 extractReleaseYear(kpl::filesystem::FileIdentifier file);
        i32 extractTrackNumber(kpl::filesystem::FileIdentifier file);

        TagLib::Tag* extractTag(kpl::filesystem::FileIdentifier file);

        kpl::String extractTitle(TagLib::Tag *tag);
        kpl::String extractArtist(TagLib::Tag *tag);
        kpl::String extractAlbum(TagLib::Tag *tag);
        kpl::String extractGenre(TagLib::Tag *tag);
        i32 extractReleaseYear(TagLib::Tag *tag);
        i32 extractTrackNumber(TagLib::Tag *tag);

        i32 extractLengthSeconds(kpl::filesystem::FileIdentifier file);

        ContinguousMemory extractAlbumArt(kpl::filesystem::FileIdentifier file);
    }
}


#endif // KPL_AUDIOFILE_H
