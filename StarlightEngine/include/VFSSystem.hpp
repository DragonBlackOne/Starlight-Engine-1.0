#pragma once
#include <map>
#include <string>
#include <vector>
#include "EngineSystem.hpp"

namespace starlight {

    struct PakEntry {
        uint64_t offset;
        uint64_t size;
        uint64_t compressedSize;
    };

    /**
     * @brief Sistema de Arquivos Virtual (VFS)
     * Suporta montagem de pastas fÃƒÆ’Ã‚Â­sicas e carregamento de arquivos .pak compactados.
     */
    class VFSSystem : public ISystem {
    public:
        // ISystem
        bool OnInitialize(const EngineContext& context) override { (void)context; Initialize(); return true; }
        void OnShutdown() override { Shutdown(); }
        const char* GetName() const override { return "VFSSystem"; }
        bool IsMainThreadOnly() const override { return true; }

        static VFSSystem& Get();

        void Initialize();
        void Shutdown();

        // Monta um caminho fÃƒÆ’Ã‚Â­sico em um alias virtual (ex: Mount("@assets", "C:/Games/Titan/Assets"))
        void Mount(const std::string& virtualPath, const std::string& physicalPath);
        
        // Carrega um arquivo .pak comercial
        bool LoadPak(const std::string& pakPath);

        // Resolve um caminho virtual para um caminho fÃƒÆ’Ã‚Â­sico ou identifica se estÃƒÆ’Ã‚Â¡ em um PAK
        std::string Resolve(const std::string& path);

        // LÃƒÆ’Ã‚Âª os dados de um arquivo (seja fÃƒÆ’Ã‚Â­sico ou de um PAK)
        std::vector<uint8_t> ReadFile(const std::string& path);

    public:
        VFSSystem() = default;
    private:
        std::map<std::string, std::string> m_mounts;
        std::map<std::string, PakEntry> m_pakEntries;
        std::string m_currentPakPath;
        bool m_isZipPak = false;
    };

}
