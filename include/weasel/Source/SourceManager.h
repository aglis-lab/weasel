#pragma once

#include <vector>

#include <weasel/Source/SourceBuffer.h>

using namespace std;

namespace weasel
{
    class SourceManager
    {
    public:
        SourceManager() {}

        void closeSource()
        {
            for (auto item : _sources)
            {
                item.closeBuffer();
            }
        }

        void loadSource(string sourcePath);

    private:
        vector<SourceBuffer> _sources;
    };
} // namespace weasel
