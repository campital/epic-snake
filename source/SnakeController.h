#pragma once

#include <random>
#include <queue>
#include <set>

enum class CellType {
	FOOD,
	SNAKE,
	EMPTY,
    INVALID
};

enum class SnakeDirection {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct pos_t {
	unsigned int x;
	unsigned int y;
};

class SnakeController {
	private:
		int m_score = 0;
		unsigned int m_rows, m_cols;
        unsigned int m_waitTail = 0;
        pos_t m_foodLoc;
		std::queue<pos_t> m_positions;
		std::set<unsigned int> m_allOccupied;
		SnakeDirection m_direction = SnakeDirection::RIGHT;
		std::mt19937 randEngine{std::random_device{}()};
        void (*m_printCharAt)(unsigned int, unsigned int, char);

		void addFood();
		CellType checkPos(pos_t pos);
		void addFront(pos_t pos);
	public:
		SnakeController(unsigned int rows, unsigned int cols, void (*printCharAt)(unsigned int, unsigned int, char));
		bool moveForward();
		bool setDirection(SnakeDirection dir);
		void removeBack();
		int reset();
		void start();
		int getLength();
		int getScore();
};