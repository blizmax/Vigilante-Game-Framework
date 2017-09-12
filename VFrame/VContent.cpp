#include "VContent.h"
#include "VGlobal.h"
#include <stdexcept>

bool VContent::TextureExists(const sf::String& name)
{
	return textureDir.find(name) != textureDir.end();
}

bool VContent::ImageExists(const sf::String& name)
{
	return imageDir.find(name) != imageDir.end();
}

bool VContent::FontExists(const sf::String& name)
{
	return fontDir.find(name) != fontDir.end();
}

bool VContent::SoundExists(const sf::String& name)
{
	return soundDir.find(name) != soundDir.end();
}

sf::Texture& VContent::LoadTexture(const sf::String& name)
{
	auto found = textureDir.find(name);
	if (found == textureDir.end())
	{
		std::unique_ptr<sf::Texture> texture = std::unique_ptr<sf::Texture>(new sf::Texture());
		if (texture->loadFromFile(name))
		{
			VBase::VLog("%s loaded", name.toAnsiString().c_str());
			textureDir[name] = std::move(texture);
		}
		else
		{
		#ifndef __linux__
			throw std::exception(("Error loading texture: " + name.toAnsiString()).c_str());
        #else
            throw ("Error loading texture: " + name.toAnsiString());
		#endif
		}
	}

	return *textureDir[name];
}

sf::Image& VContent::LoadImage(const sf::String& name)
{
	auto found = imageDir.find(name);
	if (found == imageDir.end())
	{
		std::unique_ptr<sf::Image> image = std::unique_ptr<sf::Image>(new sf::Image());
		if (image->loadFromFile(name))
		{
			VBase::VLog("%s loaded", name.toAnsiString().c_str());
			imageDir[name] = std::move(image);
		}
		else
		{
        #ifndef __linux__
			throw std::exception(("Error loading image: " + name.toAnsiString()).c_str());
        #else
            throw ("Error loading image: " + name.toAnsiString());
		#endif
		}
	}

	return *imageDir[name];
}

sf::Font& VContent::LoadFont(const sf::String& name)
{
	auto found = fontDir.find(name);
	if (found == fontDir.end())
	{
		std::unique_ptr<sf::Font> font = std::unique_ptr<sf::Font>(new sf::Font());
		if (font->loadFromFile(name))
		{
			VBase::VLog("%s loaded", name.toAnsiString().c_str());
			fontDir[name] = std::move(font);
		}
		else
		{
		#ifndef __linux__
			throw std::exception(("Error loading font: " + name.toAnsiString()).c_str());
        #else
            throw ("Error loading front: " + name.toAnsiString());
		#endif
		}
	}

	return *fontDir[name];
}

sf::SoundBuffer& VContent::LoadSound(const sf::String& name)
{
	auto found = soundDir.find(name);
	if (found == soundDir.end())
	{
		std::unique_ptr<sf::SoundBuffer> sound = std::unique_ptr<sf::SoundBuffer>(new sf::SoundBuffer());
		if (sound->loadFromFile(name))
		{
			VBase::VLog("%s loaded", name.toAnsiString().c_str());
			soundDir[name] = std::move(sound);
		}
		else
		{
		#ifndef __linux__
			throw std::exception(("Error loading sound: " + name.toAnsiString()).c_str());
        #else
            throw ("Error loading texture: " + name.toAnsiString());
		#endif
		}
	}

	return *soundDir[name];
}

bool VContent::UnloadTexture(const sf::String& name)
{
	auto found = textureDir.find(name);
	if (found != textureDir.end())
	{
		found->second.reset();
		textureDir.erase(found);
		return true;
	}

	VBase::VLog("Couldn't find texture: %s", name.toAnsiString().c_str());
	return false;
}

bool VContent::UnloadImage(const sf::String& name)
{
	auto found = imageDir.find(name);
	if (found != imageDir.end())
	{
		found->second.reset();
		imageDir.erase(found);
		return true;
	}

	VBase::VLog("Couldn't find image: %s", name.toAnsiString().c_str());
	return false;
}

bool VContent::UnloadFont(const sf::String& name)
{
	auto found = fontDir.find(name);
	if (found != fontDir.end())
	{
		found->second.reset();
		fontDir.erase(found);
		return true;
	}

	VBase::VLog("Couldn't find font: %s", name.toAnsiString().c_str());
	return false;
}

bool VContent::UnloadSound(const sf::String& name)
{
	auto found = soundDir.find(name);
	if (found != soundDir.end())
	{
		found->second.reset();
		soundDir.erase(found);
		return true;
	}

	VBase::VLog("Couldn't find sound: %s", name.toAnsiString().c_str());
	return false;
}


void VContent::UnloadAll()
{
	textureDir.clear();
	imageDir.clear();
	fontDir.clear();
	soundDir.clear();

	VBase::VLog("All content directories cleared");
}

bool VContent::StoreTexture(const sf::String& name, const sf::Texture& texture)
{
	auto found = textureDir.find(name);
	if (found == textureDir.end())
	{
		VBase::VLog("%s stored", name.toAnsiString().c_str());
		textureDir.insert(std::pair<sf::String, std::unique_ptr<sf::Texture>>(
			name, std::unique_ptr<sf::Texture>(new sf::Texture(texture))));
		return true;
	}

	VBase::VLog("Error storing texture: %s", name.toAnsiString().c_str());
	return false;
}

bool VContent::StoreImage(const sf::String& name, const sf::Image& image)
{
	auto found = imageDir.find(name);
	if (found == imageDir.end())
	{
		VBase::VLog("%s stored", name.toAnsiString().c_str());
		imageDir.insert(std::pair<sf::String, std::unique_ptr<sf::Image>>(
			name, std::unique_ptr<sf::Image>(new sf::Image(image))));
		return true;
	}

	VBase::VLog("Error storing image: %s", name.toAnsiString().c_str());
	return false;
}

bool VContent::StoreFont(const sf::String& name, const sf::Font& font)
{
	auto found = fontDir.find(name);
	if (found == fontDir.end())
	{
		VBase::VLog("%s stored", name.toAnsiString().c_str());
		fontDir.insert(std::pair<sf::String, std::unique_ptr<sf::Font>>(
			name, std::unique_ptr<sf::Font>(new sf::Font(font))));
		return true;
	}

	VBase::VLog("Error storing font: %s", name.toAnsiString().c_str());
	return false;
}

bool VContent::StoreSound(const sf::String& name, const sf::SoundBuffer& sound)
{
	auto found = soundDir.find(name);
	if (found == soundDir.end())
	{
		VBase::VLog("%s stored", name.toAnsiString().c_str());
		soundDir.insert(std::pair<sf::String, std::unique_ptr<sf::SoundBuffer>>(
			name, std::unique_ptr<sf::SoundBuffer>(new sf::SoundBuffer(sound))));
		return true;
	}

	VBase::VLog("Error storing sound: %s", name.toAnsiString().c_str());
	return false;
}
