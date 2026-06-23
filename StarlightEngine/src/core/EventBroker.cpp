#include "EventBroker.hpp"
#include "Log.hpp"

namespace starlight {

EventBroker::EventBroker() {}

EventBroker::~EventBroker() {
    OnShutdown();
}

bool EventBroker::OnInitialize(const EngineContext& context) {
    (void)context;
    Log::Info("EventBroker: Initialized.");
    return true;
}

void EventBroker::OnShutdown() {
    m_cppSubscribers.clear();
    m_luaSubscribers.clear();
}

void EventBroker::Subscribe(const std::string& eventName, CPPCallback callback) {
    m_cppSubscribers[eventName].push_back(callback);
}

void EventBroker::Publish(const std::string& eventName, const sol::table& data) {
    // Notify C++ subscribers
    auto cppIt = m_cppSubscribers.find(eventName);
    if (cppIt != m_cppSubscribers.end()) {
        for (auto& cb : cppIt->second) {
            try {
                cb(eventName, data);
            } catch (const std::exception& e) {
                Log::Error("EventBroker: C++ subscriber exception on '{}': {}", eventName, e.what());
            } catch (...) {
                Log::Error("EventBroker: Unknown C++ subscriber exception on '{}'", eventName);
            }
        }
    }

    // Notify Lua subscribers
    auto luaIt = m_luaSubscribers.find(eventName);
    if (luaIt != m_luaSubscribers.end()) {
        for (auto& cb : luaIt->second) {
            if (cb.valid()) {
                auto result = cb(data);
                if (!result.valid()) {
                    sol::error err = result;
                    Log::Error("EventBroker: Lua callback error on '{}': {}", eventName, err.what());
                }
            }
        }
    }
}

void EventBroker::SubscribeLua(const std::string& eventName, sol::protected_function callback) {
    m_luaSubscribers[eventName].push_back(callback);
}

void EventBroker::UnsubscribeAllLua() {
    m_luaSubscribers.clear();
}

} // namespace starlight
