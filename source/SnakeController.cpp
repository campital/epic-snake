#include <algorithm>
#include "SnakeController.h"

void SnakeController::addFood()
{
	std::uniform_int_distribution<unsigned int> randGen(0, (m_rows * m_cols - 1) - m_allOccupied.size());
	unsigned int currPos = randGen(randEngine);
	auto last = m_allOccupied.begin();

	unsigned int between = 1;
	while(between > 0) {
		auto before = m_allOccupied.upper_bound(currPos);
		between = std::distance(last, before);
		currPos += between;
		last = before;
	}
    (*m_printCharAt)(currPos % m_cols, currPos / m_cols, '$');
    m_foodLoc = {currPos % m_cols, currPos / m_cols};
}

CellType SnakeController::checkPos(pos_t pos)
{
    if(pos.x < m_cols && pos.y < m_rows) {
        if(pos.x == m_foodLoc.x && pos.y == m_foodLoc.y) {
            return CellType::FOOD;
		} else if(m_allOccupied.find(pos.y * m_cols + pos.x) != m_allOccupied.end()) {
			return CellType::SNAKE;
		} else {
            return CellType::EMPTY;
		}
    }
    return CellType::INVALID;
}

void SnakeController::addFront(pos_t pos)
{
    m_positions.push(pos);
    m_allOccupied.insert(pos.y * m_cols + pos.x);
    (*m_printCharAt)(pos.x, pos.y, '#');
}

void SnakeController::removeBack()
{
    pos_t pos = m_positions.front();
    m_positions.pop();
    m_allOccupied.erase(pos.y * m_cols + pos.x);
    (*m_printCharAt)(pos.x, pos.y, ' ');
}

// this does not clear any snake chars on screen, it only resets internally
int SnakeController::reset()
{
	(*m_printCharAt)(m_foodLoc.x, m_foodLoc.y, ' ');
	m_waitTail = 0;
	int tmpScore = m_score;
	m_score = 0;
	m_allOccupied.clear();
	std::queue<pos_t>().swap(m_positions);
	m_direction = SnakeDirection::RIGHT;
	return tmpScore;
}

void SnakeController::start()
{
	addFront({0, 0});
	addFood();
}

SnakeController::SnakeController(unsigned int rows, unsigned int cols, void (*printCharAt)(unsigned int, unsigned int, char)) :
	m_rows(rows), m_cols(cols), m_printCharAt(printCharAt) {}

bool SnakeController::moveForward()
{
	pos_t currHead = m_positions.back();
	switch(m_direction) {
		case SnakeDirection::DOWN:
			currHead.y++;
			break;
		case SnakeDirection::UP:
			currHead.y--;
			break;
		case SnakeDirection::RIGHT:
			currHead.x++;
			break;
		case SnakeDirection::LEFT:
			currHead.x--;
			break;
	}
    
	CellType newLoc;
	if((newLoc = checkPos(currHead)) == CellType::SNAKE) {
		return false;
    } else if(newLoc == CellType::INVALID) {
        return false;
    }
	
	addFront(currHead);
	if(newLoc == CellType::FOOD) {
        m_score++;
        m_waitTail = 1;
		addFood();
    }
    if(m_waitTail == 0) {
	    removeBack();
    } else {
        m_waitTail--;
    }
	return true;
}

bool SnakeController::setDirection(SnakeDirection dir)
{
	if((m_direction == SnakeDirection::DOWN && dir == SnakeDirection::UP) ||
		(m_direction == SnakeDirection::UP && dir == SnakeDirection::DOWN) ||
		(m_direction == SnakeDirection::LEFT && dir == SnakeDirection::RIGHT) ||
		(m_direction == SnakeDirection::RIGHT && dir == SnakeDirection::LEFT)) {
		return false;
	}
	m_direction = dir;
	return true;
}

int SnakeController::getLength()
{
	return m_positions.size();
}

int SnakeController::getScore()
{
	return m_score;
}