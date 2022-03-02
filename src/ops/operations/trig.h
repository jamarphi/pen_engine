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
#pragma once
#include "../../state/config.h"

#ifndef __PEN_MOBILE__
#include <cmath>
#endif

namespace pen {
	namespace op {
        static float Abs(float num) {
            if (num < 0.0f) {
                return (-1.0f * num);
            }
            return num;
        }

        static float SqrtSeek(float num, float lowBound, float highBound)
        {
            float midPoint = (lowBound + highBound) / 2;
            float midPointSquare = midPoint * midPoint;

            if (pen::op::Abs((midPointSquare - num) < 0.00001f)) {
                return midPoint;
            }
            else if (midPointSquare < num) {
                return SqrtSeek(num, midPoint, highBound);
            }
            else {
                return SqrtSeek(num, lowBound, midPoint);
            }
        }

        static float Sqrt(float num)
        {
            float result = 0.0f;

#ifndef __PEN_MOBILE__
            result = std::sqrt(num);
#else
            float i = 1.0f;
            bool rootFound = false;
            while (!rootFound) {
                if (i * i == num) {
                    rootFound = true;
                    result = i;
                }
                else if (i * i > num) {
                    float root = pen::op::SqrtSeek(num, i - 1, i);
                    rootFound = true;
                    result = root;
                }
                i++;
            }
#endif
            return result;
        }

        static float Pow(float base, int exp) {
            /*Calculate a number to the power of an exponent*/
            if (base != 0.0f) {
                if (exp < 0.0f) {
                    return 1.0f / (base * Pow(base, (-exp) - 1));
                }
                if (exp == 0) {
                    return 1;
                }
                if (exp == 1) {
                    return base;
                }
                return base * Pow(base, exp - 1);
            }
            return 0.0f;
        }

        static int Factorial(int num) {
            /*Calculates factorial of a starting base number*/
            return num <= 0 ? 1 : num * Factorial(num - 1);
        }

		static float Sin(float rad) {
            /*Sine approximation*/
            float sinRes = 0.0f;
#ifndef __PEN_MOBILE__
            sinRes = std::sin(rad);
#else
            while (rad > 2.0f * 3.14159f) {
                rad -= (2.0f * 3.14159f);
            }

            for (int i = 0; i < 8; i++) {
                sinRes += Pow(-1, i) * Pow(rad, 2 * i + 1) / Factorial(2 * i + 1);
            }
#endif
            return sinRes;
		}

        static float Cos(float rad) {
            /*Cosine approximation*/
            float cosRes = 0.0f;
#ifndef __PEN_MOBILE__
            cosRes = std::cos(rad);
#else
            while (rad > 2.0f * 3.14159f) {
                rad -= (2.0f * 3.14159f);
            }

            for (int i = 0; i < 8; i++) {
                cosRes += Pow(-1, i) * Pow(rad, 2 * i) / Factorial(2 * i);
            }
#endif
            return cosRes;
        }

        static float Tan(float rad) {
            /*Tangent approximation*/
            float tanRes = 0.0f;
#ifndef __PEN_MOBILE__
            tanRes = std::tan(rad);
            return tanRes;
#else
            while (rad > 2.0f * 3.14159f) {
                rad -= (2.0f * 3.14159f);
            }

            return (Sin(rad) / Cos(rad));
#endif

        }

        static float ASin(float rad) {
            /*Arc Sine approximation*/
            float arcSinRes = 0.0f;
#ifndef __PEN_MOBILE__
            arcSinRes = std::asin(rad);
#else
            float prevDistance = 100.0f;
            while (rad > 2.0f * 3.14159f) {
                rad -= (2.0f * 3.14159f);
            }

            for (float i = 0.0f; i < 2.0f * 3.14159f; i += 0.01f) {
                float sinRes = Sin(i);
                if (Abs(sinRes - rad) <= prevDistance) {
                    prevDistance = Abs(sinRes - rad);
                    arcSinRes = i;
                }
                else {
                    return arcSinRes;
                }
            }
#endif
            return arcSinRes;
        }

        static float ACos(float rad) {
            /*Arc Cosine approximation*/
            float arcCosRes = 0.0f;
#ifndef __PEN_MOBILE__
            arcCosRes = std::acos(rad);
#else
            float prevDistance = 100.0f;
            while (rad > 2.0f * 3.14159f) {
                rad -= (2.0f * 3.14159f);
            }

            for (float i = 0.0f; i < 2.0f * 3.14159f; i += 0.01f) {
                float cosRes = Cos(i);
                if (Abs(cosRes - rad) <= prevDistance) {
                    prevDistance = Abs(cosRes - rad);
                    arcCosRes = i;
                }
                else {
                    return arcCosRes;
                }
            }
#endif
            return arcCosRes;
        }

        static float ATan(float rad) {
            /*Arc Tangent approximation*/
            float arcTanRes = 0.0f;
#ifndef __PEN_MOBILE__
            arcTanRes = std::atan(rad);
            return arcTanRes;
#else
            float prevDistance = 100.0f;
            while (rad > 2.0f * 3.14159f) {
                rad -= (2.0f * 3.14159f);
            }

            return (ASin(rad) / ACos(rad));
#endif
        }
	}
}