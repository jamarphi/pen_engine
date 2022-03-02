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

bool changeColor(pen::ui::Item* item, int button, int action) {
    if (button == pen::in::KEYS::MOUSE_LEFT && action == pen::in::KEYS::PRESSED) {
        (item->color.x == pen::PEN_BLUE.x) ? item->SetColor(pen::PEN_RED) : item->SetColor(pen::PEN_BLUE);
        pen::ui::Submit();
    }
    return true;
}

class App : public pen::Pen {
public:
    double x = 0.0, y = 0.0;
    App() {

    }

    void InitializeObjects() {
        pen::Vec3 positions = pen::Vec3(300.0f, 200.0f, 0.0f);
        pen::Vec2 size = pen::Vec2((pen::Pen::ScreenWidth() / 4.0f * 3.0f) - (pen::Pen::ScreenWidth() / 4.0f), (pen::Pen::ScreenHeight() / 4.0f * 3.0f) - (pen::Pen::ScreenHeight() / 4.0f));

        /*Nav bar*/
        pen::ui::Item* navBar = pen::ui::AddItem(new pen::ui::NavBar(0, 20.0f, pen::PEN_LIGHT_GRAY));
        navBar->Push(new pen::ui::Button(ID_ANY, "File", pen::Vec3(0.0f, pen::Pen::ScreenHeight() - 40.0f, 0.0f), 4, pen::PEN_GRAY, pen::PEN_BLACK, navBar, nullptr, true));
        /*Nav bar*/

        /*Vertical list*/
        pen::ui::Item* vertList = pen::ui::AddItem(new pen::ui::VerticalList(0, pen::Vec3(10.0f, 100.0f, 0.0f),
            size, pen::PEN_LIGHT_GRAY, pen::PEN_GRAY, nullptr, nullptr, "Context"));
        vertList->Push(new pen::ui::Button(ID_ANY, "Option 1", pen::Vec3(0.0f, pen::Pen::ScreenHeight() - 40.0f, 0.0f), 9, pen::PEN_TRANSPARENT, pen::PEN_GRAY, vertList, &changeColor, true));
        vertList->Push(new pen::ui::Button(ID_ANY, "Option 2", pen::Vec3(0.0f, pen::Pen::ScreenHeight() - 40.0f, 0.0f), 9, pen::PEN_TRANSPARENT, pen::PEN_GRAY, vertList, &changeColor, true));
        vertList->Push(new pen::ui::Button(ID_ANY, "Option 3", pen::Vec3(0.0f, pen::Pen::ScreenHeight() - 40.0f, 0.0f), 9, pen::PEN_TRANSPARENT, pen::PEN_GRAY, vertList, &changeColor, true));
        vertList->Push(new pen::ui::Button(ID_ANY, "Option 4", pen::Vec3(0.0f, pen::Pen::ScreenHeight() - 40.0f, 0.0f), 9, pen::PEN_TRANSPARENT, pen::PEN_GRAY, vertList, &changeColor, true));
        vertList->Push(new pen::ui::Button(ID_ANY, "Option 5", pen::Vec3(0.0f, pen::Pen::ScreenHeight() - 40.0f, 0.0f), 9, pen::PEN_TRANSPARENT, pen::PEN_GRAY, vertList, &changeColor, true));
        vertList->Push(new pen::ui::Button(ID_ANY, "Option 6", pen::Vec3(0.0f, pen::Pen::ScreenHeight() - 40.0f, 0.0f), 9, pen::PEN_TRANSPARENT, pen::PEN_GRAY, vertList, &changeColor, true));
        vertList->Push(new pen::ui::Button(ID_ANY, "Option 7", pen::Vec3(0.0f, pen::Pen::ScreenHeight() - 40.0f, 0.0f), 9, pen::PEN_TRANSPARENT, pen::PEN_GRAY, vertList, &changeColor, true));
        vertList->Push(new pen::ui::Button(ID_ANY, "Option 8", pen::Vec3(0.0f, pen::Pen::ScreenHeight() - 40.0f, 0.0f), 9, pen::PEN_TRANSPARENT, pen::PEN_GRAY, vertList, &changeColor, true));
        vertList->Push(new pen::ui::Button(ID_ANY, "Option 9", pen::Vec3(0.0f, pen::Pen::ScreenHeight() - 40.0f, 0.0f), 9, pen::PEN_TRANSPARENT, pen::PEN_GRAY, vertList, &changeColor, true));
        vertList->Push(new pen::ui::Button(ID_ANY, "Option 10", pen::Vec3(0.0f, pen::Pen::ScreenHeight() - 40.0f, 0.0f), 9, pen::PEN_TRANSPARENT, pen::PEN_GRAY, vertList, &changeColor, true));
        vertList->Push(new pen::ui::Button(ID_ANY, "Option 11", pen::Vec3(0.0f, pen::Pen::ScreenHeight() - 40.0f, 0.0f), 9, pen::PEN_TRANSPARENT, pen::PEN_GRAY, vertList, &changeColor, true));
        vertList->Push(new pen::ui::Button(ID_ANY, "Option 12", pen::Vec3(0.0f, pen::Pen::ScreenHeight() - 40.0f, 0.0f), 9, pen::PEN_TRANSPARENT, pen::PEN_GRAY, vertList, &changeColor, true));
        vertList->Push(new pen::ui::Button(ID_ANY, "Option 13", pen::Vec3(0.0f, pen::Pen::ScreenHeight() - 40.0f, 0.0f), 9, pen::PEN_TRANSPARENT, pen::PEN_GRAY, vertList, &changeColor, true));
        /*Vertical list*/

        /*Slider*/
        pen::ui::Item* slider = pen::ui::AddItem(new pen::ui::Slider(ID_ANY, 0, pen::Vec3(200.0f, 45.0f, 0.0f), pen::Vec2(200.0f, 50.0f),
            pen::PEN_RED, pen::PEN_GRAY, nullptr, nullptr));
        /*Slider*/

        /*Text editor*/
        pen::ui::Item* textEditor = pen::ui::AddItem(new pen::ui::TextEditor(ID_ANY, pen::Vec3(500.0f, 100.0f, 0.0f),
            pen::Vec2(450.0f, 350.0f), pen::PEN_BLACK, pen::PEN_ORANGE, nullptr, nullptr, nullptr, "C++ is the best"));
        /*Text editor*/

        /*Context window*/
        pen::ui::Item* contextWindow = pen::ui::AddItem(new pen::ui::ContextWindow(ID_ANY, pen::Vec3(10.0f, 400.0f, 0.0f),
            pen::Vec2(300.0f, 100.0f), pen::PEN_BLACK, pen::PEN_ORANGE, nullptr, nullptr, "Context"));
        /*Context window*/

        pen::ui::Submit();
        pen::Pen::HandleGUIClickEvents(true, nullptr);
        pen::Pen::HandleGUIDragEvents(true);
        pen::Pen::HandleGUIKeyEvents(true);
    }

    void OnCreate() override {
        InitializeObjects();
    }

    void ShowMousePos() {
        pen::Pen::GetMousePos(&x, &y);
        std::string pos = std::to_string(x) + ", " + std::to_string(y);
        pen::ui::FindItem(0)->UpdateText(pos);
        pen::ui::Submit();
    }

    void OnRender() override {
        pen::Renderer renderer;

        while (pen::Pen::Running())
        {
            OnInput();
            renderer.Clear();
            if (pen::Render::Get()->firstTime) renderer.Background(pen::PEN_CYAN);

            //ShowMousePos();

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