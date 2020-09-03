#include <iostream>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <network.h>
#include "SnakeController.h"
#include "ConsoleUI/Screen.h"

int main(int argc, char **argv) {
    VIDEO_Init();
    WPAD_Init();
    GXRModeObj *rmode = VIDEO_GetPreferredMode(NULL);
    void *fb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    CON_Init(fb, 20,20 ,rmode->fbWidth ,rmode->xfbHeight ,rmode->fbWidth * VI_DISPLAY_PIX_SZ);
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(fb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

    int cols, rows;
    CON_GetMetrics(&cols, &rows);
    
    SnakeController snake(rows - 3, cols, [](unsigned int x, unsigned int y, char c) {
        printf("\x1b[%d;%dH%c", y + 2, x, c);
    });
    bool gameStarted = false;
    bool dying = false;
    unsigned int timer = 0;

    ConsoleUI::Screen screen({ cols, rows });
    auto nameText = screen.addText("Epic Snake", { 0, 0 }, true, ConsoleUI::Align::CENTER_ALL);
    auto scoreText = screen.addText("", { 0, 0 }, false);
    auto beginHint = screen.addButton("Press 'A' to start!", { 0, 1 }, true, std::function<void()>(), ConsoleUI::Align::CENTER_ALL);
    beginHint->setCallback([&]() {
            gameStarted = true;
            snake.start();
            nameText->setVisible(false);
            beginHint->setVisible(false);
            scoreText->setVisible(false);
    });
    
    while(1) {
        WPAD_ScanPads();
        u32 pressed = WPAD_ButtonsDown(0);
        if (pressed & WPAD_BUTTON_HOME) {
            exit(0);
        }
        if(gameStarted) {
            if(pressed & WPAD_BUTTON_UP) {
                snake.setDirection(SnakeDirection::UP);
            } else if(pressed & WPAD_BUTTON_DOWN) {
                snake.setDirection(SnakeDirection::DOWN);
            } else if(pressed & WPAD_BUTTON_LEFT) {
                snake.setDirection(SnakeDirection::LEFT);
            } else if(pressed & WPAD_BUTTON_RIGHT) {
                snake.setDirection(SnakeDirection::RIGHT);
            }
        }

        screen.update(pressed);

        if(gameStarted && timer % 5 == 0 && !dying) {
            if(!snake.moveForward()) {
                dying = true;
                gameStarted = false;
            }
        } else if(dying && snake.getLength() > 0 && timer % 5 == 0) {
            snake.removeBack();
        } else if(dying && snake.getLength() < 1) {
            int scr = snake.reset();
            scoreText->setText("So close! Score: " + std::to_string(scr));
            scoreText->move({ cols - static_cast<int>(scoreText->text().size()), 0 });
            scoreText->setVisible(true);
            nameText->setVisible(true);
            beginHint->setVisible(true);
            dying = false;
        }

        timer++;
        VIDEO_WaitVSync();
    }

    return 0;
}
