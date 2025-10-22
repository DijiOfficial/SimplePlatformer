#include "IInterface.h"

void diji::IInterface::RegisterInterface()
{
    InterfaceRegistry::GetInstance().RegisterInterface(typeid(*this), this);
}

void diji::IInterface::UnregisterInterface()
{
    InterfaceRegistry::GetInstance().UnregisterInterface(typeid(*this), this);
}
