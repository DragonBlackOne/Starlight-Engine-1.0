// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <string>
#include <vector>
#include <functional>
#include <map>

namespace titan {

    enum class EventType {
        EntityCreated,
        EntityDestroyed,
        Collision,
        InputPressed,
        SceneChanged,
        AchievementUnlocked,
        Custom
    };

    struct Event {
        EventType type;
        void* data = nullptr;
        size_t dataSize = 0;
        std::string customType = "";
    };

    using EventCallback = std::function<void(const Event&)>;

    class EventSystem {
    public:
        static EventSystem& Get();

        void Subscribe(EventType type, EventCallback callback);
        void Subscribe(const std::string& customType, EventCallback callback);

        void Emit(const Event& ev);
        void EmitDeferred(const Event& ev);

        void Flush(); // Process deferred events

    private:
        EventSystem() = default;
        
        std::map<EventType, std::vector<EventCallback>> m_subscribers;
        std::map<std::string, std::vector<EventCallback>> m_customSubscribers;
        
        std::vector<Event> m_deferredQueue;
        std::vector<unsigned char> m_payloadArena; // Memory arena for deferred payloads
    };

}
