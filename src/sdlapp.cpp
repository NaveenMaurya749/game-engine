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
    mWindow = SDL_CreateWindow(title, width, height, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_MOUSE_FOCUS);
    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
    }

    SDL_WarpMouseInWindow(mWindow, width / 2, height / 2);

    for (int i = 0; i < SDL_GetNumRenderDrivers(); i++){
        SDL_Log("Renderer %d: %s", i, SDL_GetRenderDriver(i));
    }

    mRenderer = SDL_CreateRenderer(mWindow, "vulkan");
    if (mRenderer == nullptr) {
        SDL_Log("Unable to create HW accelerated renderer");
    } else {
        SDL_Log("Initiated renderer: %s", SDL_GetRendererName(mRenderer));
    }

    // mSurface = SDL_LoadBMP("test.bmp");
    // if (!mSurface) {
    //     SDL_Log("Failed to load image: %s", SDL_GetError());
    // } else{
    //     SDL_Log("Image loaded successfully.");
    // }
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
            if (keystate[SDL_SCANCODE_F11]){
                if (SDL_GetWindowFlags(mWindow) & SDL_WINDOW_FULLSCREEN) {
                    if (SDL_SetWindowFullscreen(mWindow, false))
                        SDL_Log("Swichted out of Fullscreen mode");
                }
                else {
                    if (SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN))
                        SDL_Log("Switched to Fullscreen mode");
                }
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
    SDL_SetRenderDrawColor(mRenderer, 0x55, 0x33, 0xdd, 0xff);
    SDL_RenderClear(mRenderer);

    float fMouseXPos;
    float fMouseYPos;

    int windowX, windowY;

    SDL_GetMouseState(&fMouseXPos, &fMouseYPos);
    SDL_GetWindowSize(mWindow, &windowX, &windowY);

    SDL_SetRenderDrawColor(mRenderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderLine(mRenderer, 0, fMouseYPos, windowX, fMouseYPos);
    SDL_RenderLine(mRenderer, fMouseXPos, 0, fMouseXPos, windowY);

    SDL_RenderPresent(mRenderer);
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

        if (!FULL_FPS){
            Uint64 elapsed = SDL_GetTicks() - fpsCounter.lastUpdateTime;
            if(elapsed < TARGET_MSPF) {
                SDL_Delay(TARGET_MSPF - elapsed);
            }
        }

        fpsCounter.UpdateCounter();
        
        std::string title = "Title: " + std::to_string((Uint32)fpsCounter.fps) + " FPS";
        SDL_SetWindowTitle(mWindow, title.c_str());
    }
}