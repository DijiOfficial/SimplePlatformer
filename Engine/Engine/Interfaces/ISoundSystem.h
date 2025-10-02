#pragma once
#include <memory>
#include <queue>
#include <mutex>

namespace diji 
{
	// todo: Add a way to load sounds using an enum class. instead of the string.
	class ISoundSystem
	{
	public:
		virtual ~ISoundSystem() noexcept = default;
		virtual void AddSoundRequest(const std::string& audio, bool isMusic, int volume = 100) = 0;
		virtual void Pause() = 0;
		virtual void Resume() = 0;
	};

	class NullSoundSystem final : public ISoundSystem
	{
	public:
		void AddSoundRequest(const std::string& audio, bool isMusic, int volume = 100) override;
		void Pause() override {}
		void Resume() override {}
	};

	class ServiceLocator final
	{
	public:
		static ISoundSystem& GetSoundSystem() { return *_ss_instance; }
		static void RegisterSoundSystem(std::unique_ptr<ISoundSystem>&& ss) 
		{ 
			_ss_instance = ss == nullptr ? std::make_unique<NullSoundSystem>() : std::move(ss);
		}
	private:
		static std::unique_ptr<ISoundSystem> _ss_instance;
	};

	class SFMLISoundSystem final : public ISoundSystem
	{		
	public:
		SFMLISoundSystem();
		~SFMLISoundSystem() noexcept override;
		void AddSoundRequest(const std::string& audio, bool isMusic, int volume = 100) override;
		void Pause() override;
		void Resume() override;
	private:
		void PlayAudio(const std::string& audio, bool isMusic, int volume) const;
		std::pair<std::pair<bool, int>, std::string>  GetNextSoundRequest();
	
		void ProcessSounds();
	
		std::string m_LastMusicPlayed;
		std::jthread m_SoundThread;
		std::queue<std::pair<std::pair<bool, int>, std::string>> m_SoundQueue;
		std::mutex soundMutex_;
		bool m_IsRunning = false;
		bool m_IsPaused = false;
		// Allows threads to synchronize their execution based on certain conditions. 
		// It provides a way for one thread to notify other threads when a particular condition becomes true, allowing them to proceed with their execution
		std::condition_variable condition_;
	};

	class LoggingSoundSystem final : public ISoundSystem
	{
	public:
		explicit LoggingSoundSystem(std::unique_ptr<ISoundSystem>&& ss) : _real_ss{ std::move(ss) } {}
		~LoggingSoundSystem() noexcept override = default;

		void AddSoundRequest(const std::string& audio, bool isMusic, int volume = 100) override;
		void Pause() override;
		void Resume() override;
	private:
		std::unique_ptr<ISoundSystem> _real_ss;
	};
}