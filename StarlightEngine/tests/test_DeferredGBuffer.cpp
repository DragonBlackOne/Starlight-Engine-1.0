#include <gtest/gtest.h>
#include "Renderer.hpp"
#include "Engine.hpp"
#include "CVarSystem.hpp"

using namespace starlight;

TEST(DeferredGBufferTest, GBufferCVarConfigurationsAndToggles) {
    // Check if CVarSystem is available or if we need to mock it
    auto cvarSys = std::make_shared<CVarSystem>();
    cvarSys->RegisterBool("r_deferred", true, "Enable Deferred Shading Renderer");
    cvarSys->RegisterBool("r_taa", true, "Enable Temporal Anti-Aliasing (TAA)");
    cvarSys->RegisterBool("r_fsr", false, "Enable AMD FSR Quality Upscaling");
    cvarSys->RegisterFloat("r_fsr_sharpness", 0.8f, "AMD FSR Sharpening filter strength");

    EXPECT_TRUE(cvarSys->GetBool("r_deferred"));
    EXPECT_TRUE(cvarSys->GetBool("r_taa"));
    EXPECT_FALSE(cvarSys->GetBool("r_fsr"));
    EXPECT_FLOAT_EQ(cvarSys->GetFloat("r_fsr_sharpness"), 0.8f);

    cvarSys->SetBool("r_deferred", false);
    EXPECT_FALSE(cvarSys->GetBool("r_deferred"));

    cvarSys->SetBool("r_fsr", true);
    EXPECT_TRUE(cvarSys->GetBool("r_fsr"));

    cvarSys->SetFloat("r_fsr_sharpness", 0.5f);
    EXPECT_FLOAT_EQ(cvarSys->GetFloat("r_fsr_sharpness"), 0.5f);
}
