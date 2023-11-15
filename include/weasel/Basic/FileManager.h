#pragma once

// I modify the implementation from
// https://github.com/d0k/mappedfile

namespace weasel
{
    class FileManager
    {

    public:
        FileManager(const std::string filePath);

        bool isValid() const { return _startBuffer != nullptr; }
        size_t getSize() const { return _size; }
        char *getStartBuffer() const { return _startBuffer; }

    protected:
        char *_startBuffer;
        size_t _size;

    private:
        char *mapFile(const char *path, size_t *length);
    };

} // namespace weasel
