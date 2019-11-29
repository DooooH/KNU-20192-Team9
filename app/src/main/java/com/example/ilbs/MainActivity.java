package com.example.ilbs;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.util.List;

public class MainActivity extends AppCompatActivity {
    private Socket socket; //연결 소켓
    private BufferedReader in; //읽어오는 버퍼
    private BufferedWriter out; //쓰는 버퍼
    private String ip = "xxx.xxx.xxx.xxx"; //연결 IP
    private int port = 9999; //연결 포트

    //와이파이 받아오기용 변수
    private static int REQUEST_ACCESS_FINE_LOCATION = 1000;
    private WifiManager wifiMan;
    private List<ScanResult> results;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            int permissionCheck = ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION);

            if (permissionCheck == PackageManager.PERMISSION_DENIED) {
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, REQUEST_ACCESS_FINE_LOCATION);
            }
        }

        wifiMan = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);

/*        try {
            socket = new Socket(ip, port);
            out = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        } catch (IOException e) {
            Log.i("Socket Error", e.toString());
            e.printStackTrace();
        }*/
    }

    private BroadcastReceiver Receiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {

            results = wifiMan.getScanResults();

            Log.i("List Test", String.format("%d network found", results.size()));
            for (ScanResult result : results) {

               /* 참고용 코드
                int newRssi = wifiMan.getConnectionInfo().getRssi();
                int level = wifiMan.calculateSignalLevel(newRssi, 10);
                int percentage = (int) ((level/10.0)*100);
                String macAdd = wifiMan.getConnectionInfo().getBSSID();
                Log.i("Connection Test", String.format("RSSI : %d, LEVEL : %d, PERC : %d, MAC : %S", newRssi, level, percentage, macAdd));*/

                Log.i("List Test", String.format("SSID : %s, BSSID : %s, Level : %d, Freq : %d", result.SSID, result.BSSID, result.level, result.frequency));

/*              소켓통신용
                try {
                    out.write(String.format("SSID : %s, BSSID : %s, Level : %d, Freq : %d", result.SSID, result.BSSID, result.level, result.frequency));
                } catch (IOException e) {
                    e.printStackTrace();
                }*/
            }
        }
    };

    @Override
    public void onResume() {
        super.onResume();
        IntentFilter intentFilter = new IntentFilter(Intent.ACTION_TIME_TICK);
        this.registerReceiver(Receiver, intentFilter);
        wifiMan.startScan();
    }

    @Override
    protected void onDestroy(){
        super.onDestroy();
        unregisterReceiver(Receiver);
    }
}
