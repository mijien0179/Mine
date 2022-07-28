#pragma once
#include <d2d1.h>
#include <random>
#include <iostream>

namespace Mine {
	class Block {
	public:
		typedef unsigned char TYPE_BLOCK_STATE;
		enum class BLOCK_STATE : TYPE_BLOCK_STATE
		{
			CLOSED = 0,
			PICKED = 1,
			OPENED = 2,
		};
		friend bool operator&(BLOCK_STATE lhs, BLOCK_STATE rhs) {
			return static_cast<TYPE_BLOCK_STATE>(lhs) & static_cast<TYPE_BLOCK_STATE>(rhs);
		}

	private:
		BLOCK_STATE state;
		bool _isBomb;
		size_t aroundBomb;

	public:
		void IncAroundBomb() { ++aroundBomb; }
		size_t GetAroundBomb() const { return aroundBomb; }
		bool isBomb() const { return _isBomb; }
		void SetBomb(bool isActivate) {_isBomb = isActivate;}

		void SetState(BLOCK_STATE option) {state = option;}
		BLOCK_STATE GetState() const { return state; }

	public:
#pragma warning(suppress:26495)
		Block() : Block{false}
		{}

		Block(bool isBomb) : state{BLOCK_STATE::CLOSED}, _isBomb{isBomb}, aroundBomb{}
		{}
	};

	interface IMap {
		D2D1_SIZE_F blockSize;
		D2D1_SIZE_U mapSize;
		Block** bList;
		size_t bombCount;
		
	private:
		void MapInitialize();

	public:
		inline D2D1_SIZE_F GetBlockSize() const { return blockSize; }
		inline D2D1_SIZE_U GetMapSize() const { return mapSize; }
		Block* GetBlock(size_t x, size_t y) const {
			if (x >= mapSize.width || y >= mapSize.height) throw "IMap::GetBlock ¹üÀ§ ¹þ¾î³²";
			return bList[y] + x;
		}
		inline bool isAvailable(size_t x, size_t y) {
			if (x < 0 || x >= mapSize.width) return false;
			if (y < 0 || y >= mapSize.height) return false;

			return true;
		}
		inline bool isAvailable(D2D1_POINT_2U pos) {
			return isAvailable(pos.x, pos.y);
		}
		bool isClear() const {
			size_t bombCount = 0;
			Block* bTemp;
			for (size_t y = 0; y < mapSize.height; ++y) {
				for (size_t x = 0; x < mapSize.width; ++x) {
					bTemp = GetBlock(x, y);
					switch(bTemp->GetState()) {
						case Block::BLOCK_STATE::CLOSED:
							return false;
						case Block::BLOCK_STATE::PICKED:
							if (bTemp->isBomb()) {
								++bombCount;
							}
					}
				}
			}

			if (bombCount == this->bombCount) return true;
			return false;
		}


	public:
		IMap(D2D1_SIZE_U size, size_t bombCount, D2D1_SIZE_F blockSize = D2D1::SizeF(30.f,30.f)) : mapSize{size}, blockSize{blockSize}, bombCount{bombCount}
		{
			MapInitialize();

			for (size_t y = 0; y < mapSize.height; ++y) {
				using namespace std;
				for (size_t x = 0; x < mapSize.width; ++x) {
					if (bList[y][x].isBomb()) {
						cout << " *";
					}
					else {
						if (bList[y][x].GetAroundBomb()) {
							cout << " " << (int)bList[y][x].GetAroundBomb();
						}
						else {
							cout << " -";
						}
					}
				}
				cout << endl;
			}

		}

		virtual ~IMap() {
			if (bList) {
				for (size_t i = 0; i < mapSize.height; ++i) {
					delete[] bList[i];
				}
				delete[] bList;
			}
		}

	};

	class MapEasy : public IMap {

	public:
		MapEasy() : IMap{D2D1::SizeU(7,10), 5,D2D1::SizeF(50.f,50.f)}{}
	};

	class MapNormal : public IMap {
	public:
		MapNormal() : IMap{D2D1::SizeU(13,18), 35}{}
	};

	class MapHard : public IMap {
	public:
		MapHard() : IMap{D2D1::SizeU(33,20), 140}{}
	};

	class MapHell : public IMap {
	public:
		MapHell() : IMap{D2D1::SizeU(33,22), 230}{}
	};

}
