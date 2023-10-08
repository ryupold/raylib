/**********************************************************************************************
*
*   rcore - Common types and globals (all platforms)
*
*   LIMITATIONS:
*       - Limitation 01
*       - Limitation 02
*
*   POSSIBLE IMPROVEMENTS:
*       - Improvement 01
*       - Improvement 02
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2013-2023 Ramon Santamaria (@raysan5) and contributors
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef RCORE_H
#define RCORE_H

#include <stdlib.h>                 // Required for: srand(), rand(), atexit()
#include <stdio.h>                  // Required for: sprintf() [Used in OpenURL()]
#include <string.h>                 // Required for: strrchr(), strcmp(), strlen(), memset()
#include <time.h>                   // Required for: time() [Used in InitTimer()]
#include <math.h>                   // Required for: tan() [Used in BeginMode3D()], atan2f() [Used in LoadVrStereoConfig()]

#include "utils.h"                  // Required for: TRACELOG() macros

#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)
    #define GLFW_INCLUDE_NONE       // Disable the standard OpenGL header inclusion on GLFW3
                                    // NOTE: Already provided by rlgl implementation (on glad.h)
    #include "GLFW/glfw3.h"         // GLFW3 library: Windows, OpenGL context and Input management
                                    // NOTE: GLFW3 already includes gl.h (OpenGL) headers
#endif

#if defined(PLATFORM_ANDROID)
    #include <EGL/egl.h>            // Native platform windowing system interface
    //#include <GLES2/gl2.h>        // OpenGL ES 2.0 library (not required in this module, only in rlgl)
#endif

#if defined(PLATFORM_DRM)

    #include <fcntl.h>   // POSIX file control definitions - open(), creat(), fcntl()
    #include <unistd.h>  // POSIX standard function definitions - read(), close(), STDIN_FILENO
    #include <termios.h> // POSIX terminal control definitions - tcgetattr(), tcsetattr()
    #include <pthread.h> // POSIX threads management (inputs reading)
    #include <dirent.h>  // POSIX directory browsing

    #include <sys/ioctl.h>      // Required for: ioctl() - UNIX System call for device-specific input/output operations
    #include <linux/kd.h>       // Linux: KDSKBMODE, K_MEDIUMRAM constants definition
    #include <linux/input.h>    // Linux: Keycodes constants definition (KEY_A, ...)
    #include <linux/joystick.h> // Linux: Joystick support library

    #include <gbm.h>         // Generic Buffer Management (native platform for EGL on DRM)
    #include <xf86drm.h>     // Direct Rendering Manager user-level library interface
    #include <xf86drmMode.h> // Direct Rendering Manager mode setting (KMS) interface

    #include "EGL/egl.h"    // Native platform windowing system interface
    #include "EGL/eglext.h" // EGL extensions

    typedef struct
    {
        pthread_t threadId; // Event reading thread id
        int fd;             // File descriptor to the device it is assigned to
        int eventNum;       // Number of 'event<N>' device
        Rectangle absRange; // Range of values for absolute pointing devices (touchscreens)
        int touchSlot;      // Hold the touch slot number of the currently being sent multitouch block
        bool isMouse;       // True if device supports relative X Y movements
        bool isTouch;       // True if device supports absolute X Y movements and has BTN_TOUCH
        bool isMultitouch;  // True if device supports multiple absolute movevents and has BTN_TOUCH
        bool isKeyboard;    // True if device has letter keycodes
        bool isGamepad;     // True if device has gamepad buttons
    } InputEventWorker;

#endif

// TODO: PROVIDE A HEADER TO BE USED BY ALL THE rcore_* IMPLEMENTATIONS

#include "raylib.h"

#include "rlgl.h"

#define RAYMATH_IMPLEMENTATION
#include "raymath.h"

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#if defined(PLATFORM_DRM)
    #define USE_LAST_TOUCH_DEVICE       // When multiple touchscreens are connected, only use the one with the highest event<N> number

    #define DEFAULT_GAMEPAD_DEV    "/dev/input/js"  // Gamepad input (base dev for all gamepads: js0, js1, ...)
    #define DEFAULT_EVDEV_PATH       "/dev/input/"  // Path to the linux input events
#endif

#ifndef MAX_FILEPATH_CAPACITY
    #define MAX_FILEPATH_CAPACITY       8192        // Maximum capacity for filepath
#endif
#ifndef MAX_FILEPATH_LENGTH
    #define MAX_FILEPATH_LENGTH         4096        // Maximum length for filepaths (Linux PATH_MAX default value)
#endif

#ifndef MAX_KEYBOARD_KEYS
    #define MAX_KEYBOARD_KEYS            512        // Maximum number of keyboard keys supported
#endif
#ifndef MAX_MOUSE_BUTTONS
    #define MAX_MOUSE_BUTTONS              8        // Maximum number of mouse buttons supported
#endif
#ifndef MAX_GAMEPADS
    #define MAX_GAMEPADS                   4        // Maximum number of gamepads supported
#endif
#ifndef MAX_GAMEPAD_AXIS
    #define MAX_GAMEPAD_AXIS               8        // Maximum number of axis supported (per gamepad)
#endif
#ifndef MAX_GAMEPAD_BUTTONS
    #define MAX_GAMEPAD_BUTTONS           32        // Maximum number of buttons supported (per gamepad)
#endif
#ifndef MAX_TOUCH_POINTS
    #define MAX_TOUCH_POINTS               8        // Maximum number of touch points supported
#endif
#ifndef MAX_KEY_PRESSED_QUEUE
    #define MAX_KEY_PRESSED_QUEUE         16        // Maximum number of keys in the key input queue
#endif
#ifndef MAX_CHAR_PRESSED_QUEUE
    #define MAX_CHAR_PRESSED_QUEUE        16        // Maximum number of characters in the char input queue
#endif

#ifndef MAX_DECOMPRESSION_SIZE
    #define MAX_DECOMPRESSION_SIZE        64        // Maximum size allocated for decompression in MB
#endif

// Flags operation macros
#define FLAG_SET(n, f) ((n) |= (f))
#define FLAG_CLEAR(n, f) ((n) &= ~(f))
#define FLAG_TOGGLE(n, f) ((n) ^= (f))
#define FLAG_CHECK(n, f) ((n) & (f))

// TODO: HACK: Added flag if not provided by GLFW when using external library
// Latest GLFW release (GLFW 3.3.8) does not implement this flag, it was added for 3.4.0-dev
#if !defined(GLFW_MOUSE_PASSTHROUGH)
    #define GLFW_MOUSE_PASSTHROUGH      0x0002000D
#endif

#if (defined(__linux__) || defined(PLATFORM_WEB)) && (_POSIX_C_SOURCE < 199309L)
    #undef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 199309L // Required for: CLOCK_MONOTONIC if compiled with c99 without gnu ext.
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct { int x; int y; } Point;
typedef struct { unsigned int width; unsigned int height; } Size;

// Core global state context data
typedef struct CoreData {
    struct {
#if defined(PLATFORM_DESKTOP) || defined(PLATFORM_WEB)
        GLFWwindow *handle;                 // GLFW window handle (graphic device)
#endif
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_DRM)
#if defined(PLATFORM_DRM)
        int fd;                             // File descriptor for /dev/dri/...
        drmModeConnector *connector;        // Direct Rendering Manager (DRM) mode connector
        drmModeCrtc *crtc;                  // CRT Controller
        int modeIndex;                      // Index of the used mode of connector->modes
        struct gbm_device *gbmDevice;       // GBM device
        struct gbm_surface *gbmSurface;     // GBM surface
        struct gbm_bo *prevBO;              // Previous GBM buffer object (during frame swapping)
        uint32_t prevFB;                    // Previous GBM framebufer (during frame swapping)
#endif  // PLATFORM_DRM
        EGLDisplay device;                  // Native display device (physical screen connection)
        EGLSurface surface;                 // Surface to draw on, framebuffers (connected to context)
        EGLContext context;                 // Graphic context, mode in which drawing can be done
        EGLConfig config;                   // Graphic config
#endif
        const char *title;                  // Window text title const pointer
        unsigned int flags;                 // Configuration flags (bit based), keeps window state
        bool ready;                         // Check if window has been initialized successfully
        bool fullscreen;                    // Check if fullscreen mode is enabled
        bool shouldClose;                   // Check if window set for closing
        bool resizedLastFrame;              // Check if window has been resized last frame
        bool eventWaiting;                  // Wait for events before ending frame

        Point position;                     // Window position (required on fullscreen toggle)
        Point previousPosition;             // Window previous position (required on borderless windowed toggle)
        Size display;                       // Display width and height (monitor, device-screen, LCD, ...)
        Size screen;                        // Screen width and height (used render area)
        Size previousScreen;                // Screen previous width and height (required on borderless windowed toggle)
        Size currentFbo;                    // Current render width and height (depends on active fbo)
        Size render;                        // Framebuffer width and height (render area, including black bars if required)
        Point renderOffset;                 // Offset from render area (must be divided by 2)
        Size screenMin;                     // Screen minimum width and height (for resizable window)
        Size screenMax;                     // Screen maximum width and height (for resizable window)
        Size windowMin;                     // Window minimum width and height
        Size windowMax;                     // Window maximum width and height
        Matrix screenScale;                 // Matrix to scale screen (framebuffer rendering)

        char **dropFilepaths;               // Store dropped files paths pointers (provided by GLFW)
        unsigned int dropFileCount;         // Count dropped files strings

    } Window;
#if defined(PLATFORM_ANDROID)
    struct {
        bool appEnabled;                    // Flag to detect if app is active ** = true
        struct android_app *app;            // Android activity
        struct android_poll_source *source; // Android events polling source
        bool contextRebindRequired;         // Used to know context rebind required
    } Android;
#endif
    struct {
        const char *basePath;               // Base path for data storage
    } Storage;
    struct {
#if defined(PLATFORM_DRM)
        InputEventWorker eventWorker[10];   // List of worker threads for every monitored "/dev/input/event<N>"
#endif
        struct {
            int exitKey;                    // Default exit key
            char currentKeyState[MAX_KEYBOARD_KEYS];        // Registers current frame key state
            char previousKeyState[MAX_KEYBOARD_KEYS];       // Registers previous frame key state
            // NOTE: Since key press logic involves comparing prev vs cur key state, we need to handle key repeats specially
            char keyRepeatInFrame[MAX_KEYBOARD_KEYS];       // Registers key repeats for current frame.

            int keyPressedQueue[MAX_KEY_PRESSED_QUEUE];     // Input keys queue
            int keyPressedQueueCount;       // Input keys queue count

            int charPressedQueue[MAX_CHAR_PRESSED_QUEUE];   // Input characters queue (unicode)
            int charPressedQueueCount;      // Input characters queue count

#if defined(PLATFORM_DRM)
            int defaultMode;                // Default keyboard mode
#if defined(SUPPORT_SSH_KEYBOARD_RPI)
            bool evtMode;                   // Keyboard in event mode
#endif
            int defaultFileFlags;           // Default IO file flags
            struct termios defaultSettings; // Default keyboard settings
            int fd;                         // File descriptor for the evdev keyboard
#endif
        } Keyboard;
        struct {
            Vector2 offset;                 // Mouse offset
            Vector2 scale;                  // Mouse scaling
            Vector2 currentPosition;        // Mouse position on screen
            Vector2 previousPosition;       // Previous mouse position

            int cursor;                     // Tracks current mouse cursor
            bool cursorHidden;              // Track if cursor is hidden
            bool cursorOnScreen;            // Tracks if cursor is inside client area

            char currentButtonState[MAX_MOUSE_BUTTONS];     // Registers current mouse button state
            char previousButtonState[MAX_MOUSE_BUTTONS];    // Registers previous mouse button state
            Vector2 currentWheelMove;       // Registers current mouse wheel variation
            Vector2 previousWheelMove;      // Registers previous mouse wheel variation
#if defined(PLATFORM_DRM)
            Vector2 eventWheelMove;         // Registers the event mouse wheel variation
            // NOTE: currentButtonState[] can't be written directly due to multithreading, app could miss the update
            char currentButtonStateEvdev[MAX_MOUSE_BUTTONS]; // Holds the new mouse state for the next polling event to grab
#endif
        } Mouse;
        struct {
            int pointCount;                             // Number of touch points active
            int pointId[MAX_TOUCH_POINTS];              // Point identifiers
            Vector2 position[MAX_TOUCH_POINTS];         // Touch position on screen
            char currentTouchState[MAX_TOUCH_POINTS];   // Registers current touch state
            char previousTouchState[MAX_TOUCH_POINTS];  // Registers previous touch state
        } Touch;
        struct {
            int lastButtonPressed;          // Register last gamepad button pressed
            int axisCount;                  // Register number of available gamepad axis
            bool ready[MAX_GAMEPADS];       // Flag to know if gamepad is ready
            char name[MAX_GAMEPADS][64];    // Gamepad name holder
            char currentButtonState[MAX_GAMEPADS][MAX_GAMEPAD_BUTTONS];     // Current gamepad buttons state
            char previousButtonState[MAX_GAMEPADS][MAX_GAMEPAD_BUTTONS];    // Previous gamepad buttons state
            float axisState[MAX_GAMEPADS][MAX_GAMEPAD_AXIS];                // Gamepad axis state
#if defined(PLATFORM_DRM)
            pthread_t threadId;             // Gamepad reading thread id
            int streamId[MAX_GAMEPADS];     // Gamepad device file descriptor
#endif
        } Gamepad;
    } Input;
    struct {
        double current;                     // Current time measure
        double previous;                    // Previous time measure
        double update;                      // Time measure for frame update
        double draw;                        // Time measure for frame draw
        double frame;                       // Time measure for one frame
        double target;                      // Desired time for one frame, if 0 not applied
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_DRM)
        unsigned long long int base;        // Base time measure for hi-res timer
#endif
        unsigned int frameCounter;          // Frame counter
    } Time;
} CoreData;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
extern CoreData CORE;

#endif