#pragma once
#include "Event.hpp"
#include <functional>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <mutex>

namespace starlight::events {

class EventDispatcher {
public:
    template<typename T, typename F>
    static bool Dispatch(Event& event, const F& func) {
        if (event.GetEventType() == T::GetStaticType()) {
            event.Handled |= func(static_cast<T&>(event));
            return true;
        }
        return false;
    }
};

/**
 * @brief High-performance Event Broker with type-safe subscription and optional deferred queueing.
 */
class EventBrokerService {
public:
    using EventFn = std::function<void(Event&)>;

    template<typename TEvent, typename TReceiver>
    void Subscribe(TReceiver* receiver, void (TReceiver::*method)(TEvent&)) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::type_index typeIdx(typeid(TEvent));
        
        auto wrapper = [receiver, method](Event& e) {
            (receiver->*method)(static_cast<TEvent&>(e));
        };
        m_subscribers[typeIdx].push_back(wrapper);
    }

    template<typename TEvent>
    void Subscribe(std::function<void(TEvent&)> callback) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::type_index typeIdx(typeid(TEvent));
        
        auto wrapper = [callback](Event& e) {
            callback(static_cast<TEvent&>(e));
        };
        m_subscribers[typeIdx].push_back(wrapper);
    }

    template<typename TEvent>
    void Emit(TEvent& event) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::type_index typeIdx(typeid(TEvent));
        auto it = m_subscribers.find(typeIdx);
        if (it != m_subscribers.end()) {
            for (auto& handler : it->second) {
                handler(event);
                if (event.Handled) break;
            }
        }
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_subscribers.clear();
    }

private:
    std::unordered_map<std::type_index, std::vector<EventFn>> m_subscribers;
    std::mutex m_mutex;
};

} // namespace starlight::events
