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
package com.jamar.pen_engine;

import android.util.Log;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class PenReflectionHelper {
    public static <T> T getConstantValue(final Class aClass, final String constantName) {
        try {
            return (T)aClass.getDeclaredField(constantName).get(null);
        } catch (NoSuchFieldException e) {
            Log.e("error", "can not find " + constantName + " in " + aClass.getName());
        }
        catch (IllegalAccessException e) {
            Log.e("error", constantName + " is not accessible");
        }
        catch (IllegalArgumentException e) {
            Log.e("error", "arguments error when get " + constantName);
        }
        catch (Exception e) {
            Log.e("error", "can not get constant" + constantName);
        }

        return null;
    }

    public static <T> T invokeInstanceMethod(final Object instance, final String methodName,
                                             final Class[] parameterTypes, final Object[] parameters) {

        final Class aClass = instance.getClass();
        try {
            final Method method = aClass.getMethod(methodName, parameterTypes);
            return (T)method.invoke(instance, parameters);
        } catch (NoSuchMethodException e) {
            Log.e("error", "can not find " + methodName + " in " + aClass.getName());
        }
        catch (IllegalAccessException e) {
            Log.e("error", methodName + " is not accessible");
        }
        catch (IllegalArgumentException e) {
            Log.e("error", "arguments are error when invoking " + methodName);
        }
        catch (InvocationTargetException e) {
            Log.e("error", "an exception was thrown by the invoked method when invoking " + methodName);
        }

        return null;
    }
}