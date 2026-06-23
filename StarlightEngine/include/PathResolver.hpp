#pragma once

#include <filesystem>
#include <mutex>
#include <string>

namespace starlight {

    /**
     * @brief Resolves a relative asset path to an absolute path on disk.
     *
     * The engine receives asset paths like `"assets/shaders/batch.vert"`.
     * Depending on how the executable was launched the working directory may
     * be the project source tree (e.g. during development) or the binary
     * directory (e.g. when launched by double-click in `build/Release/`).
     * `PathResolver::Resolve` returns the first location where the file
     * actually exists, searching in this order:
     *
     *   1. The path as supplied (relative to the current working directory).
     *   2. The path joined with the directory containing the running
     *      executable.
     *   3. The path joined with the parent of the executable directory
     *      (handy when the binary lives in `build/Release/` and assets sit
     *      in `build/Release/assets/`).
     *   4. The path joined with the *grandparent* of the executable
     *      directory (handy when the project layout puts `assets/` next
     *      to `build/`).
     *
     * On Windows the executable directory is obtained via `GetModuleFileName`
     * on the main module; on other platforms `std::filesystem::current_path`
     * is the only source so step (1) covers the typical case.
     */
    class PathResolver {
    public:
        static std::string Resolve(const std::string& relativePath);

        static std::string ExeDir();
        static std::string Cwd();
    };

}
