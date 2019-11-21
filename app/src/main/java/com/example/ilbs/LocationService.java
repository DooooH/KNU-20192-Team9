package com.example.ilbs;

import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.net.wifi.WifiManager;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;

public class LocationService extends Service {
    public static Intent serviceIntent = null;
    private WifiManager wifiMan; //와이파이 매니저
    private SocketThread sThread;

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        serviceIntent = intent;
        wifiMan = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);

        setForegroundService();
        sThread = new SocketThread(wifiMan);
        sThread.setDaemon(true);
        sThread.start();

        return START_REDELIVER_INTENT;
    }

    @Override
    public void onDestroy() {
        serviceIntent = null;
        sThread.interrupt();
        Log.i("Test Message", "Service Destroy.");
        super.onDestroy();
    }

    private void setForegroundService() {
        final String CHANNEL_ID = "12345";

        Intent notificationIntent = new Intent(this, MainActivity.class)
                .setAction(Intent.ACTION_MAIN)
                .addCategory(Intent.CATEGORY_LAUNCHER);
        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, notificationIntent, 0);

        NotificationCompat.Builder builder;
        if (android.os.Build.VERSION.SDK_INT >= 26) {
            NotificationChannel channel = new NotificationChannel(CHANNEL_ID, "ILBS", NotificationManager.IMPORTANCE_LOW);
            channel.enableLights(false);
            channel.enableVibration(false);
            channel.setSound(null, null);

            ((NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE)).createNotificationChannel(channel);
            builder = new NotificationCompat.Builder(this, CHANNEL_ID);
        } else
            builder = new NotificationCompat.Builder(this);

        builder.setSmallIcon(R.mipmap.ic_launcher).setContentTitle("ILBS is on Service").setContentIntent(pendingIntent);
        startForeground(1, builder.build());
    }
}
