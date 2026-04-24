// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "EventSystem.hpp"
#include <cstring>
#include <iostream>

namespace starlight {

    EventSystem& EventSystem::Get() {
        static EventSystem instance;
        return instance;
    }

    void EventSystem::Subscribe(EventType type, EventCallback callback) {
        m_subscribers[type].push_back(callback);
    }

    void EventSystem::Subscribe(const std::string& customType, EventCallback callback) {
        m_customSubscribers[customType].push_back(callback);
    }

    void EventSystem::Emit(const Event& ev) {
        if (ev.type == EventType::Custom) {
            if (m_customSubscribers.count(ev.customType)) {
                for (auto& cb : m_customSubscribers[ev.customType]) cb(ev);
            }
        } else {
            if (m_subscribers.count(ev.type)) {
                for (auto& cb : m_subscribers[ev.type]) cb(ev);
            }
        }
    }

    void EventSystem::EmitDeferred(const Event& ev) {
        Event deferredEv = ev;
        if (ev.data && ev.dataSize > 0) {
            size_t offset = m_payloadArena.size();
            m_payloadArena.resize(offset + ev.dataSize);
            std::memcpy(&m_payloadArena[offset], ev.data, ev.dataSize);
            deferredEv.data = &m_payloadArena[offset];
        }
        m_deferredQueue.push_back(deferredEv);
    }

    void EventSystem::Flush() {
        for (const auto& ev : m_deferredQueue) {
            Emit(ev);
        }
        m_deferredQueue.clear();
        m_payloadArena.clear(); // Reset arena for next frame
    }

}
