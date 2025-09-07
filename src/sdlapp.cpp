#include "sdlapp.h"
#include <string>

FPSCounter::FPSCounter()
:   frameCount(0),
    startTime(SDL_GetTicks()),
    lastUpdateTime(0),
    fps(0.0f)
{}

FPSCounter::~FPSCounter() {}

void FPSCounter::UpdateCounter() {
    frameCount++;
    Uint64 currentTime = SDL_GetTicks();
    lastUpdateTime = currentTime;
    if (currentTime - startTime >= 1000) {
        fps = frameCount;
        frameCount = 0;
        startTime = currentTime;
    }
}

SDLApplication::SDLApplication(const char* title, int width, int height) {
    this->mWindow = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);
    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
    }

    SDL_WarpMouseInWindow(mWindow, width / 2, height / 2);

    mSurface = SDL_LoadBMP("game-engine\\src\\test.bmp");
    if (!mSurface) {
        SDL_Log("Failed to load image: %s", SDL_GetError());
        return;
    } else{
        SDL_Log("Image loaded successfully.");
    }
    
}

SDLApplication::~SDLApplication() {
    SDL_Log("Quitting...");
    SDL_Quit();
}

void SDLApplication::EventHandler(SDL_Event* event) {
    const bool* keystate = SDL_GetKeyboardState(nullptr);
    switch(event->type){    
        case SDL_EVENT_QUIT:
            isRunning = false;
            break;
        case SDL_EVENT_KEY_DOWN:
            if (keystate[SDL_SCANCODE_ESCAPE]){
                isRunning = false;
            }
            if (keystate[SDL_SCANCODE_W]){
                SDL_Log("W is pressed");
            }
            if (keystate[SDL_SCANCODE_A]){
                SDL_Log("A is pressed");
            }
            if (keystate[SDL_SCANCODE_S]){
                SDL_Log("S is pressed");
            }
            if (keystate[SDL_SCANCODE_D]){
                SDL_Log("D is pressed");
            }
            SDL_Log("Key Pressed!: %d", event->key.key);
            break;
        case SDL_EVENT_MOUSE_MOTION:
            //SDL_Log("Mouse Motion: %f, %f", event->motion.xrel, event->motion.yrel);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            switch(event->button.button){
                case SDL_BUTTON_LEFT:
                    SDL_Log("Left Button Clicked");
                    break;
                case SDL_BUTTON_MIDDLE:
                    SDL_Log("Middle Button Clicked");
                    break;
                case SDL_BUTTON_RIGHT:
                    SDL_Log("Right Button Clicked");
                    break;
            }
            break;
    }
}

void SDLApplication::Update() {

}

void SDLApplication::Render() {
    SDL_Surface* windowSurface = SDL_GetWindowSurface(mWindow);
    if (windowSurface != nullptr) {
        SDL_BlitSurface(mSurface, nullptr, windowSurface, nullptr);
        SDL_UpdateWindowSurface(mWindow);
    }
}

void SDLApplication::Tick() {
    SDL_Event event;
    
    while(SDL_PollEvent(&event)){
        EventHandler(&event);
    }
    // Game logic and rendering would go here
    Update();
    Render();
}

void SDLApplication::ControlLoop() {
    Uint64 TARGET_MSPF = 1000 / TARGET_FPS;
    FPSCounter fpsCounter;
    while(isRunning){
        Tick();

        Uint64 elapsed = SDL_GetTicks() - fpsCounter.lastUpdateTime;
        if(elapsed < TARGET_MSPF) {
            SDL_Delay(TARGET_MSPF - elapsed);
        }

        fpsCounter.UpdateCounter();
        
        std::string title = "Title: " + std::to_string((Uint32)fpsCounter.fps) + " FPS";
        SDL_SetWindowTitle(mWindow, title.c_str());
    }
}