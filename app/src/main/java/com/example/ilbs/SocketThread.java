package com.example.ilbs;

import android.annotation.SuppressLint;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import androidx.annotation.NonNull;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.NetworkInterface;
import java.net.Socket;
import java.util.Collections;
import java.util.List;

public class SocketThread extends Thread {
    private Socket socket = null; //연결 소켓
    private OutputStream out = null; //쓰는 버퍼
    private InputStream in = null; //읽어오는 버퍼

    private boolean Service = true; //서비스 상태 (On/Off)
    private WifiManager wifiMan; //와이파이 매니저
    private Handler mHandler; //메인스레드 핸들러

    SocketThread(WifiManager wifiMain, Handler handlerMain) {
        wifiMan = wifiMain;
        mHandler = handlerMain;
    }

    @SuppressLint("HandlerLeak")
    Handler threadHandler = new Handler() {
        public void handleMessage(@NonNull Message msg) {
            if (msg.what == 0)
                Service = false;

            else if (msg.what == 1)
                Service = true;
        }
    };

    public Handler getHandler () {
        return threadHandler;
    }

    private void connect() { //소켓 연결
        String ip = "172.30.1.30"; //연결 IP
        int port = 8888; //연결 포트

        try {
            socket = new Socket(ip, port);
            out = socket.getOutputStream();
            in = socket.getInputStream();
        } catch (IOException e) {
            Log.i("Socket Connect", "Error.");
        }

        mHandler.sendEmptyMessage(1);
    }

    private void disconnect() { //소켓 닫기
        try {
            socket.close();
        } catch (IOException e) { }

        mHandler.sendEmptyMessage(0);
    }

    @NonNull
    private static String getMACAddress() {
        try {
            List<NetworkInterface> interfaces = Collections.list(NetworkInterface.getNetworkInterfaces());

            for (NetworkInterface intf : interfaces) {
                byte[] mac = intf.getHardwareAddress();

                if (mac == null)
                    return "";
                StringBuilder buf = new StringBuilder();
                for (int idx = 0; idx < mac.length; idx++)
                    buf.append(String.format("%02X:", mac[idx]));

                if (buf.length() > 0)
                    buf.deleteCharAt(buf.length()-1);

                return buf.toString();
            }
        } catch (Exception ex) { }
        return "";
    }

    @Override
    public void run() {
        connect();

        if (socket.isConnected()) {
            try {
                while(Service) {
                    List<ScanResult> results = wifiMan.getScanResults(); //와이파이 리스트
                    String txt = getMACAddress() + "\n";

                    Log.i("List Test", String.format("%d network found", results.size()));

                    for (ScanResult result : results) { //와이파이 정보 전송
//                    if (result.SSID.length() > 0 && result.SSID.substring(0,2).equals("E9")) //사용 가능한 와이파이만
                        txt += String.format("%s %s %d %d\n", result.SSID, result.BSSID, result.level, result.frequency);
                        Log.i("List Test", String.format("%s %s %d %d\n", result.SSID, result.BSSID, result.level, result.frequency));
                    }

                    byte[] outText = txt.getBytes();
                    out.write(outText);
                    out.flush();

                    try { //n초 만큼 대기
                        sleep(1000 * 20);
                    } catch (InterruptedException e) { disconnect(); }
                } //while Service on

            } catch (IOException e) { }
            disconnect();
        } //if connected
    } //run

} //SocketThread
