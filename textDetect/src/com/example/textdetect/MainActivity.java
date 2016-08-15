package com.example.textdetect;

import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.view.Menu;
import com.example.textDetect.R;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		

		Log.i("EEE", "@oncreate00");
		detectTextArea();
		Log.i("EEE", "@oncreate01");
		
		
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_main, menu);
		return true;
	}

    public native void detectTextArea();
    /** Load the native library where the native method
    * is stored.
    */
    static {
          System.loadLibrary("just_scratch");
    }

}
