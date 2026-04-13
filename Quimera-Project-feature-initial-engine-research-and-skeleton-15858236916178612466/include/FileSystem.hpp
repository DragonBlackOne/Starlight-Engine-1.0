#pragma once
// ============================================================
// Vamos Engine - Virtual File System (PhysFS)
// Licença: Zlib | Origem: icculus/physfs
// ============================================================

#include <physfs.h>
#include <string>
#include <vector>
#include "Log.hpp"

namespace Vamos {
    class FileSystem {
    public:
        static bool Init(const char* argv0) {
            if (!PHYSFS_init(argv0)) {
                VAMOS_CORE_ERROR("[FileSystem] Erro ao inicializar PhysFS: {0}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
                return false;
            }
            VAMOS_CORE_INFO("[FileSystem] PhysFS inicializado. Base dir: {0}", PHYSFS_getBaseDir());
            return true;
        }

        static void Shutdown() {
            PHYSFS_deinit();
        }

        // Montar um arquivo .pak, .zip ou diretório na raiz virtual
        static bool Mount(const std::string& archivePath, const std::string& mountPoint = "/", bool appendToPath = true) {
            if (!PHYSFS_mount(archivePath.c_str(), mountPoint.c_str(), appendToPath ? 1 : 0)) {
                VAMOS_CORE_ERROR("[FileSystem] Falha ao montar {0}: {1}", archivePath, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
                return false;
            }
            VAMOS_CORE_INFO("[FileSystem] Montado: {0} em '{1}'", archivePath, mountPoint);
            return true;
        }

        // Ler arquivo virtual inteiro pra memória
        static std::vector<char> ReadAll(const std::string& filename) {
            PHYSFS_File* file = PHYSFS_openRead(filename.c_str());
            if (!file) {
                VAMOS_CORE_ERROR("[FileSystem] Arquivo não encontrado: {0}", filename);
                return {};
            }

            PHYSFS_sint64 size = PHYSFS_fileLength(file);
            std::vector<char> buffer(size);
            PHYSFS_readBytes(file, buffer.data(), size);
            PHYSFS_close(file);

            return buffer;
        }

        // Ler como string
        static std::string ReadText(const std::string& filename) {
            auto data = ReadAll(filename);
            return std::string(data.begin(), data.end());
        }

        static bool Exists(const std::string& filename) {
            return PHYSFS_exists(filename.c_str()) != 0;
        }

        // Listar arquivos em um diretório virtual
        static std::vector<std::string> GetFiles(const std::string& directory) {
            std::vector<std::string> fileList;
            char** rc = PHYSFS_enumerateFiles(directory.c_str());
            
            for (char** i = rc; *i != NULL; i++) {
                fileList.push_back(*i);
            }

            PHYSFS_freeList(rc);
            return fileList;
        }

        static bool IsDirectory(const std::string& path) {
            return PHYSFS_isDirectory(path.c_str()) != 0;
        }
    };
}
