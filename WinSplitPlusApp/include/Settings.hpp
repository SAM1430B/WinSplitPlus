#include<cstdint>


namespace SWBF
{

	enum ServerType
	{
		None = 1,
		Game = 2,
		Dedicated = 3
	};

	enum AIDifficulty
	{
		Easy = 1,
		Normal = 2,
		Hard = 3
	};

	enum HeroAwardMode
	{
		MostPoints = 1,
		LeastPoints = 4,
		Random = 7
	};

	struct Settings
	{
		std::uint8_t playerCount;
		ServerType serverType;
	};
}