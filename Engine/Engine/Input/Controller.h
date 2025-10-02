#pragma once
#include <memory>

namespace diji 
{
    class Controller final
    {
    public:
        enum class Button : unsigned int
        {
            DPadUp = 0x0001,
            DPadDown = 0x0002,
            DPadLeft = 0x0004,
            DPadRight = 0x0008,
            Start = 0x0010,
            Back = 0x0020,
            LeftThumb = 0x0040,
            RightThumb = 0x0080,
            LeftShoulder = 0x0100,
            RightShoulder = 0x0200,
            A = 0x1000,
            B = 0x2000,
            X = 0x4000,
            Y = 0x8000
        };

        explicit Controller(int controllerIdx);
        ~Controller() noexcept;

        Controller(const Controller& other) = delete;
        Controller(Controller&& other) = delete;
        Controller& operator=(const Controller& other) = delete;
        Controller& operator=(Controller&& other) = delete;

        void ProcessControllerInput() const;

        bool IsKeyDownThisFrame(const Button& button) const;
        bool IsKeyUpThisFrame(const Button& button) const;
        bool IsPressed(const Button& button) const;

    private:
        class XInput;
        std::unique_ptr<XInput> m_XInputUPtr;
    };
};