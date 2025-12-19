#include "sdlapp.h"
#include <string>
#include <random>
#include <cmath>

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
    mWindow = SDL_CreateWindow(title, width, height, SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_RESIZABLE);
    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return;
    }

    this->width = width;
    this->height = height;

    bool isPaused = false;

    SDL_WarpMouseInWindow(mWindow, width / 2, height / 2);

    mRenderer = SDL_CreateRenderer(mWindow, nullptr);
    if (mRenderer == nullptr) {
        SDL_Log("Unable to create HW accelerated renderer");
    } else {
        SDL_Log("Initiated renderer: %s", SDL_GetRendererName(mRenderer));
    }

    point = SDL_FPoint {.x = width/2.0f, .y =height/2.0f};
    previousPoint = point;

    ballPos = point;
    ballVel = SDL_FPoint {.x = 0.1f, .y = 0.15f};
    ballRadius = 40.0f;

    SDL_RenderClear(mRenderer);
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
            if (keystate[SDL_SCANCODE_P]){
                if (isPaused){
                    SDL_Log("Resumed");
                }
                else {
                    SDL_SetWindowTitle(mWindow, "(Paused)");
                    SDL_Log("Paused");
                }
                isPaused ^= true; // Toggle pause state
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
        case SDL_EVENT_WINDOW_RESIZED:
            SDL_Log("Window resized to %dx%d", event->window.data1, event->window.data2);
            SDL_GetWindowSize(mWindow, &width, &height);
            break;
    }
}


// Filled circle using horizontal scanlines (fast)
static void DrawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius) {
    for (int dy = -radius; dy <= radius; ++dy) {
        int dx = (int)std::floor(std::sqrt((double)radius * radius - dy * dy));
        SDL_RenderLine(renderer, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

// Circle outline using midpoint/Bresenham (fast)
static void DrawCircle(SDL_Renderer* renderer, int cx, int cy, int radius) {
    int x = radius;
    int y = 0;
    int err = 0;
    while (x >= y) {
        SDL_RenderPoint(renderer, cx + x, cy + y);
        SDL_RenderPoint(renderer, cx + y, cy + x);
        SDL_RenderPoint(renderer, cx - y, cy + x);
        SDL_RenderPoint(renderer, cx - x, cy + y);
        SDL_RenderPoint(renderer, cx - x, cy - y);
        SDL_RenderPoint(renderer, cx - y, cy - x);
        SDL_RenderPoint(renderer, cx + y, cy - x);
        SDL_RenderPoint(renderer, cx + x, cy - y);
        ++y;
        if (err <= 0) {
            err += 2*y + 1;
        }
        if (err > 0) {
            --x;
            err -= 2*x + 1;
        }
    }
}

void SDLApplication::Update(Uint64 step) {
    // float stddev = 0.2f;

    // float u1 = SDL_randf();
    // float u2 = SDL_randf();

    // // Box–Muller transform
    // float r = sqrtf(-2.0f * logf(u1));
    // float theta = 2.0f * (float)M_PI * u2;

    // // Gaussian components
    // float z0 = r * cosf(theta);
    // float z1 = r * sinf(theta);

    // // Scale and shift by mean and stddev
    // point.x += z0 * stddev * MS_PER_TICK;
    // point.y += z1 * stddev * MS_PER_TICK;
    
    ballPos.x += ballVel.x * step;
    ballPos.y += ballVel.y * step;

    int width, height;

    SDL_GetWindowSize(mWindow, &width, &height);

    // Bounce off walls
    if (ballPos.x - ballRadius < 0.0f) {
        ballPos.x = ballRadius;
        ballVel.x = -ballVel.x;
    } else if (ballPos.x + ballRadius > (float)width) {
        ballPos.x = (float)width - ballRadius;
        ballVel.x = -ballVel.x;
    }

    if (ballPos.y - ballRadius < 0.0f) {
        ballPos.y = ballRadius;
        ballVel.y = -ballVel.y;
    } else if (ballPos.y + ballRadius > (float)height) {
        ballPos.y = (float)height - ballRadius;
        ballVel.y = -ballVel.y;
    }
}


void SDLApplication::Render() {

    //SDL_SetRenderDrawColor(mRenderer, 0x55, 0x33, 0xdd, 0xff);
    // SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xff);
    //SDL_RenderClear(mRenderer);

    /*
    float fMouseXPos;
    float fMouseYPos;

    int windowX, windowY;

    SDL_GetMouseState(&fMouseXPos, &fMouseYPos);
    SDL_GetWindowSize(mWindow, &windowX, &windowY);

    SDL_SetRenderDrawColor(mRenderer, 0xff, 0xff, 0xff, 0x33);
    // SDL_RenderLine(mRenderer, 0, fMouseYPos, windowX, fMouseYPos);
    // SDL_RenderLine(mRenderer, fMouseXPos, 0, fMouseXPos, windowY);

    SDL_RenderLine(mRenderer, previousPoint.x, previousPoint.y, point.x, point.y);

    previousPoint = point;

    */

    SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0x00);

    SDL_RenderClear(mRenderer);

    //Draw ball
    SDL_SetRenderDrawColor(mRenderer, 0x55, 0x33, 0xdd, 0xff);

    for (int dy = -ballRadius; dy <= ballRadius; ++dy) {
        int dx = (int)std::floor(std::sqrt((double)ballRadius * ballRadius - dy * dy));
        SDL_RenderLine(mRenderer, ballPos.x - dx, ballPos.y + dy, ballPos.x + dx, ballPos.y + dy); // SDL3 name
    }

    SDL_RenderPresent(mRenderer);
}

void SDLApplication::Tick(Uint64 step) {
    SDL_Event event;
    
    while(SDL_PollEvent(&event)){
        EventHandler(&event);
    }
    // Game logic and rendering would go here
    if (!isPaused)
    {
        Update(step);
    }
    // Render();
}

void SDLApplication::ControlLoop() {
    Uint64 TARGET_MSPF = 1000 / TARGET_FPS;
    FPSCounter fpsCounter;

    Uint64 deltaTime;
    Uint64 t_lastUpdate = SDL_GetTicks();
    Uint64 t_accumulator = 0;

    while(isRunning){

        if (isPaused){
            Tick(0);
            SDL_SetWindowTitle(mWindow, "(Paused)");
            SDL_Delay(100); // Sleep to reduce CPU usage while paused
            continue;
        }

        deltaTime = SDL_GetTicks() - t_lastUpdate;
        t_lastUpdate += deltaTime;
        t_accumulator += deltaTime;

        while (t_accumulator > MS_PER_TICK) {
            Tick(STEP_SIZE);
            t_accumulator -= MS_PER_TICK;
        }
        
        Render();

        if (LIMIT_FPS){
            Uint64 elapsed = SDL_GetTicks() - fpsCounter.lastUpdateTime;
            if(elapsed < TARGET_MSPF) {
                SDL_Delay(TARGET_MSPF - elapsed);
            }
        }

        fpsCounter.UpdateCounter();
        
        std::string title = "Title: " + std::to_string((Uint32)fpsCounter.fps) + " FPS";
        // std::string title = std::to_string(point.x) + ", " + std::to_string(point.y);
        SDL_SetWindowTitle(mWindow, title.c_str());
    }
}