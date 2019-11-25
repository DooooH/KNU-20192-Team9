package com.example.ilbs;

import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.NetworkInterface;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.Collections;
import java.util.Date;
import java.util.List;

public class SocketThread extends Thread {
    private Socket socket = null; //연결 소켓
    private OutputStream out = null; //쓰는 버퍼
    private InputStream in = null; //읽어오는 버퍼

    private WifiManager wifiMan; //와이파이 매니저
    private Handler mHandler;
    private boolean onService = false;

    SocketThread(WifiManager wifiMain, Handler handlerMain) {
        wifiMan = wifiMain;
        mHandler = handlerMain;
    }

    private void connect() { //소켓 연결
        String ip = "192.168.123.154"; //연결 IP
        int port = 8888; //연결 포트

        try {
            socket = new Socket(ip, port);
            out = socket.getOutputStream();
            in = socket.getInputStream();
        } catch (IOException e) {
            Log.i("Socket Connect", "Error.");
        }

        Log.i("Test Message", "Socket Connected.");
    }

    private void disconnect() { //소켓 닫기
        try {
            socket.close();
        } catch (IOException e) { }

        Log.i("Test Message", "Socket Disconnected.");
    }

    private static String getMACAddress(String interfaceName) {
        try {
            List<NetworkInterface> interfaces = Collections.list(NetworkInterface.getNetworkInterfaces());
            for (NetworkInterface intf : interfaces) {
                if (interfaceName != null) {
                    if (!intf.getName().equalsIgnoreCase(interfaceName)) continue;
                }
                byte[] mac = intf.getHardwareAddress();
                if (mac==null) return "";
                StringBuilder buf = new StringBuilder();
                for (int idx=0; idx<mac.length; idx++)
                    buf.append(String.format("%02X:", mac[idx]));
                if (buf.length()>0) buf.deleteCharAt(buf.length()-1);
                return buf.toString();
            }
        } catch (Exception ex) { }
        return "";
    }

    @Override
    public void run() {
        connect();

        if (socket.isConnected()) {
            onService = true;
            while (onService) {
                try {
                    byte[] inText = new byte[50];
                    List<ScanResult> results = wifiMan.getScanResults(); //와이파이 리스트
                    String txt = getMACAddress("wlan0") + "\n";

                    Log.i("List Test", String.format("%d network found", results.size()));
                    for (ScanResult result : results) { //와이파이 정보 전송
                        if (result.SSID.length() > 0 && result.SSID.substring(0,2).equals("E9")) //사용 가능한 와이파이만
                            txt += String.format("%s %s %d %d\n", result.SSID, result.BSSID, result.level, result.frequency);
                        Log.i("List Test", String.format("%s %s %d %d\n", result.SSID, result.BSSID, result.level, result.frequency));
                    }

                    byte[] outText = txt.getBytes();
                    out.write(outText);
                    out.flush();

                    int size = in.read(inText);
                    String data = new String(inText, 0, size, "EUC_KR");
                    Log.i("Read Test", String.format("%d: %s", size, data));

                    sendMsg(data);

                    disconnect();
                    sleep(1000 * 60);
                    connect();

                } catch (IOException e) { } catch (InterruptedException e) { onService = false; }
            }
            disconnect();
        } //if connected
        Log.i("Test Message", "Thread Destroy.");
    } //run

    private void sendMsg (String data) {
        Bundle bundle = new Bundle();
        Message msg = new Message();

        String[] array = data.split(" ");

        if (array[0] == "1") {
            SimpleDateFormat sdfNow = new SimpleDateFormat("HH:mm:ss");
            String time = "Last Update: " + sdfNow.format(new Date(System.currentTimeMillis()));
            bundle.putString("time", time);
            String temp = array[1] + " " + array[2] + "층 " + array[3];
            bundle.putString("location", temp);
        }
        msg.setData(bundle);
        msg.what = Integer.parseInt(array[0]);
        mHandler.sendMessage(msg);
    }
} //SocketThread