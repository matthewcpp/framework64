#include "framework64/desktop/file_datasource.hpp"

namespace framework64 {

FileDataSource::FileDataSource() {
    interface.read = FileDataSource::readFunc;
    interface.size = FileDataSource::sizeFunc;
}

bool FileDataSource::open(std::filesystem::path const & filesystem_path) {
    if (!std::filesystem::exists(filesystem_path))
        return false;

    file.open(filesystem_path, std::ios::binary);

    if (!file) {
        file.close();
        return false;
    }

    file_size = static_cast<size_t>(std::filesystem::file_size(filesystem_path));

    return true;
}

void FileDataSource::close() {
    file.close();
}

size_t FileDataSource::sizeFunc(fw64DataSource* data_source) {
    auto* file_data_source = reinterpret_cast<FileDataSource*>(data_source);

    return file_data_source->file_size;
}

size_t FileDataSource::readFunc(fw64DataSource* data_source, void* buffer, size_t size, size_t count) {
    auto* file_data_source = reinterpret_cast<FileDataSource*>(data_source);
    file_data_source->file.read(reinterpret_cast<char*>(buffer), size * count);

    return file_data_source->file.gcount();
}


FileDataWriter::FileDataWriter() {
    interface.write = writeFunc;
}

bool FileDataWriter::open(std::filesystem::path const & filesystem_path) {
    file.open(filesystem_path, std::ios::binary);

    if (!file) {
        file.close();
        return false;
    }

    return true;
}

void FileDataWriter::close() {
    file.close();
}

size_t FileDataWriter::writeFunc(fw64DataWriter* data_writer, const char* buffer, size_t size, size_t count) {
    auto* file_writer = reinterpret_cast<FileDataWriter*>(data_writer);

    auto start_pos = file_writer->file.tellp();
    file_writer->file.write(buffer, size * count);
    auto end_pos = file_writer->file.tellp();

    return static_cast<size_t>(end_pos - start_pos);
}

}
