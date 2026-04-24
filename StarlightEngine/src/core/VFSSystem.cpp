// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "VFSSystem.hpp"
#include "Log.hpp"
#include <fstream>
#include <filesystem>

namespace starlight {

    VFSSystem& VFSSystem::Get() {
        static VFSSystem instance;
        return instance;
    }

    void VFSSystem::Initialize() {
        Log::Info("VFS System: Initialized for Commercial SDK.");
    }

    void VFSSystem::Shutdown() {
        m_mounts.clear();
        m_pakEntries.clear();
    }

    void VFSSystem::Mount(const std::string& virtualPath, const std::string& physicalPath) {
        m_mounts[virtualPath] = physicalPath;
        Log::Info("VFS: Mounted " + physicalPath + " to " + virtualPath);
    }

    bool VFSSystem::LoadPak(const std::string& pakPath) {
        std::ifstream file(pakPath, std::ios::binary);
        if (!file.is_open()) return false;

        // Formato simples: [MAGIC 4B][COUNT 4B]...[ENTRIES]
        char magic[4];
        file.read(magic, 4);
        if (strncmp(magic, "TPAK", 4) != 0) return false;

        uint32_t count;
        file.read((char*)&count, 4);

        for (uint32_t i = 0; i < count; i++) {
            char name[256];
            file.read(name, 256);
            PakEntry entry;
            file.read((char*)&entry.offset, 8);
            file.read((char*)&entry.size, 8);
            m_pakEntries[name] = entry;
        }

        m_currentPakPath = pakPath;
        Log::Info("VFS: Loaded commercial PAK: " + pakPath + " (" + std::to_string(count) + " files)");
        return true;
    }

    std::string VFSSystem::Resolve(const std::string& path) {
        if (path.empty()) return path;
        
        // Se estiver no PAK, o Resolve não retorna um caminho físico, o ReadFile cuidará disso
        if (m_pakEntries.count(path)) return path;

        if (path[0] == '@') {
            size_t firstSlash = path.find('/', 1);
            std::string alias = (firstSlash == std::string::npos) ? path : path.substr(0, firstSlash);
            
            if (m_mounts.count(alias)) {
                std::string remainder = (firstSlash == std::string::npos) ? "" : path.substr(firstSlash);
                return m_mounts[alias] + remainder;
            }
        }

        return path;
    }

    std::vector<uint8_t> VFSSystem::ReadFile(const std::string& path) {
        std::string resolved = Resolve(path);

        // Se estiver no PAK
        if (m_pakEntries.count(resolved)) {
            PakEntry& entry = m_pakEntries[resolved];
            std::ifstream file(m_currentPakPath, std::ios::binary);
            file.seekg(entry.offset);
            std::vector<uint8_t> buffer(entry.size);
            file.read((char*)buffer.data(), entry.size);
            return buffer;
        }

        // Se for arquivo físico
        std::ifstream file(resolved, std::ios::binary | std::ios::ate);
        if (!file.is_open()) return {};

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<uint8_t> buffer(size);
        file.read((char*)buffer.data(), size);
        return buffer;
    }

}
