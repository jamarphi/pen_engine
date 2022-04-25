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

    $(SolutionDir)pen_engine\dependencies\glfw\lib-win;

Add additional dependencies in Linker -> Input:

    glfw3.lib
    opengl32.lib

If you are using Cmake instead of Visual Studio then uncomment __PEN_CMAKE__ in pen_engine/src/state/config.h:

    //#define __PEN_CMAKE__

If you are on Linux then I suggest using Cmake, first uncomment __PEN_LINUX__ in pen_engine/src/state/config.h:

    //#define __PEN_LINUX__

Then install Cmake and create a build directory in the root of Pen Engine.

Go into the CMakeLists.txt file in the root and modify the add_executable line to your main cpp file:

    add_executable(${PROJECT_NAME} ${PROJECT_LIB_SOURCES} "../app.cpp")

Next cd into the build directory and call:

    cmake ../../pen_engine
    cmake --build .

This should compile an executable that can be ran on Linux.

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
        while (pen::Pen::Running())
        {
            OnInput();
            if(pen::Render::Get()->firstTime) pen::Render::Background(pen::PEN_GRAY);

            /*Your program logic*/

            pen::Pen::ManageRender();
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

When doing this use sprite sheets in order to load more textures.  There are 32 available texture slots for 
Windows and Linux.  There are 13 available texture slots for Mac.  The first three slots are used for solid 
colors, bitmap font rendering, and texel drawing, so that means there are 29 available texture slots left 
in this scenario for Windows and Linux, 10 available texture slots for Mac.

When doing cross platform development, your file names for assets can only contain:

    - Lowercase letters
    - Numbers
    - Underscores

When passing in the Pen Engine Directory, make sure it's an absolute path such as this on Windows:

    C://Users/user/dev/project

On Linux:

    /home/user/dev/project

On Mac:

    /Users/user/dev/project

This means that Pen Engine is inside the project folder, this is used to find the textures if added to res/textures.

To add a gui item use:

    pen::ui::AddItem(pen::ui::Item* yourItemPointer);

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

    pen::ui::Item* item = pen::CreateSprite(int startX, int startY, int width, int height, const std::string& path,
			float spriteTexCoordStartX = 0.0f, float spriteTexCoordStartY = 0.0f, float spriteTexCoordEndX = 1.0f, float spriteTexCoordEndY = 1.0f,
			bool compress = false, float (*displayFunction)(int, int, float) = nullptr)

The displayFunction is for using math functions to determine where the texture data gets drawn to with respect to the y axis.

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

    pen::DeleteItem(pen::Item* item);
    item = nullptr;

Animations for items can be swapped by doing:

    pen::Animate(pen::Item* item, const std::string& path,
		float spriteTexCoordStartX = 0.0f, float spriteTexCoordStartY = 0.0f, float spriteTexCoordEndX = 1.0f, float spriteTexCoordEndY = 1.0f);

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

There are eleven basic components already defined for creating graphical user interfaces:

    - pen::ui::TextBox
    - pen::ui::Button
    - pen::ui::VerticalList
    - pen::ui::ScrollBar
    - pen::ui::NavBar
    - pen::ui::Slider
    - pen::ui::TextEditor
    - pen::ui::ContextWindow
    - pen::ui::FileModal
    - pen::ui::Div
    - pen::ui::KeyBoard

For some of the components above such as the ScrollBar component, 
when nullptr is passed to it for your click callback function, it handles the event by default.

The sames goes for the Slider component as far as the click callback function, sliders do not need a parent item.
The pen::ui::Item class can also be used for GUIs.  For the slider it has to be double-clicked to use.

If you want Pen Engine to handle GUI component click events, a function pointer can be passed in, make sure that it is not part of a class otherwise
the type conversion will not work.  Note that the on click listener is only for clicks and releases, not for any clicks that are held.

For the pen::ui::FileModal component, there are four intents that can get passed in that determine the behavior:

    - pen::ui::FILE_INTENT::SAVE_AS
    - pen::ui::FILE_INTENT::SELECT_DIRECTORY
    - pen::ui::FILE_INTENT::SELECT_FILE
    - pen::ui::FILE_INTENT::SELECT_FILES

The pen::ui::Div component is used for aligning items together horizontally or vertically.  The div is horizontal by default 
and is convenient for quick item alignment.

The pen::ui::KeyBoard component is useful for mobile key input.  Two keys return special characters:

    - Back space: '<'
    - Caps: '^'

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

To run the animation manager after adding items do:

    pen::ui::Animation::Run();

This will run in OnRender do all animations for each item consecutively until they are all complete.

To animate sprites the same thing applies:

    pen::Anim::Add(item, type, ms, infinite, unitA, unitB, unitC);
    pen::Anim::Run();

The type is the transformation, an example is pen::AnimType::TRANSLATE.

---------------------------------------------------------------------------

# 1.7 - 3D

Before rendering 3D models using .obj files you have to call:

    pen::ui::Item* = pen::ui::AddItem3D(const uint32_t& id, const std::string& path, const pen::Vec4& objectColor, const bool& isInstanced, const std::vector<pen::Mat2x4*>& dataList, const bool& objectIsFixed, const bool& isWireFrame);
    pen::Pen::EnableDepthTesting(true);
    pen::Pen::HandleCameraInput(true);

If you enable depth testing, 2D textures will not work currently, so if you use 3D rendering you can't do 2D textures.

You can use left arrow to pan the camera left, right arrow to pan right, up arrow to pan up and down arrow to pan down.

The W key is used to pan the camera forward and the S key is used to pan the camera backwards.

3D items can be transformed with:

    - pen::_3d::Scale(pen::Item3D* item, float sx, float sy, float sz);
    - pen::_3d::Rotate(pen::Item3D* item, float theta, int axis);
    - pen::_3d::Translate(pen::Item3D* item, float tx, float ty, float tz);

For rotation, the axis can be chosen using:

    - pen::_3d::AXIS::X
    - pen::_3d::AXIS::Y
    - pen::_3d::AXIS::Z

If you want to use instancing for 3D items then initialize the data list.  There can be no more than 400 instances of a 3D item:

    int itemCount = 10;
    std::vector<pen::Vec3*> dataList;
    for (int i = 0; i < itemCount; i++) {
        /*Offset from first item position*/
        pen::Mat2x4* vec = new pen::Vec3(0.0f, 0.0f, 0.0f);       
        vec.x = 10.0f * i;
        vec.y = 10.0f * i;
        vec.z = 0.0f;
        dataList.push_back(vec);
    }

There is a bug where the textures of non-instanced items will not show if using instancing for 3D items.

---------------------------------------------------------------------------

# 1.8 - Cross Platform

I would like to thank the developers and contributors of Cocos2DX because I used their cross platform implementation
to help me as I worked on the Android implementation.  Cocos2DX is a well known open source game engine that has tons of support.
They have amazing documentation and readable code that helps people understand why they implement
the platforms the way they do.

When building for cross platform use Linux or Mac.

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
- pen::Pen::HandleGUIDragEvents
- pen::Pen::HandleGUIKeyEvents

---------------------------------------------------------------------------

# 1.8.1 - Cross Platform For Android

Currently the Windows build for android does not render any items.  Currently the Mac build for android does not render GUI items.

When using OnRender, you now have to pass your logic into a callback function as follows:

    pen::Pen::SetMobileCallbacks(&yourMobileRenderCallback, &yourMobileClickCallback, &yourMobileResumeCallback,
        &yourMobilePauseCallback, &yourMobileKeyCallback, &yourMobileTiltCallback, &yourMobileBluetoothCallback);

This function should be in the OnCreate function of a Pen Engine instantiation, for example:

    class App : public pen::Pen {
    public:
        void OnCreate() override {
            pen::Pen::SetMobileCallbacks(&yourMobileRenderCallback, &yourMobileClickCallback, &yourMobileResumeCallback,
                &yourMobilePauseCallback, &yourMobileKeyCallback, &yourMobileTiltCallback, &yourMobileBluetoothCallback);
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
pen_engine/ext/platforms/android/pen_engine_android/src/cpp/function_mapping.cpp.

There is a difference for how you would do the mobile render callback function for android though, notice instead of
a while loop for PC, you do an if statement:

    void yourAndroidRenderFunction(){
        if (pen::Pen::Running())
        {
            if(pen::Render::Get()->firstTime) pen::Render::Background(pen::PEN_GRAY);

            /*Your program logic*/

            pen::Pen::ManageRender();
        }
    }

Also input would have to be done on the native side for Android while implementing one of the mobile callback functions.

The PC function would remain the same with the while loop:

    void OnRender() override {
        while (pen::Pen::Running())
        {
            OnInput();
            if(pen::Render::Get()->firstTime) pen::Render::Background(pen::PEN_GRAY);

            /*Your program logic*/

            pen::Pen::ManageRender();
        }
    }

To get started building .apk files, if on Mac or Linux, first go to pen_engine/state/config.h and uncomment this line:

    //define __PEN_CMAKE__

Uncomment this line for Windows, Linux, and Mac:

    //#define __PEN_MOBILE__

If you uncomment this line, your application will not run for PC, so only do this once you have tested
on the PC version and are ready to build it for Android.

Now create a build directory inside the root directory of pen_engine, so it would be:

    pen_engine/build

Afterwards go to pen_engine/ext/platforms/android/local.properties and set your android
sdk path:

    Linux: sdk.dir=/home/user/Android/Sdk
    Mac: sdk.dir=/Users/user/Library/Android/sdk
    Windows: sdk.dir=C\:/Users/user/AppData/Local/Android/Sdk

Also set your ndk directory path like so, although this is claimed by Android to be deprecated in the future, this was necessary for ndk to find
its library stripping tool:

    Linux: ndk.dir=/home/user/Android/Sdk/ndk/20.0.5594570
    Mac: ndk.dir=/Users/user/Library/Android/sdk/ndk/20.0.5594570
    Windows: ndk.dir=C\:/Users/user/AppData/Local/Android/Sdk/ndk/20.0.5594570

For getting this specific version of the ndk you have to install Android Studio, open pen_engine/ext/platforms/android as a project, go to Android Studio preferences -> Appearance & Behavior -> System Settings -> Android SDK -> SDK Tools and check the version number under NDK.

If on Windows, use power shell and cd into the build core directory of the ndk:

    cd C:/Users/user/AppData/Local/Android/Sdk/ndk/20.0.5594570/build/core

Then use note pad to edit the build-binary.mk file there:

    notepad build-binary.mk

Inside this file, go to line 713 and remove this:

    $(call generate-list-file,$(linker_options),$(linker_list_file))

Update it to this:

    $(call generate-list-file,$(call escape-backslashes,$(linker_options)),$(linker_list_file))

Also set your java jdk path in the gradle.properties file:

    Linux:  org.gradle.java.home=/usr/lib/jvm/java-11-openjdk-amd64
    Mac:  org.gradle.java.home=/Library/Java/JavaVirtualMachines/jdk-18.jdk/Contents/Home
    Windows: org.gradle.java.home=C\:/Program Files/Java/jdk

You are going to need the JAVA_HOME environment variable for Linux set as well, after you download openjdk call:

    export JAVA_HOME=/usr/lib/jvm/your jdk
    export PATH=$PATH:$JAVA_HOME/bin

Once that is done install Android Studio, also install these packages for Linux:

    - ninja-build (for using the ninja generator with cmake which is what gradle uses by default for cmake builds)
    - gcc-multilib (for compiling)
    - libncurses5 (for the Android Studio debugger)
    
For Mac it would be:

    - brew install ninja

Make sure you have an internet connection because this will download any files needed for the build.
This will build the apk files into a build/outputs/apk directory inside of the android directory.

For Linux and Mac, be sure to uncomment the cmake blocks in pen_engine/ext/platforms/android/pen_engine_android/build.gradle.  If
on Windows then comment the cmake blocks out:

    android {
        ...
        defaultConfig {
            ...
            externalNativeBuild {
                 /*cmake {
                    arguments "-DCMAKE_SYSTEM_NAME=ANDROID", "-DCMAKE_ANDROID_API=28",
                            "-DANDROID_ABI=x86_64",
                            "-DCMAKE_ANDROID_NDK=/Users/user/Library/Android/sdk/ndk/20.0.5594570",
                            "-DCMAKE_ANDROID_STL_TYPE=gnustl_static"
                }*/
            }
        }

        ...
        externalNativeBuild {
            /*Linux & Mac*/
            /*cmake {
                path "../../../../CMakeLists.txt"
                version "3.16.3"
            }*/
        }
    }

If on Windows ndk-build gets used instead of cmake.  By default the source file location will be set one directory
outside of pen_engine, but it can be changed in pen_engine/ext/platforms/android/pen_engine_android/jni/Android.mk

If on Windows the APP_PROJECT_PATH variable has to be updated in
pen_engine/ext/platforms/android/pen_engine_android/jni/Application.mk:

    #Replace path_to_pen_engine with root to pen_engine
    APP_PROJECT_PATH := path_to_pen_engine/pen_engine/ext/platforms/android

If your main .cpp file is different from app.cpp, update PC_INT_MAIN in
pen_engine/ext/platforms/android/pen_engine_android/jni/Android.mk:

    PC_INT_MAIN := app.cpp

For Windows instead of building through gradle, ndk-build gets called in the command line to build the .so files:

    - C:/Users/user/AppData/local/Android/Sdk/ndk/20.0.5594570/ndk-build NDK_PROJECT_PATH=C:/Users/user/AppData/local/Android/Sdk/ndk/20.0.5594570 \
        NDK_APPLICATION_MK=path_to_pen_engine/pen_engine/ext/platforms/android/pen_engine_android/jni/Application.mk APP_BUILD_SCRIPT=path_to_pen_engine/pen_engine/ext/platforms/android/pen_engine_android/jni/Android.mk

Once the libraries are built, copy the respective libs into the jniLibs folder:

    - arm64-v8a: cp C:/Users/user/AppData/Local/Android/Sdk/ndk/20.0.5594570/libs/arm64-v8a/libpen_engine_android.so path_to_pen_engine/pen_engine/ext/platforms/android/pen_engine_android/src/main/jniLibs/arm64-v8a
    - armeabi-v7a: cp C:/Users/user/AppData/Local/Android/Sdk/ndk/20.0.5594570/libs/armeabi-v7a/libpen_engine_android.so path_to_pen_engine/pen_engine/ext/platforms/android/pen_engine_android/src/main/jniLibs/armeabi-v7a
    - x86: cp C:/Users/user/AppData/Local/Android/Sdk/ndk/20.0.5594570/libs/x86/libpen_engine_android.so path_to_pen_engine/pen_engine/ext/platforms/android/pen_engine_android/src/main/jniLibs/x86
    - x86_64: cp C:/Users/user/AppData/Local/Android/Sdk/ndk/20.0.5594570/libs/x86_64/libpen_engine_android.so path_to_pen_engine/pen_engine/ext/platforms/android/pen_engine_android/src/main/jniLibs/x86_64

For signed apks in pen_engine/ext/platforms/android/pen_engine_android/build.gradle, you will have to uncomment the signingConfigs block and add your key store 
file and information here:

    android {
        ...
        signingConfigs {
            release {
                storeFile file('path_relative_to_pen_engine/ext/platforms/android/pen_engine_android')
                storePassword "yourpassword"
                keyAlias "youralias"
                keyPassword "yourkeypass"
            }
        }
    }

These are the events that are not implemented in this version yet:

- Touch and hold events.

---------------------------------------------------------------------------

# 1.8.1.1 - Android Persisted Memory

If you want to perform manipulation of persisted memory for your android application you can call the following:

    /*Needs to be called before any database operation*/
    - pen::android::db::Start(const char* dbName, const char* tableName);

    /*Closes the database connection once you are done*/
    - pen::android::db::Close();

    /*Adds or updates a key value in the database*/
    - pen::android::db::SetItem(const char* key, const char* value);

    /*Retrieves a key value from the database as a string*/
    - pen::android::db::GetItem(const char* key);

    /*Removes a key from the database*/
    - pen::android::db::RemoveItemById(const char* key);

    /*Deletes the table from the database*/
    - pen::android::db::DeleteTable();

---------------------------------------------------------------------------

# 1.8.1.2 - Native Key Events For Android

If you want to do native key events for android, first go 
to pen_engine/ext/platforms/android/pen_engine_android/src/java/com/jamar/penengine/PenSurfaceRenderer.java.
Once you do that, create a method called nativeOnKeyPress:
    
    private static native String nativeOnKeyPress(final int keyCode,boolean isPressed);

Use this function to return a string of the character back to the native library for the callback
pen::State::mobileOnKeyCallback to act on it.

Another method, which would be easier, would be to do the edit text input completely on the native side
and save some data about it in SQL.  Then use the SQL value to change the main activity in which
the graphics application runs when it is reloaded.

---------------------------------------------------------------------------

# 1.8.1.3 - Android Logging

If you want to send messages to Java for logging use:

    pen::android::AppLog(const char* yourMessage);

This can be used for Android debugging once your application has been built and tested on PC.

---------------------------------------------------------------------------

# 1.8.1.4 - Android Loading

You can load in assets such as level packs by doing:

    pen::Asset::Load(std::string file, nullptr);

The full name should be passed in including any sub directory inside the assets directory.
For example the file could be levels/level1.txt, or it could be music1.mp3, or sounds/enemy1/sound1.wav.

You can also load in a directory in assets by doing:

    pen::Asset::LoadMobileDir(std::string dirPath);

---------------------------------------------------------------------------

# 1.8.1.5 - Android Bluetooth

If using bluetooth the permissions in pen_engine/ext/platforms/android/pen_engine_android/src/AndroidManifest.xml should be uncommented.
For connecting to other devices via bluetooth, first search for available devices:

    pen::android::conn::bt::Search(const char* deviceName);

The device name does not need to be known before searching, it will just connect via the first available device.

Once a connection is established captured by the bluetoothCallback, data can be read by doing:

    bluetoothCallback(){
        int numBytes = 0;
        char* data =  pen::android::conn::bt::Read(&numBytes);
    }

Data can also be written by doing:

    bluetoothCallback(){
         pen::android::conn::bt::Write(char* buffer, int numBytes);
    }

Once done with a connected device, the connection can be closed by doing:

    bluetoothCallback(){
         pen::android::conn::bt::Close();
    }

---------------------------------------------------------------------------

# 1.8.1.6 - Android HTTP

For sending http get and post requests do:

    - Get request: pen::Map<std::string,std::string> response = pen::android::conn::http::Get(const char* route, 
        pen::Map<std::string, std::string> parameterMap, pen::Map<std::string, std::string> headerMap);
    - Post request: pen::Map<std::string,std::string> response = pen::android::conn::http::Get(const char* route, 
        const char* jsonStr, pen::Map<std::string, std::string> headerMap);

In order to parse JSON objects as string look at section 2.0.

---------------------------------------------------------------------------

# 1.8.1.6 - Android Sockets

For using socket connections first connect to the server:

    pen::android::conn::socket::Connect(const char* route, int port);

This client connection can only be connected to one server at a time.
Once connected, you can send messages by doing:

    std::string response = pen::android::conn::socket::Send(const char* message);

To close a connection do:

    pen::android::conn::Close();

---------------------------------------------------------------------------

# 1.8.1.6 - Android Sound

In order to play sounds, the files have to be in the res/raw folder.
To play a sound do:

    Pen::android::sound::Play(std::string file, bool isMusic = false);

---------------------------------------------------------------------------

# 1.9 - AI

For documentation on implementing AI go to ext/ai/README.md

---------------------------------------------------------------------------

# 2.0 - JSON

For JSON objects, there are two kinds of items:

    - pen::JSON
    - pen::JSON::Array

The main item is always going to be pen::JSON object that encapsulates different fields and values that are all strings.
Each JSON object will have a list of key-value pairs that have the following:

    - std::string data;
	- char type;
	- int numBytes;

The type of each field can be the following:

    - a: array, array of arrays
	- b: bool, bool array
	- c: char, char array
	- f: float, float array
	- i: int, int array
	- j: sub json object, json object array
	- s: string, string array

These types can be used for the other kind of item as well.
For instance, if the field you are retrieving is an int array then do:

    if(type == 'i'){
        pen::JSON mainObject = pen::JSON(std::string jsonDataString);
        pen::JSON::Field arrField = mainObject.json.Find("arr")->second;
        std::vector<pen::JSON::Array> jsonArray = pen::JSON::ParseArrays(arrField.value);
        /*Parses the first int array from the array of arrays*/
        std::vector<int> intList = jsonArray[0].GetList<int>();
    }

For GetList with string specifically, char* has to be passed in like so:

    std::vector<std::string> stringList = jsonArray[0].GetList<char*>();

If it is a simple key-value pair then do:

   pen::JSON mainObject = pen::JSON(std::string jsonDataString);
   pen::JSON::Field cityField = mainObject.json.Find("city")->second;
   
If the city field is a string field, then this pen::JSON::Field object will be for example:

    - std::string data: detroit
    - char type: 's'
    - int numBytes: 7

Data can be retrieved from pen::JSON::Field items similarly:

    pen::JSON mainObject = pen::JSON(std::string jsonDataString);
    pen::JSON::Field highScoreField = mainObject.json.Find("highScore")->second;
    float highScore = highScoreField::Get<float>();

Also for Get similar to GetList with string specifically, char* has to be passed in like so:

    std::string str = highScoreField.Get<char*>();

If you have a pen::Map of strings that need parsing do:

    pen::JSON json = pen::JSON(pen::Map<std::string,std::string> jsonMap);

If a pen::JSON object needs to be converted into a string do:

    std::string jsonStr = json.ToString();

---------------------------------------------------------------------------