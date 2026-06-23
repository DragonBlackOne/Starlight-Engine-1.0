#include "PathResolver.hpp"
#include "Log.hpp"

#include <array>

#if defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#endif

namespace starlight {

    namespace {
        std::string g_exeDir;
        std::once_flag g_once;

        std::filesystem::path DetectExecutableDir() {
#if defined(_WIN32)
            std::array<char, MAX_PATH> buffer{};
            DWORD length = GetModuleFileNameA(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
            if (length == 0 || length >= buffer.size()) {
                return std::filesystem::current_path();
            }
            std::filesystem::path exePath(std::string(buffer.data(), length));
            return exePath.parent_path();
#else
            return std::filesystem::current_path();
#endif
        }

        std::string& ExeDirCached() {
            std::call_once(g_once, []() {
                g_exeDir = DetectExecutableDir().string();
            });
            return g_exeDir;
        }

        bool FileExists(const std::filesystem::path& p) {
            std::error_code ec;
            return std::filesystem::is_regular_file(p, ec);
        }
    }

    std::string PathResolver::ExeDir() {
        return ExeDirCached();
    }

    std::string PathResolver::Cwd() {
        std::error_code ec;
        return std::filesystem::current_path(ec).string();
    }

    std::string PathResolver::Resolve(const std::string& relativePath) {
        namespace fs = std::filesystem;

        if (relativePath.empty()) return {};

        // 1. As supplied, relative to current working directory.
        {
            fs::path candidate(relativePath);
            if (candidate.is_absolute() && FileExists(candidate)) {
                return candidate.string();
            }
            if (FileExists(candidate)) {
                return fs::absolute(candidate).string();
            }
        }

        const fs::path exeDir = ExeDirCached();

        // 2. Relative to the executable directory.
        {
            fs::path candidate = exeDir / relativePath;
            if (FileExists(candidate)) {
                return candidate.string();
            }
        }

        // 3. Relative to the parent of the executable directory
        //    (binary lives in build/<Config>/, assets in build/<Config>/assets/).
        {
            fs::path candidate = exeDir.parent_path() / relativePath;
            if (FileExists(candidate)) {
                return candidate.string();
            }
        }

        // 4. Relative to the grandparent of the executable directory
        //    (binary in build/<Config>/, assets in project root).
        if (exeDir.has_parent_path()) {
            fs::path candidate = exeDir.parent_path().parent_path() / relativePath;
            if (FileExists(candidate)) {
                return candidate.string();
            }
        }

        Log::Warn("PathResolver: '{}' not found in any known search path "
                  "(cwd='{}', exeDir='{}')",
                  relativePath, Cwd(), ExeDir());
        return relativePath;
    }

}
