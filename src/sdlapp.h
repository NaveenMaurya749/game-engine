#define SDL_MAIN_HANDLED

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_surface.h>

const Uint64 TARGET_FPS = 60;
// const Uint64 MATURATION_TIME = 1000;

struct FPSCounter
{
    Uint16 frameCount;
    Uint64 startTime;
    Uint64 lastUpdateTime;
    float fps;

    FPSCounter();
    ~FPSCounter();

    void UpdateCounter();
};

struct SDLApplication
{
    SDL_Window* mWindow;
    SDL_Surface* mSurface;
    bool isRunning;

    SDLApplication(const char* title, int width, int height);
    ~SDLApplication();

    void EventHandler(SDL_Event* event);
    void Update();
    void Render();
    void Tick();
    void ControlLoop();
};