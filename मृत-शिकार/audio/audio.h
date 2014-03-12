#pragma once

#include "irrKlang.h"
#include <vector>
#include "../glm/glm.hpp"

#pragma comment(lib, "libs/irrKlang.lib")


class SoundSource
{
protected:
	irrklang::vec3df m_position;
	irrklang::ISoundEngine* m_audioengine;
	std::vector<irrklang::ISoundSource*> m_sources;
public:
	SoundSource() :m_audioengine(0)
	{}
	SoundSource(irrklang::ISoundEngine* audioengine, glm::vec3 position = glm::vec3(0.0f))
	{
		Initialize(audioengine, position);
	}
	void Initialize(irrklang::ISoundEngine* audioengine, glm::vec3 position = glm::vec3(0.0f))
	{
		m_audioengine = audioengine;
		SetPosition(position);
	}
	void AddTrack(char* audiofilename, float defaultVolume = 1.0f, float minDist = 50.0f, float maxDist = 2000.0f)
	{
		if (m_audioengine)
		{
			m_sources.resize(m_sources.size() + 1);
			m_sources[m_sources.size()] = m_audioengine->addSoundSourceFromFile(audiofilename, irrklang::ESM_AUTO_DETECT, true);
		}

		if (m_sources.size() > 0 && m_sources[m_sources.size() - 1] != 0)
		{
			m_sources[m_sources.size() - 1]->setDefaultVolume(defaultVolume);
			m_sources[m_sources.size() - 1]->setDefaultMinDistance(minDist);
			m_sources[m_sources.size() - 1]->setDefaultMaxDistance(maxDist);
		}
	}
	void AddSource(char* audiofilename, float defaultVolume = 1.0f, float minDist = 50.0f, float maxDist = 2000.0f)
	{
		if (m_audioengine)
		{
			m_sources.resize(m_sources.size() + 1);
			m_sources[m_sources.size()] =  m_audioengine->addSoundSourceFromFile(audiofilename, irrklang::ESM_AUTO_DETECT, true);
		}

		if (m_sources.size() > 0 && m_sources[m_sources.size() - 1] != 0)
		{
			m_sources[m_sources.size() - 1]->setDefaultVolume(defaultVolume);
			m_sources[m_sources.size() - 1]->setDefaultMinDistance(minDist);
			m_sources[m_sources.size() - 1]->setDefaultMaxDistance(maxDist);
		}
	}
	void Play(int index)
	{
		if (m_audioengine != 0 && index >= 0 && static_cast<int>(m_sources.size()) > index)
		{
			m_audioengine->play3D(m_sources[index], m_position);
		}
	}
	irrklang::ISound* PlayTrack(int index)
	{
		if (m_audioengine != 0 && index >= 0 && static_cast<int>(m_sources.size()) > index)
		{
			return m_audioengine->play3D(m_sources[index], m_position, false, false, true);
		}
		return 0;
	}
	void SetPosition(glm::vec3 position)
	{
		m_position.X = position.x;
		m_position.Y = position.y;
		m_position.Z = position.z;
	}
};

//extern irrklang::ISoundEngine* g_audioengine;
//irrklang::ISoundEngine* g_audioengine = irrklang::createIrrKlangDevice();
extern irrklang::ISoundEngine* g_audioengine;
