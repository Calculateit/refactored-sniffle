#include "Action_Doc.h"

namespace fs = std::filesystem;

Action_Doc::Action_Doc(int xpos, int ypos, int width, int height, SDL_Renderer* m_pRenderer, TTF_Font* font) :
action_doc{ .x=xpos, .y = ypos, .w = width, .h = height}
{
    this->m_pRenderer = m_pRenderer;
    this->font = font;

    source.x = 0;
    source.y = 0;
}

void Action_Doc::update() {
    dest.x = action_doc.x;
    dest.y = action_doc.y;
    dest.h = action_doc.h;
    dest.w = action_doc.w;

    SDL_SetRenderDrawColor(m_pRenderer, ACTION_DOC_COLOR);
    SDL_RenderFillRect(m_pRenderer, &dest);
    for(int i = 0; i <static_cast<int>(actions_massive.size()); ++i) {
        renderText(actions_massive[i], ACTION_DOC_BUTTONS_COLOR);
        dest.x += 100;
    }

    if(dialog_window) {
        switch(dialog_window->getWindowStatus()) {
            case PATH_CHOSEN:
                switch(dialog_window->getWindowType()) {
                    case COPY_WINDOW:
                        copy(dialog_window->getConstantString(), dialog_window->getChangingString());
                        break;
                    case MOVE_WINDOW:
                        move(dialog_window->getConstantString(), dialog_window->getChangingString());
                        break;
                    case MKDIR_WINDOW:
                        mkdir(dialog_window->getConstantString(), dialog_window->getChangingString());
                        break;
                    case DELETE_WINDOW:
                        clear(dialog_window->getConstantString());
                        break;
                    case RENAME_WINDOW:
                        rename(dialog_window->getConstantString(), dialog_window->getChangingString());
                        break;
                }
            case PATH_NOT_CHOSEN:
                delete dialog_window;
                dialog_window = NULL;
                dialog_mode = false;
                break;
            case IN_JOB:
                dialog_window->update();
                break;
        }
    }
    if(view_file_mode) {
        view_file_window->update();
    }
}

void Action_Doc::removeViewFileWindow() {
    delete view_file_window;
    view_file_window = NULL;
    view_file_mode = false;
}

void Action_Doc::userAsksCopyTo(const std::string& from, const std::string& to){
    showDialogWindow(COPY_WINDOW, from, to);
}

void Action_Doc::userAsksMoveTo(const std::string& from, const std::string& to){
    showDialogWindow(MOVE_WINDOW, from, to);
}

void Action_Doc::userAsksMkDir(const std::string& where, const std::string& null_string){
    showDialogWindow(MKDIR_WINDOW, where, null_string);
}

void Action_Doc::userAsksDelete(const std::string& where, const std::string& null_string){
    showDialogWindow(DELETE_WINDOW, where, null_string);
}

void Action_Doc::userAsksRename(const std::string& old_name, const std::string& new_name){
    showDialogWindow(RENAME_WINDOW, old_name, new_name);
}

void Action_Doc::copy(const std::string& from, const std::string& to) {
    std::string dir_name;
    std::string tmp=to;
    if(fs::is_directory(from)) {
        int p = from.find_last_of('/');
        if(p != -1) {
            dir_name = from.substr(p + 1);
            mkdir(to, dir_name);
            tmp += '/'; tmp += dir_name;
        }
    }
    //сама функция копирования с обработкой исключений
    try {
        fs::copy(from, tmp, fs::copy_options::recursive | fs::copy_options::skip_existing);
    } catch(fs::filesystem_error& err) {
        std::cerr << "Could not copy: " << err.what() << std::endl;
    }
}

void Action_Doc::move(const std::string& from, const std::string& to) {
    copy(from, to);
    clear(from);
}

void Action_Doc::mkdir(const std::string& to, const std::string& name) {
    if(name.size()) {
        try {
            fs::create_directory(to + '/' + name);
        } catch(fs::filesystem_error& err) {
            std::cerr << "Could not create directory: " << err.what() << std::endl;
        }
    }
}

void Action_Doc::clear(const std::string& where) {
    try {
        if(!fs::remove_all(where)) {
            std::cerr << "Not deleted \"from\" directory\file: " << std::endl;
        }
    } catch(fs::filesystem_error& err) {
        std::cerr << "Could not delete \"from\" directory\file: " << err.what() << std::endl;
    }
}

void Action_Doc::rename(const std::string& old_path, const std::string& new_name) {
    auto const pos = old_path.find_last_of('/');
    std::string new_path = old_path.substr(0, pos + 1) + new_name;
    try {
        fs::rename(old_path, new_path);
    } catch(fs::filesystem_error& err) {
        std::cerr << "Could not rename directory\file: " << err.what() << std::endl;
    }
}

void Action_Doc::showDialogWindow(const dialog_window_type operation_type, const std::string& arg1, const std::string& arg2) {
    dialog_window = new Dialog_Window((WINDOW_WIDTH-600) / 2, (WINDOW_HEIGHT - 300) / 2, 600, 300, m_pRenderer, font, operation_type, arg1, arg2);
    dialog_mode = true;
}

void Action_Doc::showViewFileWindow(const std::string& file_name) {
    view_file_window = new View_File_Window(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, m_pRenderer, font, file_name);
    view_file_mode = true;
}

void Action_Doc::renderText(const std::string& str, const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a) {
    SDL_Texture* text;
    SDL_Surface* surf=TTF_RenderUTF8_Blended(font, str.c_str(), {0, 0, 0});

    text=SDL_CreateTextureFromSurface(m_pRenderer, surf);
    if(text!=0) {
        SDL_FreeSurface(surf);
    } else {
        std::cerr << "TTF text error\n";
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

Action_Doc::~Action_Doc() {}
