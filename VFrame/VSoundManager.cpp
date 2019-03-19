#include "VSoundManager.h"
#include "VGlobal.h"

VSoundManager::VSoundManager() {}
VSoundManager::~VSoundManager()
{
	if (sounds.size())
		Cleanup();
}

void VSoundManager::SetMasterVolume(float volume)
{
	masterVolume = volume;
}

void VSoundManager::SetMasterPitch(float pitch)
{
	masterPitch = pitch;
}

float VSoundManager::MasterVolume()
{
	return masterVolume;
}

float VSoundManager::MasterPitch()
{
	return masterPitch;
}

bool VSoundManager::Load(sf::String filename, sf::String id)
{
	if (sounds.find(id) == sounds.end() || sounds[id].getBuffer() == nullptr)
	{
		soundBuffers[id] = &VGlobal::p()->Content->LoadSound(filename);
		sounds[id] = sf::Sound(*soundBuffers[id]);
	}

	return true;
}

void VSoundManager::Play(sf::String id, float volume, float pitch)
{
	if (sounds.find(id) != sounds.end())
	{
		sounds[id].setVolume((volume / 100) * masterVolume);
		sounds[id].setPitch(pitch * masterPitch);
		sounds[id].play();
	}
}

void VSoundManager::Stop(sf::String id)
{
	if (sounds.find(id) != sounds.end())
	{
		sounds[id].stop();
	}
}

void VSoundManager::StopAll()
{
	for (std::pair<std::string, sf::Sound> pair : sounds)
	{
		sounds[pair.first].stop();
	}
}

sf::Sound* VSoundManager::GetSound(sf::String id)
{
	if (sounds.find(id) != sounds.end())
	{
		return &sounds[id];
	}

	return nullptr;
}

void VSoundManager::Cleanup()
{
	sounds.clear();
	soundBuffers.clear();
}
