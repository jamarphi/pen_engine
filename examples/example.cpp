/*************************************************************************************************
 Copyright 2021 Jamar Phillip

Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*************************************************************************************************/
#include "pen_engine/src/pen_engine.h"

const unsigned int SCR_WIDTH = 960;
const unsigned int SCR_HEIGHT = 540;

class App : public pen::Pen {
public:
    App() {

    }

    void InitializeObjects() {
        pen::Vec3 positions = pen::Vec3(100.0f, 100.0f, 0.0f);
        pen::Vec2 size = pen::Vec2((pen::Pen::ScreenWidth() / 4.0f * 3.0f) - (pen::Pen::ScreenWidth() / 4.0f), (pen::Pen::ScreenHeight() / 4.0f * 3.0f) - (pen::Pen::ScreenHeight() / 4.0f));
        pen::ui::AddItem(new pen::ui::TextBox(0, "C++", positions, size.x, pen::PEN_RED));
        //pen::ui::AddItem(new pen::ui::Item(0, positions, size, pen::ui::Shape::QUAD, pen::PEN_WHITE));
    }

    void OnCreate() override {
        InitializeObjects();
    }

    void OnRender() override {
        /*
        When manually updating properties of your items be sure to call pen::ui::Submit when you're done.
        This is to recombine all of the batch data for rendering.
        Ex:
        pen::Vec3 pos = object->GetPosition();
        pos.x += 0.01f;
        object->SetPosition(pos);
        pen::ui::Submit();
        */
        pen::Renderer renderer;
        
        while (pen::Pen::Running())
        {
            OnInput();
            renderer.Clear();
            if (pen::Render::Get()->firstTime) renderer.Background(pen::PEN_GRAY);

            pen::Pen::ManageRender(&renderer);
        }
    }

    void OnInput() override {
        
    }
};



//int main()
//{
//    App app = App();
//    app.CreateApplication("Demo", SCR_WIDTH, SCR_HEIGHT, "/home/user/dev/project");
//    app.OnCreate();
//    app.OnRender();
//    app.End();
//
//    return 0;
//}