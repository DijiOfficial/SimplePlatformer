#include "ISoundSystem.h"
#include "../Singleton/ResourceManager.h"
#include <SFML/Audio/Sound.hpp>

#include <iostream>

namespace diji 
{
	std::unique_ptr<ISoundSystem> ServiceLocator::_ss_instance{ std::make_unique<NullSoundSystem>() };

	void ISFMLSoundSystem::PlayAudio(const std::string& audio, const bool isMusic, const int volume)
	{
		if (audio == "invalid")
			return;

		if (isMusic)
		{
			if (!m_LastMusicPlayed.empty() && m_LastMusicPlayed != audio)
			{
				auto& soundEffect = ResourceManager::GetInstance().LoadSoundEffect(m_LastMusicPlayed);
					soundEffect.pause();
			}

			auto& soundEffect = ResourceManager::GetInstance().LoadSoundEffect(audio);
			m_LastMusicPlayed = audio;

			soundEffect.setLooping(true);
			soundEffect.setVolume(static_cast<float>(volume));
			soundEffect.play();
			return;
		}

        auto& soundEffect = ResourceManager::GetInstance().LoadSoundEffect(audio);
		
        soundEffect.setVolume(static_cast<float>(volume));
        soundEffect.play();
	}

	ISFMLSoundSystem::ISFMLSoundSystem()
	{
		if (!m_IsRunning)
		{
			m_IsRunning = true;
			m_SoundThread = std::jthread([this]() { ProcessSounds(); });
		}
	}

	ISFMLSoundSystem::~ISFMLSoundSystem() noexcept
	{
		if (m_IsRunning)
		{
			m_IsRunning = false;
			condition_.notify_one();
		}
	}

	void ISFMLSoundSystem::AddSoundRequest(const std::string& audio, bool isMusic, int volume)
	{
		std::lock_guard<std::mutex> lock(soundMutex_);

		if (m_IsPaused)
			return;

		if (not m_SoundQueue.empty())
		{
			const auto [pair, name] = m_SoundQueue.front();
			if (audio == name) // check volume and set the highest volume
				return;
		}

		m_SoundQueue.push({ { isMusic, volume }, audio });
		condition_.notify_one();
	}

	void ISFMLSoundSystem::Pause()
	{
		m_IsPaused = true;

		if (!m_LastMusicPlayed.empty())
		{
			auto& soundEffect = ResourceManager::GetInstance().LoadSoundEffect(m_LastMusicPlayed);
			soundEffect.pause();
		}
	}

	void ISFMLSoundSystem::Resume()
	{
		m_IsPaused = false;

		if (!m_LastMusicPlayed.empty())
		{
			auto& soundEffect = ResourceManager::GetInstance().LoadSoundEffect(m_LastMusicPlayed);
			soundEffect.play();
		}
	}

	void ISFMLSoundSystem::StopMusic()
	{
		if (!m_LastMusicPlayed.empty())
		{
			auto& soundEffect = ResourceManager::GetInstance().LoadSoundEffect(m_LastMusicPlayed);
			soundEffect.stop();
		}
	}

	std::pair<std::pair<bool, int>, std::string> ISFMLSoundSystem::GetNextSoundRequest()
	{
		std::unique_lock<std::mutex> lock(soundMutex_);

		// Block the request until a new sound request is added or the thread is stopped
		condition_.wait(lock, [this] { return !m_SoundQueue.empty() || not m_IsRunning; });

		if (!m_IsRunning)
			return { { false, 0 }, "invalid"};

		auto request = m_SoundQueue.front();
		m_SoundQueue.pop();
		return request;
	}

	void ISFMLSoundSystem::ProcessSounds()
	{
		while (m_IsRunning)
		{
			auto [pairIsMusicVolume, audio] = GetNextSoundRequest();
			PlayAudio(audio, pairIsMusicVolume.first, pairIsMusicVolume.second);
		}
	}

	void NullSoundSystem::AddSoundRequest(const std::string& , bool , int)
	{
		std::cout << "No Sound System available\n";
	}

	void LoggingSoundSystem::AddSoundRequest(const std::string& audio, const bool isMusic, const int volume)
	{
		_real_ss->AddSoundRequest(audio, isMusic, volume);
		std::cout << "Adding sound request " << audio << " at volume " << volume << "\n";
	}

	void LoggingSoundSystem::Pause()
	{
		_real_ss->Pause();
		std::cout << "Pausing Audio\n";
	}

	void LoggingSoundSystem::Resume()
	{
		_real_ss->Resume();
		std::cout << "Resuming Audio\n";
	}

	void LoggingSoundSystem::StopMusic()
	{
		_real_ss->StopMusic();
		std::cout << "Stopped Music\n";
	}
}
