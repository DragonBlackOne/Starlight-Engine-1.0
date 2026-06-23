#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "EngineSystem.hpp"
#include <sol/sol.hpp>

namespace starlight {

class EventBroker : public ISystem {
public:
    EventBroker();
    ~EventBroker();

    // ISystem
    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    const char* GetName() const override { return "EventBroker"; }
    bool IsMainThreadOnly() const override { return true; }

    // PubSub for C++ listeners
    using CPPCallback = std::function<void(const std::string&, const sol::table&)>;
    void Subscribe(const std::string& eventName, CPPCallback callback);
    
    // Publish events (will notify both CPP and Lua subscribers)
    void Publish(const std::string& eventName, const sol::table& data);

    // PubSub for Lua listeners
    void SubscribeLua(const std::string& eventName, sol::protected_function callback);
    void UnsubscribeAllLua();

private:
    std::unordered_map<std::string, std::vector<CPPCallback>> m_cppSubscribers;
    std::unordered_map<std::string, std::vector<sol::protected_function>> m_luaSubscribers;
};

} // namespace starlight
