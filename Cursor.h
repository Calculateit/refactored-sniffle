#ifndef CURSOR_H
#define CURSOR_H

#include <SDL2/SDL.h>
#include <iostream>
#include <SDL2/SDL_ttf.h>

#include "Constants.h"

class Cursor
{
    public:
        Cursor(int xpos, int ypos, int width, int height, SDL_Renderer* m_pRenderer);
        void update_cursor();
        virtual ~Cursor();
        void up();
        void down();
        void changeArea();
        int getCursorIndex() { return cursor_index; }
        bool getNextPageFlag() { return next_page_flag; }
        bool getPrevPageFlag() { return prev_page_flag; }
        bool getCursorArea();// 0 - первая зона, 1 - вторая
        void setPrevOrNextPageCursor(bool is_next);
        void setNewDocumentCursor();
    private:
        struct pos{
            int x, y;
        } start_pos;
        SDL_Renderer* m_pRenderer = NULL;
        SDL_Rect cursor_rect;
        int window_height = 0;
        int cursor_index = 0;
        bool prev_page_flag = false;
        bool next_page_flag = false;
};

#endif // CURSOR_H
