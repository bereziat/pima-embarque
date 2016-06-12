#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/video.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/* Function Headers */
void detectAndDisplay( Mat frame );
/* Global variables */
String face_cascade_name = "data/lbpcascades/lbpcascade_frontalface.xml";
String eyes_cascade_name = "data/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
String window_name = "Capture - Face detection";

/* @function main */
int main( void )
{
  VideoCapture capture;
  Mat frame;
  //-- 1. Load the cascades
  if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };
  if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading eyes cascade\n"); return -1; };
  //-- 2. Read the video stream
  capture.open( -1 );
  if ( ! capture.isOpened() ) { printf("--(!)Error opening video capture\n"); return -1; }
  while (  capture.read(frame) )
    {
      if( frame.empty() )
        {
          printf(" --(!) No captured frame -- Break!");
          break;
        }
      //-- 3. Apply the classifier to the frame
      detectAndDisplay( frame );
      int c = waitKey(10);
      if( (char)c == 27 ) { break; } // escape
    }
  return 0;
}

/* @function detectAndDisplay */
void detectAndDisplay( Mat frame )
{
  static int framecounter = 0;
  static Mat prevImg;
  static std::vector<cv::Point2f> prevPts, nextPts;
  static bool tracking = false;

  std::vector<Rect> faces;
  Mat frame_gray_orig;
  cvtColor( frame, frame_gray_orig, COLOR_BGR2GRAY );
  Mat frame_gray_eq;
  equalizeHist( frame_gray_orig, frame_gray_eq );

  for (float alpha = 0; alpha <= 30.0; alpha += 45.0)
    {
      float ralpha = alpha * M_PI / 180.0f;

      Mat matRotation = getRotationMatrix2D(Point(frame.cols / 2, frame.rows / 2), alpha, 1);
      // Rotate the image
      Mat frame_gray;
      warpAffine(frame_gray_eq, frame_gray, matRotation, frame_gray.size());

      if (!tracking)
        {
          //-- Detect faces
          face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0, Size(30, 30) );
          for( size_t i = 0; i < faces.size(); i++ )
            {
              Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );

              center.x -= frame.cols / 2;
              center.y -= frame.rows / 2;
              center = Point(center.x * cos(ralpha) - center.y * sin(ralpha) + frame.cols / 2, center.x * sin(ralpha) + center.y * cos(ralpha) + frame.rows / 2);

              cv::Point2f p1(faces[i].x, faces[i].y);
              cv::Point2f p2(faces[i].width, faces[i].height);
              Mat roi = frame_gray(Rect(p1.x, p1.y, p2.x, p2.y));
              cv::rectangle(frame, p1, p1 + p2, Scalar(255, 0, 255), 4, 8, 0);

              float minDistance = 10.0;
              float qualityLevel = 0.01;
              int maxCorners = 20;

              cv::goodFeaturesToTrack(roi, nextPts, maxCorners, qualityLevel, minDistance, Mat(), 3, 0, 0.04);
              cv::cornerSubPix(roi, nextPts, Point(10, 10), Point(-1, -1), TermCriteria(TermCriteria::COUNT|TermCriteria::EPS,20,0.03));

              for (cv::Point2f &pt : nextPts)
                {
                  pt += p1;
                  cv::circle(frame, pt, 2, cv::Scalar(0, 0, 255), -1);
                }
              if (framecounter > 50)
                tracking = true;
            }

          // ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
        }
      else
        {
          std::vector<unsigned char> status;
          std::vector<float> err;
          cv::Point pmin, pmax;
          bool boundsInit = false;
          prevPts = nextPts;

          cv::calcOpticalFlowPyrLK(prevImg, frame_gray_orig, prevPts, nextPts, status, err);
          for (cv::Point2f &pt : nextPts)
            {
              if (!boundsInit || pt.x < pmin.x)
                pmin.x = pt.x;
              if (!boundsInit || pt.y < pmin.y)
                pmin.y = pt.y;
              if (!boundsInit || pt.x > pmax.x)
                pmax.x = pt.x;
              if (!boundsInit || pt.y > pmax.y)
                pmax.y = pt.y;
              boundsInit = true;

              cv::circle(frame, pt, 3, cv::Scalar(0, 255, 0), -1, 8);
            }
          cv::rectangle(frame, pmin, pmax, Scalar(255, 0, 255), 4, 8, 0);
        }

    }
  //-- Show what you got
  imshow( window_name, frame );
  framecounter++;
  prevImg = frame_gray_orig.clone();
}
