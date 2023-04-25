#pragma once

#ifndef MATTOQIMAGE_H
#define MATTOQIMAGE_H

// Qt
#include <QtGui/QImage>
// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

QImage MatToQImage(const Mat&);

#endif // MATTOQIMAGE_H
