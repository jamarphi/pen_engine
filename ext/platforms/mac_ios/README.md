# 1.8.2 - Cross Platform For Mac And IOS

When building for Mac and IOS all GUI points and lines will be filled as triangles.

When using OnRender, you now have to pass your logic into a callback function as follows:

    pen::Pen::SetMobileCallbacks(&yourMobileRenderCallback, &yourMobileClickCallback, &yourMobileResumeCallback,
        &yourMobilePauseCallback, &yourMobileKeyCallback, &yourMobileTiltCallback, &yourAndroidMobileBluetoothCallback, &yourMacIosMobileBluetoothCallback);

This function should be in the OnCreate function of a Pen Engine instantiation, for example:

    class App : public pen::Pen {
    public:
        void OnCreate() override {
            pen::Pen::SetMobileCallbacks(&yourMobileRenderCallback, &yourMobileClickCallback, &yourMobileResumeCallback,
                &yourMobilePauseCallback, &yourMobileKeyCallback, &yourMobileTiltCallback, &yourAndroidMobileBluetoothCallback, &yourMacIosMobileBluetoothCallback
                &yourMobileHttpCallback, &yourMobileSocketCallback);
            pen::Pen::SetMobileTextures(pathListVector);

            /*All logic in OnCreate should come after the two previous functions*/
        }
    }

pen::Pen::SetMobileTextures is only used for gui sprite sheets, if loading anything else do:

    pen::Asset asset = pen::Asset::Load(std::string file, char*(*onLoad)(const char* path, long* fileLength) = nullptr);

Check out section 1.1.1 for more on managing assets.
If specifically loading in images to be rendered then you can take the asset data and add it to the sprite map by doing:

    pen::State::Get()->pixelSprites.Insert(asset.name, pen::Sprite{asset.name, (unsigned char*)asset.data, 0, 0});
    
Pen Engine looks for this instantiation in app.h, but you can change the name of that file include in 
pen_engine/ext/platforms/mac_ios/mac_ios_view_delegate.h.

There is a difference for how you would do the mobile render callback function for Mac and IOS though, notice instead of
a while loop for PC, you do an if statement:

    void yourMacIOSRenderFunction(){
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

To get started building for Mac and IOS, first go to pen_engine/state/config.h and uncomment this line:

    //#define __PEN_MAC_IOS__

If you uncomment this line, your application will not run for PC, so only do this once you have tested
on the PC version and are ready to build it for Mac and IOS.

For using Xcode, first go and create two targets inside of your application if you have not done so already,
one for Mac and one for IOS.  Also remove any folders associated with these two targets.

Next for both targets go to Build Phases -> Link Binary With Libraries -> and clear out any entries, then add 
the following libraries:

- Foundation
- Metal
- QuartzCore
- MetalKit
- CoreBluetooth
- AVFAudio
- libsqlite3.tbd

For IOS:

- UIKit
- CoreMotion

For Mac:

- AppKit

After that, right click on your project in the project explorer and add pen_engine to both targets,
make sure that copy files as needed and create groups are both selected,
this may duplicate pen_engine so remove the first one and rename the linked one to pen_engine, 
this should automatically populate Build Phases -> Compile Sources for both targets.

In the Mac build target remove the ios directory mac_ios_view_controller.mm,
and remove the mac directory one from the ios target.

In Build Phases -> Copy Bundle Resources for both targets remove everything except the
bitmap.png in pen_engine/res/textures/fonts and the respective storyboards for each target.

In Build Settings -> Packaging, set the Info.plist File entry for the mac target to
$(PROJECT_DIR)/your project folder/pen_engine/ext/platforms/mac_ios/mac/Info.plist,
and the ios target entry to $(PROJECT_DIR)/your project folder/pen_engine/ext/platforms/mac_ios/ios/Info.plist

In Build Settings -> C++ Language Dialect, make sure it is C++17.

---------------------------------------------------------------------------

# 1.8.2.1 - Mac And IOS Instancing

For instancing pen::Pen::SetInstancedOffsets(std::vector<pen::Vec3*>* offsets) is used to update the
global instance offsets.

---------------------------------------------------------------------------

# 1.8.2.2 - Mac And IOS Bluetooth

# Bluetooth As A Central

If using bluetooth you have to add a description for the bluetooth permission in
your Info.plist file.

For connecting to other devices via bluetooth, first search for available devices:

    pen::ios::conn::bt::Scan(std::vector<std::string> devices = {});

The devices' names do not need to be known before searching, it will just find all of the available devices.

While scanning, potential connections can be constantly queried with:

    std::vector<std::string> devices = pen::ios::conn::bt::QueryPotentialConnections();
    
Once you have a desired device that you want to connect with, you connect by doing:

    pen::ios::conn::bt::Connect(const char* device, const char* descriptor);

This descriptor is the same name as the characteristic associated with the service you want to interact
with for the given device, this should be known beforehand when connecting.

You can connect with multiple devices at once.

Once a connection is established a subscription is made to automatically read data from the device
within the bluetoothCallback, data can be read by doing:

    void macIosBluetoothCallback(char* bytes, long length, unsigned int type){
        /*Handle the value from the characteristic*/
        if(type == pen::ios::conn::bt::TYPE::CENTRAL){
        
        }
    }
    
You can also read data manually by doing with the callback being the same:

    pen::ios::conn::bt::Read(const char* device);

    void macIosBluetoothCallback(char* bytes, long length, unsigned int type){
        /*Handle the value from the characteristic*/
        if(type == pen::ios::conn::bt::TYPE::CENTRAL){
        
        }
    }

Data can written to the current characteristic for all connected devices by doing:

    pen::ios::conn::bt::Write(char* data, long length);

Once done with a connected device, the connection can be closed by doing:

    pen::ios::conn::bt::Disconnect(const char* device);
    
# Bluetooth As A Peripheral

When using bluetooth as a peripheral device you initiate bluetooth by doing:

    pen::ios::conn::bt::rec::AddService(const char* service, const char* characteristic, char* value, long length, unsigned int type);

The types can be defined as the following:

    - pen::ios::conn::bt::TYPE::READ for a readable service characteristic
    - pen::ios::conn::bt::TYPE::READ for a writable service characteristic
    
Now your service will be advertised to any centrals that are available to connect.

If a central subscribes to your service, then you can update the value of the characteristic by doing:

    void macIosBluetoothCallback(char* bytes, long length, unsigned int type){
        /*Update the value of the characteristic*/
        if(type == pen::ios::conn::bt::TYPE::PERIPHERAL){
            /*
            The bytes variable is now a string that has the name of the service and characteristic
            separated by a colon
            */
            std::string serviceCharacteristicPair(bytes);
            std::string serviceName = serviceCharacteristicPair.substr(0, serviceCharacteristicPair.find(":"));
            std::string characteristicName = serviceCharacteristicPair.substr(serviceCharacteristicPair.find(":") + 1);
            
            /*The length is used to update the value with the correct number of bytes*/
            char* data = new char[length];
            
            /*Set the data*/

            pen::ios::conn::bt::rec::UpdateCharacteristic(serviceName.c_str(), characteristicName.c_str(), data);
            delete[] data;
        }
        
    }

---------------------------------------------------------------------------

# 1.8.2.3 - Mac And IOS HTTP

To do an http requests do the following:

    pen::ios::conn::http::Send(const std::string& url, unsigned int type, pen::Map<std::string,std::string>* httpBody = nullptr);
    
The types are defined as the following:

    - pen::ios::conn::http::TYPE::GET
    - pen::ios::conn::http::TYPE::POST
    
If expecting anything back from the request do:

    void your httpCallback(pen::Map<std::string,std::string> response){
        /*Handle the response*/
    }
}

---------------------------------------------------------------------------

# 1.8.2.4 - Mac And IOS Sockets

To do a socket request to a server first connect:

    pen::ios::conn::socket::Connect(const char* url);
    
Once connected you can be notified by doing:

    void your socketCallback(char* data, unsigned int messageType){
        /*Perform send or receive requests*/
        if(messageType == pen::ios::conn::socket::TYPE::CLOSED_WITH_ERROR){
            /*An error has occurred during the connection with the server
        }
    }

The message types are defined as the following:

    - pen::ios::conn::socket::TYPE::CONNECTED after the device has connected a server
    - pen::ios::conn::socket::TYPE::DATA_RECEIVED after the device has received data from a server
    - pen::ios::conn::socket::TYPE::CLOSED_WITH_ERROR if an error has occurred during a connection
    
When sending data to a server do:

    pen::ios::conn::socket::Send(char* data, long length);

When receiving data from a server do:

    pen::ios::conn::socket::Receive();
        
Once the data is received you can be notified by doing:

    void your socketCallback(char* data, unsigned int messageType){
        /*Handle received data*/
        if(messageType == pen::ios::conn::socket::TYPE::CLOSED_WITH_ERROR){
            /*An error has occurred during the connection with the server
        }else{
        
        }
    }

---------------------------------------------------------------------------

# 1.8.2.5 - Mac And IOS Database

If you want to use persisted memory for your mac and ios applications you can call the following:

    /*Needs to be called before any database operation*/
    - pen::ios::db::Start(const char* database, const char* table);
    
    /*Creates a table if it does not exist already in the database*/
    - pen::ios::db::CreateTable(const char* table);

    /*Adds or updates a key value in the database*/
    - pen::ios::db::Insert(const char* table, const char* key, const char* value);

    /*Retrieves a key value from the database as a string*/
    - pen::ios::db::Get(const char* table, const char* key);

    /*Removes a key from the database*/
    - pen::ios::db::Delete(const char* table, const char* key);

    /*Deletes a table from the database*/
    - pen::ios::db::Drop(const char* table);

---------------------------------------------------------------------------

# 1.8.1.6 - Android Loading

You can load in assets such as level packs by doing:

    pen::Asset::Load(std::string file, nullptr);

---------------------------------------------------------------------------

# 1.8.2.7 - Mac And IOS Sound

To play sounds and music do:

    - pen::ios::sound::Play(const char* file, unsigned int loopNumber = 0);
    
    - pen::ios::sound::Pause(const char* file);
    
    - pen::ios::sound::Stop(const char* file);
    
    - pen::ios::sound::Remove(const char* file);

---------------------------------------------------------------------------

# 1.8.2.8 - IOS Accelerometer

For tilting events you can access them by doing:

    void tiltCallback(double acelX, double acelY, double acelZ){
         /*Handle the accelerometer update*/
    }

---------------------------------------------------------------------------
