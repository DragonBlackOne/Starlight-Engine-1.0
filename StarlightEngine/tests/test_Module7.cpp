#include <gtest/gtest.h>
#include "core/Editor/Profiler.hpp"
#include "core/Editor/EditorPanel.hpp"
#include "core/Editor/EditorTheme.hpp"
#include "core/Editor/SceneHierarchyPanel.hpp"
#include "core/Editor/PropertiesPanel.hpp"
#include "core/Editor/ViewportPanel.hpp"
#include "core/Scene/Scene.hpp"

using namespace starlight::editor;
using namespace starlight::ecs;

// ---------------------------------------------------------------------------
// 1. Profiler Tests
// ---------------------------------------------------------------------------
TEST(Module7ProfilerTest, RecordAndAggregateMetrics) {
    auto& profiler = Profiler::Get();
    profiler.Clear();

    profiler.RecordTime("RenderPass_Shadow", 2.5f);
    profiler.RecordTime("RenderPass_Shadow", 3.5f);

    EXPECT_FLOAT_EQ(profiler.GetMetric("RenderPass_Shadow"), 3.5f);
    EXPECT_FLOAT_EQ(profiler.GetAverageMetric("RenderPass_Shadow"), 3.0f);
    EXPECT_FLOAT_EQ(profiler.GetMaxMetric("RenderPass_Shadow"), 3.5f);

    const auto& hist = profiler.GetHistory("RenderPass_Shadow");
    EXPECT_EQ(hist.size(), 2u);
}

TEST(Module7ProfilerTest, ScopedTimerExecution) {
    auto& profiler = Profiler::Get();
    profiler.Clear();

    {
        PROFILE_SCOPE("Scoped_Test_Work");
        // Simulate small computational workload
        volatile int count = 0;
        for (int i = 0; i < 10000; ++i) {
            count += i;
        }
    }

    EXPECT_GT(profiler.GetMetric("Scoped_Test_Work"), 0.0f);
}

// ---------------------------------------------------------------------------
// 2. Editor Panels & Selection Context Tests
// ---------------------------------------------------------------------------
TEST(Module7EditorPanelsTest, SceneHierarchyAndPropertiesSelection) {
    auto scene = std::make_shared<Scene>("Test Scene");
    Entity e1 = scene->CreateEntity("Hero");
    Entity e2 = scene->CreateEntity("Enemy");

    SceneHierarchyPanel hierarchy(scene);
    PropertiesPanel properties;

    EXPECT_FALSE(hierarchy.GetSelectedEntity().IsValid());

    hierarchy.SetSelectedEntity(e1);
    EXPECT_TRUE(hierarchy.GetSelectedEntity().IsValid());
    EXPECT_EQ(hierarchy.GetSelectedEntity(), e1);

    properties.SetSelectedEntity(e1);
    EXPECT_EQ(properties.GetSelectedEntity(), e1);
    EXPECT_EQ(properties.GetSelectedEntity().GetComponent<TagComponent>().tag, "Hero");

    // Change selection
    hierarchy.SetSelectedEntity(e2);
    properties.SetSelectedEntity(e2);
    EXPECT_EQ(properties.GetSelectedEntity().GetComponent<TagComponent>().tag, "Enemy");
}

TEST(Module7EditorPanelsTest, ViewportStateAndMetrics) {
    ViewportPanel viewport;
    viewport.SetViewportSize(1920, 1080);

    EXPECT_EQ(viewport.GetViewportSize().x, 1920u);
    EXPECT_EQ(viewport.GetViewportSize().y, 1080u);
    EXPECT_NEAR(viewport.GetAspectRatio(), 16.0f / 9.0f, 0.001f);

    viewport.SetState(ViewportState::Play);
    EXPECT_EQ(viewport.GetState(), ViewportState::Play);

    viewport.SetHovered(true);
    viewport.SetFocused(true);
    EXPECT_TRUE(viewport.IsHovered());
    EXPECT_TRUE(viewport.IsFocused());
}

TEST(Module7EditorPanelsTest, EditorThemeConfiguration) {
    EditorTheme theme;
    EXPECT_FLOAT_EQ(theme.cyberCyan.r, 0.0f);
    EXPECT_FLOAT_EQ(theme.cyberCyan.g, 0.85f);
    EXPECT_FLOAT_EQ(theme.windowRounding, 6.0f);
}
