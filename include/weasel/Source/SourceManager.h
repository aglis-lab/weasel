#pragma once

#include <vector>

#include <weasel/Source/SourceBuffer.h>

using namespace std;

namespace weasel
{
    class SourceManager
    {
    private:
        vector<SourceBuffer> _sources;
        FileId _fileId = 1;

    public:
        SourceManager() {}

        void closeSource();
        void loadSource(string sourcePath);
        vector<SourceBuffer> &getSources() { return _sources; }
    };
} // namespace weasel
