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

unsigned int waterBodyHeight = 0;
unsigned int waveHeight = 30;
float waveOffset = 0.0f;
float angularVelocity = -200.0f;
pen::Vec4 waveColor = pen::PEN_BLUE;

class WaveAlgorithm : public pen::Pen {
public:
    WaveAlgorithm() {

    }

    void OnCreate() override {
        pen::InitializePixels();

        waterBodyHeight = pen::PixelBufferHeight() / 2;
    }

    void Draw() {
        /*Clear the previous draw*/
        pen::Flush();

        /*Draw the water body height*/
        for (int i = 0; i < waterBodyHeight; i++) {
            for (int j = 0; j < pen::PixelBufferWidth(); j++) {
                pen::Draw(j, i, waveColor);
            }
        }


        /*Draw the sine wave*/
        float waveTop = waterBodyHeight + waveHeight;
        for (int i = waterBodyHeight; i < waveTop; i++) {
            for (int j = 0; j < pen::PixelBufferWidth(); j++) {
                float jConverted = j * 3.14159f / 100.0f;

                float output = std::sin(jConverted + waveOffset) / (jConverted < 1.0f ? jConverted + (1.0f - jConverted) : jConverted);
                float outputConverted = output * waveHeight / 1.0f;
                float outputYCoordinate = std::abs(i + outputConverted);

                if (outputYCoordinate > pen::PixelBufferHeight()) {
                    outputYCoordinate = pen::PixelBufferHeight() - 1;
                }

                pen::Draw(j, outputYCoordinate, waveColor);
            }
        }

        /*
        This part of the algorithm below uses the native pixel buffer which is null when building for mobile, it was used to check the color,
        buffers are typically uni-directional for writing to only, but in this example I used the buffer to read the color to make things quick.
        */
#ifndef __PEN_MOBILE__
        /*Fill in each wave*/
        auto pixels = pen::PixelBuffer();
        for (int i = waterBodyHeight; i < waveTop; i++) {
            bool isBlue = false;
            bool nonBlueConfirmed = false;
            for (int j = 0; j < pen::PixelBufferWidth(); j++) {
                if (j == 0 && (std::fabs(waveOffset) > 0.0f)) {
                    isBlue = true;
                    nonBlueConfirmed = true;
                }
                if (
                    pixels[i * (pen::PixelBufferWidth() * 4) + (4 * j)] / 255 == waveColor.x &&
                    pixels[i * (pen::PixelBufferWidth() * 4) + (4 * j) + 1] / 255 == waveColor.y &&
                    pixels[i * (pen::PixelBufferWidth() * 4) + (4 * j) + 2] / 255 == waveColor.z &&
                    pixels[i * (pen::PixelBufferWidth() * 4) + (4 * j) + 3] / 255 == waveColor.w && !isBlue) {
                    /*Set isBlue to true*/
                    isBlue = true;
                }
                else {
                    if (!(pixels[i * (pen::PixelBufferWidth() * 4) + (4 * j)] / 255 == waveColor.x &&
                        pixels[i * (pen::PixelBufferWidth() * 4) + (4 * j) + 1] / 255 == waveColor.y &&
                        pixels[i * (pen::PixelBufferWidth() * 4) + (4 * j) + 2] / 255 == waveColor.z &&
                        pixels[i * (pen::PixelBufferWidth() * 4) + (4 * j) + 3] / 255 == waveColor.w) && isBlue) {
                        /*If the color is not blue and blue was previously confirmed*/
                        pen::Draw(j, i, waveColor);
                        nonBlueConfirmed = true;
                    }
                    else {
                        /*Wave has ended*/
                        if (nonBlueConfirmed) isBlue = false;
                        nonBlueConfirmed = false;
                    }
                }
            }
        }

        /*Updates the wave offset*/
        waveOffset += ((angularVelocity * 3.14159f / 180.0f) * pen::Pen::GetDeltaTime());

        /*Wraps back around to 0.0f if the wave offset is greater than 2 pi*/
        if (waveOffset >= 2 * 3.14159f) waveOffset = 0.0f;
#endif
    }

    void OnRender() override {
        while (pen::Pen::Running())
        {
            OnInput();
            if (pen::Render::Get()->firstTime) pen::Render::Background(pen::PEN_CYAN);

            Draw();

            pen::Pen::ManageRender();
        }
    }

    void OnInput() override {

    }
};

//int main()
//{
//    WaveAlgorithm waveAlgorithm = WaveAlgorithm();
//    waveAlgorithm.CreateApplication("Wave Algorithm", SCR_WIDTH, SCR_HEIGHT, "/home/user/dev/project");
//    waveAlgorithm.OnCreate();
//    waveAlgorithm.OnRender();
//    waveAlgorithm.End();
//
//    return 0;
//}