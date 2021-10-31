#include "View_File_Window.h"

View_File_Window::View_File_Window(int xpos, int ypos, int width, int height, SDL_Renderer* m_pRenderer, TTF_Font* font, std::string file_name)
{
    this->view_file_window.x = xpos;
    this->view_file_window.y = ypos;
    this->view_file_window.h = height;
    this->view_file_window.w = width;
    this->font = font;
    this->m_pRenderer = m_pRenderer;

    source.x = 0;
    source.y = 0;

    dest.x = view_file_window.x;
    dest.y = view_file_window.y;
    dest.h = view_file_window.h;
    dest.w = view_file_window.w;

    std::ifstream in;   //поток для чтения

    std::string line;
    in.open(file_name); //окрываем файл для определения длины дин. массива строк
    if (in.is_open()) {
        while (getline(in, line)) {
            file_size++;
        }
    }
    in.close();
    file = new std::string[++file_size];

    in.open(file_name); //окрываем файл для чтения
    int i = 0;
    if (in.is_open()) {
        while (getline(in, file[i])) {
            ++i;
        }
    }
    in.close();
}

void View_File_Window::update() {
    std::string substring;
    int substrings_amount = 0;
    int symbols_in_one_string = 0;

    dest.x = view_file_window.x;
    dest.y = view_file_window.y;
    dest.h = view_file_window.h;
    dest.w = view_file_window.w;

    SDL_SetRenderDrawColor(m_pRenderer, DIALOG_WINDOW_BACKGROUND_COLOR);//отрисовка фона окна
    SDL_RenderFillRect(m_pRenderer, &dest);

    int i = 0;
    int page_counter = 0;
    while( page_counter - 1 != page && i != file_size) {

        dest.y = 0;
        for(; i < file_size && dest.y < (view_file_window.h - TEXT_HEIGHT); ++i) {
            if(static_cast<float>(file[i].size()) * SYMBOL_PER_PIX_HORIZ < static_cast<float>(view_file_window.w)) {
                if(page_counter == page) {
                    renderLine(file[i], dest);
                }
                dest.y += TEXT_HEIGHT;
            } else {
                std::string str = file[i];
                substrings_amount = static_cast<int>((static_cast<float>(file[i].size()) * SYMBOL_PER_PIX_HORIZ) / static_cast<float>(view_file_window.w)) + 1;
                symbols_in_one_string = static_cast<int>(static_cast<float>(view_file_window.w) / SYMBOL_PER_PIX_HORIZ);
                for(int j = 0; j < substrings_amount && dest.y < (view_file_window.h - TEXT_HEIGHT); ++j) {
                    if(page_counter == page) {
                        substring = str.substr(j * symbols_in_one_string, (j + 1) * symbols_in_one_string);
                        renderLine(substring, dest);
                    }
                    dest.y += TEXT_HEIGHT;
                }
            }
        }
        ++page_counter;
    }
}

void View_File_Window::renderLine(std::string str, SDL_Rect& rect) {
    if(!str.size()) {
        return;
    }
    SDL_Texture* text;
    SDL_Surface* surf=TTF_RenderUTF8_Blended(font, str.c_str(), {0, 0, 0});

    text=SDL_CreateTextureFromSurface(m_pRenderer, surf);
    if(text!=0) {
        SDL_FreeSurface(surf);
    } else {
        std::cerr<<"Can`t render symbols of the file\n";
        return;
    }
    SDL_QueryTexture(text, 0, 0, &source.w, &source.h);
    rect.w = source.w;
    rect.h = source.h;

    SDL_RenderCopy(m_pRenderer, text, &source, &rect);

    SDL_DestroyTexture(text);
}

void View_File_Window::setPrevPage() {
    if(page > 0) {
        --page;
    }
}

void View_File_Window::setNextPage() {
    if(page < INT_MAX) {
        page++;
    }
}

View_File_Window::~View_File_Window()
{
    delete[] file;
}
