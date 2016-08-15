




#include <opencv2/opencv.hpp>
#include <stdio.h>

#include <android/log.h>
#include <string>
#include <jni.h>


#define LOG_TAG "FaceDetection/DetectionBasedTracker"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

using namespace cv;

extern "C" {

    JNIEXPORT jstring JNICALL
    Java_com_example_readimage_MainActivity_stringFromJNI
    (JNIEnv *env, jobject obj, jstring jPath)
    {

    	//std::stringstream timeStream;
    	//timeStream << time;
    	const string srcPath = string(env->GetStringUTFChars(jPath, 0));

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

        LOGD("Read Image native method started");
        //LOGD("Read Image native %d",time(0));


        Mat src = imread(srcPath+"1.jpg");
        LOGD("Read Image native method started: It may grabbed the image from");
        //LOGD("PPP::"+srcPath);
        LOGD(":D");
        Mat temp = src.clone();
        Mat gray,blur,rotate, crop;
        Size_<int> imgSize(src.rows,src.cols);
        LOGD("height:%d;width:%d",src.cols,src.rows);

        /* hunting white areas in our image */
        for(int i=0;i<(temp.rows);i++){
            for(int j=0;j<(temp.cols);j++){
                Vec3b pix = temp.at<Vec3b>(i, j);
                if( abs(pix.val[1]-pix.val[0]) < whiteFactor &&
                    abs(pix.val[2]-pix.val[0]) < whiteFactor &&
                    abs(pix.val[2]-pix.val[1]) < whiteFactor
                    ){
                    continue;
                }
                temp.at<Vec3b>(i, j) = (0,0,0);
            }
        }

        imwrite(srcPath+"_gray.jpg", temp);//####
		LOGD("gray image saved");//####

        cvtColor(temp, gray, CV_BGR2GRAY);
        imwrite(srcPath+"gray1.jpg", gray);//####
		LOGD("gray1 image saved");//####
        threshold( gray, gray, whiteThreshold, 255, THRESH_BINARY );
        imwrite(srcPath+"gray2.jpg", gray);//####
		LOGD("gray2 image saved");//####
        medianBlur(gray, blur, blurFactor);
        imwrite(srcPath+"gray3.jpg", blur);//####
		LOGD("gray3 image saved");//####

        std::vector<std::vector<Point> > contours;
        std::vector<Vec4i> hierarchy;
        findContours( blur, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );


        std::vector<RotatedRect> possibles;
    	for (int i=0; i<contours.size(); i++) {
    		std::vector<Point> contour = contours[i];
            if (minContourSize < contour.size()){
                RotatedRect newRect = minAreaRect(contour);
                Size_<float> size = newRect.size;
                if(minRectDiagonal < sqrtf(powf(size.width,2)+powf(size.height,2))){
                	possibles.insert(possibles.begin(), newRect );
                }
            }
        }

        LOGD("available possible rectangles:%d",possibles.size());

		/// Draw Rectangles
		Mat drawing =  src.clone();
		for( int i = 0; i< possibles.size(); i++ ){
			Scalar color = Scalar( 0+10*i, 255-10*i, 0+10*i);
			rectangle(drawing, possibles[i].boundingRect(), color, 5, 8, 0 );
		}
        imwrite(srcPath+"rect.jpg", drawing);//####
		LOGD("rects in image saved");//####


        RotatedRect newPix = possibles.at(0);
        if(newPix.size.width < newPix.size.height){
    		LOGD("shit has rotated.");//####
        	newPix.angle += 90.0;
        	std::swap(newPix.size.height,newPix.size.width);
        	std::swap(newPix.center.x,newPix.center.y);
        }
		LOGD(":::::width:%f,height:%f,centerX:%f,centerY:%f",newPix.size.width,newPix.size.height,newPix.center.x,newPix.center.y);//####
        temp = getRotationMatrix2D(newPix.center, newPix.angle, 1.0);
        warpAffine(src, rotate, temp, imgSize);
        imwrite(srcPath+"warp0.jpg", rotate);//####
		LOGD("warp0 saved");//####
        warpAffine(gray, gray, temp, imgSize);
        imwrite(srcPath+"warpG0.jpg", gray);//####
		LOGD("warpG0 saved");//####
        getRectSubPix(rotate, newPix.size, newPix.center, crop);
        imwrite(srcPath+"crop0.jpg", crop);//####
		LOGD("crop0 saved");//####
        getRectSubPix(gray, newPix.size, newPix.center, gray);
        imwrite(srcPath+"cropG0.jpg", gray);//####
		LOGD("cropG0 saved");//####
        Size_<int> defaultSize(defaultWidth,defaultHeight);
        resize(crop, crop, defaultSize );
        resize(gray, gray, defaultSize );


        LOGD("new:-height:%d;width:%d",gray.cols,gray.rows);
        imwrite(srcPath+"crop1.jpg", crop);//####
		LOGD("crop1 saved");//####
        imwrite(srcPath+"cropG1.jpg", gray);//####
		LOGD("cropG1 saved");//####

        for(int i=0;i<(gray.rows);i++){
    	    gray.at<uchar>(i, gray.cols-1) = 255;
        }
        for(int i=0;i<(gray.cols);i++){
    	    gray.at<uchar>(gray.rows-1, i) = 255;
        }



        int last = 0;
        for(int i=0;i<(gray.rows);i++){
            last = 0;
            for(int j=1;j<(gray.cols);j++){
            	uchar pix = gray.at<uchar>(i, j);
            	if(pix == 0){
            	    if((j-last) < strToStrWidth){
            	        for (int k=last ; k < j; k++){
            			gray.at<uchar>(i, k) = 0;
            		    }
            	    }
            	    last = j;
            	}
            }
        }

        for(int i=0;i<(gray.rows);i++){
            last = 0;
            for(int j=1;j<(gray.cols);j++){
            	uchar pix = gray.at<uchar>(i, j);
            	if(pix == 255){
            	    if((j-last) < minSubRectWidth){
            	        for (int k=last ; k < j; k++){
            			    gray.at<uchar>(i, k) = 255;
            		    }
            	    }
            	    last = j;
            	}
            }
        }


        for(int i=1;i<(gray.cols);i++){
            last = 0;
            for(int j=0;j<(gray.rows);j++){
            	uchar pix = gray.at<uchar>(j, i);
            	if(pix == 255){
            	    if((j-last) < minSubRectHeight){
            	        for (int k=last ; k < j; k++){
            			    gray.at<uchar>(k, i) = 255;
            		    }
            	    }
            	    last = j;
            	}
            }
        }


        std::vector<std::vector<Point> > contours1;
        std::vector<Vec4i> hierarchy1;
        findContours( gray, contours1, hierarchy1, RETR_TREE, CHAIN_APPROX_SIMPLE );

        //get only possible values
        std::vector<RotatedRect> possibles1;
    	for (int i=0; i<contours1.size(); i++) {
    		std::vector<Point> contour1 = contours1[i];
    		LOGD("available shapes:%d",contour1.size());
            if (minSubContourSize < contour1.size()){
    	    //printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                RotatedRect newRect = minAreaRect(contour1);
                Size_<float> size = newRect.size;
                if(minSubRectWidth < size.width && minSubRectHeight < size.height){
                	possibles1.insert(possibles1.begin(), newRect );
                }
            }
        }

    	LOGD("possible shapes:%d",possibles1.size());
        newPix = possibles1.at(0);
        if(newPix.size.width < newPix.size.height){
        	newPix.angle += 90.0;
        	std::swap(newPix.size.height,newPix.size.width);
        }
        LOGD("DDDDD%f\n",newPix.size.width);
        newPix.size.width +=edgeFillValue;
        newPix.size.height +=edgeFillValue;
        LOGD("DDDDDDDD%f\n",newPix.size.width);
        temp = getRotationMatrix2D(newPix.center, newPix.angle, 1.0);
        warpAffine(crop, rotate, temp, imgSize);
        warpAffine(gray, gray, temp, imgSize);
        getRectSubPix(rotate, newPix.size, newPix.center, crop);
        getRectSubPix(gray, newPix.size, newPix.center, gray);


        imwrite(srcPath+"_out.jpg", crop);
		LOGD("final image saved");//####

        //env->ReleaseStringUTFChars(jPath, srcPath);

        return env->NewStringUTF("Hello from C++ over JNI!");
    }

}

/*

jlong JNICALL Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeCreateObject
(JNIEnv * jenv, jclass, jstring jFileName, jint faceSize)
{
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeCreateObject enter");
    const char* jnamestr = jenv->GetStringUTFChars(jFileName, NULL);
    string stdFileName(jnamestr);
    jlong result = 0;

    try
    {
        DetectionBasedTracker::Parameters DetectorParams;
        if (faceSize > 0)
            DetectorParams.minObjectSize = faceSize;
        result = (jlong)new DetectionBasedTracker(stdFileName, DetectorParams);
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeCreateObject caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeCreateObject caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
        return 0;
    }

    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeCreateObject exit");
    return result;
}

JNIEXPORT void JNICALL Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeDestroyObject
(JNIEnv * jenv, jclass, jlong thiz)
{
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeDestroyObject enter");
    try
    {
        if(thiz != 0)
        {
            ((DetectionBasedTracker*)thiz)->stop();
            delete (DetectionBasedTracker*)thiz;
        }
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeestroyObject caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeDestroyObject caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
    }
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeDestroyObject exit");
}

JNIEXPORT void JNICALL Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeStart
(JNIEnv * jenv, jclass, jlong thiz)
{
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeStart enter");
    try
    {
        ((DetectionBasedTracker*)thiz)->run();
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeStart caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeStart caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
    }
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeStart exit");
}

JNIEXPORT void JNICALL Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeStop
(JNIEnv * jenv, jclass, jlong thiz)
{
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeStop enter");
    try
    {
        ((DetectionBasedTracker*)thiz)->stop();
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeStop caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeStop caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
    }
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeStop exit");
}

JNIEXPORT void JNICALL Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeSetFaceSize
(JNIEnv * jenv, jclass, jlong thiz, jint faceSize)
{
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeSetFaceSize enter");
    try
    {
        if (faceSize > 0)
        {
            DetectionBasedTracker::Parameters DetectorParams = \
            ((DetectionBasedTracker*)thiz)->getParameters();
            DetectorParams.minObjectSize = faceSize;
            ((DetectionBasedTracker*)thiz)->setParameters(DetectorParams);
        }
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeStop caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeSetFaceSize caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
    }
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeSetFaceSize exit");
}


JNIEXPORT void JNICALL Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeDetect
(JNIEnv * jenv, jclass, jlong thiz, jlong imageGray, jlong faces)
{
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeDetect enter");
    try
    {
        vector<Rect> RectFaces;
        ((DetectionBasedTracker*)thiz)->process(*((Mat*)imageGray));
        ((DetectionBasedTracker*)thiz)->getObjects(RectFaces);
        vector_Rect_to_Mat(RectFaces, *((Mat*)faces));
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeCreateObject caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeDetect caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
    }
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeDetect exit");
}

*/
