#pragma once

// I modify the implementation from
// https://github.com/d0k/mappedfile

using namespace std;

namespace weasel
{
    class FileManager
    {

    public:
        explicit FileManager(const string &filePath);
        ~FileManager() {}

        bool isValid() const { return _startBuffer != nullptr; }
        uint getSize() const { return _size; }
        char *getStartBuffer() const { return _startBuffer; }
        char *getEndBuffer() const { return _startBuffer + _size; }

    protected:
        char *_startBuffer;
        char *_endBuffer;
        uint _size;

    private:
        char *mapFile(const char *path, uint *length);
    };
} // namespace weasel
