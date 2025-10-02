#pragma once
#include "Command.h"

namespace diji
{
    class GameObject;

    class GameActorCommands : public Command
    {
    public:
        explicit GameActorCommands(GameObject* actor) : m_ActorPtr{ actor } {}
        ~GameActorCommands() noexcept override = default;

    protected:
        [[nodiscard]] GameObject* GetGameActor() const { return m_ActorPtr; }

    private:
        GameObject* m_ActorPtr;
    };
}