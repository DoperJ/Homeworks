package com;

import java.io.*;
import java.net.*;
import java.util.Scanner;

import com.alibaba.fastjson.JSONObject;

/**
 * 测试类
 */
public class HttpClient {
    private static final String JSON = "application/json;charset=UTF-8";
    private static final String FORM = "application/x-www-form-urlencoded;charset=UTF-8";
    private static final String GET = "GET";
    private static final String POST = "POST";
    private static final int OK = 200;
    private static final int ERROR = 500;

	public static void main(String[] args) throws Exception{
	    int  choice;
	    while (true) {
            Scanner sc = new Scanner(System.in);
            System.out.println("Choose the way to send request: ");
            System.out.println("1. Get");
            System.out.println("2. PostJson");
            System.out.println("3. PostForm");
            choice = sc.nextInt();
            switch (choice) {
                case 1:
                    sendGet();
                    break;
                case 2:
                    sendPostJson();
                    break;
                case 3:
                    sendPostForm();
                    break;
                default:
                    System.out.println("Not a valid choice!");
                    break;
            }
        }
	}

	private static HttpURLConnection sendMessage(byte[] data, String path, String method, String content_type) throws MalformedURLException, IOException {
        URL url = new java.net.URL(path);
        HttpURLConnection conn = (java.net.HttpURLConnection) url.openConnection();
        conn.setRequestMethod(method);
        conn.setDoOutput(true);
        if (method == POST) {
            conn.setRequestProperty("Connection", "keep-alive");
            conn.setRequestProperty("Content-Type", content_type);
            conn.setRequestProperty("Content-Length", String.valueOf(data.length));
            OutputStream outStream = conn.getOutputStream();
            outStream.write(data);
            outStream.flush();
            outStream.close();
        } else if (method == GET) {
            conn.connect();
        }
	    return conn;
    }

    private static HttpURLConnection receiveMessage(HttpURLConnection conn)  throws IOException {
        if(conn.getResponseCode() == 200){
            BufferedReader in = new BufferedReader(new InputStreamReader((InputStream) conn.getInputStream(), "UTF-8"));
            String msg = in.readLine();
            System.out.println("Received message from server: " + msg);
            in.close();
        } else if(conn.getResponseCode() == 500){
            System.out.println("Failed! Connetion closed.");
        }
	    return conn;
    }

	private static void sendPostJson() throws Exception{
		String path = "http://127.0.0.1:8080";
		JSONObject obj = new JSONObject();
		obj.put("id", "10001");
		obj.put("name", "JiangZequn");
		obj.put("sex", "M");
		String jsonStr = obj.toJSONString();
		byte[] data = jsonStr.getBytes();
		HttpURLConnection conn = sendMessage(data, path, POST, JSON);
		conn = receiveMessage(conn);
        conn.disconnect();
	}
	
	private static void sendPostForm() throws Exception{
		String path = "http://127.0.0.1:8080/";
		String parm = "id=10001&name=JiangZequn&sex=M";
		byte[] data = parm.getBytes();
        HttpURLConnection conn = sendMessage(data, path, POST, FORM);
        conn = receiveMessage(conn);
        conn.disconnect();
	}
	
	private static void sendGet() throws Exception{
		String path = "http://127.0.0.1:8080/";
		String reqUrl = path + "?id=10001&name=JiangZequn&sex=M";
		HttpURLConnection conn = sendMessage(null, reqUrl, GET, null);
            conn = receiveMessage(conn);
            conn.disconnect();
        }
    }
