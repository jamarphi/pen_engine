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
package com.jamar.penengine;

import android.annotation.SuppressLint;
import android.os.Build;

import androidx.annotation.RequiresApi;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.Reader;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLEncoder;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

public class PenHttp {

    public PenHttp(){

    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    public static String httpGet(String route, String parameterStr, String headerStr) {
        /*Sends an http get request*/
        try {
            URL url = new URL(route);
            HttpURLConnection conn = (HttpURLConnection) url.openConnection();
            conn.setRequestMethod("GET");

            if(parameterStr != ""){
                Map<String, String> parameterMap = new HashMap<>();
                String[] parameters = parameterStr.split(";");
                for(int i = 0; i < parameters.length; i++){
                    String[] parameterPair = parameters[i].split("^");
                    parameterMap.put(parameterPair[0], parameterPair[1]);
                }
                conn.setDoOutput(true);
                DataOutputStream output = new DataOutputStream(conn.getOutputStream());
                output.writeBytes(PenParameterBuilder.parse(parameterMap));
                output.flush();
                output.close();
            }

            if(headerStr != ""){
                String[] headers = headerStr.split(";");
                for(int i = 0; i < headers.length; i++){
                    String[] headerPair = headers[i].split("^");
                    conn.setRequestProperty(headerPair[0], headerPair[1]);
                }
            }

            String response = PenHttpResponse.get(conn);
            conn.disconnect();
            return response;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "";
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    public static String httpPost(String route, String jsonStr, String headerStr) {
        /*Sends an http post request*/
        try {
            URL url = new URL(route);

            HttpURLConnection conn = (HttpURLConnection) url.openConnection();
            conn.setRequestMethod("POST");

            if(jsonStr != ""){
                conn.setDoOutput(true);
                OutputStream output = conn.getOutputStream();
                byte[] input = jsonStr.getBytes("utf-8");
                output.write(input, 0, input.length);
            }

            if(headerStr != ""){
                String[] headers = headerStr.split(";");
                for(int i = 0; i < headers.length; i++){
                    String[] headerPair = headers[i].split("^");
                    conn.setRequestProperty(headerPair[0], headerPair[1]);
                }
            }

            conn.setRequestProperty("Content-Type", "application/json; utf-8");
            conn.setRequestProperty("Accept", "application/json");

            String response = null;
                response = PenHttpResponse.get(conn);
            conn.disconnect();
            return response;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "";
    }

    public static class PenHttpResponse {
        @RequiresApi(api = Build.VERSION_CODES.N)
        public static String get(HttpURLConnection conn) throws IOException {
            StringBuilder response = new StringBuilder();
            response.append("Status:").append(conn.getResponseCode()).append("\n");

            conn.getHeaderFields()
                    .entrySet()
                    .stream()
                    .filter(entry -> entry.getKey() != null)
                    .forEach(entry -> {
                        response.append(entry.getKey())
                                .append(":");
                        List<String> headerValues = entry.getValue();
                        Iterator<String> it = headerValues.iterator();
                        if (it.hasNext()) {
                            response.append(it.next());
                            while (it.hasNext()) {
                                response.append(",")
                                        .append(it.next());
                            }
                        }
                        response.append("\n");
                    });

            Reader streamReader = null;

            if (conn.getResponseCode() < 300) {
                streamReader = new InputStreamReader(conn.getInputStream());
                BufferedReader in = new BufferedReader(streamReader);
                String inputLine;
                StringBuilder content = new StringBuilder();
                while ((inputLine = in.readLine()) != null) {
                    content.append(inputLine);
                }
                in.close();
                response.append("Response:").append(content);
            }else {
                response.append("Response:");
            }
            response.append("\n");
            return response.toString();
        }
    }

    public static class PenParameterBuilder {
        public static String parse(Map<String, String> parameterMap) throws Exception {
            StringBuilder result = new StringBuilder();
            for (Map.Entry<String, String> entry : parameterMap.entrySet()) {
                result.append(URLEncoder.encode(entry.getKey(), "UTF-8"));
                result.append("=");
                result.append(URLEncoder.encode(entry.getValue(), "UTF-8"));
                result.append("&");
            }
            String resultString = result.toString();
            return resultString.length() > 0 ? resultString.substring(0, resultString.length() - 1) : resultString;
        }
    }
}