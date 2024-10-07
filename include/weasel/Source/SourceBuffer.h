#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <assert.h>

using namespace std;

using FileId = uint;

namespace weasel
{
    class SourceBuffer
    {
    public:
        explicit SourceBuffer(const string &filePath);

        bool isValid() const { return _startBuffer != nullptr; }
        uint getSize() const { return _size; }
        char *getStartBuffer() const { return _startBuffer; }
        char *getEndBuffer() const { return _startBuffer + _size; }
        void closeBuffer() { delete (_startBuffer); }

    private:
        char *_startBuffer;
        uint _size;
    };
} // namespace weasel
