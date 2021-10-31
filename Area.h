#ifndef AREA_H
#define AREA_H

#include <filesystem>
#include <ios>
#include <iostream>
#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <chrono>

#include "Constants.h"


enum file_type {
        file = 0,
        dir = 1
    };

struct file {
    enum file_type type;
    std::string path;
    std::string name;
    std::string size;
    std::string last_change_time;
};

struct file_button {
    struct file* file;
    int index_on_page;
    bool is_chosen;
    int page;       //отсчёт страниц начиная с 0
};

class Area {
    public:
        Area(const int xpos, const int ypos, const int width, const int height, SDL_Renderer* m_pRenderer, TTF_Font* font, bool is_main);
        virtual ~Area();
        const int getFileAmount() const { return file_amount; }
        const int getMaxElementsInAreaAmount() const { return max_elements_in_area; }
        const std::string& getRootPath() const { return root_path; }

        void update();                                                            //метод обновелния поверхности зоны

        void up();                                                                //методы управления курсором
        void down();

        void changeArea( Area* who_takes_control );                               //смена активной зоны

        bool isFocused() const { return chosen_element_index != -1; }                   //получение статуса активности зоны

        const int getFirstElementInPageIndex() const { return  first_element_in_page_index; } //получение индекса первого элемента,
                                                                                              //отображаемого на активной странице зоны

        void BridgeInChooseElement(const int index_of_chosen_element );           //вспомогательный метод по передаче активности зон:
                                                                                  //зона делает активным элемент с переданным в аргументе
                                                                                  //индексом или, если такого индекса нет (в директории меньше
                                                                                  //папок, чем в изначально активной зоне),
                                                                                  //то активным становится последний элем.

        void openNewDirectory(std::string new_path);                       //открытие новой папки в зоне

        void openChosenFile();                                                    //открытие файла или папки, выбранного в зоне

        void openPrevDir();                                                       //открытие в предыдущую отображаемой в зоне директории

        const std::string& getChosenFileName() const { return button_massive[chosen_element_index].file->path; }   //Возвращает путь с именем файла

        bool getFlagOpenFile() const { return flag_open_file; }
        void makeFalseFlagOpenFile() { flag_open_file = false; }
    private:
        std::uintmax_t getSize(std::string path);
        void renderText(const std::string& text, const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a);    //отрисовка чёрного
                                                                                                                 //текста на фоне заданного цвета
        const int getIndexByPage(const int page);
        void setPrevPage();
        void setNextPage();
        void setPageAmount();

        void initAll(const int xpos, const int ypos, const int width, const int height,
                               SDL_Renderer* m_pRenderer, TTF_Font* font, bool is_main, const std::string path);
        void clearAll();

        TTF_Font* font = NULL;
        SDL_Rect source;
        SDL_Rect dest;
        SDL_Rect area_rect;
        SDL_Renderer* m_pRenderer = NULL;
        std::string root_path;
        int file_amount = 0;
        int max_elements_in_area = 0;
        int chosen_element_index = -1;
        int first_element_in_page_index = 0;
        struct file* file_massive = NULL;

        struct file_button* button_massive = NULL;

        const std::string SizeToString(const std::uintmax_t size_in_bytes);

        const std::string& getDirectorySize(const int file_massive_index);
        const std::uintmax_t getInnerDirectorySize( const std::string& path);

        bool flag_open_file = false;
};

#endif // AREA_H
