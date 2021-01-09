#pragma once

#include <string>
#include "SnakeController.h"

class NetSnakeController: public SnakeController {
    private:
    public:
        NetSnakeController(unsigned int rows, unsigned int cols, void (*printCharAt)(unsigned int, unsigned int, char));
};