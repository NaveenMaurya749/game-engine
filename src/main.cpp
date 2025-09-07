#include "sdlapp.h"

int main(int argc, char* argv[])
{
    SDLApplication app("Title", 800, 600);
    app.ControlLoop();
    return 0;
}