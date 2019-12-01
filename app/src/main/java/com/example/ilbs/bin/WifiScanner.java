package com.example.ilbs.bin;

import android.annotation.SuppressLint;
import android.content.Context;
import android.net.wifi.ScanResult;
import android.net.wifi.rtt.RangingRequest;
import android.net.wifi.rtt.RangingResult;
import android.net.wifi.rtt.RangingResultCallback;
import android.net.wifi.rtt.WifiRttManager;
import android.os.Build;
import android.util.Log;

import androidx.annotation.RequiresApi;

import java.util.List;
import java.util.concurrent.Executor;

@RequiresApi(api = Build.VERSION_CODES.P)
public class WifiScanner {
    private final Executor mainExecutor;
    private final WifiRttManager rttManager;

    WifiScanner(final Context context) {
        rttManager = (WifiRttManager) context.getSystemService(Context.WIFI_RTT_RANGING_SERVICE);
        mainExecutor = context.getMainExecutor();
    }

    @SuppressLint("MissingPermission")
    public void startRanging(List<ScanResult> results) {
        RangingRequest.Builder builder = new RangingRequest.Builder();
        builder.addAccessPoints(results);

        RangingRequest req = builder.build();
        rttManager.startRanging(req, mainExecutor, new RangingResultCallback() {
            @Override
            public void onRangingFailure(int code) {
                Log.i("Test", String.format("Failure Code : %d", code));
            }

            @Override
            public void onRangingResults(List<RangingResult> results) {
                for (RangingResult result : results) { //와이파이 정보 전송
                    String txt = String.format("Distance : %d, RSSI : %s, Mac : %s*", result.getDistanceMm(), result.getRssi(), result.getMacAddress());
                    Log.i("List Test", txt);
                }
            }
        });
    }
}