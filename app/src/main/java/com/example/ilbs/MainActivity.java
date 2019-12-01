package com.example.ilbs;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    private static Intent serviceIntent = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Switch activateSwitch = findViewById(R.id.activate_switch);
        activateSwitch.setOnCheckedChangeListener(new threadActivateListener());

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
            checkPermission();

        if (serviceIntent != null) {
            TextView onText = findViewById(R.id.onoff_text);
            activateSwitch.setChecked(true);
            onText.setText("ON");
        }
    }

    @Override
    protected void onDestroy() {
        Log.i("Test Message", "Destroyed.");
        super.onDestroy();
    }

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