#include "App.h"

App::App(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    init(title, xpos, ypos, width, height, fullscreen);

}

bool App::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags=0;

    if(fullscreen) {
        flags=SDL_WINDOW_FULLSCREEN;
    }

    if(SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        m_pWindow=SDL_CreateWindow(title, xpos, ypos, width, height, flags);
        if(m_pWindow!=0) {
            m_pRenderer=SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(!m_pRenderer) {
                std::cerr << "Renderer init fail\n";
                return false;
            }
        }
        else {
            std::cerr << "Window init fail\n";
            return false;
        }
    }
    else {
        std::cerr << "SDL init fail\n";
        return false;
    }

    if(TTF_Init()==0) {
        font=TTF_OpenFont("RobotoMono-Medium.ttf", 18);
        if(!font) {
            std::cerr << "TTF font error\n";
            return false;
        }
    }
    else {
        std::cerr << "TTF init error\n";
        return false;
    }
    area[0] = new Area(0, UP_OFFSET, width/2, height, m_pRenderer, font, true);
    area[1] = new Area(width/2, UP_OFFSET, width/2, height, m_pRenderer, font, false);
    action_doc = new Action_Doc(0, WINDOW_HEIGHT - DOWN_OFFSET, WINDOW_WIDTH, DOWN_OFFSET, m_pRenderer, font);

    m_bRunning=true;
    render();

    return true;
}

void App::render() {
    while(m_bRunning) {
        handleEvents();
        SDL_SetRenderDrawColor(m_pRenderer, BACKGROUND_COLOR);
        SDL_RenderClear(m_pRenderer);

        for(int i=0; i<2; ++i){
            area[i]->update();
            if(area[i]->isFocused()) {
                if(area[i]->getFlagOpenFile()){
                    action_doc->showViewFileWindow(area[i]->getChosenFileName());
                    area[i]->makeFalseFlagOpenFile();
                }
            }
        }

        action_doc->update();
        SDL_RenderPresent(m_pRenderer);
    }
}

void App::clean() {
    SDL_DestroyWindow(m_pWindow);
    SDL_DestroyRenderer(m_pRenderer);
    SDL_Quit();
}

void App::handleEvents() {
    SDL_Event event;
    if(SDL_WaitEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                m_bRunning=false;
                break;
            case SDL_TEXTINPUT:
                if(action_doc->getDialogMode()) {
                    action_doc->dialog_window->userAddString(event.text.text);
                }
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_UP:
                        if(!action_doc->getDialogMode() && !action_doc->getViewFileMode()) {
                            if(area[0]->isFocused()) {
                                area[0]->up();
                            } else {
                                area[1]->up();
                            }
                        } else
                        if(action_doc->getViewFileMode() && !action_doc->getDialogMode()) {
                            action_doc->view_file_window->setPrevPage();
                        }
                        break;
                    case SDLK_BACKSPACE:
                        if(action_doc->getDialogMode()) {
                            action_doc->dialog_window->userPressedBackSpace();
                        }
                        break;
                    case SDLK_DOWN:
                        if(!action_doc->getDialogMode() && !action_doc->getViewFileMode()) {
                            if(area[0]->isFocused()) {
                                area[0]->down();
                            } else {
                                area[1]->down();
                            }
                        } else
                        if(action_doc->getViewFileMode() && !action_doc->getDialogMode()) {
                            action_doc->view_file_window->setNextPage();
                        }
                        break;
                    case SDLK_LEFT:
                        if(!action_doc->getDialogMode() && !action_doc->getViewFileMode()) {
                            if(area[0]->isFocused()) {
                                area[0]->changeArea(area[1]);
                            } else {
                                area[1]->changeArea(area[0]);
                            }
                        } else
                        if(!action_doc->getViewFileMode()) {
                            action_doc->dialog_window->dialogCursorLeft();
                        }
                        break;
                    case SDLK_RIGHT:
                        if(!action_doc->getDialogMode() && !action_doc->getViewFileMode()) {
                            if(area[0]->isFocused()) {
                                area[0]->changeArea(area[1]);
                            } else {
                                area[1]->changeArea(area[0]);
                            }
                        } else
                        if(!action_doc->getViewFileMode()) {
                            action_doc->dialog_window->dialogCursorRight();
                        }
                        break;
                    case SDLK_RETURN:
                        if(!action_doc->getDialogMode() && !action_doc->getViewFileMode()) {
                            if(area[0]->isFocused()) {
                                area[0]->openChosenFile();
                            } else {
                                area[1]->openChosenFile();
                            }
                        } else
                        if(!action_doc->getViewFileMode()) {
                            action_doc->dialog_window->dialogCursorChoose();
                        }
                        break;
                    case SDLK_p:
                        if(!action_doc->getDialogMode() && !action_doc->getViewFileMode()) {
                            if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) {
                                if(area[0]->isFocused()) {
                                    area[0]->openPrevDir();
                                } else {
                                    area[1]->openPrevDir();
                                }
                            }
                        }
                        break;
                    case SDLK_1://просмотр текстовых файлов
                        if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) {
                            if(!action_doc->getDialogMode() && !action_doc->getViewFileMode()) {
                                if(area[0]->isFocused()) {
                                    action_doc->showViewFileWindow(area[0]->getChosenFileName());
                                } else {
                                    action_doc->showViewFileWindow(area[1]->getChosenFileName());
                                }
                            }
                        }
                        break;
                    case SDLK_2://копирование
                        if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) {
                            if(!action_doc->getDialogMode() && !action_doc->getViewFileMode()) {
                                if(area[0]->isFocused()) {
                                    action_doc->showDialogWindow(COPY_WINDOW, area[0]->getChosenFileName(), area[1]->getRootPath());
                                } else {
                                    action_doc->showDialogWindow(COPY_WINDOW, area[1]->getChosenFileName(), area[0]->getRootPath());
                                }
                            }
                        }
                        break;
                    case SDLK_3://перемещение
                        if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) {
                            if(!action_doc->getDialogMode() && !action_doc->getViewFileMode()) {
                                if(area[0]->isFocused()) {
                                    action_doc->showDialogWindow(MOVE_WINDOW, area[0]->getChosenFileName(), area[1]->getRootPath());
                                } else {
                                    action_doc->showDialogWindow(MOVE_WINDOW, area[1]->getChosenFileName(), area[0]->getRootPath());
                                }
                            }
                        }
                        break;
                    case SDLK_4://создание папки
                        if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) {
                            if(!action_doc->getDialogMode() && !action_doc->getViewFileMode()) {
                                if(area[0]->isFocused()) {
                                    action_doc->showDialogWindow(MKDIR_WINDOW, area[0]->getRootPath(), std::string());
                                } else {
                                    action_doc->showDialogWindow(MKDIR_WINDOW, area[1]->getRootPath(), std::string());
                                }
                            }
                        }
                        break;
                    case SDLK_5://удаление файла
                        if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) {
                            if(!action_doc->getDialogMode() && !action_doc->getViewFileMode()) {
                                if(area[0]->isFocused()) {
                                    action_doc->showDialogWindow(DELETE_WINDOW, area[0]->getChosenFileName(), std::string());
                                } else {
                                    action_doc->showDialogWindow(DELETE_WINDOW, area[1]->getChosenFileName(), std::string());
                                }
                            }
                        }
                        break;
                    case SDLK_6://переименование файла
                        if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) {
                            if(!action_doc->getDialogMode() && !action_doc->getViewFileMode()) {
                                if(area[0]->isFocused()) {
                                    action_doc->showDialogWindow(RENAME_WINDOW, area[0]->getChosenFileName(), std::string());
                                } else {
                                    action_doc->showDialogWindow(RENAME_WINDOW, area[1]->getChosenFileName(), std::string());
                                }
                            }
                        }
                        break;
                    case SDLK_7://выход из программы
                        if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) {
                            if(!action_doc->getDialogMode() && !action_doc->getViewFileMode()) {
                                m_bRunning=false;
                            } else
                            if(action_doc->getViewFileMode() && !action_doc->getDialogMode()) {
                                action_doc->removeViewFileWindow();
                            }
                        }
                        break;
                }
            break;
        }
    }
}

App::~App() {
    clean();
    for(int i=0; i<2; ++i)
        delete area[i];
    delete action_doc;
    TTF_CloseFont(font);
    TTF_Quit();
}
