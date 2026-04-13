#pragma once
#include <string>
#include <map>

namespace titan {

    class VFSSystem {
    public:
        static VFSSystem& Get();

        void Mount(const std::string& virtualPath, const std::string& physicalPath);
        std::string Resolve(const std::string& path);

    private:
        VFSSystem() = default;
        std::map<std::string, std::string> m_mounts;
    };

}
