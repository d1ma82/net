#pragma once

#include <opencv2/highgui.hpp>
#include "manager/repo.h"
#include "repo/photo.h"

namespace test {

    static void read_word(cv::Mat word, int& num) {
        
        cv::Point2f pts[4];
        cv::Mat binary = binarize(word);
        cv::Mat dilated;
        auto kernel = cv::getStructuringElement(cv::MORPH_CROSS, {3,3});
        cv::dilate(binary, dilated, kernel, {-1, -1}, 1);
#ifdef WRITEIMG
        stringstream wordname;
        wordname<<"./debug/regions/word_"<<num++<<".jpg";
        cv::imwrite(wordname.str(), dilated);
#endif
        Contours contours;
        cv::findContours(dilated, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE); // TODO: Нужно попробовать через иерархию выбирать, так как предлоги повторяются
        std::vector<cv::Rect> chars;
        for (const auto& cont: contours) {
            
            cv::Rect rect=cv::boundingRect(cont);
            if (rect.area()>1000.0 and rect.area()<10000) { // TODO: Здесь нужен какой-то другой способ!

                cv::rectangle(word, rect, {0,255,0},2);
                chars.push_back(rect);
            }
        }
        sort(chars.begin(), chars.end(), 
            [] (const cv::Rect& r1, const cv::Rect& r2) {return r1.x<r2.x;});

        for (auto& ch: chars) {

            cv::Mat cropped_char;
            cv::Point2f center;
            center.x=(ch.br().x+ch.tl().x)/2;
            center.y=(ch.br().y+ch.tl().y)/2;
            LOG(cout, "Char: "<<num++<<' '<<ch.area()<<' '<<center.x<<'x'<<center.y<<'\n')
            cv::getRectSubPix(word, ch.size(), center, cropped_char);
#ifdef WRITEIMG
            stringstream charname;
            charname<<"./debug/chars/char_"<<num-1<<".jpg";
            cv::imwrite(charname.str(), cropped_char);
#endif
            // TODO: Далее уменьшаем размер до размера сети и передаем в сеть
        }
    }

    bool read() {
        try{
            Photo photo(cout, "./db/DCIM/IMG_20220603_142405.jpg");
            select(photo, 2);
            int num{0};
            for (const auto& rect: photo) {

                cv::Mat word=photo.get_next(rect); 
                read_word(word, num);
                if (num==3) break;
            }
        } catch(runtime_error& ex) {cout<<ex.what()<<'\n';}
        return false;
    }
}