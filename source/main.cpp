#include <iostream>
#include <queue>
#include <gccore.h>
#include <network.h>
#include <wiiuse/wpad.h>
#include "SnakeController.h"
#include "NetSnakeController.h"
#include "ConsoleUI/Canvas.h"
#include "Protocol/Connection.h"

int main(int argc, char **argv) {
    VIDEO_Init();
    WPAD_Init();
    GXRModeObj *rmode = VIDEO_GetPreferredMode(NULL);
    void *fb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    CON_Init(fb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(fb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if(rmode->viTVMode & VI_NON_INTERLACE)
        VIDEO_WaitVSync();
    int cols, rows;
    CON_GetMetrics(&cols, &rows);
    
    SnakeController snake(rows - 3, cols, [](unsigned int x, unsigned int y, char c) {
        printf("\033[%d;%dH%c", y + 2, x, c);
    });
    std::queue<u32> snakeInputs;
    bool gameStarted = false;
    // startGame ensures that canvas doesn't clear snake
    bool startGame = false;
    bool dying = false;
    unsigned int timer = 0;

    ConsoleUI::Canvas canvas({ cols, rows });
    auto screen_blank = canvas.addScreen();
    auto screen_title = canvas.addScreen();
    canvas.setScreen(screen_title);
    auto text_name = screen_title->addText("Epic Snake", { 0, 0 }, true, ConsoleUI::Align::CENTER_ALL);
    auto text_score = screen_title->addText("", { 0, 0 }, false);
    auto btn_start = screen_title->addButton("Start", { -10, 4 }, true, std::function<void()>(), ConsoleUI::Align::CENTER_ALL);
    auto btn_options = screen_title->addButton("Options", { 10, 4 }, true, std::function<void()>(), ConsoleUI::Align::CENTER_ALL);

    btn_start->setCallback([&]() {
            startGame = true;
            canvas.setScreen(screen_blank);
    });

    auto screen_options = canvas.addScreen();
    auto btn_back = screen_options->addButton("Back", { cols - 5, rows - 4 });
    auto text_online = screen_options->addText("Online: ", { -15, 0 }, true, ConsoleUI::Align::CENTER_ALL);
    auto btn_online = screen_options->addButton("False", { -8, 0 }, true, std::function<void()>(), ConsoleUI::Align::CENTER_ALL);
    bool online = false;
    /*Protocol::Connection test;
    if(!test.connect("192.168.0.1", 5554))
        exit(EXIT_FAILURE);*/

    btn_online->setCallback([&]() {
        btn_online->setText((online = !online) ? "True" : "False");
    });
    btn_options->setCallback([&]() {
            canvas.setScreen(screen_options);
    });
    btn_back->setCallback([&]() {
            canvas.setScreen(screen_title);
    });
    
    while(true) {
        WPAD_ScanPads();
        u32 pressed = WPAD_ButtonsDown(0);
        if (pressed & WPAD_BUTTON_HOME)
            exit(EXIT_SUCCESS);

        if(gameStarted && (pressed & (WPAD_BUTTON_UP | WPAD_BUTTON_DOWN | WPAD_BUTTON_LEFT | WPAD_BUTTON_RIGHT))) {
            snakeInputs.push(pressed);
        }

        if(startGame) {
            canvas.update(pressed);
            gameStarted = true;
            startGame = false;
            snake.start();
        } else {
            canvas.update(pressed);
        }

        if(gameStarted && timer % 5 == 0 && !dying) {
            u32 in = 0;
            if(!snakeInputs.empty()) {
                in = snakeInputs.front();
                snakeInputs.pop();
            }

            if(in & WPAD_BUTTON_UP) {
                snake.setDirection(SnakeDirection::UP);
            } else if(in & WPAD_BUTTON_DOWN) {
                snake.setDirection(SnakeDirection::DOWN);
            } else if(in & WPAD_BUTTON_LEFT) {
                snake.setDirection(SnakeDirection::LEFT);
            } else if(in & WPAD_BUTTON_RIGHT) {
                snake.setDirection(SnakeDirection::RIGHT);
            }
            if(!snake.moveForward()) {
                dying = true;
                gameStarted = false;
            }
        } else if(dying && snake.getLength() > 0 && timer % 5 == 0) {
            snake.removeBack();
        } else if(dying && snake.getLength() < 1) {
            std::queue<u32>().swap(snakeInputs);
            int scr = snake.reset();
            text_score->setText("So close! Score: " + std::to_string(scr));
            text_score->move({ cols - static_cast<int>(text_score->text().size()), 0 });
            canvas.setScreen(screen_title);
            dying = false;
        }

        ++timer;
        VIDEO_WaitVSync();
    }

    return 0;
}
