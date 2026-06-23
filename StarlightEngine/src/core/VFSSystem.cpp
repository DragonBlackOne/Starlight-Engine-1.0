#include "VFSSystem.hpp"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include "Engine.hpp"
#include "Log.hpp"
#include "PathResolver.hpp"
#include "miniz.h"

namespace starlight {

VFSSystem& VFSSystem::Get() {
    if (Engine::IsInitialized()) {
        auto sys = Engine::Get().GetSystem<VFSSystem>();
        if (sys)
            return *sys;
    }
    static VFSSystem fallback;
    return fallback;
}

void VFSSystem::Initialize() {
    Log::Info("VFS System: Initialized for Open Source SDK.");
}

void VFSSystem::Shutdown() {
    m_mounts.clear();
    m_pakEntries.clear();
    m_isZipPak = false;
    m_currentPakPath.clear();
}

void VFSSystem::Mount(const std::string& virtualPath, const std::string& physicalPath) {
    m_mounts[virtualPath] = physicalPath;
    Log::Info("VFS: Mounted " + physicalPath + " to " + virtualPath);
}

bool VFSSystem::LoadPak(const std::string& pakPath) {
    std::ifstream file(pakPath, std::ios::binary);
    if (!file.is_open())
        return false;

    // Formato simples: [MAGIC 4B][COUNT 4B]...[ENTRIES]
    char magic[4];
    file.read(magic, 4);
    if (strncmp(magic, "TPAK", 4) == 0) {
        uint32_t count;
        file.read((char*)&count, 4);

        for (uint32_t i = 0; i < count; i++) {
            char name[256];
            file.read(name, 256);
            name[255] = '\0';  // Hardened: prevent buffer overread if PAK is malformed
            PakEntry entry;
            file.read((char*)&entry.offset, 8);
            file.read((char*)&entry.size, 8);
            m_pakEntries[name] = entry;
        }

        m_isZipPak = false;
        m_currentPakPath = pakPath;
        Log::Info("VFS: Loaded PAK (TPAK): " + pakPath + " (" + std::to_string(count) + " files)");
        return true;
    }

    // Try standard ZIP format
    file.close();
    mz_zip_archive zipArchive;
    memset(&zipArchive, 0, sizeof(zipArchive));
    if (mz_zip_reader_init_file(&zipArchive, pakPath.c_str(), 0)) {
        mz_uint numFiles = mz_zip_reader_get_num_files(&zipArchive);
        for (mz_uint i = 0; i < numFiles; i++) {
            mz_zip_archive_file_stat fileStat;
            if (mz_zip_reader_file_stat(&zipArchive, i, &fileStat)) {
                if (mz_zip_reader_is_file_a_directory(&zipArchive, i))
                    continue;
                PakEntry entry;
                entry.offset = 0;
                entry.size = fileStat.m_uncomp_size;
                entry.compressedSize = fileStat.m_comp_size;
                m_pakEntries[fileStat.m_filename] = entry;
            }
        }
        mz_zip_reader_end(&zipArchive);
        m_isZipPak = true;
        m_currentPakPath = pakPath;
        Log::Info("VFS: Loaded PAK (ZIP): " + pakPath + " (" + std::to_string(m_pakEntries.size()) + " files)");
        return true;
    }

    return false;
}

std::string VFSSystem::Resolve(const std::string& path) {
    if (path.empty())
        return path;

    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');

    // Se estiver no PAK, o Resolve nao retorna um caminho fisico, o ReadFile cuidara disso
    if (m_pakEntries.count(normalized))
        return normalized;

    if (normalized[0] == '@') {
        size_t firstSlash = normalized.find('/', 1);
        std::string alias = (firstSlash == std::string::npos) ? normalized : normalized.substr(0, firstSlash);

        if (m_mounts.count(alias)) {
            std::string remainder = (firstSlash == std::string::npos) ? "" : normalized.substr(firstSlash);
            return m_mounts[alias] + remainder;
        }
    }

    return PathResolver::Resolve(path);
}

std::vector<uint8_t> VFSSystem::ReadFile(const std::string& path) {
    std::string resolved = Resolve(path);
    std::string normalized = resolved;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');

    // Se estiver no PAK
    if (m_pakEntries.count(normalized)) {
        if (m_isZipPak) {
            size_t uncompSize = 0;
            void* pBuffer = mz_zip_extract_archive_file_to_heap(m_currentPakPath.c_str(), normalized.c_str(), &uncompSize, 0);
            if (!pBuffer) {
                Log::Error("VFS: Failed to extract '" + normalized + "' from ZIP: " + m_currentPakPath);
                return {};
            }
            std::vector<uint8_t> buffer((uint8_t*)pBuffer, (uint8_t*)pBuffer + uncompSize);
            mz_free(pBuffer);
            return buffer;
        } else {
            PakEntry& entry = m_pakEntries[normalized];
            std::ifstream file(m_currentPakPath, std::ios::binary);
            file.seekg(entry.offset);
            std::vector<uint8_t> buffer(entry.size);
            file.read((char*)buffer.data(), entry.size);
            return buffer;
        }
    }

    // Se for arquivo fisico
    std::ifstream file(resolved, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        return {};

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(size);
    file.read((char*)buffer.data(), size);
    return buffer;
}

}  // namespace starlight
