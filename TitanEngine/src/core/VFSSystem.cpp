// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "VFSSystem.hpp"
#include <algorithm>

namespace titan {

    VFSSystem& VFSSystem::Get() {
        static VFSSystem instance;
        return instance;
    }

    void VFSSystem::Mount(const std::string& virtualPath, const std::string& physicalPath) {
        m_mounts[virtualPath] = physicalPath;
    }

    std::string VFSSystem::Resolve(const std::string& path) {
        if (path.empty() || path[0] != '@') return path;

        size_t firstSlash = path.find('/', 1);
        std::string alias = (firstSlash == std::string::npos) ? path : path.substr(0, firstSlash);
        
        if (m_mounts.count(alias)) {
            std::string remainder = (firstSlash == std::string::npos) ? "" : path.substr(firstSlash);
            return m_mounts[alias] + remainder;
        }

        return path;
    }

}
