package com.example.readimage;

import java.io.File;

import android.os.Bundle;
import android.os.Environment;
import android.app.Activity;
import android.util.Log;
import android.view.Menu;

//import org.opencv.android.OpenCVLoader;
//import org.opencv.core.Mat;
//import org.opencv.core.Scalar;
//import org.opencv.core.Size;
//import org.opencv.highgui.Highgui;

public class MainActivity extends Activity {

	private HeavyStuff hs;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
//		if (!OpenCVLoader.initDebug()) {
//			Log.d("CVerror", "OpenCV library Init failure");
//		} else {
			// load your library and do initializing stuffs like
			// System.loadLibrary();
//		}
		Log.i("EEE", "@oncreate00");
		readImage();
		Log.i("EEE", "@oncreate01");
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_main, menu);
		return true;
	}

	public void readImage() {

		int whiteFactor = 25;
		int whiteThreshold = 100;
		int blurFactor = 101;
		int minRectDiagonal = 1000;
		int minSubRectWidth = 800;
		int strToStrWidth = 100;
		int minSubRectHeight = 60;
		int minContourSize = 100;
		int minSubContourSize = 10;
		double edgeFillValue = 25;
		int defaultWidth = 1600;
		int defaultHeight = 500;
//		Mat src = null;
//		Mat temp = null;
//		Mat gray, blur, rotate, crop;
//		Size imgSize;
//		Scalar pix;

		Log.i("EEE", "@readImage00");// ####
		try {
//			src = Highgui.imread(Environment.getExternalStorageDirectory()
//					.toString() + "/test/imgr/1.jpg");
		} catch (Exception e) {
			// TODO: handle exception
			Log.i("EEE", "Cannot Read Image @readImage");// ####

		}
		Log.i("EEE", "@readImage01");// ####

//		if (src == null) {
//			Log.i("EEE", "Eimage nullllll");// ####
//		} else {
//			Log.i("EEE", "E " + src.cols());// ####
//			temp = src.clone();
//			imgSize = new Size(src.rows(), src.cols());

			String path = Environment.getExternalStorageDirectory().toString() + "/test/imgr/";
            Log.i("EEE","E"+stringFromJNI(path));
			
			
			/* hunting white areas in our image */
			/*for (int i = 0; i < (temp.rows()); i++) {
				for (int j = 0; j < (temp.cols()); j++) {
					if (j == 0 && i % 100 == 0) {
						Log.i("EEE", i + "::" + j);// ####
					}
					pix = new Scalar(temp.get(i, j));
					if (Math.abs(pix.val[1] - pix.val[0]) < whiteFactor
							&& Math.abs(pix.val[2] - pix.val[0]) < whiteFactor
							&& Math.abs(pix.val[2] - pix.val[1]) < whiteFactor) {
						continue;
					}
					// temp.at<Vec3b>(i, j) = (0,0,0);
				}*/
			//}

		//}

//		Log.i("EEE", "@readImage02");// ####

	}

    public native String stringFromJNI(String path);
    /** Load the native library where the native method
    * is stored.
    */
    static {
          System.loadLibrary("read_image_jni");
    }

}
