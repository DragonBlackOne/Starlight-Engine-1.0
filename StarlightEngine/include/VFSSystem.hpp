// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include "CoreMinimal.hpp"
#include <string>
#include <map>
#include <vector>

namespace starlight {

    struct PakEntry {
        uint64_t offset;
        uint64_t size;
        uint64_t compressedSize;
    };

    /**
     * @brief Sistema de Arquivos Virtual (VFS)
     * Suporta montagem de pastas físicas e carregamento de arquivos .pak compactados.
     */
    class VFSSystem {
    public:
        static VFSSystem& Get();

        void Initialize();
        void Shutdown();

        // Monta um caminho físico em um alias virtual (ex: Mount("@assets", "C:/Games/Titan/Assets"))
        void Mount(const std::string& virtualPath, const std::string& physicalPath);
        
        // Carrega um arquivo .pak comercial
        bool LoadPak(const std::string& pakPath);

        // Resolve um caminho virtual para um caminho físico ou identifica se está em um PAK
        std::string Resolve(const std::string& path);

        // Lê os dados de um arquivo (seja físico ou de um PAK)
        std::vector<uint8_t> ReadFile(const std::string& path);

    private:
        VFSSystem() = default;
        std::map<std::string, std::string> m_mounts;
        std::map<std::string, PakEntry> m_pakEntries;
        std::string m_currentPakPath;
    };

}
