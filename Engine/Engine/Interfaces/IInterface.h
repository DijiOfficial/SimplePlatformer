#pragma once
#include "../Singleton/Singleton.h"
#include "../Core/GameObject.h"

#include <typeindex>
#include <unordered_map>

namespace diji
{
    class InterfaceRegistry;

    class IInterface
    {
    public:
        virtual ~IInterface() noexcept
        {
            UnregisterInterface();
        }

        IInterface(const IInterface&) = default;
        IInterface& operator=(const IInterface&) = default;
        IInterface(IInterface&&) = default;
        IInterface& operator=(IInterface&&) = default;
        
    protected:
        IInterface()
        {
            RegisterInterface();
        }
        
    private:
        void RegisterInterface();
        void UnregisterInterface();
    };

    class InterfaceRegistry final : public Singleton<InterfaceRegistry>
    {
    public:
        void RegisterInterface(const std::type_info& typeInfo, void* implementer)
        {
            const std::type_index idx(typeInfo);
            m_InterfacesUMap[idx].push_back(implementer);
        }
        
        void UnregisterInterface(const std::type_info& typeInfo, void* implementer)
        {
            const std::type_index idx(typeInfo);
            auto& vec = m_InterfacesUMap[idx];
            std::erase(vec, implementer);
        }
        
        // Query a component on a GameObject for an interface
        template<typename IInterfaceType>
        [[nodiscard]] static IInterfaceType* GetInterface(GameObject* gameObject)
        {
            return gameObject->GetInterface<IInterfaceType>();
        }

        template<typename IInterfaceType>
        [[nodiscard]] static IInterfaceType* GetInterface(const GameObject* gameObject)
        {
            const auto nonConstGameObject = const_cast<GameObject*>(gameObject);
            return nonConstGameObject->GetInterface<IInterfaceType>();
        }

        // Execute a function on all implementers
        template<typename IInterfaceType, typename Func>
        void ForEachImplementer(Func&& func)
        {
            auto implementers = GetAllImplementers<IInterfaceType>();
            for (auto impl : implementers)
            {
                std::forward<Func>(func)(impl);
            }
        }
        
        template<typename IInterfaceType>
        [[nodiscard]] std::vector<IInterfaceType*> GetAllImplementers()
        {
            const std::type_index idx(typeid(IInterfaceType));
            std::vector<IInterfaceType*> result;
            if (m_InterfacesUMap.contains(idx))
            {
                for (auto ptr : m_InterfacesUMap[idx])
                {
                    result.push_back(static_cast<IInterfaceType*>(ptr));
                }
            }
            return result;
        }
        
        template<typename IInterfaceType>
        [[nodiscard]] bool Implements(void* obj)
        {
            auto implementers = GetAllImplementers<IInterfaceType>();
            return std::find(implementers.begin(), implementers.end(), static_cast<IInterfaceType*>(obj)) != implementers.end();
        }
        
        void Clear()
        {
            m_InterfacesUMap.clear();
        }
        
    private:
        std::unordered_map<std::type_index, std::vector<void*>> m_InterfacesUMap;
        
        friend class IInterface;
    };
}
