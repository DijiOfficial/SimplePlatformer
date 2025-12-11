#pragma once
namespace superMarioBros 
{
    enum class superMarioBrosState
    {
        Invalid = -1,
        StartMenu,
        StartUp,
        Paused,
        Level,
        GameOver,
        LivesDisplayMenu,
        TransitionToNextLevel
    };
}
