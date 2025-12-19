#define SDL_MAIN_HANDLED

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_surface.h>

const bool   LIMIT_FPS         = false;
const Uint64 TARGET_FPS        = 100;
const Uint64 TICKS_PER_SECOND  = 100;
const Uint64 MS_PER_TICK       = 1000 / TICKS_PER_SECOND;
const Uint64 STEP_SIZE         = 100;

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
    SDL_Renderer* mRenderer;
    SDL_Surface* mSurface;
    bool isRunning;
    bool isPaused;

    int width, height;

    SDL_FPoint previousPoint;
    SDL_FPoint point;

    SDL_FPoint ballPos;
    SDL_FPoint ballVel;
    float ballRadius;

    SDLApplication(const char* title, int width, int height);
    ~SDLApplication();

    void EventHandler(SDL_Event* event);
    void Update(Uint64 step);
    void Render();
    void Tick(Uint64 step);
    void ControlLoop();
};