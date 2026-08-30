#include <gtest/gtest.h>
#include "core/Platform/Window.hpp"
#include "core/Events/Event.hpp"
#include "core/Events/EventDispatcher.hpp"
#include "core/RHI/RHI.hpp"

using namespace starlight;
using namespace starlight::platform;
using namespace starlight::events;
using namespace starlight::rhi;

// ---------------------------------------------------------------------------
// 1. Platform / Window Props Tests
// ---------------------------------------------------------------------------
TEST(Module2PlatformTest, WindowPropsDefaults) {
    WindowProps props;
    EXPECT_EQ(props.width, 1280u);
    EXPECT_EQ(props.height, 720u);
    EXPECT_TRUE(props.vsync);
    EXPECT_TRUE(props.resizable);
    EXPECT_EQ(props.mode, WindowMode::Windowed);
    EXPECT_EQ(props.cursor, CursorMode::Normal);
}

TEST(Module2PlatformTest, WindowAspectRatioCalculation) {
    class MockWindow : public IWindow {
    public:
        bool Initialize(const WindowProps& props) override { m_props = props; return true; }
        void Shutdown() override {}
        void PollEvents() override {}
        void SwapBuffers() override {}
        bool ShouldClose() const override { return false; }
        void SetShouldClose(bool) override {}
        uint32_t GetWidth() const override { return m_props.width; }
        uint32_t GetHeight() const override { return m_props.height; }
        void SetTitle(const std::string&) override {}
        void SetVSync(bool) override {}
        bool IsVSync() const override { return m_props.vsync; }
        void SetWindowMode(WindowMode mode) override { m_props.mode = mode; }
        WindowMode GetWindowMode() const override { return m_props.mode; }
        void SetCursorMode(CursorMode mode) override { m_props.cursor = mode; }
        CursorMode GetCursorMode() const override { return m_props.cursor; }
        void* GetNativeWindowHandle() const override { return nullptr; }
        void* GetGraphicsContext() const override { return nullptr; }
        void SetEventCallback(const EventCallbackFn&) override {}

    private:
        WindowProps m_props;
    };

    MockWindow win;
    WindowProps p;
    p.width = 1920;
    p.height = 1080;
    win.Initialize(p);

    EXPECT_FLOAT_EQ(win.GetAspectRatio(), 1920.0f / 1080.0f);
}

// ---------------------------------------------------------------------------
// 2. Event System & Polymorphic Dispatch Tests
// ---------------------------------------------------------------------------
TEST(Module2EventTest, WindowResizeEventProperties) {
    WindowResizeEvent e(1920, 1080);
    EXPECT_EQ(e.GetWidth(), 1920u);
    EXPECT_EQ(e.GetHeight(), 1080u);
    EXPECT_EQ(e.GetEventType(), EventType::WindowResize);
    EXPECT_TRUE(e.IsInCategory(EventCategoryApplication));
    EXPECT_FALSE(e.IsInCategory(EventCategoryInput));
    EXPECT_EQ(e.ToString(), "WindowResizeEvent: 1920, 1080");
}

TEST(Module2EventTest, KeyEventsProperties) {
    KeyPressedEvent pressed(65, 2);
    EXPECT_EQ(pressed.GetKeyCode(), 65);
    EXPECT_EQ(pressed.GetRepeatCount(), 2);
    EXPECT_EQ(pressed.GetEventType(), EventType::KeyPressed);
    EXPECT_TRUE(pressed.IsInCategory(EventCategoryKeyboard));
    EXPECT_TRUE(pressed.IsInCategory(EventCategoryInput));

    KeyReleasedEvent released(65);
    EXPECT_EQ(released.GetKeyCode(), 65);
    EXPECT_EQ(released.GetEventType(), EventType::KeyReleased);
}

TEST(Module2EventTest, MouseEventsProperties) {
    MouseMovedEvent move(100.5f, 200.5f);
    EXPECT_FLOAT_EQ(move.GetX(), 100.5f);
    EXPECT_FLOAT_EQ(move.GetY(), 200.5f);
    EXPECT_TRUE(move.IsInCategory(EventCategoryMouse));

    MouseButtonPressedEvent btn(1);
    EXPECT_EQ(btn.GetMouseButton(), 1);
    EXPECT_TRUE(btn.IsInCategory(EventCategoryMouseButton));
}

TEST(Module2EventTest, EventDispatcherHandling) {
    WindowResizeEvent e(800, 600);
    bool handled = false;

    EventDispatcher::Dispatch<WindowResizeEvent>(e, [&](WindowResizeEvent& resizeEv) {
        EXPECT_EQ(resizeEv.GetWidth(), 800u);
        EXPECT_EQ(resizeEv.GetHeight(), 600u);
        handled = true;
        return true;
    });

    EXPECT_TRUE(handled);
    EXPECT_TRUE(e.Handled);
}

TEST(Module2EventTest, EventBrokerPubSub) {
    EventBrokerService broker;
    int receivedCount = 0;
    uint32_t lastWidth = 0;

    broker.Subscribe<WindowResizeEvent>([&](WindowResizeEvent& ev) {
        receivedCount++;
        lastWidth = ev.GetWidth();
    });

    WindowResizeEvent e1(1280, 720);
    broker.Emit(e1);

    EXPECT_EQ(receivedCount, 1);
    EXPECT_EQ(lastWidth, 1280u);

    WindowResizeEvent e2(2560, 1440);
    broker.Emit(e2);

    EXPECT_EQ(receivedCount, 2);
    EXPECT_EQ(lastWidth, 2560u);
}

// ---------------------------------------------------------------------------
// 3. RHI Base Types & Descriptors Tests
// ---------------------------------------------------------------------------
TEST(Module2RHITest, BufferDescInitializers) {
    BufferDesc desc;
    desc.type = BufferType::Uniform;
    desc.usage = BufferUsage::Dynamic;
    desc.size = 256;
    desc.debugName = "PerFrameUBO";

    EXPECT_EQ(desc.type, BufferType::Uniform);
    EXPECT_EQ(desc.usage, BufferUsage::Dynamic);
    EXPECT_EQ(desc.size, 256u);
    EXPECT_EQ(desc.debugName, "PerFrameUBO");
}

TEST(Module2RHITest, TextureDescInitializers) {
    TextureDesc desc;
    desc.type = TextureType::Texture2D;
    desc.format = TextureFormat::RGBA16_FLOAT;
    desc.width = 1920;
    desc.height = 1080;
    desc.mipLevels = 4;
    desc.sampler.minFilter = TextureFilter::Linear;
    desc.sampler.magFilter = TextureFilter::Linear;

    EXPECT_EQ(desc.type, TextureType::Texture2D);
    EXPECT_EQ(desc.format, TextureFormat::RGBA16_FLOAT);
    EXPECT_EQ(desc.width, 1920u);
    EXPECT_EQ(desc.height, 1080u);
    EXPECT_EQ(desc.mipLevels, 4u);
}

TEST(Module2RHITest, PipelineDescInitializers) {
    PipelineDesc pso;
    pso.topology = PrimitiveTopology::Triangles;
    pso.rasterizer.cullMode = CullMode::Back;
    pso.rasterizer.polygonMode = PolygonMode::Fill;
    pso.depthStencil.depthTestEnable = true;
    pso.depthStencil.depthCompareOp = CompareOp::LessOrEqual;
    pso.blend.attachments[0].blendEnable = true;

    EXPECT_EQ(pso.topology, PrimitiveTopology::Triangles);
    EXPECT_EQ(pso.rasterizer.cullMode, CullMode::Back);
    EXPECT_TRUE(pso.depthStencil.depthTestEnable);
    EXPECT_EQ(pso.depthStencil.depthCompareOp, CompareOp::LessOrEqual);
    EXPECT_TRUE(pso.blend.attachments[0].blendEnable);
}

TEST(Module2RHITest, ViewportAndScissor) {
    Viewport vp{ 0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f };
    EXPECT_FLOAT_EQ(vp.width, 1920.0f);
    EXPECT_FLOAT_EQ(vp.height, 1080.0f);

    ScissorRect sc{ 100, 100, 800, 600 };
    EXPECT_EQ(sc.x, 100);
    EXPECT_EQ(sc.y, 100);
    EXPECT_EQ(sc.width, 800u);
    EXPECT_EQ(sc.height, 600u);
}
