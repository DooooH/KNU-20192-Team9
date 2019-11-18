package com.example.ilbs;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.app.NotificationCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;


public class MainActivity extends AppCompatActivity {
    private NotificationManager notificationManager;
    private WifiManager wifiMan;
    private SocketThread sThread;
    private boolean onService = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Switch activateSwitch = findViewById(R.id.activate_switch);
        activateSwitch.setOnCheckedChangeListener(new threadActivateListener());

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
            checkPermission();

        wifiMan = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
    }

    @Override
    public void onBackPressed() {
        if(onService)
            moveTaskToBack(true);
        else
            super.onBackPressed();
    }

    public void checkPermission() { //인터넷 권한 확인
        int REQUEST_ACCESS_FINE_LOCATION = 1000;

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_DENIED)
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, REQUEST_ACCESS_FINE_LOCATION);
    }

    @SuppressLint("HandlerLeak")
    Handler mainHandler = new Handler() {
        public void handleMessage(@NonNull Message msg) {
            TextView textView = findViewById(R.id.connection_text);

            if (msg.what == 0)
                textView.setText("OFFLINE");
            else
                textView.setText("ONLINE");
        }
    };

    private void setForegroundService() {
        final String CHANNEL_ID = "12345";

        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
            notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);

            Intent notificationIntent = new Intent(this, MainActivity.class);
            notificationIntent.setFlags(Intent.FLAG_ACTIVITY_BROUGHT_TO_FRONT);
            PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);

            NotificationChannel notificationChannel = new NotificationChannel(CHANNEL_ID, "ILBS", NotificationManager.IMPORTANCE_LOW);
            notificationChannel.enableLights(false);
            notificationChannel.enableVibration(false);
            notificationChannel.setSound(null, null);
            notificationManager.createNotificationChannel(notificationChannel);

            NotificationCompat.Builder builder = new NotificationCompat.Builder(this, CHANNEL_ID)
                    .setContentTitle("ILBS")
                    .setContentText("ILBS is on service.")
                    .setSmallIcon(R.drawable.ic_launcher_foreground)
                    .setPriority(NotificationCompat.PRIORITY_DEFAULT)
                    .setContentIntent(pendingIntent)
                    .setAutoCancel(false);

            final Notification notification = builder.build();
            notification.flags = Notification.FLAG_NO_CLEAR;

            notificationManager.createNotificationChannel(notificationChannel);
            startForegroundService(notificationIntent);
        }

    }

    class threadActivateListener implements CompoundButton.OnCheckedChangeListener{
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            TextView txt = findViewById(R.id.onoff_text);
            if(isChecked) {
                txt.setText("ON");
                setForegroundService();
                sThread = new SocketThread(wifiMan, mainHandler);
                sThread.setDaemon(true);
                sThread.start();
                sThread.getHandler().sendEmptyMessage(1);
                onService = true;
            }

            else {
                txt.setText("OFF");
                sThread.getHandler().sendEmptyMessage(0);
                sThread.interrupt();
                notificationManager.cancel(1);
                onService = false;
            }
        }
    }
}