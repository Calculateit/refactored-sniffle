#ifndef ACTION_DOC_H
#define ACTION_DOC_H

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Dialog_Window.h"
#include "View_File_Window.h"

#include "Constants.h"

class Action_Doc {
    public:
        Action_Doc(const int xpos, const int ypos, const int width, const int height, SDL_Renderer* m_pRenderer, TTF_Font* font);
        virtual ~Action_Doc();
        void update();
        void userAsksCopyTo(const std::string& from, const std::string& to);
        void userAsksMoveTo(const std::string& from, const std::string& to);

        void userAsksMkDir(const std::string& where, const std::string& null_string);
        void userAsksDelete(const std::string& where, const std::string& null_string);
        void userAsksRename(const std::string& old_name, const std::string& new_name);

        void showDialogWindow(const dialog_window_type operation_type, const std::string& arg1, const std::string& arg2);
        void showViewFileWindow(const std::string& file_name);
        bool getDialogMode() const { return dialog_mode; }

        bool getViewFileMode() const { return view_file_mode; }

        void removeViewFileWindow();

        Dialog_Window* dialog_window = NULL;
        View_File_Window* view_file_window = NULL;

    private:
        std::vector<std::string> actions_massive = { "1 View", "2 Copy", "3 Move", "4 Mkdir", "5 Delete", "6 Rename", "7 Quit" };
        void copy(const std::string& from, const std::string& to);
        void move(const std::string& from, const std::string& to);
        void mkdir(const std::string& where, const std::string& name);
        void clear(const std::string& where);
        void rename(const std::string& old_path, const std::string& new_name);


        SDL_Rect action_doc;
        SDL_Rect dest;
        TTF_Font* font = NULL;
        SDL_Rect source;
        SDL_Renderer* m_pRenderer = NULL;

        bool dialog_mode = false;   //false - диалог. окно не активно, true - активно
        bool view_file_mode = false;   //false - окно просмотра файла не активно, true - активно

        void renderText(const std::string& str, const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a);//отрисовка чёрного текста
                                                                                                            //на фоне заданного цвета
};

#endif // ACTION_DOC_H
