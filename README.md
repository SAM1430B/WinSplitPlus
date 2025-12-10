# WinSplitPlus

Some games can not be launched with multi instance even with killing the game mutex, so it checks for the existed window class name and prevents multi game run.

WinSplitPlus is a window hooks to change the window class, window name and kill the game mutex.

CMD usage:
```CMD
WinSplitPlus.exe -Player <Num> -WinClass -WinName -Mutex <"Mutex Name"> -Width <Num> -Height <Num> -Posx <Num> -Posy <Num> <"Game Path"> <Game arguments>
```
Example:
```CMD
WinSplitPlus.exe -Player 1 -A -WinClass -WinName -Mutex "GameMutex" -Width 1280 -Height 720 -Posx 100 -Posy 100 "C:\Games\Game.exe" -window
```

For Batch file example:
```batch
@echo off

START WinSplitPlus.exe ^
-Player 1 ^
-A ^
-WinClass ^
-WinName ^
-Mutex "GameMutex" ^
-Width 960 ^
-Height 1080 ^
-Posx 0 ^
-Posy 0 ^
"C:\Game\Game.exe" -window
```
Options:
| Argument                    | Description                                   |
|-----------------------------|-----------------------------------------------|
| `-Player <Number>`          | Identifier for this game instance             |
| `-WinClass`                 | Enable Window class hook                      |
| `-WinName`                  | Enable Window Name hook                       |
| `-Mutex <BaseMutexName>`    | The base name for the game's mutex            |
| `-Width <Width>`            | The window width                              |
| `-Height <Height>`          | The window height                             |
| `-Posx <X>`                 | The window X position                         |
| `-Posy <Y>`                 | The window Y position                         |
| `-A`                        | Enable ANSI hooks (Best for old games)        |
| `-W`                        | Enable Unicode hooks (Best for modern games)  |
| `-Std`                      | Enable Standard hooks                         |
| `-Ex`                       | Enable Extended hooks                         |

Project imported from [here](https://www.swbfgamers.com/index.php?topic=11251.msg112827#msg112827) by SleepKiller
