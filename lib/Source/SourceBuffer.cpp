#include <weasel/Basic/MapFile.h>
#include <weasel/Source/SourceBuffer.h>

using namespace weasel;

// instead of using ifstream
SourceBuffer::SourceBuffer(const std::string &filePath, FileId fileId)
{
    _startBuffer = MapFile(filePath.c_str(), &_size);
    _fileId = fileId;
}
