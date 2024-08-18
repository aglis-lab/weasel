#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <assert.h>

using namespace std;

using FileId = uint;

namespace weasel
{
    struct SourceDetail
    {
        string FilePath;
        char *Buffer = nullptr;
        uint Size;

        explicit SourceDetail() = default;
        explicit SourceDetail(string filePath, char *buffer, uint size)
        {
            this->FilePath = filePath;
            this->Buffer = buffer;
            this->Size = size;
        }
    };

    class FileManager
    {
    public:
        explicit FileManager(const string &filePath);

        bool isValid() const { return _startBuffer != nullptr; }
        uint getSize() const { return _size; }
        char *getStartBuffer() const { return _startBuffer; }
        char *getEndBuffer() const { return _startBuffer + _size; }

    private:
        char *_startBuffer;
        uint _size;

        char *mapFile(const char *path, uint *length);

        // TODO: New File Manager Convention
        // public:
        //     explicit FileManager(const FileManager &) = delete;

        //     static FileManager &GetInstance()
        //     {
        //         static FileManager instance;
        //         return instance;
        //     }

        //     void LoadSource(string sourcePath);
        //     void Close();

        //     size_t GetSourcesCount() const { return sourceMap.size(); }
        //     vector<FileId> GetFileIds();

        //     char *GetBuffer(FileId fileId, uint location)
        //     {
        //         assert(sourceMap.contains(fileId));

        //         return sourceMap[fileId].Buffer + location;
        //     }

        // private:
        //     explicit FileManager() = default;

        //     char *MapFile(const char *path, uint *length);

        //     FileId currentFileId = 0;
        //     unordered_map<FileId, SourceDetail> sourceMap = {};
    };

    class FileManagerLegacy
    {
    public:
        explicit FileManagerLegacy(const string &filePath);

        bool isValid() const { return _startBuffer != nullptr; }
        uint getSize() const { return _size; }
        char *getStartBuffer() const { return _startBuffer; }
        char *getEndBuffer() const { return _startBuffer + _size; }

        void loadFiles();

    private:
        char *_startBuffer;
        uint _size;

        char *mapFile(const char *path, uint *length);
    };
} // namespace weasel
