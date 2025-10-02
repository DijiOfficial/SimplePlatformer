#pragma once
#include "../Singleton/Singleton.h"
#include "Controller.h"
#include "GameActorCommand.h"

#include <ranges>
// #include <map>
#include <optional>
#include <variant>
#include <vector>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <unordered_map>

namespace sf
{
    class Event;
    class RenderWindow;
}

namespace diji
{
    enum class KeyState
    {
        PRESSED,
        RELEASED,
        HELD,
    };

    // todo: make this variable/editable
    enum class PlayerIdx
    {
        KEYBOARD = -1,
        PLAYER1,
        PLAYER2,
        PLAYER3,
        PLAYER4,
    };
    
    class Input final
    {
    public:
        typedef std::variant<sf::Keyboard::Key, sf::Mouse::Button, Controller::Button> InputType;

        explicit Input(const InputType input)
            : m_Input{ input }
        {
        }
        ~Input() = default;

        Input(const Input& other) = default;
        Input(Input&& other) noexcept = default;
        Input& operator=(const Input& other) = delete;
        Input& operator=(Input&& other) noexcept = delete;

        InputType GetInput() const { return m_Input; }

    private:
        InputType m_Input;
    };

    
    // todo: Create Input Mappings and Input Actions with easily configurable bindings because it's really annoying to write them now
    class InputManager final : public Singleton<InputManager>
    {
    public:
        void Init(sf::RenderWindow* window) { m_WindowPtr = window; }
        bool ProcessInput();
        
        template<typename T, typename... Args>
            requires std::derived_from<T, GameActorCommands>
        void BindCommand(const PlayerIdx playerIdx, const KeyState state, const Input::InputType input, GameObject* actor, Args... args)
        {
            // if (playerIdx != PlayerIdx::KEYBOARD)
            // {
            //     BindController(static_cast<int>(playerIdx));
            // }

            const auto key = CommandKey{ .state = state, .input = input};
            m_CommandUMap[key].emplace_back(PlayerCommand{ playerIdx, std::make_unique<T>(actor, std::forward<Args>(args)...) });
        }
        
        template<typename T, typename... Args>
            requires std::derived_from<T, GameActorCommands>
        void BindMouseMoveCommand(GameObject* actor, Args&&... args)
        {
            m_MouseMoveCommandsVec.emplace_back(std::make_unique<T>(actor, std::forward<Args>(args)...));
        }

        void ResetCommands() { m_CommandUMap = std::unordered_map<CommandKey, std::vector<PlayerCommand>, CommandKeyHash>(); }
        void Quit() { m_Continue = false; }
        [[nodiscard]] sf::Vector2i GetMousePosition() const { return m_MousePosition; }
        
    private:
        bool m_Continue = true;
        struct PlayerCommand
        {
            PlayerIdx playerIndex;
            std::unique_ptr<GameActorCommands> commandUPtr;
        };
		
        // std::map<int, std::unique_ptr<Controller>> m_PlayersMap;
        // std::vector<int> m_ControllersIdxs;
        sf::RenderWindow* m_WindowPtr = nullptr;
        
        // Keyboard state tracking
        std::unordered_map<sf::Keyboard::Key, bool> m_KeyPressedState;
        std::unordered_map<sf::Keyboard::Key, bool> m_KeyHeldState;

        // Mouse state tracking
        std::unordered_map<sf::Mouse::Button, bool> m_MousePressedState;
        std::unordered_map<sf::Mouse::Button, bool> m_MouseHeldState;
        sf::Vector2i m_MousePosition = { 0, 0 };
        
        // O(1) lookup for commands
        struct CommandKey
        {
            KeyState state;
            Input::InputType input;

            bool operator==(const CommandKey& other) const
            {
                return state == other.state && input == other.input;
            }
        };
        
        struct CommandKeyHash
        {
            std::size_t operator()(const CommandKey& k) const
            {
                const std::size_t h1 = std::hash<int>{}(static_cast<int>(k.state));
                const std::size_t h2 = std::visit([]<typename T0>(T0&& arg) -> std::size_t
                {
                    return std::hash<std::underlying_type_t<std::decay_t<T0>>>{}(static_cast<std::underlying_type_t<std::decay_t<T0>>>(std::forward<T0>(arg)));
                }, k.input);

                return h1 ^ (h2 << 1);
            }
        };
        std::unordered_map<CommandKey, std::vector<PlayerCommand>, CommandKeyHash> m_CommandUMap;
        std::vector<std::unique_ptr<GameActorCommands>> m_MouseMoveCommandsVec; // using separate container for mouse movements as there's no need for O(1) lookup, playerIdx or State/InputType

        // Functions
        template <typename InputEnum>
        void HandleInput(KeyState state, InputEnum input)
        {
            const CommandKey key{ state, input };
            
            const auto it = m_CommandUMap.find(key);
            if (it == m_CommandUMap.end())
                return;

            for (auto& [playerIdx, commandUPtr] : it->second)
            {
                if (commandUPtr)
                    commandUPtr->Execute();
            }
        }

        // Alternatively use template traits to dictate the member from the event type. Makes it easier for users but more complex and less maintainable code.
        template <typename CodeType, typename PressedEvent, typename ReleasedEvent>
        void SetInputState(const std::optional<sf::Event>& event, std::unordered_map<CodeType, bool>& pressedMap, std::unordered_map<CodeType, bool>& heldMap, CodeType PressedEvent::*pressedMember, CodeType ReleasedEvent::*releasedMember)
        {
            if (!event) return;

            if (event->type == sf::Event::KeyPressed)
            {
                const CodeType& code = event->key.*pressedMember;

                if (!heldMap[code])
                    pressedMap[code] = true;

                heldMap[code] = true;
            }
            else if (event->type == sf::Event::KeyReleased)
            {
                const CodeType& code = event->key.*releasedMember;

                pressedMap[code] = false;
                heldMap[code] = false;
            }
        }

        template <typename InputEnum>
        void ProcessInputMap(const std::unordered_map<InputEnum, bool>& pressedInputMap, const std::unordered_map<InputEnum, bool>& heldInputMap)
        {
            for (const auto& [input, isPressed] : pressedInputMap | std::views::filter([](const auto& pair) { return pair.second; }))
            {
                HandleInput(KeyState::PRESSED, Input::InputType{input});
            }

            for (const auto& [input, isHeld] : heldInputMap | std::views::filter([](const auto& pair) { return pair.second; }))
            {
                HandleInput(KeyState::HELD, Input::InputType{input});
            }
        }
        
        void ResetPressedStates();
        [[nodiscard]] bool PollEvents();
        void ProcessAllInputMaps();
        // void ProcessControllerInput();
        // void BindController(int controllerIdx);
    };
}
