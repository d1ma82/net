#pragma once

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

typedef std::vector<std::vector<cv::Point>> Contours;

static cv::Scalar randomColor(cv::RNG& rng) {

        auto icolor = (unsigned) rng;
        return cv::Scalar(icolor&255, (icolor>>8)&255, (icolor>>16)&255);
}

cv::Mat binarize(cv::Mat input) {
        
    cv::cvtColor(input, input, cv::COLOR_BGR2GRAY);
    cv::Mat binary;
    cv::threshold(input, binary, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    int whites = cv::countNonZero(binary);
    int blacks = binary.size().area()-whites;
    
    return whites<blacks and binary.at<uchar>(0)==0? binary: ~binary;   // TODO: Добавлено условие и нужно потестить Photo
}

std::vector<cv::RotatedRect> text_areas(cv::Mat input) {
        // Dilate
    auto kernel = cv::getStructuringElement(cv::MORPH_CROSS, {3,3});
    cv::Mat dilated;
    cv::dilate(input, dilated, kernel, {-1, -1}, 10);

    // Contours
    Contours contours;
    cv::findContours(dilated, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::RotatedRect> areas;
    for (const auto& cont: contours) {

        auto box=cv::minAreaRect(cont);

        if (box.size.width<50 or box.size.height<50) continue;
        if (box.size.height >= input.rows/2 or box.size.width >= input.cols/2) continue;
        
        areas.push_back(box);
    }
    return areas;
}

cv::Mat deskewAndcrop(cv::Mat input, const cv::RotatedRect& region) {

    double angle = region.angle;
    auto size = region.size;
    
    if (angle>=45.0) {
        angle = 0.0;
        std::swap(size.width, size.height);
    } 
    else if (angle>0.0 and angle<45.0) {
        angle = 0.0;
    }

    auto transform=cv::getRotationMatrix2D(region.center, angle, 1.0);
    cv::Mat rotated;
    cv::warpAffine(input, rotated, transform, input.size(), cv::INTER_CUBIC);

    cv::Mat cropped;
    cv::getRectSubPix(rotated, size, region.center, cropped);
    //cv::copyMakeBorder(cropped, cropped, 5, 5, 5, 5, cv::BORDER_CONSTANT, cv::Scalar(0));
    return cropped;
}

cv::Mat remove_shadows(cv::Mat input) {

    std::vector<cv::Mat> planes;
    std::vector<cv::Mat> result_planes;
    cv::split(input, planes);
    for (auto& plane: planes) {
        cv::Mat dilated;
        cv::dilate(plane, dilated, cv::Mat::ones({7,7}, CV_8UC1));
        cv::Mat blured;
        cv::medianBlur(dilated, blured, 21);
        cv::Mat abs_diff;
        cv::absdiff(plane, blured, abs_diff);
        cv::Mat diff= 255-abs_diff;
        cv::Mat norm;
        cv::normalize(diff, norm, 0.0, 255.0, cv::NORM_MINMAX, CV_8UC1);
        result_planes.push_back(norm);
    }
    cv::Mat result;
    cv::merge(result_planes, result);

    return result;
}