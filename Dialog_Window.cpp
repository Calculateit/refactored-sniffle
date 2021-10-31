#include "Dialog_Window.h"

Dialog_Window::Dialog_Window(int xpos, int ypos, int width, int height, SDL_Renderer* m_pRenderer, TTF_Font* font, dialog_window_type window_type, std::string arg1, std::string arg2) {
    this->dialog_window.x = xpos;
    this->dialog_window.y = ypos;
    this->dialog_window.h = height;
    this->dialog_window.w = width;

    this->m_pRenderer = m_pRenderer;
    this->font = font;
    this->type = window_type;

    switch(window_type) {
        case COPY_WINDOW:
        case MOVE_WINDOW:
        case RENAME_WINDOW:
            constant_string = arg1;
            changing_string = arg2;
            SDL_StartTextInput();
            break;
        case MKDIR_WINDOW:
            changing_string.clear();
            constant_string = arg1;
            SDL_StartTextInput();
            break;
        case DELETE_WINDOW:
            constant_string = arg1;
            break;
    }

    //инициализация кнопок "ок" и "отмена"
    simple_button_type tmp_button{{.x = xpos + 30, .y = ypos + height - 10 - 50, .w = 70, .h = 50}, false, "ОК"};
    button_massive.push_back(tmp_button);

    tmp_button.rect.x = xpos - 30 + width - 70;
    tmp_button.rect.y = ypos + height - 10 - 50;
    tmp_button.rect.w = 70;
    tmp_button.rect.h = 50;
    tmp_button.is_chosen = true;
    tmp_button.str = "Отмена";
    button_massive.push_back(tmp_button);
    chosenButtonIndex = 1;

    source.x = 0;
    source.y = 0;
}

void Dialog_Window::dialogCursorLeft() {    //без проверки на истинность индекса chosenButtonIndex
    if(chosenButtonIndex >=0){
        button_massive[chosenButtonIndex].is_chosen = false;
        if(chosenButtonIndex > 0) {
            button_massive[--chosenButtonIndex].is_chosen = true;
        } else {
            chosenButtonIndex = button_massive.size();
            button_massive[chosenButtonIndex].is_chosen = true;
        }
    }
}

void Dialog_Window::dialogCursorRight() {   //без проверки на истинность индекса chosenButtonIndex
    if(chosenButtonIndex >= 0){
        button_massive[chosenButtonIndex].is_chosen = false;
        if(chosenButtonIndex < static_cast<int>(button_massive.size())) {
            button_massive[++chosenButtonIndex].is_chosen = true;
        } else {
            chosenButtonIndex = 0;
            button_massive[chosenButtonIndex].is_chosen = true;
        }
    }
}

void Dialog_Window::dialogCursorChoose() {
    if(button_massive[chosenButtonIndex].str == "ОК") {
        window_status = PATH_CHOSEN;
    } else {
        window_status = PATH_NOT_CHOSEN;
    }
}

void Dialog_Window::update() {
    std::string title = getWindowTitle();
    std::string question = generateQuestion();

    dest.x = dialog_window.x;
    dest.y = dialog_window.y;
    dest.h = dialog_window.h;
    dest.w = dialog_window.w;

    SDL_SetRenderDrawColor(m_pRenderer, DIALOG_WINDOW_BACKGROUND_COLOR);//отрисовка фона окна
    SDL_RenderFillRect(m_pRenderer, &dest);

    dest.x = static_cast<int>(dest.x + static_cast<float>(dest.w - title.size() * SYMBOL_PER_PIX_HORIZ) / 2 );//отрисовка заголовка окна
    dest.y += DIALOG_WINDOW_INNER_OFFSET;
    renderText(title, DIALOG_WINDOW_BACKGROUND_COLOR);

    dest.x = dialog_window.x + DIALOG_WINDOW_INNER_OFFSET;//отрисовка внутренней части окна
    dest.y += 2 * TEXT_HEIGHT;
    renderText(question, DIALOG_WINDOW_BACKGROUND_COLOR);
    if(question.find("Would you like to") != std::string::npos) {
        dest.y += TEXT_HEIGHT;
        renderText("by the following path?", DIALOG_WINDOW_BACKGROUND_COLOR);
    }

    if(type != DELETE_WINDOW) {
        dest.y += 2 * TEXT_HEIGHT;
        renderText(changing_string, ACTION_DOC_BUTTONS_COLOR);
        dest.w = dialog_window.w - 2 * DIALOG_WINDOW_INNER_OFFSET;
    }

    renderButtons();
}

std::string Dialog_Window::generateQuestion() {
    int pos = constant_string.find_last_of('/');
    std::string tmp_string = constant_string.substr(pos + 1);
    switch(type) {
        case COPY_WINDOW:
            return "Would you like to copy file " + tmp_string;
        case MOVE_WINDOW:
            return "Would you like to move file " + tmp_string;
        case MKDIR_WINDOW:
            return "Enter new name of the directory.";
        case DELETE_WINDOW:
            return "Are you really would like to remove " + tmp_string + "?";
        case RENAME_WINDOW:
            return "Enter new name for the file " + tmp_string + ".";
    }
    return "Error";
}

void Dialog_Window::renderButtons() {
    for(int i = 0; i < static_cast<int>(button_massive.size()); ++i) {
        if(button_massive[i].is_chosen) {
            SDL_SetRenderDrawColor(m_pRenderer, CURSOR_COLOR);
        } else {
            SDL_SetRenderDrawColor(m_pRenderer, DIALOG_WINDOW_BACKGROUND_COLOR);
        }
        SDL_RenderFillRect(m_pRenderer, &button_massive[i].rect);
        renderText(button_massive[i].str, button_massive[i].rect);
    }

}

const std::string Dialog_Window::getWindowTitle() {
    switch(type) {
        case COPY_WINDOW:
            return "Copy";
        case MOVE_WINDOW:
            return "Move";
        case MKDIR_WINDOW:
            return "Create directory";
        case DELETE_WINDOW:
            return "Remove file";
        case RENAME_WINDOW:
            return "Rename file";
        default:
            return "New window";
    }
}

void Dialog_Window::renderText(const std::string& str, const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a) {
    SDL_Texture* text = NULL;
    SDL_Surface* surf = NULL;

    if(!str.size()) {
        return;
    }
    surf = TTF_RenderUTF8_Blended(font, str.c_str(), {0, 0, 0});

    text=SDL_CreateTextureFromSurface(m_pRenderer, surf);
    if(text!=0) {
        SDL_FreeSurface(surf);
    } else {
        std::cerr<<"TTF4 text error\n";
        return;
    }
    SDL_QueryTexture(text, 0, 0, &source.w, &source.h);
    dest.w = source.w;
    dest.h = source.h;

    SDL_SetRenderDrawColor(m_pRenderer, r, g, b, a);
    SDL_RenderFillRect(m_pRenderer, &dest);

    SDL_RenderCopy(m_pRenderer, text, &source, &dest);

    SDL_DestroyTexture(text);
}

void Dialog_Window::renderText(const std::string& str, SDL_Rect& rect) {
    SDL_Texture* text;
    SDL_Surface* surf=TTF_RenderUTF8_Blended(font, str.c_str(), {0, 0, 0});

    text=SDL_CreateTextureFromSurface(m_pRenderer, surf);
    if(text!=0) {
        SDL_FreeSurface(surf);
    } else {
        std::cerr << "TTF2 text error\n";
        return;
    }
    SDL_QueryTexture(text, 0, 0, &source.w, &source.h);
    rect.w = source.w;
    rect.h = source.h;

    SDL_RenderCopy(m_pRenderer, text, &source, &rect);

    SDL_DestroyTexture(text);
}

Dialog_Window::~Dialog_Window() {
    SDL_StopTextInput();
}
