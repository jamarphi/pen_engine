# 1.8.1 - Cross Platform For Android

Currently the Windows build for android does not render any items.  Currently the Mac build for android does not render GUI items.

When using OnRender, you now have to pass your logic into a callback function as follows:

    pen::Pen::SetMobileCallbacks(&yourMobileRenderCallback, &yourMobileClickCallback, &yourMobileResumeCallback,
        &yourMobilePauseCallback, &yourMobileKeyCallback, &yourMobileTiltCallback, &yourAndroidMobileBluetoothCallback, &yourMacIosMobileBluetoothCallback);

This function should be in the OnCreate function of a Pen Engine instantiation, for example:

    class App : public pen::Pen {
    public:
        void OnCreate() override {
            pen::Pen::SetMobileCallbacks(&yourMobileRenderCallback, &yourMobileClickCallback, &yourMobileResumeCallback,
                &yourMobilePauseCallback, &yourMobileKeyCallback, &yourMobileTiltCallback, &yourAndroidMobileBluetoothCallback, &yourMacIosMobileBluetoothCallback
                &yourMobileHttpCallback);
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
pen_engine/ext/platforms/android/pen_engine_android/src/cpp/android_manager.cpp.

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

Uncomment this line:

    //#define __PEN_ANDROID__

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

# 1.8.1.2 - Android Logging

If you want to send messages to Java for logging use:

    pen::android::AppLog(const char* yourMessage);

This can be used for Android debugging once your application has been built and tested on PC.

---------------------------------------------------------------------------

# 1.8.1.3 - Android Loading

You can load in assets such as level packs by doing:

    pen::Asset::Load(std::string file, nullptr);

The full name should be passed in including any sub directory inside the assets directory.
For example the file could be levels/level1.txt, or it could be music1.mp3, or sounds/enemy1/sound1.wav.

You can also load in a directory in assets by doing:

    pen::Asset::LoadMobileDir(std::string dirPath);

---------------------------------------------------------------------------

# 1.8.1.4 - Android Bluetooth

If using bluetooth the permissions in pen_engine/ext/platforms/android/pen_engine_android/src/AndroidManifest.xml should be uncommented.
For connecting to other devices via bluetooth, first search for available devices:

    pen::android::conn::bt::Search(const char* deviceName);

The device name does not need to be known before searching, it will just connect via the first available device.

Once a connection is established captured by the bluetoothCallback, data can be read by doing:

    void androidBluetoothCallback(){
        int numBytes = 0;
        char* data =  pen::android::conn::bt::Read(&numBytes);
    }

Data can also be written by doing:

    void androidBluetoothCallback(){
         pen::android::conn::bt::Write(char* buffer, int numBytes);
    }

Once done with a connected device, the connection can be closed by doing:

    void androidBluetoothCallback(){
         pen::android::conn::bt::Close();
    }

---------------------------------------------------------------------------

# 1.8.1.5 - Android HTTP

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

# 1.8.1.7 - Android Sound

In order to play sounds, the files have to be in the res/raw folder.
To play a sound do:

    Pen::android::sound::Play(std::string file, bool isMusic = false);

---------------------------------------------------------------------------
