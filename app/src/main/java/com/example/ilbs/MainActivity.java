package com.example.ilbs;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.NetworkInterface;
import java.net.Socket;
import java.util.Collections;
import java.util.List;

import static java.lang.Thread.sleep;

public class MainActivity extends AppCompatActivity implements Runnable {
    private static int REQUEST_ACCESS_FINE_LOCATION = 1000;
    private WifiManager wifiMan; //와이파이 매니저
    private List<ScanResult> results; //와이파이 리스트
    private boolean done = false; //스레드 종료 조건

    Thread sThread = new Thread(MainActivity.this);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) { //인터넷 권한 확인
            int permissionCheck = ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION);

            if (permissionCheck == PackageManager.PERMISSION_DENIED) {
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, REQUEST_ACCESS_FINE_LOCATION);
            }
        }

        sThread.setDaemon(true);
        sThread.start(); //thread 생성
    }

    @Override
    protected void onDestroy() { //종료시 스레드 종료
        super.onDestroy();
        done = true;
        sThread.interrupt();
    }

    @Override
    public void run() {
        Socket socket = null; //연결 소켓
        OutputStream out = null; //쓰는 버퍼
        InputStream in = null; //읽어오는 버퍼
        String ip = "20.20.0.101"; //연결 IP
        int port = 9999; //연결 포트

        try { //소켓 연결
            socket = new Socket(ip, port);
            out = socket.getOutputStream();
            in = socket.getInputStream();
        } catch (IOException e) {
            Log.i("Socket Connect", "Error.");
            e.printStackTrace();
        }

        wifiMan = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE); //와이파이 정보 받아오기

        try {
            while(!done) { //어플리케이션 종료시까지
                results = wifiMan.getScanResults();
                String mac = getMACAddress("wlan0");

                Log.i("List Test", String.format("%d network found", results.size()));

                for (ScanResult result : results) { //와이파이 정보 전송
                    String txt = String.format("%s %s %d %d*", mac, result.BSSID, result.level, result.frequency);
                    Log.i("List Test", txt);
                    byte[] outText  = txt.getBytes();
                    out.write(outText);
                    out.flush();
                }
                try {
                    sleep(1000 * 10); //n초만큼 대기 (현재 10초 -> 1~5분 예정)
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        try {
            socket.close(); //소켓 닫기
        } catch (IOException e) {
            e.printStackTrace();
        }
        Log.i("Socket Test", "Socket Closed.");
    }

    public static String getMACAddress(String interfaceName) {
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

    public void onReset(View view) { //리셋버튼 눌렀을 시 interrupt 발생, 다시 정보 발송
        sThread.interrupt();
    }
}