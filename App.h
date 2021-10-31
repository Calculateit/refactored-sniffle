#ifndef APP_H
#define APP_H
#include <SDL2/SDL.h>
#include <iostream>
#include <SDL2/SDL_ttf.h>
#include <sstream>

#include "Area.h"   //Класс зоны просмотра
#include "Action_Doc.h"//Класс нижнего дока
#include "Constants.h"

class App
{
    public:
        App(const char* title, const int xpos, const int ypos, const int width, const int height, bool fullscreen);
        virtual ~App();
        bool init(const char* title, const int xpos, const int ypos, const int width, const int height, bool fullscreen);
        void render();
        void handleEvents();
        void clean();
        bool running() const {return m_bRunning;}
    private:
        Area* area[2]{NULL, NULL};
        Action_Doc* action_doc = NULL;

        SDL_Window* m_pWindow;
        SDL_Renderer* m_pRenderer;
        TTF_Font* font;
        SDL_Texture* text;
        bool m_bRunning;
};

#endif // APP_H
