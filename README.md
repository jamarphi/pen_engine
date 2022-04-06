# PenEngine

![Pen Engine Logo](https://github.com/fentacoder/pen_engine/blob/master/res/textures/pen_engine_logo.png?raw=true)

Pen Engine is a cross-platform C++ graphics engine that supports 2D/3D drawing, texel drawing, and GUI development. 
It uses OpenGL underneath for graphics rendering. It is made as a library for use in
applications with graphics rendering code being abstracted.

There is an Example.cpp file in Pen Engine to show you how to do the basics.

Windows initial set up using Visual Studio:

Include these directories in C/C++ -> General:
    
    $(SolutionDir)pen_engine\dependencies\glfw\include;$(SolutionDir)pen_engine\dependencies\glad;

Linker config:
Add this to Additional Library Directories: 

    $(SolutionDir)pen_engine\dependencies\glfw\lib-vc2019;

Add additional dependencies in Linker -> Input:

    glfw3.lib
    opengl32.lib

If you are using Cmake instead of Visual Studio then uncomment __PEN_CMAKE__ in pen_engine/src/state/config.h:

    //#define __PEN_CMAKE__

If you are on Linux then I suggest using Cmake, first uncomment __PEN_LINUX__ in pen_engine/src/state/config.h:

    //#define __PEN_LINUX__

If you are on a Mac then uncomment __PEN_MAC__ in pen_engine/src/state/config.h:

    //#define __PEN_MAC__

Then install Cmake and create a build directory in the root of Pen Engine.

Go into the CMakeLists.txt file in the root and modify the add_executable line to your main cpp file:

    add_executable(${PROJECT_NAME} ${PROJECT_LIB_SOURCES} "../YourMainCPP.cpp")

Next cd into the build directory and call:

    cmake ../../pen_engine
    cmake --build .

This should compile an executable that can be ran on Linux.

If on Windows I suggest using Visual Studio, if on Linux use Cmake.
Currently on Windows the executable built from Cmake doesn't display the objects due to an issue with linking GLFW although it does link, so for now use Visual Studio on Windows.

For cross-platform projects, use pen::Map for maps, this is to remove the dependency of cmath which causes problems for Android cross compiling.
Also avoid using the standard math libraries whenever you can inside of your cross-platform projects, using operations such as pen::op::Sin() give
good approximations as long as the given value is between 0 and 2 pi.
If doing something strictly for PC it is fine, but if you decide to change your mind later it could take some time to change these things.

Remember to use

    pen::Pen::ScreenWidth()

and

    pen::Pen::ScreenHeight()

when referencing your application's screen width and height so that it will be accurate across all platforms.

---------------------------------------------------------------------------

# 1.1 - Introduction

To use Pen Engine inherit it with a class like this:

      class App : public pen::Pen {
      public:
          void OnCreate() override {

          }

          void OnInput() override {

          }

          void OnRender() override {

          }
      };

Setting up the render cycle:

    void OnRender() override {
        pen::Renderer renderer;
        
        while (pen::Pen::Running())
        {
            OnInput();
            renderer.Clear();
            if(pen::Render::Get()->firstTime) renderer.Background(pen::PEN_GRAY);

            /*Your program logic*/

            pen::Pen::ManageRender(&renderer);
        }
    }

Setting up int main:

    int main()
    {
        App app = App();
        app.CreateApplication("App", SCR_WIDTH, SCR_HEIGHT, "/home/user/dev/project");
        app.OnCreate();
        app.OnRender();
        app.End();

        return 0;
    }

To load in texture files, add an std::string vector to CreateApplication():

    app.CreateApplication("App", SCR_WIDTH, SCR_HEIGHT, pen_engineDir, pathList);

When doing this use sprite sheets in order to load more textures.  Once all the texture slots get used,
for laptops it's 32, then you will get terrible performance when using textures not within that first 29 range.
The first three slots are used for solid colors, bitmap font rendering, and texel drawing, so that means there are
29 available texture slots left in this scenario.

When doing cross platform development, your file names for assets can only contain:

    - Lowercase letters
    - Numbers
    - Underscores

When passing in the Pen Engine Directory, make sure it's an absolute path such as this on Windows:

    C:/Users/user/dev/project

Or this on Linux:

    /home/user/dev/project

This means that Pen Engine is inside the project folder, this is used to find the textures if added to res/textures.

To add a gui item use:

    pen::ui::AddItem(yourItemPointer);

You can also make objects wireframe by setting their isWireFrame boolean to true before adding them to LayerManager.

When removing gui items you can either use:

    pen::ui::RemoveItemById(id);

    or

    pen::ui::RemoveItem(itemPtr);

If deleting gui items yourself from your own data, call pen::ui::RemoveItem and set each pointer you delete to nullptr:
    
    pen::ui::RemoveItem(itemPtr);

If debug mode is set to true, the debug items will use an id of 456789.

If adding an item where the id does not matter, you can use ID_ANY which uses 476890.

Be sure to exclude the examples folder from your project.

---------------------------------------------------------------------------

# 1.1.1 - Managing Assets

To make managing assets easier you can do:

    pen::Asset asset = pen::Asset::Load(std::string file, char*(*onLoad)(const char* path, long* fileLength) = nullptr);

The file passed in should be the full path.  The onLoad function is your loading function that you pass in for whatever file you want to load.

If you want to access the assets use:

    pen::Asset asset = pen::Asset::Find(const std::string& fileName);

---------------------------------------------------------------------------

# 1.2 - Complex Shapes

To add your own vertex and index data for gui items, pass in pointers to that data like so:

    std::vector<float> position = { 300.0f, 200.0f, 0.0f, 400.0f, 200.0f, 0.0f, 350.0f, 300.0f, 0.0f };
    std::vector<int> indices = { 0, 1, 2 };
    pen::ui::AddItem(new pen::ui::Item(0, &position[0], 3, &indices[0], 3));

---------------------------------------------------------------------------

# 1.3 - Pixel Drawing

To draw pixels first call:
    
    pen::InitializePixels();
    
This will let Pen Engine know that you want to use pixels to draw.

Then in order to draw something, for example a line, you would do this inside the render loop: 

    for (int i = 0; i < pen::PixelBufferWidth(); i++) {
        pen::Draw(i, 100, pen::PEN_RED, maskOptional);
    }

Pixel drawing must be done within the render loop.

You can also pass in a boolean of false to the mask parameter to draw transparency to the buffer if you want,
by default it does not draw pixels with an alpha of 0.0f so it does not overwrite what is already there with transparency.

If you want to access the pixel buffer directly you can do:

    unsigned char* pixels = pen::PixelBuffer();   

The pixel buffer will return a nullptr if accessed directly for mobile builds, this is because the pixel buffer happens on the mobile side.

pen::PixelBufferWidth() and pen::PixelBufferHeight() are used since the texels on the screen are in a different space, this way all of the pixels can be
covered properly.

You can pan the pixel buffer based on screen dimensions with:

    pen::Pan(float panX, float panY, bool reset = false);

You can zoom in or out on the pixel buffer based on screen dimensions with:

    pen::Zoom(float mag);

You can create sprites as well in the pixel buffer using:

    pen::ui::Item* item = pen::CreateSprite(int startX, int startY, int length, int height, const std::string& path,
			float spriteTexCoordStartX = 0.0f, float spriteTexCoordStartY = 0.0f, float spriteTexCoordEndX = 1.0f, float spriteTexCoordEndY = 1.0f,
			bool compress = false, float (*userDisplayFunction)(int, int, float) = nullptr)

The userDisplayFunction is for using math functions to determine where the texture data gets drawn to with respect to the y axis.

Then afterwards within the render loop you can do:

    item->Draw();

There are functions that can be used to create pixel items as shapes:

    - pen::DrawCircle(int x, int y, int radius, pen::Vec4 color, bool fill = true)
    - pen::DrawLine(int startX, int startY, int endX, int endY, pen::Vec4 color, unsigned char* parentBuffer = nullptr, int parentWidth = 0, int parentHeight = 0)
    - pen::DrawTri(int aX, int aY, int bX, int bY, int cX, int cY, pen::Vec4 color, bool fill = true)
    - pen::DrawRect(int startX, int startY, int length, int height, pen::Vec4 color, bool fill = true)

Pixel items can be used without CreateSprite as well if you want to pass in your own buffer data instead of loading one in.

There are transformations you can do with pixel items as well:

    - pen::Scale(pen::ui::Item* item, float sx, float sy)
    - pen::Rotate(pen::ui::Item* item, float theta)
    - pen::Translate(pen::ui::Item* item, float tx, float ty)

When deleting pixel items you can use DeleteItem to make things more convenient, this just deletes the buffer inside of the item and
then the item itself so there are no memory leaks:

    pen::DeleteItem(pen::ui::Item* item);
    item = nullptr;

---------------------------------------------------------------------------

# 1.4 - User Input

For mouse input, the mouse position can be called using:

double x = 0.0, y = 0.0;
pen::Pen::GetMousePos(&x, &y);

The cursor can be hidden or shown using pen::Pen::MakeMouseHidden or pen::Pen::MakeMouseShow respectively.

The cursor position can be checked to see if it is still within your application window using pen::Pen::WindowActive.

The mouse state is used for events such as button presses, releases, and holding.  For example, to check for a left click you would do:

pen::Pen::MouseState(pen::in::KEYS::MOUSE_LEFT) == pen::in::KEYS::PRESSED;

When using click callback functions with Pen Engine to handle GUI mouse click events you have to do for example:

button == pen::in::KEYS::MOUSE_LEFT && action == pen::in::KEYS::PRESSED

KeyState() is not called because KeyState() is used to grab the button event at that time, when using Pen Engine with callback functions the timing is different 
than when your specific callback function gets ran.

For getting mouse scroll events do:

pen::State::MouseScrollXOffset();
pen::State::MouseScrollYOffset();

The key state is similar to mouse events for things such as button presses, releases, and holding.  For example, to check for the escape key being pressed you would do:

pen::Pen::KeyState(pen::in::KEYS::ESCAPE) == pen::in::KEYS::PRESSED;

---------------------------------------------------------------------------

# 1.5 - GUI

To allow Pen Engine to handle GUI click events for you, call:

pen::Pen::HandleGUIClickEvents(true, &yourClickCatchAllFunction);

This only handles items that have isUI and isClickable set to true for GUI components.

There are nine basic components already defined for creating graphical user interfaces:

    - pen::ui::TextBox
    - pen::ui::Button
    - pen::ui::VerticalList
    - pen::ui::ScrollBar
    - pen::ui::NavBar
    - pen::ui::Slider
    - pen::ui::TextEditor
    - pen::ui::ContextWindow
    - pen::ui::FileModal

For some of the components above such as the ScrollBar component, 
when nullptr is passed to it for your click callback function, it handles the event by default.

The sames goes for the Slider component as far as the click callback function, sliders do not need a parent item.
The pen::ui::Item class can also be used for GUIs.  For the slider it has to be double-clicked to use.

If you want Pen Engine to handle GUI component click events, a function pointer can be passed in, make sure that it is not part of a class otherwise
the type conversion will not work.  Note that the on click listener is only for clicks and releases, not for any cliks that are held.

For the pen::ui::FileModal component, there are four intents that can get passed in that determine the behavior:

    - pen::ui::FILE_INTENT::SAVE_AS
    - pen::ui::FILE_INTENT::SELECT_DIRECTORY
    - pen::ui::FILE_INTENT::SELECT_FILE
    - pen::ui::FILE_INTENT::SELECT_FILES

---------------------------------------------------------------------------

# 1.5.1 - Dragging Components

To set an on drag listener, use:

    pen::Pen::HandleGUIDragEvents(true);

Your function has to be of the format:

    bool yourFunctionName(pen::ui::Item*, double*, double*)

In order for your draggable item to be callable though, you have to use:

    pen::State::SetDraggable((void*) yourItem);

This way Pen Engine can handle this item.

---------------------------------------------------------------------------

# 1.5.2 - Keyable Components

To set an on key listener, use:

    pen::Pen::HandleGUIKeyEvents(true);

Your function has to be of the format:

    bool yourFunctionName(pen::ui::Item*, int, int)

In order for your keyable item to be callable though, you have to use:

    pen::State::SetKeyable((void*) yourItem);

This way Pen Engine can handle this item.

---------------------------------------------------------------------------

# 1.6 - Animations

To animate a gui item, simply add an item to the animation manager:

    pen::ui::Animation::Add(item, type, ms, infinite, unitA, unitB, unitC);

1. The item is a pointer to an item you already created.
2. The type is the transformation, an example is pen::ui::AnimationType::TRANSLATE.
3. The ms is the amount of milliseconds for the duration.
4. The infinite boolean is for if you want to run this animation forever, this means that any other animation
   with the same item and type will not run.
5. The three units unitA, unitB, and unitC, are used to pass in values for the transformations.  If you are doing
   rotation for instance, then you only need to use unitA, this means unitB and unitC will be ignored.

---------------------------------------------------------------------------

# 1.7 - 3D

Before rendering 3D models using .obj files you have to call:

    pen::ui::AddItem3D(0, "your_model_path.obj");
    pen::Pen::EnableDepthTesting(true);
    pen::Pen::HandleCameraInput(true);

If you enable depth testing, 2D textures will not work currently, so if you use 3D rendering you can't do 2D textures.

You can use left arrow to pan the camera left, right arrow to pan right, up arrow to pan up and down arrow to pan down.

The W key is used to pan the camera forward and the S key is used to pan the camera backwards.

Currently 3D textures from .matl files are not implemented.

3D items can be transformed with:

    - pen::_3d::Scale(pen::Item3D* item, float sx, float sy, float sz);
    - pen::_3d::Rotate(pen::Item3D* item, float theta, int axis);
    - pen::_3d::Translate(pen::Item3D* item, float tx, float ty, float tz);

For rotation, the axis can be chosen using:

    - pen::_3d::AXIS::X
    - pen::_3d::AXIS::Y
    - pen::_3d::AXIS::Z

---------------------------------------------------------------------------

#1.8 - Cross Platform

I would like to thank the developers and contributors of Cocos2DX because I used their cross platform implementation
to help me as I worked on the platforms below.  Cocos2DX is a well known open source game engine that has tons of support.
They have amazing documentation and readable code that helps people understand why they implement
the platforms the way they do.

When building for cross platform use Linux.

Cross platform is currently supported for the following:

- Android

Functions that are not compatible with mobile:

- pen::Pen::GetWindow
- pen::Pen::CloseWindow
- pen::Pen::MakeMouseHidden
- pen::Pen::MakeMouseShow
- pen::Pen::WindowActive
- pen::Pen::MouseState
- pen::Pen::KeyState
- pen::Pen::HandleGUIClickEvents
- pen::Pen::HandleGUIDragEvents
- pen::Pen::HandleGUIKeyEvents
- pen::Pen::HandleClick

---------------------------------------------------------------------------

#1.8.1 - Cross Platform For Android

When doing cross platform applications not much changes, but a few things do.  Let's start with Android.

When using OnRender, you now have to pass your logic into a callback function as follows:

    pen::Pen::SetMobileCallbacks(&yourMobileRenderCallback, &yourMobileClickCallback, &yourMobileResumeCallback,
        &yourMobilePauseCallback, &yourMobileKeyCallback, &yourMobileTiltCallback);

This function should be in the OnCreate function of a Pen Engine instantiation, for example:

    class App : public pen::Pen {
    public:
        void OnCreate() override {
            pen::Pen::SetMobileCallbacks(&yourMobileRenderCallback, &yourMobileClickCallback, &yourMobileResumeCallback,
                &yourMobilePauseCallback, &yourMobileKeyCallback, &yourMobileTiltCallback);
            pen::Pen::SetMobileTextures(pathListVector);

            /*All logic in OnCreate should come after the two previous functions*/
        }
    }

pen::Pen::SetMobileTextures is only used for gui sprite sheets, if loading anything else do:

    pen::Asset asset = pen::Asset::Load(std::string file, char*(*onLoad)(const char* path, long* fileLength) = nullptr);

Check out section 1.1.1 for more on managing assets.
If specifically loading in images to be rendered then you can take the asset data and add it to the sprite map by doing:

    pen::State::Get()->pixelSprites.Insert(asset.name, pen::Sprite{asset.name, (unsigned char*)asset.data, width, height});

When doing this though, the width and height of the image should be known before, such as with png having the width and height in the header of the file.
The pixelSprites map needs this information so it will not attempt to get it loaded in from the res/drawables folder of android when creating sprites.
Loading in images from res/drawables is completely fine and is what happens by default when using pen::CreateSprite();

If setting your app orientation horizontal, also add this to the MainActivity tag in the AndroidManifest.xml:

    android:screenOrientation="landscape"

The GLSurfaceView orientation can not be changed at run time otherwise it will mess up the resources and crash.

Pen Engine looks for this instantiation in app.h, but you can change the name of that file include in 
pen_engine/ext/platforms/android/app/src/cpp/FunctionMapping.cpp.

There is a difference for how you would do the mobile render callback function for android though, notice instead of
a while loop for PC, you do an if statement:

    void yourAndroidRenderFunction(){
        pen::Renderer renderer;
        
        if (pen::Pen::Running())
        {
            renderer.Clear();
            if(pen::Render::Get()->firstTime) renderer.Background(pen::PEN_GRAY);

            /*Your program logic*/

            pen::Pen::ManageRender(&renderer);
        }
    }

Also input would have to be done on the native side for Android while implementing one of the mobile callback functions.

The PC function would remain the same with the while loop:

    void OnRender() override {
        pen::Renderer renderer;
        
        while (pen::Pen::Running())
        {
            OnInput();
            renderer.Clear();
            if(pen::Render::Get()->firstTime) renderer.Background(pen::PEN_GRAY);

            /*Your program logic*/

            pen::Pen::ManageRender(&renderer);
        }
    }

To get started building .apk files, first go to pen_engine/state/config.h and uncomment this line:

    //#define __PEN_MOBILE__

If you uncomment this line, your application will not run for PC, so only do this once you have tested
on the PC version and are ready to build it for Android.

Now create a build directory inside the root directory of pen_engine, so it would be:

    pen_engine/build

Afterwards go to pen_engine/ext/platforms/android/local.properties and set your android
sdk path:

    sdk.dir=/path/to/sdk

Also set your ndk directory path like so, although this is claimed by Android to be deprecated in the future, this was necessary for ndk to find
its library stripping tool:

    ndk.dir=/home/user/Android/Sdk/ndk/20.0.5594570

Also set your java jdk path in the gradle.properties file:

    org.gradle.java.home=/your_jdk_path

You are going to need the JAVA_HOME environment variable set as well, after you download openjdk call:

    export JAVA_HOME=/usr/lib/jvm/your jdk
    export PATH=$PATH:$JAVA_HOME/bin

Once that is done install Android Studio, also install these packages for Linux:

    - ninja-build (for using the ninja generator with cmake which is what gradle uses by default for cmake builds)
    - gcc-multilib (for compiling)
    - libncurses5 (for the Android Studio debugger)

Make sure you have an internet connection because this will download any files needed for the build.
This will build the apk files into a build/outputs/apk directory inside of the android directory.

For signed apks in pen_engine/ext/platforms/android/app/gradle.build, you will have to uncomment and add your key store 
file and information here:

    signingConfigs {

        release {

            storeFile file('path_relative_to_pen_engine/ext/platforms/android/app')
            storePassword "yourpassword"
            keyAlias "youralias"
            keyPassword "yourkeypass"

        }
    }

These are the events that are not implemented in this version yet:

- Touch and hold events.

---------------------------------------------------------------------------

#1.8.1.1 - Android Persisted Memory

If you want to perform manipulation of persisted memory for your android application you can call the following:

    /*Needs to be called before any database operation*/
    - pen::platforms::android::db::Start(const char* dbName, const char* tableName);

    /*Closes the database connection once you are done*/
    - pen::platforms::android::db::Close();

    /*Adds or updates a key value in the database*/
    - pen::platforms::android::db::SetItem(const char* key, const char* value);

    /*Retrieves a key value from the database as a string*/
    - pen::platforms::android::db::GetItem(const char* key);

    /*Removes a key from the database*/
    - pen::platforms::android::db::RemoveItemById(const char* key);

    /*Deletes the table from the database*/
    - pen::platforms::android::db::DeleteTable();

---------------------------------------------------------------------------

#1.8.1.2 - Native Key Events For Android

If you want to do native key events for android, first go 
to pen_engine/ext/platforms/android/app/src/java/com/jamar/penengine/PenSurfaceRenderer.java.
Once you do that, create a method called nativeOnKeyPress:
    
    private static native String nativeOnKeyPress(final int keyCode,boolean isPressed);

Use this function to return a string of the character back to the native library for the callback
pen::State::mobileOnKeyCallback to act on it.

Another method, which would be easier, would be to do the edit text input completely on the native side
and save some data about it in SQL.  Then use the SQL value to change the main activity in which
the graphics application runs when it is reloaded.

---------------------------------------------------------------------------

#1.8.1.3 - Android Logging

If you want to send messages to Java for logging use:

    pen::platforms::android::AppLog(const char* yourMessage);

This can be used for Android debugging once your application has been built and tested on PC.

---------------------------------------------------------------------------

#1.8.1.4 - Android Loading

You can load in assets such as level packs by doing:

    pen::Asset::Load(std::string file, nullptr);

The full name should be passed in including any sub directory inside the assets directory.
For example the file could be levels/level1.txt, or it could be music1.mp3, or sounds/enemy1/sound1.wav.

You can also load in a directory in assets by doing:

    pen::Asset::LoadMobileDir(std::string dirPath);

#1.9 - AI

For documentation on implementing AI go to ext/ai/README.md

---------------------------------------------------------------------------