#ifndef VIEW_FILE_WINDOW_H
#define VIEW_FILE_WINDOW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <fstream>
#include <climits>

#include "Constants.h"

class View_File_Window
{
    public:
        View_File_Window(int xpos, int ypos, int width, int height, SDL_Renderer* m_pRenderer, TTF_Font* font, std::string file_name);
        virtual ~View_File_Window();
        void update();
        void setNextPage();
        void setPrevPage();
    private:
        TTF_Font* font = NULL;
        SDL_Renderer* m_pRenderer = NULL;
        SDL_Rect view_file_window;
        SDL_Rect dest;
        SDL_Rect source;

        int page = 0;

        void renderLine(std::string str, SDL_Rect& rect);

        int file_size = 0;
        std::string* file = NULL;
};

#endif // VIEW_FILE_WINDOW_H
