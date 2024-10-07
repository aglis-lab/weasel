#include <weasel/Source/SourceManager.h>
#include <filesystem>

using namespace weasel;

void SourceManager::closeSource()
{
    for (auto item : _sources)
    {
        item.closeBuffer();
    }
}

void SourceManager::loadSource(string sourcePath)
{
    // Source Path
    for (auto &path : filesystem::directory_iterator(sourcePath))
    {
        if (path.is_directory())
        {
            continue;
        }

        _sources.push_back(SourceBuffer(path.path(), _fileId++));
        assert(_sources.back().isValid());
    }

    // Folder Inside Source Path
}
