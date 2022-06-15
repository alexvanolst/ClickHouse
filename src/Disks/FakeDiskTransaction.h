#pragma once

#include <Disks/IDiskTransaction.h>

namespace DB
{

struct FakeDiskTransaction : public IDiskTransaction
{
public:
    explicit FakeDiskTransaction(IDisk & disk_)
        : disk(disk_)
    {}

    void commit() override {}

    void createDirectory(const std::string & path) override
    {
        disk.createDirectory(path);
    }

    void createDirectories(const std::string & path) override
    {
        disk.createDirectories(path);
    }

    void clearDirectory(const std::string & path) override
    {
        disk.createDirectory(path);
    }

    void moveDirectory(const std::string & from_path, const std::string & to_path) override
    {
        disk.moveDirectory(from_path, to_path);
    }

    void replaceFile(const std::string & from_path, const std::string & to_path) override
    {
        disk.replaceFile(from_path, to_path);
    }

    void copy(const std::string & from_path, const std::shared_ptr<IDisk> & to_disk, const std::string & to_path) override
    {
        disk.copy(from_path, to_disk, to_path);
    }

    void copyDirectoryContent(const std::string & from_dir, const std::shared_ptr<IDisk> & to_disk, const std::string & to_dir) override
    {
        disk.copyDirectoryContent(from_dir, to_disk, to_dir);
    }

    void copyFile(const std::string & from_file_path, IDisk & to_disk, const std::string & to_file_path) override
    {
        disk.copyFile(from_file_path, to_disk, to_file_path);
    }

    std::unique_ptr<WriteBufferFromFileBase> writeFile( /// NOLINT
        const std::string & path,
        size_t buf_size = DBMS_DEFAULT_BUFFER_SIZE,
        WriteMode mode = WriteMode::Rewrite,
        const WriteSettings & settings = {}) override
    {
        return disk.writeFile(path, buf_size, mode, settings);
    }

    void removeFile(const std::string & path) override
    {
        disk.removeFile(path);
    }

    void removeFileIfExists(const std::string & path) override
    {
        disk.removeFileIfExists(path);
    }

    void removeDirectory(const std::string & path) override
    {
        disk.removeDirectory(path);
    }

    void removeRecursive(const std::string & path) override
    {
        disk.removeRecursive(path);
    }

    void removeSharedFile(const std::string & path, bool keep_shared_data) override
    {
        disk.removeSharedFile(path, keep_shared_data);
    }

    void removeSharedRecursive(const std::string & path, bool keep_all_shared_data, const NameSet & file_names_remove_metadata_only) override
    {
        disk.removeSharedRecursive(path, keep_all_shared_data, file_names_remove_metadata_only);
    }

    void removeSharedFileIfExists(const std::string & path, bool keep_shared_data) override
    {
        disk.removeSharedFileIfExists(path, keep_shared_data);
    }

    void removeSharedFiles(const RemoveBatchRequest & files, bool keep_all_batch_data, const NameSet & file_names_remove_metadata_only) override
    {
        disk.removeSharedFiles(files, keep_all_batch_data, file_names_remove_metadata_only);
    }

    void setLastModified(const std::string & path, const Poco::Timestamp & timestamp) override
    {
        disk.setLastModified(path, timestamp);
    }

    void setReadOnly(const std::string & path) override
    {
        disk.setReadOnly(path);
    }

    void createHardLink(const std::string & src_path, const std::string & dst_path) override
    {
        disk.createHardLink(src_path, dst_path);
    }

    /// Truncate file to specified size.
    void truncateFile(const std::string & path, size_t size) override
    {
        disk.truncateFile(path, size);
    }

private:
    IDisk & disk;
};

}
