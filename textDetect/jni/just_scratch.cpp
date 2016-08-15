//============================================================================
// Name        : js_01.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

//#include <iostream>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <android/log.h>
#include <jni.h>

#define LOG_TAG "JS/cpp"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))


using namespace cv;
using namespace std;

extern "C" {

int findWhiteFilter(Mat src);
vector<Mat> findWhiteAreas(Mat src, int valX);
vector<Mat> findTextArea(Mat cropped);
vector<vector<Rect> > groupByYPosition(vector<vector<Rect> >);
vector<vector<Rect> > groupByHeight(vector<vector<Rect> >);
vector<vector<Rect> > groupByAdjacency(vector<vector<Rect> >);
vector<vector<Rect> > filterByHeight(vector<vector<Rect> >);

bool sortFunctionY (Rect i,Rect j) { return (i.y<j.y); }
bool sortFunctionH (Rect i,Rect j) { return (i.height<j.height); }
bool sortFunctionX (Rect i,Rect j) { return (i.x<j.x); }

JNIEXPORT void JNICALL
    Java_com_example_textdetect_MainActivity_detectTextArea
    (JNIEnv *env, jobject obj){

	LOGD("Native method started");
	//const char* src_file_name = "/home/first_volume/PROJECTS/data/js/1/2.1.jpg";
	//const char* gray_file_name = "/home/first_volume/PROJECTS/data/js/1/2.jpg";

	for (int i = 0; i < 1; ++i) {
		char outimg[200];
		sprintf(outimg, "/mnt/sdcard/justscratch/in/%02d.jpg",i);
		Mat src = imread(outimg);
		LOGD("Image Read");
		int valX = findWhiteFilter(src);
		vector<Mat> whites = findWhiteAreas(src,valX);
		for (int j = 0; j < (int)whites.size(); ++j) {
			vector<Mat> textMats = findTextArea(whites.at(j));
			for (int k = 0; k < textMats.size(); ++k) {
				sprintf(outimg, "/mnt/sdcard/justscratch/out/%02d_%02d.jpg",j,k);
				LOGD("writing: %s",outimg);
				imwrite(outimg,textMats.at(k));

			}
		}
		//cout << "~~~~~~~~~~~~~~~~~ end  ~~~~~~~~~~~~~~~~~~~~\n" <<endl;
	}
	LOGD("Native method completed");



	/// Initialize values

	//imshow( "Linear Blend", drawing );//####
	//imwrite( "/home/ruwan/Desktop/js.png", src_gray );//####



	//imwrite(gray_file_name,src_hsv);




	/// Wait until user press some key
	//waitKey(0);

}

/*
 * Identifying a value to set in inrange() filter value.
 */
int findWhiteFilter(Mat src){

	int valueX=0;
	Mat src_hsv, thresh, tmp, blured;

	cvtColor( src, src_hsv, CV_BGR2HSV );

	Mat ch1, ch2, ch3;
	vector<Mat> channels(3);
	blur(src, blured, Size( 21, 21 ), Point(-1,-1), BORDER_DEFAULT);
	split(blured, channels);

	ch1 = channels[0];
	ch2 = channels[1];
	ch3 = channels[2];
	int count[256];
	for (int i = 0; i < 256; ++i) {
		count[i] = 0;
	}
	bitwise_and(ch1,ch2,ch1);
	bitwise_and(ch1,ch3,ch1);
	for (int i = 0; i < ch1.rows; ++i) {
		for (int j = 0; j < ch1.cols; ++j) {
			count[ch1.at<uchar>(i,j)] ++;
		}
	}
	int all = 0;
	int allSet = ch1.cols*ch1.rows;
	for (int i = 0; i < 256; ++i) {
		all += count[i];
		if (90 < all*100/allSet ){
			valueX = i;
			break;
		}
	}

	LOGD("White filter value: %d",valueX);
	return valueX;
}

/**
 *  Identifying white area.
 */
vector<Mat> findWhiteAreas(Mat src, int valueX){

	vector<Mat> results;
	int minContourSize = 100;
	int minRectDiagonal;
	Mat src_hsv, thresh, tmp, blured;

	int rWidth;
	if(src.cols < src.rows){
		minRectDiagonal = src.cols*2/3;
		rWidth = src.rows;
	}
	else{
		minRectDiagonal = src.rows*2/3;
		rWidth = src.cols;
	}

	inRange(src, Scalar(valueX, valueX, valueX), Scalar(255, 255, 255), src_hsv);

	std::vector<std::vector<Point> > contours;
	std::vector<Vec4i> hierarchy;
	Mat cont_img = src_hsv.clone();
	findContours( cont_img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );
	//cout << "All white areas identified::" << contours.size() << endl;//####

	//filtering contours.
	std::vector<RotatedRect> possibles;
	for (int i=0; i< (int) contours.size(); i++) {
		std::vector<Point> contour = contours[i];
		if (minContourSize < (int) contour.size()){
			RotatedRect newRect = minAreaRect(contour);
			Size_<float> size = newRect.size;
			if(minRectDiagonal < size.width || minRectDiagonal < size.height){
				possibles.push_back( newRect );
			}
		}
	}
	//cout << "Suitable white areas identified::" << possibles.size() << endl;//####

	Mat rotated(rWidth, rWidth, src.type());
    Size_<int> imgSize(rotated.rows,rotated.cols);
	for( int i = 0; i< (int) possibles.size(); i++ ){
		int newAngle = 0;
		if(possibles[i].size.width < possibles[i].size.height){
			newAngle = 90;
		}
		Point2f pts[4];
		Mat rmat = getRotationMatrix2D(possibles[i].center, possibles[i].angle+newAngle, 1);
		warpAffine(src, rotated, rmat, imgSize );

		possibles[i].angle = newAngle;
		possibles[i].points(pts);
		Rect zRect(pts[0],pts[2]);
		results.push_back(rotated(zRect));

	}

	LOGD("Found White areas: %d",results.size());
	return results;
}

/**
 * Identifying areas which contains text.
 */
vector<Mat> findTextArea(Mat cropped_image){

	Mat cropped;
	resize(cropped_image, cropped, Size(2000,cropped_image.rows*2000/cropped_image.cols), 0, 0, INTER_CUBIC);
	LOGD("@findTextArea");//####
	vector<Mat> result;
	Mat cpSrc = cropped.clone();
	Mat cropped_gray, thresh_gray;
	cvtColor( cropped, cropped_gray, CV_BGR2GRAY );
	int img_mean = mean(cropped_gray)[0];
	int thresh_value = 20+(img_mean*17/23);
	threshold(cropped_gray, thresh_gray, thresh_value, 255, THRESH_BINARY );
	//cout << "Applied threshold value to detect text area::" << thresh_value << endl;//####


	//finding contours and generating bounding rectangles vector.
	std::vector<std::vector<Point> > contours;
	std::vector<Vec4i> hierarchy;
	Mat drawing =  thresh_gray.clone();
	findContours( thresh_gray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );

	Mat temp =  cropped.clone();									//####
	drawContours(temp, contours, -1, Scalar(0,255,0));				//####
	imwrite("/mnt/sdcard/justscratch/test/contours_01.jpg",temp);	//####

	vector<Rect> hzPts;

	Mat temp2 =  cropped.clone(); //####

	for( int i = 0; i< (int)contours.size(); i++ ){
		std::vector<Point> contour = contours[i];
		RotatedRect newRect = minAreaRect(contour);
		Rect br = newRect.boundingRect();
		hzPts.push_back(br);

		rectangle(temp2,br,Scalar(255-((i*8)%255), (i*64)%255, (i*255)%255)); //####


	}

	imwrite("/mnt/sdcard/justscratch/test/rects_0_01.jpg",temp2);	//####
	//cout << "Identifying bounding recangles::" << contours.size() << endl;



	LOGD("L,SP: %d",hzPts.size());//####
	vector<vector<Rect> > input;
	input.push_back(hzPts);
	vector<vector<Rect> > filter01 = filterByHeight(input);
	LOGD("L,SP: %d",filter01.at(0).size());//####

	Mat temp3 =  cropped.clone(); //####
	for (int i = 0; i < filter01.size(); ++i) {//####
		vector<Rect> tempRect = filter01.at(i);//####
		for (int j = 0; j < tempRect.size(); ++j) {//####
			rectangle(temp3,tempRect[j],Scalar(255-((i*8)%255), (i*64)%255, (i*128)%255)); //####
		}//####
	}//####
	imwrite("/mnt/sdcard/justscratch/test/rects_1_01.jpg",temp3);	//####


	LOGD("L2: %d",filter01.size());//####


	vector<vector<Rect> > filter02 = groupByYPosition(filter01);


	Mat temp4 =  cropped.clone(); //####
	for (int i = 0; i < filter02.size(); ++i) {//####
		vector<Rect> tempRect = filter02.at(i);//####
		for (int j = 0; j < tempRect.size(); ++j) {//####
			rectangle(temp4,tempRect[j],Scalar(255-((i*36)%255), (i*80)%255, (i*120)%255)); //####
		}//####
	}//####
	imwrite("/mnt/sdcard/justscratch/test/rects_2_01.jpg",temp4);	//####

	LOGD("L3: %d",filter02.size());//####


	vector<vector<Rect> > filter03 = groupByAdjacency(filter02);

	Mat temp5 =  cropped.clone(); //####
	for (int i = 0; i < filter03.size(); ++i) {//####
		vector<Rect> tempRect = filter03.at(i);//####
		for (int j = 0; j < tempRect.size(); ++j) {//####
			rectangle(temp5,tempRect[j],Scalar(255-((i*36)%255), (i*80)%255, (i*120)%255)); //####
		}//####
	}//####
	imwrite("/mnt/sdcard/justscratch/test/rects_3_01.jpg",temp5);	//####


	LOGD("L4: %d",filter03.size());//####
	//Generating all identified text areas
	for (int i = 0; i < (int) filter03.size(); ++i) {
		vector<Rect> hzPtCurrent = filter03.at(i);
		Rect begin = hzPtCurrent.front();
		Rect end = hzPtCurrent.back();
		float yMean = 0, hMean = 0;
		for(int j = 0; j < (int) hzPtCurrent.size(); ++j){
			yMean += hzPtCurrent[i].y;
			hMean += hzPtCurrent[i].height;
		}
		yMean = yMean/hzPtCurrent.size();
		hMean = hMean/hzPtCurrent.size();
		Point a0(begin.x-begin.height, yMean-hMean/8);
		Point a2(end.x+end.height, yMean+hMean*5/4);
		Rect zRect(a0,a2);
		LOGD("zRect.width:%d",zRect.width);//####
		if(200 < zRect.width){
			cv::Mat textImage = cropped(zRect);
			result.push_back(textImage);
		}
	}
	LOGD("Found text areas: %d",result.size());
	return result;
}

vector<vector<Rect> > groupByYPosition(vector<vector<Rect> > rects){
	//generating textareas by grouping bounding rects based on y position.
	vector<vector<Rect> > output;
	for (int i = 0; i < rects.size(); ++i) {
		vector<Rect> hzPtCurrent = rects.at(i);
		vector<Rect> tempRect;
		sort(hzPtCurrent.begin(), hzPtCurrent.end(), sortFunctionY);
		int previous = 0;
		for (int j = 0; j < (int)hzPtCurrent.size(); ++j) {
			Rect pt = hzPtCurrent.at(j);
			if(pt.y-previous < 20 || previous==0){
				tempRect.push_back(hzPtCurrent.at(j));
				LOGD("%3d+++++%3d::%3d::%3d::%3d",j, pt.height ,pt.width, pt.x, pt.y );//#############
			}
			else{
				LOGD("%3d-----%3d::%3d::%3d::%3d",j, pt.height ,pt.width, pt.x, pt.y );//#############
				if(10 <= tempRect.size()){
					output.push_back(tempRect);
				}
				tempRect.clear();
				tempRect.push_back(hzPtCurrent.at(j));
			}
			previous = pt.y;
		}
		if(10 <= tempRect.size()){
			output.push_back(tempRect);
			previous = 0;
		}
		tempRect.clear();
	}
	//cout << "TextArea filtering Level 01::" << hzPtNested.size() << endl;
	return output;
}

vector<vector<Rect> > groupByHeight(vector<vector<Rect> > rects){
	//filtering textareas based on bounding rects' height.
	vector<vector<Rect> > output;
	for (int i = 0; i < (int) rects.size(); ++i) {
		vector<Rect> hzPtCurrent = rects.at(i);
		vector<Rect> tempRect;
		sort(hzPtCurrent.begin(),hzPtCurrent.end(), sortFunctionH);
		int previous = 0;
		for (int j = 0; j < (int) hzPtCurrent.size(); ++j) {
			Rect pt = hzPtCurrent.at(j);
			LOGD("%d",pt.height);//#############
			if(pt.height-previous < 10){
				tempRect.push_back(pt);
			}
			else{
				if(10 <= tempRect.size()){
					output.push_back(tempRect);
				}
				tempRect.clear();
				tempRect.push_back(pt);
			}
			previous = pt.height;
		}
		if(10 <= tempRect.size()){
			output.push_back(tempRect);
		}
		tempRect.clear();
	}
	//cout << "TextArea filtering Level 02::" << hzPtNested2.size() << endl;
	return output;
}

vector<vector<Rect> > groupByAdjacency(vector<vector<Rect> > rects){

	//filtering textareas based on x distance of adjacent bounding rects.
	vector<vector<Rect> > output;
	for (int i = 0; i < (int) rects.size(); ++i) {
		vector<Rect> hzPtCurrent = rects.at(i);
		vector<Rect> tempRect;
		sort(hzPtCurrent.begin(),hzPtCurrent.end(), sortFunctionX);
		int previous = 0;
		for (int j = 0; j < (int) hzPtCurrent.size(); ++j) {
			Rect pt = hzPtCurrent.at(j);
			if(pt.x-previous < pt.height || previous == 0){
				tempRect.push_back(pt);
				previous = pt.x;
			}
			else{
				if(8 <= tempRect.size()){
					output.push_back(tempRect);
				}
				tempRect.clear();
				tempRect.push_back(pt);
				previous = pt.x;
			}
		}
		if(8 <= tempRect.size()){
			output.push_back(tempRect);
		}
		tempRect.clear();
	}
	//cout << "TextArea filtering Level 03::" << hzPtNested3.size() << endl;
	return output;
}


vector<vector<Rect> > filterByHeight(vector<vector<Rect> > rects){
	//filtering textareas based on bounding rects' height threshold.
	vector<vector<Rect> > output;
	for (int i = 0; i < (int) rects.size(); ++i) {
		vector<Rect> hzPtCurrent = rects.at(i);
		vector<Rect> tempRect;
		LOGD("L,SP1: %d",rects.at(i).size());//####
		sort(rects.at(i).begin(),rects.at(i).end(), sortFunctionH);
		LOGD("L,SP2: %d",rects.at(i).size());//####
		for (int j = 0; j < (int) rects.at(i).size(); ++j) {
			Rect pt = rects.at(i).at(j);
			if(60 < pt.height && pt.height < 160 ){
				tempRect.push_back(pt);
				LOGD("%3d+++++%3d::%3d::%3d::%3d",j, pt.height ,pt.width, pt.x, pt.y );//#############
				//rects.at(i).erase(rects.at(i).begin()+j);
			}
			else{
				LOGD("%3d-----%3d::%3d::%3d::%3d:",j, pt.height ,pt.width, pt.x, pt.y );//#############

			}
		}
		output.push_back(tempRect);
		tempRect.clear();
	}
	return output;
}

vector<vector<Rect> > groupByYPositionWithHeight(vector<vector<Rect> > rects){
	//filtering textareas based on bounding rects' height.
	vector<vector<Rect> > output;
	for (int i = 0; i < (int) rects.size(); ++i) {
		vector<Rect> hzPtCurrent = rects.at(i);
		vector<Rect> tempRect;
		sort(hzPtCurrent.begin(), hzPtCurrent.end(), sortFunctionY);
		sort(hzPtCurrent.begin(),hzPtCurrent.end(), sortFunctionH);
		int previous = 0;
		for (int j = 0; j < (int) hzPtCurrent.size(); ++j) {
			Rect pt = hzPtCurrent.at(j);
			LOGD("%d",pt.height);//#############
			if(pt.height-previous < 10){
				tempRect.push_back(pt);
			}
			else{
				if(10 <= tempRect.size()){
					output.push_back(tempRect);
				}
				tempRect.clear();
			}
			previous = pt.height;
		}
		if(10 <= tempRect.size()){
			output.push_back(tempRect);
		}
		tempRect.clear();
	}
	//cout << "TextArea filtering Level 02::" << hzPtNested2.size() << endl;
	return output;
}

}

