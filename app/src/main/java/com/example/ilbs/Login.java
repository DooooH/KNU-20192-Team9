package com.example.ilbs;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.io.BufferedInputStream;
import java.net.NetworkInterface;
import java.util.Collections;
import java.util.List;

public class Login extends AppCompatActivity {
    private TextView Mac;
    private EditText ID;
    private EditText PW;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        Mac = (TextView) findViewById(R.id.txt_mac);
        ID = (EditText) findViewById(R.id.edit_ID);
        PW = (EditText) findViewById(R.id.edit_PW);
    }

    public static String getMacAddress(String interfaceName) {
        try {
            List<NetworkInterface> interfaces = Collections.list(NetworkInterface.getNetworkInterfaces());
            for (NetworkInterface intf : interfaces) {
                if (interfaceName != null) {
                    if (!intf.getName().equalsIgnoreCase(interfaceName))
                        continue;
                }
                byte[] mac = intf.getHardwareAddress();

                if (mac==null)
                    return "";

                StringBuilder buf = new StringBuilder();
                for (int idx = 0; idx < mac.length; idx++)
                    buf.append(String.format("%02X:", mac[idx]));
                if (buf.length()>0) buf.deleteCharAt(buf.length() - 1);
                return buf.toString();
            }
        } catch (Exception ex) { }
        return "";
    }

    public void onClickLogin(View view) {
        if (view == findViewById(R.id.btn_signIn)) {
            String loginData;

            loginData = ID.getText().toString() + " " + PW.getText().toString() + " "
                    + getMacAddress("wlan0");
            Mac.setText(loginData);
        }
    }
}