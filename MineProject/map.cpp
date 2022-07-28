#include "map.h"

void Mine::IMap::MapInitialize() {
	static std::default_random_engine engine{GetTickCount()};
	std::uniform_int_distribution<size_t> rHeight{0U,mapSize.height-1};
	std::uniform_int_distribution<size_t> rWidth{0U,mapSize.width-1};


	bList = new Block * [mapSize.height];
	for (size_t i = 0; i < mapSize.height; ++i) {
		bList[i] = new Block[mapSize.width]{};
	}

	
	POINT offset[8]{
		{-1,-1},{-1,0},{-1,1},
		{0,-1},{0,1},
		{1,-1},{1,0},{1,1}
	};

	D2D1_POINT_2U offsetPoint;
	size_t x, y;
	size_t i = 0;
	do {
		x = rWidth(engine);
		y = rHeight(engine);
		
		if (bList[y][x].isBomb()) continue;

		bList[y][x].SetBomb(true);
		for (int k = 0; k < 8; ++k) {
			offsetPoint = D2D1::Point2U(x + offset[k].x, y + offset[k].y);
			if (isAvailable(offsetPoint)) {
				bList[offsetPoint.y][offsetPoint.x].IncAroundBomb();
			}
		}

		++i;
	} while (i < bombCount);

}
