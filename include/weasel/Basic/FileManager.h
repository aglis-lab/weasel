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

        bool isValid() const { return _startBuffer != nullptr; }
        size_t getSize() const { return _size; }
        char *getStartBuffer() const { return _startBuffer; }
        char *getEndBuffer() const { return _endBuffer; }

    protected:
        char *_startBuffer;
        char *_endBuffer;
        size_t _size;

    private:
        char *mapFile(const char *path, size_t *length);
    };

} // namespace weasel
