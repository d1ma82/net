#pragma once

#include <algorithm>
#include <numeric>

#include "repo/database.h"
#include "utils/opencv.h"

class Photo: public Database {
private:

typedef vector<vector<cv::RotatedRect>> Page;
enum {BL, TL, TR, BR};
const double INTER_LINE = 40.0;
size_t count {0};
Data data;
ostream& ostr;
cv::Point2f p1[4];  
cv::Point2f p2[4];
    
void points(const cv::RotatedRect& r1, cv::Point2f* pts) {

    r1.points(pts);
    if (r1.angle>=45.0) {
        swap(pts[TL].x, pts[BR].x);
        swap(pts[TR].y, pts[BL].y);
    }
}

Page make_page(const vector<cv::RotatedRect>& input) {
    
    Page page;
    if (input.empty()) return page;

    auto page_width= max_element(input.begin(), input.end(), 
            [&] (const cv::RotatedRect& r1, const cv::RotatedRect& r2) { 

                r1.points(p1);      
                r2.points(p2);
                return p1[BR].x < p2[BR].x;
    });
    auto page_height= max_element(input.begin(), input.end(), 
        [&] (const cv::RotatedRect& r1, const cv::RotatedRect& r2) {
                
                r1.points(p1);      
                r2.points(p2);
                return p1[BL].y < p2[BL].y;
    });
    auto avg_line_height= accumulate(input.begin(), input.end(), 0.0, 
        [&] (double summ, const cv::RotatedRect& r) {

                r.points(p1);    
                return summ+(p1[BL].y-p1[TL].y+INTER_LINE);
    })/ input.size();
    
    page_width->points(p1);
    page_height->points(p2);
    size_t lines=(p2[BL].y/avg_line_height)+1;
    LOG(ostr, "Page: "<<p1[BR].x<<'x'<<p2[BL].y<<'\n')
    LOG(ostr, "AVG Line height: "<<avg_line_height<<'\n')
    LOG(ostr, "Total lines: "<<lines<<'\n')
    
    page.resize(lines);
    for (const auto& region: input) {
        
        region.points(p1);
        float centerY=(p1[BL].y+p1[TL].y)/2;
        int line=centerY/avg_line_height;
        if (line >= page.size()) {LOG(ostr, line<<" out of page\n") continue;}    
        page[line].push_back(region);
    }
    shrink_page(page, avg_line_height);
    return page;
}

void shrink_page(Page& page, double avg_line_height) {

    LOG(ostr, "Shrink page\n")
    if (page.empty()) return;
    
    // Сравнить конец строки n+1 c началом строки n
    // Сравнить начало строки n+1 с концом строки n
    // если какой либо из вариантов истинен мы имеем дело с одной строкой

    for (int i=0; i<page.size(); i++) {

        if (page[i].empty()) continue;
        // Найти начало строки n
        auto minBL= min_element(page[i].begin(), page[i].end(), 
            [&] (const cv::RotatedRect& r1, const cv::RotatedRect& r2) {    
                
                points(r1, p1); points(r2, p2);
                return p1[BL].x < p2[BL].x;
        });

        for (int j=i+1; j<page.size(); j++) {
            
            if (page[j].empty()) {LOGI(ostr, "Line "<<j<<" empty\n") continue;}
            
                    // Найти конец строки n+1
            auto maxBR= max_element(page[j].begin(), page[j].end(), 
                [&] (const cv::RotatedRect& r1, const cv::RotatedRect& r2) {

                    points(r1, p1); points(r2, p2);
                    return p1[BR].x < p2[BR].x;
            });
            points(*minBL, p1); points(*maxBR, p2);
            
            // Если конец строки n+1 находитья на одной высоте со строкой n
            if (abs(p1[TL].y-p2[TR].y)<avg_line_height) {

                LOG(ostr, "Line "<<j<<" same as line "<<i<<'\n')
                for (auto& word: page[j]) 
                    page[i].push_back(move(word));
                
                page[j].resize(0);
            }
            break;
        }
    } 
}

public:

Photo(ostream& ostr, const char* filename): ostr{ostr} {

    int interest_line=2;
    cv::Mat input = cv::imread(filename);
    cv::Mat no_shadows = remove_shadows(input);
    cv::Mat binary = binarize(no_shadows);
#ifdef WRITEIMG
    cv::imwrite("./debug/binary.jpg", binary);
#endif
    auto regions = text_areas(binary);
    Page page=make_page(regions);

    for (int l=-1; auto& line: page) {
        
        l++;
        if (l!=interest_line) continue;
        if (line.empty()) continue;
        sort(line.begin(), line.end(), 
            [&] (const cv::RotatedRect& r1, const cv::RotatedRect& r2) {

                points(r1, p1); points(r2, p2);
                return p1[BL].x < p2[BL].x;
        });
        LOG(ostr, "Read line: "<<l<<'\n')
        for (int i=0; const auto& region: line) {

            auto cropped=deskewAndcrop(input, region);
            stringstream filename;
            filename<<"./debug/regions/region"<<i++<<".jpg";
            cv::imwrite(filename.str(), cropped);
            points(region, p1);
            cout<<i-1<< ", TL: "<<p1[TL].x<<'x'<<p1[TL].y<<
                        " TR: "<<p1[TR].x<<'x'<<p1[TR].y<<
                        ", WxH: "<<(region.angle>45.0? region.size.height:region.size.width)<<'x'
                                 <<(region.angle>45.0? region.size.width:region.size.height)<<
                        ", a: "<<region.angle<<'\n';
            
            for (int j=0; j<4; j++) 
                cv::line(input, p1[j], p1[(j+1)%4], {0,255,0}, 2);
        }
        if (l==3) break;
    }
#ifdef WRITEIMG
    cv::imwrite("./debug/rects.jpg", input);
#endif
}
    
const Data& get_next() final {

    return data;
}

inline size_t total() const final {return count;} 
//TODO: Функции OpenCV перенести в отдельый файл утилит
//      Удалить тени
};