# WinSplitPlus

Window Hooks to change the Window Class, Window Name, Kill mutex and Create Window

Usage for CMD:
```CMD
WinSplitPlus.exe -player <Num> -winclass -winname -mutex <"Mutex Name"> -width <Num> -height <Num> -posx <Num> -posy <Num> <"Game Path"> <Game arguments>
```
Example:
```CMD
WinSplitPlus.exe -player 1 -winclass -winname -mutex "Game Mutex" -width 1280 -height 720 -posx 100 -posy 100 "C:\Games\Game.exe" -window
```

For Batch file example:
```batch
@echo off

START WinSplitPlus.exe ^
-Player 1 ^
-winclass ^
-winname ^
-mutex "Game Mutex" ^
-width 960 ^
-height 1080 ^
-posx 0 ^
-posy 0 ^
"C:\Game\Game.exe" -window
```
Options:
| Argument                    | Description                       |
|-----------------------------|-----------------------------------|
| `-Player <Number>`          | Identifier for this game instance |
| `-winclass`                 | Enable Window class hook          |
| `-winname`                  | Enable Window Name hook           |
| `-mutex <baseMutexName>`    | The base name for the game's mutex|
| `-width <width>`            | The window width                  |
| `-height <height>`          | The window height                 |
| `-posx <x>`                 | The window X position             |
| `-posy <y>`                 | The window Y position             |

Project imported from [here](https://www.swbfgamers.com/index.php?topic=11251.msg112827#msg112827) by SleepKiller
