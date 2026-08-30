#include <gtest/gtest.h>
#include "VFSSystem.hpp"
#include "miniz.h"
#include <fstream>
#include <filesystem>

using namespace starlight;

class VFSSystemTest : public ::testing::Test {
protected:
    std::string m_tempZipPath = "temp_test_archive.pak";

    void SetUp() override {
        // Create a temporary ZIP archive containing a file "test_inside.txt" with content "Hello from ZIP!"
        mz_zip_archive zipArchive;
        memset(&zipArchive, 0, sizeof(zipArchive));
        
        if (!mz_zip_writer_init_file(&zipArchive, m_tempZipPath.c_str(), 0)) {
            FAIL() << "Failed to initialize temp zip writer: " << m_tempZipPath;
        }

        const char* fileContent = "Hello from ZIP!";
        if (!mz_zip_writer_add_mem(&zipArchive, "test_inside.txt", fileContent, strlen(fileContent), static_cast<mz_uint>(MZ_DEFAULT_COMPRESSION))) {
            mz_zip_writer_end(&zipArchive);
            FAIL() << "Failed to add file to temp zip archive";
        }

        if (!mz_zip_writer_finalize_archive(&zipArchive)) {
            mz_zip_writer_end(&zipArchive);
            FAIL() << "Failed to finalize temp zip archive";
        }

        mz_zip_writer_end(&zipArchive);

        VFSSystem::Get().Initialize();
    }

    void TearDown() override {
        VFSSystem::Get().Shutdown();
        if (std::filesystem::exists(m_tempZipPath)) {
            std::filesystem::remove(m_tempZipPath);
        }
    }
};

TEST_F(VFSSystemTest, LoadZipAndReadFile) {
    bool loaded = VFSSystem::Get().LoadPak(m_tempZipPath);
    ASSERT_TRUE(loaded);

    // Read the file from VFS
    std::vector<uint8_t> data = VFSSystem::Get().ReadFile("test_inside.txt");
    std::string strContent(data.begin(), data.end());

    EXPECT_EQ(strContent, "Hello from ZIP!");
}

TEST_F(VFSSystemTest, ReadFileFromPhysicalPathFallback) {
    // Write a physical file
    std::string physicalFile = "temp_physical.txt";
    std::ofstream out(physicalFile);
    out << "Hello Physical!";
    out.close();

    std::vector<uint8_t> data = VFSSystem::Get().ReadFile(physicalFile);
    std::string strContent(data.begin(), data.end());
    EXPECT_EQ(strContent, "Hello Physical!");

    std::filesystem::remove(physicalFile);
}
