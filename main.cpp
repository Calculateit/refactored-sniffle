#include <App.h>
#include "Constants.h"

int main(int argc, char** argv)
{
    int return_code = 0;
    try {
        App app("File Explorer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    }
    catch(std::exception& err) {
        std::cerr << "FATAL Error: " << err.what() << '\n';
        return_code = 1;
    }
    return return_code;
}
