#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>
#include <cstdio>

using namespace std;
using namespace cv;

extern "C" {
  void faceDetect(void *data);
  int initFaceDetect(int width, int height, float scalefact, int minneigh, int i_minsz, int i_maxsz, char i_tracking);
}

/* Global variables */
static String face_cascade_name = "data/lbpcascades/lbpcascade_frontalface.xml";
// String eyes_cascade_name = "data/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
static CascadeClassifier face_cascade;
// static CascadeClassifier eyes_cascade;
static Mat frame;

static float scaleFactor = 1.3;	// How much the image is reduced each scale
static int minNeighbors = 2;	// How many neighbors a candidate should have to be valid
static int minsz = 20;		    // Minimum search size
static int maxsz = 40;		    // Minimum search size
static char enabletracking = 1; // If tracking has to be used instead of raw Viola Jones


int initFaceDetect(int width, int height, float scalefact, int minneigh, int i_minsz, int i_maxsz, char i_tracking)
{
  if (!face_cascade.load(face_cascade_name))
    {
      printf("--(!)Error loading face cascade\n");
      return -1;
    }
  // if (!eyes_cascade.load(eyes_cascade_name))
  //   {
  //     printf("--(!)Error loading eyes cascade\n");
  //     return -1;
  //   }
  frame = cv::Mat(height, width, CV_8U);

  scaleFactor = scalefact;
  minNeighbors = minneigh;
  minsz = i_minsz;
  maxsz = i_maxsz;
  enabletracking = i_tracking;
  return 0;
}

void exportFaceData(int x, int y, int w, int h)
{
  printf("x:%d, y:%d, w:%d, h:%d\n", x, y, w, h);
}

void searchDisplayFace(Mat &out, Mat &in, float alpha)
{
  std::vector<Rect> faces;
  Mat *detectIn = &in;

  if (alpha)
  {
    Mat matRotation = getRotationMatrix2D(Point(out.cols / 2, out.rows / 2), alpha, 1);
    Mat frame_rot;
    // Rotate the image
    warpAffine(in, frame_rot, matRotation, frame.size());
    detectIn = &frame_rot;
  }
  // Detect faces
  face_cascade.detectMultiScale(*detectIn, faces, scaleFactor, minNeighbors, CV_HAAR_SCALE_IMAGE, Size(minsz, minsz), Size(maxsz, maxsz));

  for(size_t i = 0; i < faces.size(); i++)
  {
    Point center(faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2);

    if (alpha)
    {
      float ralpha = alpha * M_PI / 180.0f; // Convert to radians for math functions
      center.x -= frame.cols / 2;
      center.y -= frame.rows / 2;
      center = Point(center.x * cos(ralpha) - center.y * sin(ralpha) + frame.cols / 2, center.x * sin(ralpha) + center.y * cos(ralpha) + frame.rows / 2);
    }
    ellipse(out, center, Size(faces[i].width/2, faces[i].height/2), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
    exportFaceData(faces[i].x, faces[i].y, faces[i].width, faces[i].height);
  }
}

struct DetectedFace
{
  float x;
  float y;
  float w;
  float h;
  int nframes;
  bool updated;

  DetectedFace(float px, float py, float pw, float ph)
    : x(px), y(py), w(pw), h(ph), nframes(1), updated(true)
  { }

  bool update(float px, float py, float pw, float ph)
  {
    float ratioThreshold = 0.75;
    float pcx = px + pw / 2;
    float pcy = py + ph / 2;

    if (pcx <= x || pcx >= x + w ||
        pcy <= y || pcy >= y + h)
      return false;
    if (min(pw, w) / max(pw, w) < ratioThreshold ||
        min(ph, h) / max(ph, h) < ratioThreshold)
      return false;
    x = px;
    y = py;
    w = pw;
    h = ph;
    nframes++;
    updated = true;
    return true;
  }
};

void searchTrackFace(Mat &out, Mat &in)
{
  static std::vector<DetectedFace> detectedFaces;
  static int framecounter = 0;
  static Mat prevImg;
  static std::vector<cv::Point2f> prevPts, nextPts;
  static bool tracking = false;

  std::vector<Rect> faces;
  Mat frame_gray_eq;
  equalizeHist(in, frame_gray_eq);

  for (DetectedFace &dface : detectedFaces)
    dface.updated = false;

  for (float alpha = 0; alpha <= 30.0; alpha += 45.0)
    {
      float ralpha = alpha * M_PI / 180.0f;

      Mat matRotation = getRotationMatrix2D(Point(in.cols / 2, in.rows / 2), alpha, 1);
      // Rotate the image
      Mat frame_gray;
      warpAffine(frame_gray_eq, frame_gray, matRotation, frame_gray.size());

      if (!tracking)
        {
          //-- Detect faces
          face_cascade.detectMultiScale(frame_gray, faces, scaleFactor, minNeighbors, CV_HAAR_SCALE_IMAGE, Size(minsz, minsz), Size(maxsz, maxsz));
          for(size_t i = 0; i < faces.size(); i++)
            {
              Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);

              center.x -= in.cols / 2;
              center.y -= in.rows / 2;
              center = Point(center.x * cos(ralpha) - center.y * sin(ralpha) + in.cols / 2,
                  center.x * sin(ralpha) + center.y * cos(ralpha) + in.rows / 2);

              cv::Point2f p1(faces[i].x, faces[i].y);
              cv::Point2f p2(faces[i].width, faces[i].height);

              bool updated = false;
              for (DetectedFace &dface : detectedFaces)
              {
                if (dface.update(p1.x, p1.y, p2.x, p2.y))
                {
                  updated = true;
                  break;
                }
              }
              if (!updated)
                detectedFaces.push_back(DetectedFace(p1.x, p1.y, p2.x, p2.y));

              exportFaceData(p1.x, p1.y, p2.x, p2.y);
              cv::rectangle(out, p1, p1 + p2, Scalar(255, 0, 255), 4, 8, 0);
            }
        }
      else
        {
          std::vector<unsigned char> status;
          std::vector<float> err;
          cv::Point pmin, pmax;
          bool boundsInit = false;
          prevPts = nextPts;

          cv::calcOpticalFlowPyrLK(prevImg, in, prevPts, nextPts, status, err);
          for (int pti = nextPts.size() - 1; pti >= 0; pti--)
          {
            cv::Point2f &pt = nextPts[pti];

            if (pt.x <= 0 || pt.x >= in.cols - 1 ||
                pt.y <= 0 || pt.y >= in.rows - 1)
            {
              nextPts.erase(nextPts.begin() + pti);
              continue;
            }

            if (!boundsInit || pt.x < pmin.x) pmin.x = pt.x;
            if (!boundsInit || pt.y < pmin.y) pmin.y = pt.y;
            if (!boundsInit || pt.x > pmax.x) pmax.x = pt.x;
            if (!boundsInit || pt.y > pmax.y) pmax.y = pt.y;
            boundsInit = true;

            cv::circle(out, pt, 3, cv::Scalar(0, 255, 0), -1, 8);
          }
          cv::rectangle(out, pmin, pmax, Scalar(255, 0, 255), 4, 8, 0);
          exportFaceData(pmin.x, pmin.y, pmax.x - pmin.x, pmax.y - pmin.y);
          if (nextPts.size() <= 5)
          {
            tracking = false;
            detectedFaces.clear();
          }
        }
    }

  for (int i = detectedFaces.size() - 1; i >= 0 && !tracking; i--)
  {
    DetectedFace &dface = detectedFaces[i];
    
    if (!dface.updated)
      detectedFaces.erase(detectedFaces.begin() + i);
    else if (dface.nframes > 15)
    {
      tracking = true;

      Mat roi = frame_gray_eq(Rect(dface.x, dface.y, dface.w, dface.h));

      float minDistance = 10.0;
      float qualityLevel = 0.01;
      int maxCorners = 20;

      cv::goodFeaturesToTrack(roi, nextPts, maxCorners, qualityLevel, minDistance, Mat(), 3, 0, 0.04);
      cv::cornerSubPix(roi, nextPts, Point(10, 10), Point(-1, -1), TermCriteria(TermCriteria::COUNT|TermCriteria::EPS,20,0.03));

      // Translate to global coordinates
      for (cv::Point2f &pt : nextPts)
        pt += cv::Point2f(dface.x, dface.y);
      i = -1;
      detectedFaces.clear();
    }
  }
  
  framecounter++;
  prevImg = in.clone();
}

void faceDetect(void *data)
{
  memcpy(frame.data, data, frame.rows * frame.cols);

  flip(frame, frame, 0);
  Mat frame_cpy(frame);
  equalizeHist(frame_cpy, frame_cpy);

  if (enabletracking)
    searchTrackFace(frame_cpy, frame);
  else
    searchDisplayFace(frame_cpy, frame, 0.0f);

  imshow("camcvWin", frame_cpy);
}

