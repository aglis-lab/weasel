#pragma once

#include <vector>
#include <thread>

#include <weasel/Parser/Parser.h>

using namespace std;

namespace weasel
{
    class ParserPool
    {
    private:
        vector<unique_ptr<Parser>> _pools;

    public:
        ParserPool() = default;

        void createPool(SourceBuffer buffer, FileASTHandle moduleHandle)
        {
            _pools.push_back(make_unique<Parser>(buffer, moduleHandle));
        }

        void wait()
        {
            vector<thread> threads;
            for (auto &item : _pools)
            {
                threads.push_back(item->launchParse());
            }

            for (auto &item : threads)
            {
                item.join();
            }
        }
    };
} // namespace weasel
