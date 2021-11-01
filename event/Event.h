#pragma once

#include <unordered_map>
#include <functional>

namespace ige::scene
{
    template<class... Args>
    class Event
    {
    public:
        using Callback = std::function<void(Args...)>;
        uint64_t addListener(Callback callback);
        bool removeListener(uint64_t id);
        void removeAllListeners();
        size_t getListenerCount();
        void invoke(Args... args);
    protected:
        std::unordered_map<uint64_t, Callback>	m_callbacks;
        static uint64_t s_idCounter;
    };

    template<class... Args>
    uint64_t Event<Args...>::s_idCounter = 0;


    template<class... Args>
    uint64_t Event<Args...>::addListener(Callback callback)
    {
        m_callbacks.emplace(s_idCounter, callback);
        return s_idCounter++;
    }

    template<class... Args>
    bool Event<Args...>::removeListener(uint64_t id)
    {
        return m_callbacks.erase(id) > 0;
    }

    template<class... Args>
    void Event<Args...>::removeAllListeners()
    {
        m_callbacks.clear();
    }

    template<class... Args>
    size_t Event<Args...>::getListenerCount()
    {
        return m_callbacks.size();
    }

    template<class... Args>
    void Event<Args...>::invoke(Args... args)
    {
        for (auto const& [key, value] : m_callbacks)
            value(std::forward<Args>(args)...);
    }
}
