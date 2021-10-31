#include "Cursor.h"

Cursor::Cursor(int xpos, int ypos, int width, int height, SDL_Renderer* m_pRenderer) {
    this->m_pRenderer = m_pRenderer;

    int w;
    SDL_GetRendererOutputSize(m_pRenderer, &w, &window_height);

    cursor_rect.h = height;
    cursor_rect.w = width;
    start_pos.x = cursor_rect.x = xpos;
    start_pos.y = cursor_rect.y = ypos;
}

void Cursor::update_cursor() {
    SDL_SetRenderDrawColor(m_pRenderer, CURSOR_COLOR);
    SDL_RenderFillRect(m_pRenderer, &cursor_rect);
}

void Cursor::up() {
    if(cursor_rect.y - CURSOR_HEIGHT >= UP_OFFSET) {
        next_page_flag = false;
        cursor_rect.y -= CURSOR_HEIGHT;
        --cursor_index;
    } else {
        prev_page_flag = true;
    }
}

void Cursor::down() {
    if((cursor_rect.y + CURSOR_HEIGHT) + CURSOR_HEIGHT < window_height - DOWN_OFFSET) {
        prev_page_flag = false;
        cursor_rect.y += CURSOR_HEIGHT;
        ++cursor_index;
    } else {
        next_page_flag = true;
    }
}

void Cursor::changeArea() {
    if(!getCursorArea()) {
        cursor_rect.x += cursor_rect.w;
    } else {
        cursor_rect.x -= cursor_rect.w;
    }
}

 void Cursor::setPrevOrNextPageCursor(bool is_next) {
    if(is_next) {
        cursor_rect.y = start_pos.y;
        ++cursor_index;
        next_page_flag = false;
    } else {
        cursor_rect.y = window_height - 2*DOWN_OFFSET;
        --cursor_index;
        prev_page_flag = false;
    }

 }

 void Cursor::setNewDocumentCursor() {
    cursor_rect.y = start_pos.y;
    cursor_index = 0;
    next_page_flag = false;
    prev_page_flag = false;
 }

 bool Cursor::getCursorArea() {
    if(cursor_rect.x == cursor_rect.w) return 1;
    else return 0;
 }



Cursor::~Cursor() {
    //dtor
}
