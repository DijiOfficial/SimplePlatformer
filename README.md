# <ins>**Nameless Engine**</ins>

**Nameless is the thrid installement in my engine programming journey, a lightweight C++ engine built with [SFML (Simple and Fast Multimedia Library)](https://www.sfml-dev.org), implemented following the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).**

# <ins>**Overview**</ins>

This Engine is built on the foundation of my [**SFML-Engine**](https://github.com/DijiOfficial/SFML-Engine) which itself is built on top of [**Diji-Engine**](https://github.com/DijiOfficial/Diji-Engine). The goal of this engine was to deepen my understanding of physics engine by making my own. 
Finding resources concerning Physics Engine is rather difficult, so my implementation of the physics engine was made through trial and errors.

# <ins>**Contents**</ins>

- [**Nameless**](#nameless)
  - [**Engine Pipeline**](#engine-pipeline)
  - [**Physics Engine**](#physics-engine)
- [**Conclusion**](#conclusion)
- [**References**](#references) 

# <ins>**Nameless**</ins>

 Not having named my engine yet it shall be named Nameless. This time around I won't go in depth about the new features and changes to the engine, feel free to discover those by yourself.
 I will instead cover the physics engine as it is the biggest addition to the engine to date, as it could have been its own engine.

## <ins>**Engine Pipeline**</ins>

Nameless has seen some changes to the pipeline once again, with the addition of the physics engine. So here's a quick overview of the new pipeline:

![Picture of the Pipeline](https://github.com/DijiOfficial/SFML-Engine/blob/main/GithubAssets/PipelineSFML.png)

## <ins>**Physics Engine**</ins>

WIP

# <ins>**Conclusion**</ins>

The **SFML Engine** represents a significant step forward from the original [**Diji-Engine**](https://github.com/DijiOfficial/Diji-Engine), not only in terms of library migration but also in system design, architecture, and extensibility. By transitioning from SDL to SFML, many systems were simplified, made more maintainable, and enhanced with new functionality inspired by industry-standard engines like **Unity** and **Unreal Engine**.

While the engine is still a work in progress, it provides a solid foundation for learning and game development in C++. Many features, such as the Event System, Input Manager, Timers, and SceneManager, have been redesigned to be more modular and flexible, while future improvements promise to further optimize performance and expand functionality.

I hope to expand on this engine in the coming months learning more about engine development and game development to solidify my knowledge and abilities to develop games!

## <ins>**References**</ins>
- [**SFML-Engine**](https://github.com/DijiOfficial/SFML-Engine) Second Engine.
- [**Diji-Engine**](https://github.com/DijiOfficial/Diji-Engine) Original engine upon which SFML Engine is based.
- [**SFML**](https://www.sfml-dev.org) Simple and Fast Multimedia Library used as the core graphics and input library.
- [**Unreal Engine**](https://www.unrealengine.com) Inspiration for the event system, AI framework, scene management, and various design patterns.
- [**Unity Engine**](https://unity.com) Referenced for the engine pipeline, component architecture, input management, and naming conventions.
- [**C++ Core Guidelines**](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) Followed as best practices for modern, safe, and maintainable C++ code.
- [**Game Programming Patterns** by Robert Nystrom](https://gameprogrammingpatterns.com/) Used as guidance for architecture patterns and system design in a decoupled engine environment.
