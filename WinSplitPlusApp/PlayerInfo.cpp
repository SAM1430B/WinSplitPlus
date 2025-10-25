#include "include\PlayerInfo.hpp"



bool PlayerInfo::setName(std::wstring name)
{
	if (name.length() > MAX_NAME_LENGTH)
		return false;
	
	_playerName = name;

	return true;
}

std::wstring PlayerInfo::getName()
{
	return _playerName;
}

std::wstring PlayerInfo::getCommandLine()
{
	if(! _windowInfo)
		throw std::exception("Window infomation not supplied, unable to generate commandline.");

	_profilePath = L"SaveGames/";

	_profilePath += _playerName + L".profile";

	DWORD fileAttributes = GetFileAttributes(_profilePath.c_str());

	if (fileAttributes != INVALID_FILE_ATTRIBUTES && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		_profileExisted = true;
	}

	std::wstring commandLine = L" /win /resolution " +
		std::to_wstring(_wndSizeX) +
		L" "
		+
		std::to_wstring(_wndSizeY) +
		L" /lan /name SplitScreen";

	return commandLine;
}

void PlayerInfo::genWindowInfo(std::size_t playerCount, std::size_t screenSizeX, std::size_t screenSizeY)
{
	if (playerCount > 4 || playerCount < 2)
	{
		throw std::exception("So sorry dear fellow but that many players simply won't work here.");
	}

	std::size_t screenHalfSizeX = screenSizeX / 2;
	std::size_t screenHalfSizeY = screenSizeY / 2;
	
	if (playerCount == 2)
	{
		_wndSizeX = screenHalfSizeX;
		_wndSizeY = screenHalfSizeX * 0.75f;
		_wndPosX = 0;
		_wndPosY = (screenSizeY - _wndSizeY) / 2;

		if (_playerIndex == PlayerID::PlayerTwo)
		{
			_wndPosX = screenHalfSizeX;
		}

	}
	else if (playerCount == 3)
	{
		_wndSizeX = screenHalfSizeX;
		_wndSizeY = screenHalfSizeY;
		_wndPosX = 0;
		_wndPosY = 0;

		if (_playerIndex == PlayerID::PlayerTwo)
		{
			_wndPosX = screenHalfSizeX;
		}
		else if (_playerIndex == PlayerID::PlayerThree)
		{
			_wndPosX = screenSizeX / 4;
			_wndPosY = screenHalfSizeY;
		}
	}
	else
	{
		_wndSizeX = screenHalfSizeX;
		_wndSizeY = screenHalfSizeY;
		_wndPosX = 0;
		_wndPosY = 0;

		if (_playerIndex == PlayerID::PlayerTwo)
		{
			_wndPosX = screenHalfSizeX;
		}
		else if (_playerIndex == PlayerID::PlayerThree)
		{
			_wndPosY = screenHalfSizeY;
		}
		else if (_playerIndex == PlayerID::PlayerFour)
		{
			_wndPosX = screenHalfSizeX;
			_wndPosY = screenHalfSizeY;
		}
	}

	_windowInfo = true;
}


PlayerInfo::PlayerInfo(PlayerID playerID)
{
	_playerIndex = playerID;
	_defaultName();
}

void PlayerInfo::_defaultName()
{
	_playerName = L"Player #" + std::to_wstring(int (_playerIndex) + 1);
}

PlayerInfo::~PlayerInfo()
{
	if (!_profileExisted)
	{
		DeleteFile(_profilePath.c_str());
	}
}