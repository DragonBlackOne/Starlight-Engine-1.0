#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "Log.hpp"

// #define TITAN_USE_PHYSFS 

#ifdef TITAN_USE_PHYSFS
#include <physfs.h>
#endif

namespace titan {
    class FileSystem {
    public:
        static bool Initialize(const char* argv0) {
#ifdef TITAN_USE_PHYSFS
            if (!PHYSFS_init(argv0)) {
                Log::Error("PhysFS Initialization Failed: " + std::string(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
                return false;
            }
            Log::Info("PhysFS Initialized. Base dir: " + std::string(PHYSFS_getBaseDir()));
#else
            (void)argv0;
            Log::Info("FileSystem: Using Native Fallback (PhysFS Disabled).");
#endif
            return true;
        }

        static void Shutdown() {
#ifdef TITAN_USE_PHYSFS
            PHYSFS_deinit();
#endif
        }

        static bool Mount(const std::string& archivePath, const std::string& mountPoint = "/", bool appendToPath = true) {
#ifdef TITAN_USE_PHYSFS
            if (!PHYSFS_mount(archivePath.c_str(), mountPoint.c_str(), appendToPath ? 1 : 0)) {
                Log::Error("FileSystem: Failed to mount " + archivePath);
                return false;
            }
            Log::Info("FileSystem: Mounted " + archivePath + " at '" + mountPoint + "'");
#else
            (void)archivePath; (void)mountPoint; (void)appendToPath;
#endif
            return true;
        }

        static std::vector<char> ReadAll(const std::string& filename) {
#ifdef TITAN_USE_PHYSFS
            PHYSFS_File* file = PHYSFS_openRead(filename.c_str());
            if (!file) {
                Log::Error("FileSystem: File not found: " + filename);
                return {};
            }

            PHYSFS_sint64 size = PHYSFS_fileLength(file);
            std::vector<char> buffer(size);
            PHYSFS_readBytes(file, buffer.data(), size);
            PHYSFS_close(file);

            return buffer;
#else
            (void)filename;
            return {};
#endif
        }

        static std::string ReadText(const std::string& filename) {
            auto data = ReadAll(filename);
            return std::string(data.begin(), data.end());
        }

        static bool Exists(const std::string& filename) {
#ifdef TITAN_USE_PHYSFS
            return PHYSFS_exists(filename.c_str()) != 0;
#else
            (void)filename;
            return false;
#endif
        }
    };
}
