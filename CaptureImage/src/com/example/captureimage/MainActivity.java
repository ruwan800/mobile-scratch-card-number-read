package com.example.captureimage;



import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.app.Activity;
import android.content.pm.PackageManager;
import android.hardware.Camera;
import android.hardware.Camera.AutoFocusCallback;
import android.hardware.Camera.PictureCallback;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;

public class MainActivity extends Activity {
	private Camera mCamera;
private CameraPreview mPreview;

@Override
public void onCreate(Bundle savedInstanceState) {
	super.onCreate(savedInstanceState);
	setContentView(R.layout.activity_main);
	
	// Create an instance of Camera
	mCamera = getCameraInstance();
	
	// Create our Preview view and set it as the content of our activity.
	mPreview = new CameraPreview(this, mCamera);
	FrameLayout preview = (FrameLayout) findViewById(R.id.camera_preview);
	preview.addView(mPreview);


	//Add a listener to the Capture button
	Button captureButton = (Button) findViewById(R.id.button_capture);
	captureButton.setOnClickListener(
	    new View.OnClickListener() {
	        @Override
	        public void onClick(View v) {
	            // get an image from the camera
	        	mCamera.autoFocus(capturePhoto);
	        }
	    }
	);}

	AutoFocusCallback capturePhoto = new AutoFocusCallback(){

		@Override
		public void onAutoFocus(boolean arg0, Camera arg1) {
			// TODO Auto-generated method stub
			Log.i("JS","autofocus success: "+arg0);
			arg1.takePicture(null, null, mPicture);
		}
	};

@Override
	protected void onPause() {
		if (mCamera != null) {
			mCamera.release();
			mCamera = null;
		}
		super.onPause();
	}
	
	
	/** A safe way to get an instance of the Camera object. */
	public Camera getCameraInstance(){
		if(! checkCameraHardware()){
			return null;
		}
	Camera c = null;
	try {
	c = Camera.open(); // attempt to get a Camera instance
		}
		catch (Exception e){
		// Camera is not available (in use or does not exist)
		}
		return c; // returns null if camera is unavailable
	}
	
	
	/** Check if this device has a camera */
	private boolean checkCameraHardware() {
		if (getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA)){
			// this device has a camera
			return true;
		} else {
			// no camera on this device
			return false;
		}
	}
	
	
	private PictureCallback mPicture = new PictureCallback() {
	
		@Override
		public void onPictureTaken(byte[] data, Camera camera) {
	
			File pictureFile = new File("/mnt/sdcard/justscratch/in/00.jpg");

		
			try {
				FileOutputStream fos = new FileOutputStream(pictureFile);
				fos.write(data);
				fos.close();
			} catch (FileNotFoundException e) {
				Log.d("JS", "File not found: " + e.getMessage());
			} catch (IOException e) {
				Log.d("JS", "Error accessing file: " + e.getMessage());
			}
		}
	};
	
	

} 