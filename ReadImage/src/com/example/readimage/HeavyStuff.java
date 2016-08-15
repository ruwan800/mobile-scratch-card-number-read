package com.example.readimage;

//import org.opencv.core.Mat;
//import org.opencv.highgui.Highgui;

import android.os.Environment;
import android.util.Log;



public class HeavyStuff {


	public void readImage(){
		Log.i("EEE","@readImage00");//####
		try{
//			Mat image = Highgui.imread(	Environment.getExternalStorageDirectory().getPath()+"/test/imgr/1.jpg");
		}
		catch (Exception e) {
			// TODO: handle exception
			Log.i("EEE","Cannot Read Image @readImage");//####
			
		}

		Log.i("EEE","@readImage01");//####
		
	}
		
		
		
}
