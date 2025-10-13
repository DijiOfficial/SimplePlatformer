#pragma once
#include <functional>
#include <string>
#include <vector>

namespace diji
{
    struct Keyframe
    {
        float time = 0.f;
        float value = 0.f;
    };

    struct FloatTrack
    {
        bool needsUpdate = true;
        std::string name;
        std::vector<Keyframe> keys;
        std::function<void(float)> onValue;
        [[nodiscard]] float Evaluate(float time) const;
    };

    struct EventKey
    {
        float time = 0.f;
        std::function<void()> callback;
        bool fired = false;
    };

    struct EventTrack
    {
        std::string name;
        std::vector<EventKey> keys;
    };

    // todo: ideally all tracks should have at least two default keyframes but without UI it might get confusing if enforced
    // also I haven't fully tested the timeline implementation yet
    class Timeline final
    {
    public:
        Timeline() = default;
        ~Timeline() noexcept = default;

        Timeline(const Timeline& other) = delete;
        Timeline(Timeline&& other) = delete;
        Timeline& operator=(const Timeline& other) = delete;
        Timeline& operator=(Timeline&& other) = delete;
        
        void Play() { m_Playing = true; }
        void Pause() { m_Playing = false; }
        void PlayFromStart();
        void Stop();
        void JumpToTime(float time, bool fireEvents = false);
        void SetPlayRate(const float playRate) { m_PlayRate = playRate; }
        void SetLooping(const bool isLooping) { m_Loop = isLooping; }
        void SetReverse(const bool isReverse) { m_Reverse = isReverse; }

        [[nodiscard]] bool IsPlaying() const { return m_Playing; }
        // [[nodiscard]] bool IsLooping() const { return m_Loop; }
        // [[nodiscard]] bool IsReversed() const { return m_Reverse; }
        [[nodiscard]] float GetTime() const { return m_Time; }
        [[nodiscard]] float GetLength() const { return m_Length; }
        // [[nodiscard]] float GetPlayRate() const { return m_PlayRate; }

        FloatTrack& AddFloatTrack(const std::string& name);
        EventTrack& AddEventTrack(const std::string& name);

        void Update(float dt);

        [[nodiscard]] bool IsFinished() const { return m_Finished; }

    private:
        float m_Time = 0.f;
        float m_Length = 0.f;
        float m_PlayRate = 1.f;
        bool m_Playing = true;
        bool m_Loop = false;
        bool m_Reverse = false;
        bool m_Finished = false;

        std::vector<FloatTrack> m_FloatTracks;
        std::vector<EventTrack> m_EventTracks;

        void RecalculateLength();
    };
}
