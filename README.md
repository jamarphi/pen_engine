# PenEngine

![Pen Engine Logo](https://github.com/jamarphi/pen_engine/blob/master/res/textures/pen_engine_logo.png?raw=true)

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
    
For Mac builds check out section 1.8.2 in ext/platforms/mac_ios/README.md.

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
Windows and Linux.  There are 8 available texture slots for Mac.  The first three slots are used for solid 
colors, bitmap font rendering, and texel drawing, so that means there are 29 available texture slots left 
in this scenario for Windows and Linux, 8 available texture slots for Mac.  There are 8 slots available
for Android and IOS as well.

For Mac and IOS there is a limit of 32 layers that can be rendered to the screen.  Each layer consists of 10,000 vertices.
For most use cases there will be usually one layer for the GUI, one layer for the pixel buffer, and the other 30 layers
for non-fixed items such as 3D sprites.

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

You can also pass in a boolean of false to the mask parameter to draw transparency to the buffer if you want,
by default it does not draw pixels with an alpha of 0.0f so it does not overwrite what is already there with transparency.

If you want to access the pixel buffer directly you can do:

    unsigned char* pixels = pen::PixelBuffer();   

The pixel buffer will return a nullptr if accessed directly for mobile builds, this is because the pixel buffer happens on the mobile side.

pen::PixelBufferWidth() and pen::PixelBufferHeight() are used since the texels on the screen are in a different space, this way all of the pixels can be
covered properly.

If you want to convert between screen and pixel buffer coordinates you can do:

    - pen::ScreenToPixel(float* x, float* y);
    - pen::PixelToScreen(float* x, float* y);

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

Click can be used to rotate the camera around on Windows and Linux, the space bar is used for Mac.

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

- Windows
- Linux
- Mac
- Android
- IOS

Functions that are not compatible with mobile:

- pen::Pen::GetWindow
- pen::Pen::CloseWindow
- pen::Pen::MakeMouseHidden
- pen::Pen::MakeMouseShow
- pen::Pen::WindowActive
- pen::Pen::MouseState
- pen::Pen::KeyState

These two work for __PEN_MAC_IOS__ if building for Mac:
- pen::Pen::HandleGUIDragEvents
- pen::Pen::HandleGUIKeyEvents

---------------------------------------------------------------------------

# 1.8.1 - Cross Platform For Android

 For documentation on implementing Android go to ext/platforms/android/README.md.
 
 ---------------------------------------------------------------------------

# 1.8.2 - Cross Platform For Mac And IOS

 For documentation on implementing Mac and IOS go to ext/platforms/mac_ios/README.md.
 
---------------------------------------------------------------------------

# 1.9 - AI

For documentation on implementing AI go to ext/ai/README.md.

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
