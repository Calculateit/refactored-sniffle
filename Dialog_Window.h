#ifndef DIALOG_WINDOW_H
#define DIALOG_WINDOW_H

#include <string>
#include <vector>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Constants.h"

enum dialog_window_type {
    COPY_WINDOW,
    MOVE_WINDOW,
    MKDIR_WINDOW,
    DELETE_WINDOW,
    RENAME_WINDOW
};

enum dialog_window_state {
    PATH_CHOSEN,
    PATH_NOT_CHOSEN,
    IN_JOB
};

struct simple_button_type {
    SDL_Rect rect;
    bool is_chosen;
    std::string str;
};


class Dialog_Window {
    public:
        Dialog_Window(int xpos, int ypos, int width, int height, SDL_Renderer* m_pRenderer, TTF_Font* font, dialog_window_type window_type, std::string arg1, std::string arg2);
        virtual ~Dialog_Window();
        void update();

        void userAddString(const std::string sym) { changing_string += sym; }
        void userPressedBackSpace() { if(changing_string.size()) changing_string.pop_back(); }
        void dialogCursorLeft();
        void dialogCursorRight();
        void dialogCursorChoose();

        dialog_window_type getWindowType() { return type; }
        dialog_window_state getWindowStatus() { return window_status; }
        const std::string& getChangingString() const { return changing_string; }
        const std::string& getConstantString() const { return constant_string; }
    private:
        std::vector<simple_button_type> button_massive;
        const std::string getWindowTitle();
        void renderText(const std::string& str, const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a);
        void renderText(const std::string& str, SDL_Rect& rect);
        std::string generateQuestion();
        void renderButtons();

        int chosenButtonIndex = -1;
        dialog_window_state window_status = IN_JOB;

        std::string changing_string;
        std::string constant_string;

        TTF_Font* font = NULL;
        SDL_Renderer* m_pRenderer = NULL;
        SDL_Rect dialog_window;
        SDL_Rect dest;
        SDL_Rect source;
        dialog_window_type type;

};

#endif // DIALOG_WINDOW_H
