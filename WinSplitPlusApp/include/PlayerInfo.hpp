#pragma once
#include<Windows.h>

#include<string>

#include"constants.h"

enum PlayerID {
	PlayerOne = 0,
	PlayerTwo = 1,
	PlayerThree = 2,
	PlayerFour = 3
 
};



class PlayerInfo
{
private:
	PlayerID _playerIndex;

	std::wstring _playerName;
	std::wstring _profilePath;

	bool _windowInfo = false;
	bool _profileExisted = false;

	std::size_t _wndSizeX = 0;
	std::size_t _wndSizeY = 0;
	std::size_t _wndPosX = 0;
	std::size_t _wndPosY = 0;

	PlayerInfo();

	void _defaultName();
	
public:
	PROCESS_INFORMATION processInfo = {};
	HWND windowHandle = NULL;

	bool nameChangedInProcess = false;

	bool setName(std::wstring name);
	std::wstring getName();

	std::wstring getCommandLine();

	void genWindowInfo(std::size_t playerCount, std::size_t screenSizeX, std::size_t screenSizeY);

	std::size_t getWndSizeX() { return _wndSizeX; };
	std::size_t getWndSizeY() { return _wndSizeY; };
	std::size_t getWndPosX() { return _wndPosX; };
	std::size_t getWndPosY() { return _wndPosY; };

	explicit PlayerInfo(PlayerID playerID);
	~PlayerInfo();
};

