package com.example.tess_01;

import com.googlecode.tesseract.android.TessBaseAPI;

import android.os.Bundle;
import android.app.Activity;
import android.graphics.BitmapFactory;
import android.util.Log;
import android.view.Menu;
import android.widget.Toast;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // steps to invoke tesseract from android application 
        TessBaseAPI baseApi = new TessBaseAPI();
        // change the first argument to the place of the tesseract directory on your SDCARD
        baseApi.init("/mnt/sdcard/justscratch", "eng");
        baseApi.setVariable("tessedit_char_whitelist", "0123456789 ");
        // Image has to be ARGB_8888 or the next api call will fail
        baseApi.setImage(BitmapFactory.decodeFile("/mnt/sdcard/justscratch/out/00_00.jpg"));
        // this call below will return the recognized text
        Toast.makeText(this, baseApi.getUTF8Text(), Toast.LENGTH_LONG).show();
        Log.i("TTTTTTTTTTTTTTTTTTT",baseApi.getUTF8Text());
     }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.activity_main, menu);
        return true;
    }
    
}
