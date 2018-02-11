package com.geecko.fpcalc;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import static com.geecko.fpcalc.FpCalc.fpCalc;

public class MainActivity extends Activity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("fpcalc");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        Log.i("FINGERPRINT", "test" + fpcalc("/sdcard/Download/02 Under Your Spell.mp3"));

    }
    private static String fpcalc(String path) {

        String[] args = {"-length", "16", path};
        String output;
        try {
            long startTime = System.nanoTime();
            output = fpCalc(args);
            long endTime = System.nanoTime();
            long duration = (endTime - startTime) / 1000000;
            if (path.toLowerCase().contains("spell")) Log.d("qllogging", "" + duration);
        } catch (Exception e) {
            output = null;
            Log.e("chromaprint", "Exception when executing fpcalc" + e);
        }
        return output;
    }


}
