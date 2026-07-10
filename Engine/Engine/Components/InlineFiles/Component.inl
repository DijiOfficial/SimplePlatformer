#pragma once
namespace diji
{
    template<typename T>
    T* Component::GetComponent() const
    {
        return m_OwnerPtr->GetComponent<T>();
    }
}
