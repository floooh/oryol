# Input Module

**Updated 23-Mar-2017 (Gamepad Input)**

The Oryol Input module provides unified access to input devices 
across platforms. The available types of input depends on the
platform, currently it looks like this:

Platform        | Keyboard | Mouse | Gamepad | Touch | Sensors
----------------|----------|-------|---------|-------|---------
OSX             | YES      | YES   | YES*    | -     | -
Linux           | YES      | YES   | YES     | -     | -
Windows         | YES      | YES   | YES*    | -     | -
iOS             | -        | -     | -       | YES   | YES
Android         | -        | -     | -       | YES   | YES
HTML5 (desktop) | YES      | YES   | YES     | -     | - 
HTML5 (mobile)  | -        | -     | -       | YES   | YES
RaspberryPi     | YES      | YES   | -       | -     | -

> NOTE: gamepad support on OSX and Windows is currently only
implemented via GLFW, it is not available when using the
non-GL rendering backends
 
### Sample Code

Most of the basic Input module functionality is demonstrated
in the [TestInput sample](http://floooh.github.io/oryol/asmjs/TestInput.html).

### Initialization and Shutdown

This works exactly like in other Oryol modules: 
- include the module header **Input/Input.h**
- call the **Input::Setup()** method in your app's OnInit() method
- and in OnCleanup() call **Input::Discard** to shut down the module

```cpp
#include "Input/Input.h"
...
AppState::Code MyApp::OnInit() {
    ...
    Input::Setup();
    ...
}
...
AppState::Code MyApp::OnCleanup() {
    ...
    Input::Discard();
    ...
}
```

It is possible to tweak the Input module's behaviour by providing
an InputSetup object to the Input::Setup method. Right now 
this only allows to disable some features on mobile platforms (which
might save some CPU cycles):

```cpp
...
    // we don't need some touch gesture recognizers
    InputSetup inputSetup;
    inputSetup.PinchEnabled = false;
    inputSetup.PanEnabled = false;
    inputSetup.AccelerometerEnabled = false;
    Input::Setup(inputSetup);
...
```

### Polling vs Input Events

The Input module supports two ways to get input: **polling** and 
**event callbacks**.

Polling is by far the more convenient way, you just call static Input module
methods anywhere in the game loop and immediately get a result back (for instance
whether a key has been pressed down or released during the last frame).

In some situations polling might not be sufficient, in this
case the Oryol input module allows to register an event callback
which will be called whenever an input event happens.

**Use polling if:**

- you want to check the state or state changes of specific keys 
or buttons
- you only want to check a small number of keys or buttons (e.g.
not the entire keyboard)
- you're only interested if a specific input action happened at least 
once during the last frame, but you're not interested how often it happened
- you just want to get the current mouse position or movement information

**Use an input event callback if:**

- you need to know about all key or button presses but don't know upfront
what keys or buttons you're looking for
- you need to keep track of the exact number of input actions happening
during a frame, even if the same action happens multiple times in the
same frame
- your application flow is better suited to handling events instead
of polling each frame

Event handling is described in detail at the bottom of this file, let's
first have a look at polling:

### Keyboard Input

To check whether the current platform provides keyboard input, 
call **Input::KeyboardAttached()**:

```cpp
    if (Input::KeyboardAttached()) {
        // handle keyboard input
        ...
    }
```

This method always returns the same hardcoded value, depending on the host
platform (e.g. desktop platforms will always return true, and mobile platforms
will always return false). Most notably, even if the physical keyboard is
detached, Input::KeyboardAttached() will still return true.

Nothing bad will happen if you call the keyboard functions even if 
Input::KeyboardAttached() returns false.

To check the current state of specific keys:

```cpp
    // check if a key is currently pressed (held down) or was pressed during the last frame:
    if (Input::KeyPressed(Key::W)) {
        ...
    }
    
    // check if a key was pressed down at least once during the last frame:
    if (Input::KeyDown(Key::A)) {
        ...
    }

    // check if a key was released at least once during the last frame:
    if (Input::KeyUp(Key::S)) {
        ...
    }

    // check if a key-repeat happened at least once during the last frame:
    if (Input::KeyRepeat(Key::D)) {
        ...
    }
```

> NOTE: the key-codes used here (Key::X), are virtual keycodes
> of physical keys, similar to the VK\_x codes on Windows,
> they don't necessarily represent what's actually printed on the 
> key cap (especially on international keyboards)

A list of all key codes can be found in the **Key** enum class in
the [Input/InputTypes.h header](https://github.com/floooh/oryol/blob/master/code/Modules/Input/InputTypes.h).

### Character Input 

The key-code functions described above are fine for most games that
map actions to physical keys, but they are not useful to get converted
character input instead of raw key codes. To capture entered text via 
polling, call the **Input::Text()** method, this will return a pointer 
to an internal zero-terminated wchar\_t string with the text input 
that happened during the last frame:

```cpp
    ...
    // use a string builder to capture text across several frame
    StringBuilder utf8Text;
    ...
    // poll text input, convert to UTF-8 and store in string builder
    const wchar_t* str = Input::Text();
    if (str[0]) {
        // at least one character was entered
        utf8Text.Append(StringConverter::WideToUTF8(str));
    }
```

The returned pointer points to a string with zero, one or sometimes multiple
wchar\_t characters. The character buffer will be reset each frame, so 
in order to catch all characters, you need to call the Input::Text() function
each frame.

Because of the restriction that text input must be polled each frame it
often makes sense to use an event callback which will be called
once for every entered character. See the end of this file for details
on input event handling.

### Mouse Input

Mouse input works very similar to keyboard input. To find out whether
the host platform provides mouse input, call **Input::MouseAttached()**:

```cpp
    if (Input::MouseAttached()) {
        // handle mouse input
        ...
    }
```

To check the state of the (up to 3) mouse buttons, use the following 
functions:

```cpp
    // check if left mouse button is current held down, or was held down during the last frame:
    if (Input::MouseButtonPressed(MouseButton::Left)) {
        ...
    }

    // check if middle mouse button was pressed down at least once during the last frame:
    if (Input::MouseButtonDown(MouseButton::Middle)) {
        ...
    }

    // check if the right mouse button was released at least once during the last frame:
    if (Input::MouseButtonUp(MouseButton::Right)) {
        ...
    }
``` 

To get the current **mouse position** and the last frame's **mouse movement**
in pixels:

```cpp
    const glm::vec2& mousePos = Input::MousePosition();
    const glm::vec2& mouseMov = Input::MouseMovement();
```

To get **mouse-wheel** information:

```cpp
    const glm::vec2& mouseScroll = Input::MouseScroll();
```

This returns a 2D vector because on some platforms there is both
vertical and horizontal wheel information (mostly for vertical/
hortizontal scrolling on notebook touchpads).

The traditional mouse wheel info is stored in the Y component of the vector.
If horizontal scrolling information is available, it will be in the
X component.

A few **important notes** about mouse input behaviour and differences
between platforms:

- By default, mouse input stops working when the mouse leaves
the window or hits the screen border, this is similar to how
a 'point-n-click' mouse in an UI system works. If you need 
a 'first-person-shooter' mouse, look at the **Pointer Lock** 
section below.

- On some platforms, mouse input has sub-pixel precision, on others 
mouse input only has pixel precision. 

- Mouse positions and movement are reported in pixel 
coordinates which makes it difficult to derive useful movement
information out of it (dividing by the framebuffer resolution 
is a simple but very fuzzy workaround). On some platforms, 
enabling pointer-lock fixes the issue and returns mouse coordinates
in 'mouse resolution', but this is not guaranteed.

Unfortunately cross-platform mouse input is a complete mess, and there's no way
around testing mouse input on all platforms, and sometimes apply tweaks and
hacks to make it 'feel right'.

### Pointer Lock 

By default, mouse input will 'stop' at window or screen borders which
is not useful if the mouse should be used for any type of camera control.

To get continuous mouse movement information, the so-called **Pointer Lock**
mode must be enabled. Pointer lock means:

- the mouse pointer will be hidden
- the mouse position will become 'undefined' (usually it will be pinned to the
center of the screen)
- mouse movement will be continuous even if the mouse position would
move outside of the screen boundary

Pointer-lock mode can only be enabled from a callback function invoked when
certain mouse events happen. This is a security-related restriction enforced by
the HTML5 [Pointer Lock
API](https://developer.mozilla.org/en-US/docs/Web/API/Pointer_Lock_API). 

Here's an example that enables pointer-lock when the left mouse button
is pressed, and disables pointer-lock again when the left mouse button
is released. This is useful for a mixed input scenario, where the
mouse should act as a 'point-n-click' mouse, but also should
control a camera when a mouse button is held down:

```cpp
// register a pointer-lock event callback implemented in a lambda function
Input::SetPointerLockHandler([](const InputEvent& e) -> PointerLockMode::Code {
    
    if (e.Button == MouseButton::Left) {
        if (e.Type == InputEvent::MouseButtonDown) {
            // enable pointer-lock on LMB down
            return PointerLockMode::Enable;
        }
        else if (e.Type == InputEvent::MouseButtonUp) {
            // ...and disable again when LMB released
            return PointerLockMode::Disable;
        }
        else {
            // all other events: do nothing
            return PointerLockMode::DontCare;
        }   
    }
});
```

The current pointer-lock implementation has a few issues, especially
on web browsers:

- some browsers will either show a passive message, or even require
  an user action if pointer-lock-mode is enabled
- browsers might either not implement the pointer-lock API at all,
  or have a broken implementation
- on native desktop platforms, the pointer-lock activation inside an
  event callback isn't needed, there might be a simpler, alternative way to
  enable/disable pointer-lock in the future
  
### Touch Input

The touch input functions provide access to basic touch- and multi-touch-input,
and is currently only implemented on mobile platforms and not for notebook
touchpads (although there's a gray area on browsers running on notebooks, which
might expose the notebook's touchpad as touch device).

Oryol's touch support includes a few platform-agnostic **gesture recognizers**.
Gesture input isn't standardized between platforms in the same way that mouse
input is unfortunately, thus Oryol needs to implement its own gesture
recognition based on low-level touch input. 

To find out whether touch input is available on the current host platform, 
call **Input::TouchpadAttached()**:

```cpp
    if (Input::TouchpadAttached()) {
        // handle touch input
        ...
    }
```

To check for **single- and double-taps**, and get the position of the tap:

```cpp
    // check for single-tap and get the tap-position
    if (Input::TouchTapped()) {
        const glm::vec2& pos = Input::TouchPosition(0);
        ...
    }
    // check for double-tap and get the tap-position
    if (Input::TouchDoubleTapped()) {
        const glm::vec2& pos = Input::TouchPosition(0);
        ...
    }
```

**Panning gesture** (single finger dragging) is checked as follows:

```cpp
// check if panning has started:
if (Input::TouchPanningStarted()) {
    // get the position where panning has started:
    const glm::vec2& startPos = Input::TouchStartPosition(0);
    // the 'current position' should be the same
    // as the starting position here:
    const glm::vec2& curPos = Input::TouchPosition(0);
}
// check if panning is currently underway:
if (Input::TouchPanning()) {
    // the start position stays the same:
    const glm::vec2& startPos = Input::TouchStartPosition(0);
    // the current position moves with the finger:
    const glm::vec2& curPos = Input::TouchPosition(0);
    // distance travelled during the last frame:
    const glm::vec2& mov = Input::TouchMovement(0);
}
// check if panning has ended:
if (Input::TouchPanningEnded()) {
    // to get the last position before the finger was lifted:
    const glm::vec2& lastPos = Input::TouchPosition(0);
}
```

**Pinching gesture** (basically a 2-finger panning) is checked as
follows:

```cpp
// check if a pinching gesture has started:
if (Input::TouchPinchingStarted()) {
    // positions are now provided for 2 fingers:
    const glm::vec2& startPos0 = Input::TouchStartPosition(0);
    const glm::vec2& startPos1 = Input::TouchStartPosiiton(1);
    const glm::vec2& curPos0 = Input::TouchPosition(0);
    const glm::vec2& curPos1 = Inpit::TouchPosition(1);
}
// check if pinching is underway:
if (Input::TouchPinching()) {
    // all touch position and movement functions work the same
    // as during panning, but provide information for the
    // second finger in the slot 1:
    const glm::vec2& startPos0 = Input::TouchStartPosition(0);
    const glm::vec2& startPos1 = Input::TouchStartPosition(1);
    const glm::vec2& curPos0 = Input::TouchPosition(0);
    const glm::vec2& curPos1 = Input::TouchPosition(1);
    const glm::vec2& mov0 = Input::TouchMovement(0);
    const glm::vec2& mov1 = Input::TouchMovement(1);
}
// check if pinching has ended:
if (Input::TouchPinchingEnded()) {
    // the last positions of the 2 fingers:
    const glm::vec2& lastPos0 = Input::TouchPosition(0);
    const glm::vec2& lastPos1 = Input::TouchPosition(1);
}
```

For touch input, using event callbacks is often the better way because
an input event contains all the additional position information (for
instance an InputEvent::TouchTapped also contains the position where
the tap happened).

### Device Sensors

Most mobile devices have at least gravity and gyroscope sensors that provide
information about the current orientation of the device relative to
the closest planetary body.

In the Oryol Input module, these sensors can be queried with the 
following functions:

```cpp
// test if host platform provides sensory input:
if (Input::SensorsAttached()) {
    // get the current acceleration vector:
    const glm::vec3& acc = Input::SensorAcceleration();
    // get the current yaw, pitch, roll angles (in radians)
    const glm::vec3& ypr = Input::SensorYawPitchRoll();
}
```
> NOTE: yaw/pitch/roll is currently not supported on Android, only the 
> acceleration vector


### Gamepad Input

#### Cooked vs Raw Gamepad Input

Gamepad access between platforms and gamepads is not standardized.
Oryol needs to perform a mapping from physical buttons and
axes to a 'standard mapping'. The Input module provides built-in
mappings for a few gamepad types (PS4 and Xbox360 controller) and
platforms (Windows, Linux, OSX and HTML5). User code can provide
additional mappings, or override the builtin mappings. 

> NOTE: Gamepad input is only available via polling, not through
input events!

#### Checking For Attached Gamepads

To check whether a gamepad is attached, call the **Input::GamepadAttached()** method:

```cpp
for (int i = 0; i < Input::MaxNumGamepads; i++) {
    if (Input::GamepadAttached(i)) {
        // gamepad is attached
    }
}
```
Up to 4 gamepads can be attached (this is the value of the **Input::MaxNumGamepads** constant).

Please note that the behaviour of the Input::GamepadAttached() method is quite different between platforms:

- on HTML5, an attached gamepad will only be detected when
the user triggers a button or axis
- an attached gamepad that was removed might still be reported
as attached, but all buttons and axes will be idle

#### Getting Cooked Gamepad Input

To get cooked (aka 'mapped') gamepad input, call the following
methods:

```cpp
// check status of first gamepad
if (Input::GamepadAttached(0)) {
    // check if 'A' button is currently pressed down
    if (Input::GamepadButtonPressed(0, GamepadButton::A)) {
        ...
    }
    // check if 'B' button changed from released to pressed last frame:
    if (Input::GamepadButtonDown(0, GamepadButton::B)) {
        ...
    }
    // check if 'X' button was released during last frame:
    if (Input::GamepadButtonUp(0, GamepadButton::X)) {
        ...
    }
    
    // get the current horizontal position (-1.0..1.0) of the left stick
    float v = Input::GamepadAxisValue(0, GamepadAxis::LeftStickHori);

    // get the current value of the right trigger (0.0..1.0)
    float t = Input::GamepadAxisValue(0, GamepadAxis::RightTrigger);
}
```
These are the button and axis identifiers (but note that not all
buttons or axes will be functional on all gamepads or even the same
gamepad type on different platforms):

```cpp
struct GamepadButton {
    enum Code {
        A = 0,
        B,
        X,
        Y,
        LeftBumper,
        RightBumper,
        LeftTrigger,
        RightTrigger,
        Back,
        Start,
        LeftStick,
        RightStick,
        DPadUp,
        DPadDown,
        DPadLeft,
        DPadRight,
        Center,         // Xbox-button or DS4 touchpad

        NumButtons,
        InvalidButton
    };
};

struct GamepadAxis {
    enum Code {
        LeftStickHori = 0,
        LeftStickVert,
        RightStickHori,
        RightStickVert,
        LeftTrigger,
        RightTrigger,

        NumAxes,
        InvalidAxis
    };
};
```

#### Raw Gamepad Input

The Input module also provides raw (aka unmapped) gamepad input. This
is useful if the standard mapping mechanism isn't sufficient. But please
be aware that the meaning of raw buttons/axes or the value range
of axes may differ between platforms and gamepad types.

Accessing raw input works with button and axes indexes:

```cpp
// get raw input from gamepad 0
if (Input::GamepadAttached(0)) {
    // check state of all raw buttons
    for (int btnIndex = 0; btnIndex < Input::MaxNumRawButtons; btnIndex++) {
        if (Input::GamepadRawButtonPressed(0, btnIndex)) {
            ...
        }
        if (Input::GamepadRawButtonDown(0, btnIndex)) {
            ...
        }      
        if (Input::GamepadRawButtonUp(0, btnIndex)) {

        }
    }
    // check state of all raw axes
    for (int axisIndex = 0; axisIndex < Input::MaxNumRawAxes; axisIndex++) {
        float v = Input::GamepadRawAxisValue(0, axisIndex);
        ...
    }
}
```

#### Adding New Gamepad Mappings

It is possible to add new gamepad mappings or override the built-in
standard mappings. This can either happen when the Input module
is initialized, or at any later time.

A gamepad mapping provides the following information to the 
Oryol Input module:

- A gamepad-type identifier string to associate the mapping
with a specific gamepad type. The special identifier string **"__default"** is used to override the default-mapping.
- For each supported 'logical button' (e.g. GamepadButton::A) the
physical button index.
- For each supported 'logical axis' (e.g. GamepadAxis::LeftStickHori) the
physical axis index.
- For each axis, optional scale and bias values to move the 
input values into a new range (for instance from -1..1 to 0..1)

Here's some sample code how to add mappings:

```cpp
// setup a new GamepadMapping object (this example uses the
// PS4 controller mapping on Linux):
GamepadMapping m;
m.Buttons[GamepadButton::A] = 1;
m.Buttons[GamepadButton::B] = 2;
m.Buttons[GamepadButton::X] = 0;
m.Buttons[GamepadButton::Y] = 3;
m.Buttons[GamepadButton::LeftBumper] = 4;
m.Buttons[GamepadButton::RightBumper] = 5;
m.Buttons[GamepadButton::LeftTrigger] = 6;
m.Buttons[GamepadButton::RightTrigger] = 7;
m.Buttons[GamepadButton::Back] = 8;
m.Buttons[GamepadButton::Start] = 9;
m.Buttons[GamepadButton::LeftStick] = 10;
m.Buttons[GamepadButton::RightStick] = 11;
m.Buttons[GamepadButton::Center] = 13;
m.Axes[GamepadAxis::LeftStickHori].Axis = 0;
m.Axes[GamepadAxis::LeftStickVert].Axis = 1;
m.Axes[GamepadAxis::RightStickHori].Axis = 2;
m.Axes[GamepadAxis::RightStickVert].Axis = 5;
m.Axes[GamepadAxis::LeftTrigger].Axis = 3;
m.Axes[GamepadAxis::LeftTrigger].Scale = 0.5f;
m.Axes[GamepadAxis::LeftTrigger].Bias = 0.5f;
m.Axes[GamepadAxis::RightTrigger].Axis = 4;
m.Axes[GamepadAxis::RightTrigger].Scale = 0.5f;
m.Axes[GamepadAxis::RightTrigger].Bias = 0.5f;

// add the mapping during the Input module initialization
InputSetup setup;
setup.GamepadMappings.Add("Sony Computer Entertainment Wireless Controller", m);

//... or you can also do this later:
Input::AddGamepadMapping("Sony ... Controller", m);
```

Use the [GamepadExplorer sample](https://floooh.github.com/oryol/asmjs/GamepadExplorer.html)
to find out what the physical button and axis indices, and the identifier
string of a specific gamepad are. But please be aware that this may
differ for the same gamepad type on different platforms, so make sure
to check this on each browser and platform (for the emscripten version),
and also check the natively compiled version of the GamepadExplorer sample
on OSX, Windows and Linux!

### Input Event Callbacks

Event-based input handling works by registering an event handler
callback that is called whenever an input event happens. Several
handlers can be attached, but the order how the handlers are
called is currently undefined. It is also not currently possible
to control event visibility between handlers, all input events
will be dispatched to all handlers.

```cpp
// subscribe to input events with a lambda function:
Input::SubscribeEvents([](const InputEvent& e) {

    Key::Code key = Key::InvalidKey;
    wchar_t chr = 0;
    glm::vec2 mouseMove;

    switch (e.Type) {
        case InputEvent::KeyDown:
            key = e.KeyCode;
            break;
        case InputEvent::KeyUp:
            key = e.KeyCode;
            break;
        case InputEvent::WChar:
            chr = e.WCharCode;
            break;
        case InputEvent::MouseMove:
            mouseMove = e.Movement;
            break;
        ...
    }
});
```

Currently, keyboard, mouse and touch events are exposed to the 
input event callback.

It is possible to unsubscribe from input events, for this reason,
Input::SuscribeEvents() returns an opaque handle which needs
to be provided to Input::UnsubscribeEvents() later:

```cpp
// subscribe to events, store returned handle
this->callbackId = Input::SubscribeEvents([](const InputEvent& e) {
    ...
});

// at some later point, unsubscribe
Input::UnsubscribeEvents(this->callbackId);
```
