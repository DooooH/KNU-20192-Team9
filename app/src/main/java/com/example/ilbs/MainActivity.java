package com.example.ilbs;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;

import java.text.SimpleDateFormat;
import java.util.Date;


public class MainActivity extends AppCompatActivity {
    private static Intent serviceIntent = null;
    private static String time = "";
    public static Context mContext;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mContext = this;

        Switch activateSwitch = findViewById(R.id.activate_switch);
        activateSwitch.setOnCheckedChangeListener(new threadActivateListener());

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
            checkPermission();

        if (serviceIntent != null) {
            TextView ontxt = findViewById(R.id.onoff_text);
            TextView timetxt = findViewById(R.id.connection_text);
            activateSwitch.setChecked(true);
            ontxt.setText("ON");
            timetxt.setText(time);
        }
    }

    @Override
    protected void onDestroy() {
        Log.i("Test", "Destroyed.");
        super.onDestroy();
    }

    Handler mHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);

            SimpleDateFormat sdfNow = new SimpleDateFormat("HH:mm:ss");
            time = "Last Update: " + sdfNow.format(new Date(System.currentTimeMillis()));

            TextView txt = findViewById(R.id.connection_text);
            txt.setText(time);
        }
    };

    public void checkPermission() { //인터넷 권한 확인
        int REQUEST_ACCESS_FINE_LOCATION = 1000;

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_DENIED)
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, REQUEST_ACCESS_FINE_LOCATION);
    }

    class threadActivateListener implements CompoundButton.OnCheckedChangeListener{
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            TextView txt = findViewById(R.id.onoff_text);
            if(isChecked) {
                txt.setText("ON");

                serviceIntent = new Intent(getApplicationContext(), LocationService.class);
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O && LocationService.serviceIntent == null)
                    startForegroundService(serviceIntent);
                else
                    serviceIntent = LocationService.serviceIntent;
            }

            else {
                txt.setText("OFF");
                if (serviceIntent != null)
                    stopService(serviceIntent);
                serviceIntent = null;
            }
        }
    }
}