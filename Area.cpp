#include "Area.h"

namespace fs = std::filesystem;

Area::Area(const int xpos, const int ypos, const int width, const int height, SDL_Renderer* m_pRenderer, TTF_Font* font, bool is_main) {
    initAll(xpos, ypos, width, height, m_pRenderer, font, is_main, fs::current_path().root_path());
}

Area::~Area() {
    clearAll();
}

void Area::initAll(const int xpos, const int ypos, const int width, const int height,
                               SDL_Renderer* m_pRenderer, TTF_Font* font, bool is_main, const std::string path) {
    file_amount = 0;
    max_elements_in_area = 0;
    chosen_element_index = -1;
    first_element_in_page_index = 0;

    this->m_pRenderer = m_pRenderer;
    area_rect.h = height;
    area_rect.w = width;
    area_rect.x = xpos;
    area_rect.y = ypos;
    this->root_path = path;   //Считали путь до корня и записали в root_path
    try {
        for(auto& p: fs::directory_iterator(path)) {
            ++file_amount;
        }
        file_massive = new struct file[file_amount];        //Зарезервировали память под массив информации о файлах

        button_massive = new struct file_button[file_amount];//Зарезервировали память под массив информации о кнопках

        this->font = font;
        this->max_elements_in_area = (area_rect.h - DOWN_OFFSET - UP_OFFSET) / CURSOR_HEIGHT;//макс. кол-во элементов в зоне (на одной странице)

        unsigned i = 0;
        std::stringstream fin_date;

        for(auto& p: fs::directory_iterator(path)) {
            file_massive[i].name = p.path().filename();     //Заполняем поле имени файла
            file_massive[i].path = p.path();                //Заполняем поле пути до файла
            if(!is_directory(p)) {
                file_massive[i].type = file;                //Заполняем поле типа файла
                file_massive[i].size = SizeToString(getSize(file_massive[i].path));
            } else {
                file_massive[i].type = dir;
                file_massive[i].size = "<Dir>";
            }

            using namespace std::chrono;
            fs::file_time_type ftime = fs::last_write_time(p);
            time_t cftime = system_clock::to_time_t(
                            time_point_cast<system_clock::duration>
                                        (ftime - std::filesystem::file_time_type::clock::now() + system_clock::now()));
            std::tm *time = std::gmtime(&cftime);
            fin_date << std::put_time(time, "%d %b %Y");
            file_massive[i].last_change_time = fin_date.str();
            fin_date.str(std::string());

            button_massive[i].file = &file_massive[i];
            if(i == 0 && is_main) {
                button_massive[i].is_chosen = true;
                chosen_element_index = i;
            } else {
                button_massive[i].is_chosen = false;
            }
            button_massive[i].page = i / (max_elements_in_area);
            button_massive[i].index_on_page = i % (max_elements_in_area);

            ++i;
        }
    } catch (fs::filesystem_error& err) {
        std::cerr << err.what() << '\n';
        initAll(area_rect.x, area_rect.y, area_rect.w, area_rect.h, m_pRenderer, font, true, fs::current_path().root_path());
    }

    source.x=0;
    source.y=0;
}

std::uintmax_t Area::getSize(std::string path) {
    std::uintmax_t size;
    try {
        size = fs::file_size(path);
    }
    catch(fs::filesystem_error& err) {
        std::cerr << err.what() << '\n';
        size = 0;
    }
    return size;
}

void Area::clearAll() {
    delete[] file_massive;
    delete[] button_massive;
}

void Area::up() {
    //проверка на то, является ли выделеныый элемент первым на странице(зоне) и является ли первым вообще в папке
    if(button_massive[chosen_element_index].index_on_page > 0) {
        button_massive[chosen_element_index].is_chosen = false;
        button_massive[--chosen_element_index].is_chosen = true;
    } else
    if(chosen_element_index > 0) {//проверка на то, является ли выделенный элемент первым в папке
        first_element_in_page_index = chosen_element_index - max_elements_in_area;
        button_massive[chosen_element_index].is_chosen = false;
        button_massive[--chosen_element_index].is_chosen = true;
    }
}

void Area::down() {
    //проверка на то, является ли выделеныый элемент последним на странице(зоне) и является ли последним вообще в папке
    if(button_massive[chosen_element_index].index_on_page < max_elements_in_area - 1 && chosen_element_index < file_amount - 1) {
        button_massive[chosen_element_index].is_chosen = false;
        button_massive[++chosen_element_index].is_chosen = true;
    } else
    if(chosen_element_index < file_amount - 1) {//проверка на то, является ли выделенный элемент последним в папке
        first_element_in_page_index = chosen_element_index + 1;
        button_massive[chosen_element_index].is_chosen = false;
        button_massive[++chosen_element_index].is_chosen = true;
    }
}

void Area::BridgeInChooseElement(int index_of_chosen_element) {
    if(index_of_chosen_element < file_amount - 1) {//если индекс выделяемого меньше индекса поседнего элемента
        button_massive[index_of_chosen_element].is_chosen = true;//тогда выделенным становится объект с этим индексом
        chosen_element_index = index_of_chosen_element;
    } else {
        button_massive[file_amount - 1].is_chosen = true;//иначе выделенным становится последний элемент
        chosen_element_index = file_amount - 1;
    }
}

void Area::changeArea( Area* who_takes_control ) {
    button_massive[chosen_element_index].is_chosen = false;//делаем неактивной единственную активную кнопку
    who_takes_control->BridgeInChooseElement(who_takes_control->getFirstElementInPageIndex() + button_massive[chosen_element_index].index_on_page);//передаём активность в другую зону
    chosen_element_index = -1;//признак неактивности зоны
}

const std::string Area::SizeToString(const std::uintmax_t size_in_bytes) {//перевод размера в байтах в нормированную по длине строку с соответствующей размерностью объёма занятой памяти
    std::stringstream fin_size;

    double size = static_cast<double>(size_in_bytes);   //Шаблон: XXX.XX Kb
    std::vector<std::string> byte_multiplier{"B", "Kb", "Mb", "Gb", "Tb", "Pb"};

    int i = 0;
    for(; size>=1000 && i < byte_multiplier.size(); size /= 1024, ++i);

    fin_size.precision(2);
    fin_size.width(5);
    fin_size<< std::fixed << size << byte_multiplier[i];
    return fin_size.str();
}

void Area::update() {
    //добавление строки с названием открытой директории
    dest.x = area_rect.x + FOLDER_LABEL_X;
    dest.y = FOLDER_LABEL_Y;
    renderText(root_path, FOLDER_LABEL_COLOR);

    //добавление кнопки возврата в пред. директорию
    dest.x = area_rect.x + PREV_DIR_BUTTON_X;
    dest.y = PREV_DIR_BUTTON_Y;
    renderText("/.. (Ctrl + P чтобы вернуться назад)", PREV_DIR_BUTTON_COLOR);

    //добавление кнопок файлов и папок директории
    std::stringstream name, size, mod_time;
    int name_length = int(0.5 * float(area_rect.w) / SYMBOL_PER_PIX_HORIZ) - 1;

    dest.x = area_rect.x;
    dest.y = area_rect.y;
    for(int i = first_element_in_page_index; i < file_amount && dest.y < (area_rect.h- 2 * DOWN_OFFSET); i++) {

        name.width(name_length);   //Единицу в каждом случае вычитаем из-за |, не входящего в width
        size.width(int(0.2 * float(area_rect.w) / SYMBOL_PER_PIX_HORIZ) - 1);
        mod_time.width(int(0.3 * float(area_rect.w) / SYMBOL_PER_PIX_HORIZ) - 1);
        if(static_cast<int>(file_massive[i].name.size()) > name_length) {       //обработка случая, когда name больше выделенного для него места
            name << std::left << file_massive[i].name.substr(0, name_length-3) + "..." << "|";
        } else {
            name << std::left << file_massive[i].name << "|";
        }
        size << std::left << file_massive[i].size << "|";
        mod_time << std::left << file_massive[i].last_change_time << "|";
        std::string str = name.str()+size.str()+mod_time.str();

        if(button_massive[i].is_chosen) {
            renderText(str, CURSOR_COLOR);
        } else {
            renderText(str, BACKGROUND_COLOR);
        }
        dest.y += CURSOR_HEIGHT;

        name.str(std::string());
        size.str(std::string());
        mod_time.str(std::string());
    }
}

void Area::renderText(const std::string& str, const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a) {
    SDL_Texture* text;
    SDL_Surface* surf=TTF_RenderUTF8_Blended(font, str.c_str(), {0, 0, 0});

    text=SDL_CreateTextureFromSurface(m_pRenderer, surf);
    if(text!=0) {
        SDL_FreeSurface(surf);
    } else {
        std::cerr << "TTF1 text error\n";
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

void Area::openNewDirectory(std::string new_path) {//открытие новой папки в зоне
    clearAll();
    initAll(area_rect.x, area_rect.y, area_rect.w, area_rect.h, m_pRenderer, font, true, new_path);
}

void Area::openChosenFile() {                       //открытие файла или папки, выбранного в зоне
    if(button_massive[chosen_element_index].file->type == file) {
        flag_open_file = true;
    } else
    if(button_massive[chosen_element_index].file->type == dir) {
        openNewDirectory(button_massive[chosen_element_index].file->path);
    }
}

void Area::openPrevDir() {

                                                        //открытие в предыдущую отображаемой в зоне директории
    if(root_path != fs::current_path().root_path()) {   //если директория не корневая(/ или <лат.буква>:/)
        std::string new_root_path;
        int slash_counter = 0;
        for(int i = 0; i < static_cast<int>(root_path.size()); i++) {
            if(root_path[i] == '/')
                slash_counter++;
        }
        if(slash_counter > 1) {                         //если несколько слэшей, значит, до корневой директории как минимум есть две папки
            auto const pos=root_path.find_last_of('/');
            new_root_path=root_path.substr(0, pos);
        } else {                                        //иначе переход осуществляем в корневую директорию
            new_root_path = "/";
        }
        openNewDirectory(new_root_path);
    }
}
