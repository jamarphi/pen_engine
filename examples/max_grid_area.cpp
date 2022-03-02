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


int N = 10, M = 10;
bool searching = false, horizSearch = false, vertiSearch = false, innerHorizSearch = false, innerVertiSearch = false;
int rectArea = 0, rectWidth = 0, rectHeight = 0, updatedRectWidth = 0, updatedRectHeight = 0;
std::vector<std::vector<int>> userMatrix { 
    {0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,1,1,1,1,1,0},
    {1,0,0,0,0,0,1,1,1,0},
    {1,0,0,0,0,1,1,1,1,0},
    {0,0,0,0,0,1,1,1,1,0},
    {0,0,0,0,0,0,0,1,1,0},
    {0,0,1,1,0,0,0,0,0,0},
    {0,0,1,1,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,0,0},
    {0,0,0,0,1,1,1,1,0,0}
};

int countVertical(const std::vector<std::vector<int>> matrix, int rectHeight, int rowPos, int colPos) {
    int count = 0;
    for (int i = rowPos; i < N; i++) {
        if (matrix[i][colPos] == 1) count++;

        if (matrix[i][colPos] == 0 && i > rowPos) break;
    }
    return count;
}

int countHorizontal(const std::vector<int> row, int rectWidth, int colPos) {
    int count = 0;
    for (int i = colPos; i < M; i++) {
        if (row[i] == 1) count++;

        if (row[i] == 0 && i > colPos) break;
    }
    return count;
}

int innerSearchVertical(const std::vector<std::vector<int>> matrix, int rowPos, int colPos, int rectWidth, int rectHeight) {
    int returnHeight = rectHeight;
    //count continuous 1s from this rowPos

    /*Shift the row position back to where the rectangle begins if it starts before the current row position*/
    int vertiRowPos = (N - rowPos > N - rectHeight ? (rowPos - ((N - rowPos) - (N - rectHeight))) : rowPos);
    if (vertiRowPos < 0) vertiRowPos = 0;

    int height = countVertical(matrix, rectHeight, vertiRowPos, colPos);
    /*Update the screen with counted squares, batch will get submitted later once area is updated*/
    for (int i = vertiRowPos; i < vertiRowPos + height; i++) {
        pen::ui::Item* item = pen::ui::FindItem(i * N + colPos);
        if(item->origText == "1") item->SetColor(pen::PEN_VIOLET);
    }

    if (height < rectHeight) returnHeight = height;

    return returnHeight;
}

int innerSearchHorizontal(const std::vector<std::vector<int>> matrix, int rowPos, int colPos, int rectWidth) {
    int returnWidth = rectWidth;
    /*Shift the column position back to where the rectangle begins if it starts before the current column position*/
    int horizColPos = (M - colPos > M - rectWidth ? (colPos - ((M - colPos) - (M - rectWidth))) : colPos);
    if (horizColPos < 0) horizColPos = 0;

    int width = countHorizontal(matrix[rowPos], rectWidth, horizColPos);
    /*Update the screen with counted squares, batch will get submitted later once area is updated*/
    for (int i = horizColPos; i < horizColPos + width; i++) {
        pen::ui::Item* item = pen::ui::FindItem(rowPos * N + i);
        if(item->origText == "1") item->SetColor(pen::PEN_GREEN);
    }

    if (width < rectWidth) returnWidth = width;

    return returnWidth;
}

int bidirectionalVertical(const std::vector<std::vector<int>> matrix, int rowPos, int colPos) {
    if (matrix[rowPos][colPos] == 1) {
        pen::ui::FindItem(rowPos * N + colPos)->SetColor(pen::PEN_VIOLET);
        pen::ui::Submit();
        return 1;
    }
    else {
        return 0;
    }
}

int bidirectionalHorizontal(const std::vector<std::vector<int>> matrix, int rowPos, int colPos) {
    if (matrix[rowPos][colPos] == 1) {
        pen::ui::FindItem(rowPos * N + colPos)->SetColor(pen::PEN_GREEN);
        pen::ui::Submit();
        return 1;
    }else{
        return 0;
    }
}

void displayMatrix(std::vector<std::vector<int>> matrix) {
    std::cout << "\n";

    for (int j = 0; j < N; j++) {

        std::cout << "\n|";
        for (int k = 0; k < M; k++) {
            if (k < M - 1) { 
                std::cout << matrix[j][k] << ",";
            }
            else {
                std::cout << matrix[j][k];
            }
        }
        std::cout << "|";
    }
    std::cout << "\n";
}

const unsigned int SCR_WIDTH = 960;
const unsigned int SCR_HEIGHT = 540;

class App : public pen::Pen {
public:
    App() {

    }

    void InitializeObjects() {
        float posX = 1.0f;
        float posY = pen::Pen::ScreenHeight() - (pen::Pen::ScreenHeight() / 10.0f);
        pen::Vec2 size = pen::Vec2(pen::Pen::ScreenWidth() / 10.0f - 5.0f, pen::Pen::ScreenHeight() / 10.0f - 5.0f);

        /*Create the grid*/
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                pen::ui::AddItem(new pen::ui::TextBox(i * N + j, std::to_string(userMatrix[i][j]), 
                    pen::Vec3(posX + ((pen::Pen::ScreenWidth() / 10) * j), posY - ((pen::Pen::ScreenHeight() / 10.0f) * i), 0.0f), size.x, size.y, 
                    (userMatrix[i][j] == 1) ? pen::BLUE : pen::PEN_RED, pen::PEN_WHITE));
            }
        }
        
        /*Add textbox for current area*/
        pen::ui::AddItem(new pen::ui::TextBox(102, "Current area: 0", pen::Vec3(30.0f, pen::Pen::ScreenHeight() - 50.0f, 0.0f), pen::Pen::ScreenWidth() / 3.0f,
            pen::PEN_WHITE, pen::PEN_BLACK));
    }

    int Search(std::vector<std::vector<int>> matrix, int rowPos, int colPos, int currentMaxArea) {
        int rectArea = 0;
        //rectWidth and rectHeight are the max dimensions of the rectangle from the current 1
        int rectWidth = bidirectionalHorizontal(matrix, rowPos, colPos);
        int rectHeight = bidirectionalVertical(matrix, rowPos, colPos);

        //these next two lines check for the inner ones to get the true area of the rectangle going both axis t0 determine the largest one

        //if newRectWidth is actually smaller, then it will be updated
        int newRectWidth = innerSearchHorizontal(matrix, rowPos, colPos, rectWidth);

        rectArea = newRectWidth * rectHeight;

        //if rectHeight is actually smaller, then it will be updated
        int newRectHeight = innerSearchVertical(matrix, rowPos, colPos, rectWidth, rectHeight);

        if (newRectWidth < rectWidth) rectWidth = newRectWidth;
        if (newRectHeight < rectHeight) rectHeight = newRectHeight;

        if (rectWidth * rectHeight < rectArea) rectArea = rectWidth * rectHeight;

        return (currentMaxArea > rectArea) ? currentMaxArea : rectArea;
    }

    void MakeActive(pen::ui::Item* item) {
        item->SetColor(pen::PEN_ORANGE);
        pen::ui::Submit();
    }

    void Refresh() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                pen::ui::Item* item = pen::ui::FindItem(i * N + j);
                (item->origText == "1") ? item->SetColor(pen::BLUE) : item->SetColor(pen::PEN_RED);
            }
        }
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
        int counter = 0;
        
        /*Left and down directions*/
        int firstScanCounter = 0;
        int firstScanBound = 0;
        /*Right and up directions*/
        int secondScanCounter = 0;
        int secondScanBound = 0;
        int innerCount = 1;
        int innerHorizWidth = 0;
        int innerVertiHeight = 0;
        while (pen::Pen::Running())
        {
            OnInput();
            renderer.Clear();
            if (pen::Render::Get()->firstTime) renderer.Background(pen::PEN_GRAY);

            /*Put a delay so it can be seen more clearly*/
            //pen::Pen::Sleep(100);

            if (counter == N * M) break;

            if (counter == N * M - 1) {
                /*Algorithm is finished*/
                const std::string maxArea = "Your max area is: " + std::to_string(rectArea);
                pen::ui::AddItem(new pen::ui::TextBox(101, maxArea,
                    pen::Vec3(pen::Pen::ScreenWidth() / 2.0f, pen::Pen::ScreenHeight() / 2.0f - 10.0f, 0.0f), (pen::Pen::ScreenWidth() / 4.0f), (pen::Pen::ScreenHeight() / 10.0f),
                    pen::PEN_WHITE, pen::PEN_BLACK));
            }
            else {
                if (searching) {
                    /*Scan the grid for area*/
                    if (horizSearch) {
                        /*Initial horizontal scan*/
                        if (firstScanCounter > 0) {
                            /*First scan*/
                            int firstHorizWidth = bidirectionalHorizontal(userMatrix, counter / N, firstScanCounter);
                            firstScanCounter--;
                            if (firstHorizWidth == 0) {
                                firstScanCounter = 0; 
                            }
                            else {
                                rectWidth += firstHorizWidth;
                            }
                        }
                        else if (secondScanCounter < secondScanBound) {
                            /*Second scan*/
                            int secondHorizWidth = bidirectionalHorizontal(userMatrix, counter / N, secondScanCounter);
                            secondScanCounter++;
                            if (secondHorizWidth == 0) {
                                secondScanCounter = secondScanBound;
                            }
                            else {
                                rectWidth += secondHorizWidth;
                            }
                        }
                        else {
                            horizSearch = false;
                            vertiSearch = true;
                            firstScanBound = N;
                            secondScanBound = 0;
                            firstScanCounter = counter / N;
                            secondScanCounter = counter / N - 1;
                            rectArea  = (rectArea < rectWidth) ? rectWidth : rectArea;
                            pen::ui::FindItem(102)->UpdateText("Current area: " + std::to_string(rectArea));
                            pen::ui::Submit();
                        }
                    }
                    else if (vertiSearch) {
                        /*Initial vertical scan*/
                        if (counter >= 94) {
                            std::cout << "a";
                        }
                        if (firstScanCounter < firstScanBound) {
                            /*First scan*/
                            int firstVertiHeight = bidirectionalVertical(userMatrix, firstScanCounter, counter % M);
                            firstScanCounter++;
                            if (firstVertiHeight == 0) {
                                firstScanCounter = firstScanBound;
                            }
                            else {
                                rectHeight += firstVertiHeight;
                            }
                        }
                        else if (secondScanCounter > 0) {
                            /*Second scan*/
                            int secondVertiHeight = bidirectionalVertical(userMatrix, secondScanCounter, counter % M);
                            secondScanCounter--;
                            if (secondVertiHeight == 0) {
                                secondScanCounter = 0;
                            }
                            else {
                                rectHeight += secondVertiHeight;
                            }
                        }
                        else {
                            vertiSearch = false;
                            innerHorizSearch = true;
                            firstScanBound = rectHeight + (counter / N);
                            if (firstScanBound > N) firstScanBound = N;
                            //firstScanCounter = (N - (counter / N) > rectHeight) ?((counter / N) - (N - (counter / N))) : counter / N;
                            firstScanCounter = counter / N;
                            if (firstScanCounter < 0) firstScanCounter = 0;
                            rectArea = (rectArea < rectHeight) ? rectHeight : rectArea;
                            pen::ui::FindItem(102)->UpdateText("Current area: " + std::to_string(rectArea));
                            pen::ui::Submit();
                            innerHorizWidth = rectWidth;
                        }
                    }
                    else if (innerHorizSearch) {
                        /*Second horizontal scan*/
                        if (firstScanCounter < firstScanBound) {
                            innerHorizWidth = innerSearchHorizontal(userMatrix, firstScanCounter, counter % M, innerHorizWidth);
                            firstScanCounter++;
                            if (innerHorizWidth == 0) {
                                firstScanCounter = firstScanBound;
                            }
                            else {
                                updatedRectHeight = countVertical(userMatrix, rectHeight, counter / N, counter % M);
                                rectArea = (updatedRectHeight * innerHorizWidth > rectArea) ? (updatedRectHeight * innerHorizWidth) : rectArea;
                                pen::ui::FindItem(102)->UpdateText("Current area: " + std::to_string(rectArea));
                                pen::ui::Submit();
                            }
                        }
                        else {
                            innerHorizSearch = false;
                            innerVertiSearch = true;
                            firstScanCounter = counter % M;
                            if (firstScanCounter < 0) firstScanCounter = 0;
                            firstScanBound = rectWidth + (counter % M);
                            if (firstScanBound > M) firstScanBound = M;
                            innerVertiHeight = rectHeight;
                        }
                    }
                    else if (innerVertiSearch) {
                        /*Second vertical scan*/
                        if (firstScanCounter < firstScanBound) {
                            innerVertiHeight = innerSearchVertical(userMatrix, counter / N, firstScanCounter, rectWidth, innerVertiHeight);
                            firstScanCounter++;
                            if (innerVertiHeight == 0) {
                                firstScanCounter = firstScanBound;
                            }
                            else {
                                updatedRectWidth = countHorizontal(userMatrix[counter / N], rectWidth, counter % M);
                                rectArea = (updatedRectWidth * innerVertiHeight > rectArea) ? (updatedRectWidth * innerVertiHeight) : rectArea;
                                pen::ui::FindItem(102)->UpdateText("Current area: " + std::to_string(rectArea));
                                pen::ui::Submit();
                            }
                        }
                        else {
                            innerVertiSearch = false;
                            searching = false;
                            counter++;
                        }
                    }
                }
                else {
                    /*Sets all grid cells back to red or blue*/
                    Refresh();

                    //App logic
                    MakeActive(pen::ui::FindItem(counter));
                    if (userMatrix[counter / N][counter % M] == 1) {
                        searching = true;
                        horizSearch = true;

                        /*This is for the left scan bound for the horizontal search to get things started*/
                        firstScanBound = 0;
                        firstScanCounter = counter % M - 1;

                        /*This is for the right scan bound for the horizontal search to get things started*/
                        secondScanBound = M;
                        secondScanCounter = counter % M;

                        rectWidth = 0;
                        rectHeight = 0;
                    }
                    else {
                        counter++;
                    }
                }
            }
            
            pen::Pen::ManageRender(&renderer);
        }
    }

    void OnInput() override {

    }
};

//int main()
//{
//    displayMatrix(userMatrix);

//    App app = App();
//    app.CreateApplication("Max Grid Area Algorithm", SCR_WIDTH, SCR_HEIGHT, "/home/user/dev/project", {}, true);
//    app.OnCreate();
//    app.OnRender();
//    //app.End();
//    std::system("Pause");
//    return 0;
//}