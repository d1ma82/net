#pragma once

#include <algorithm>
#include <numeric>

#include "repo/database.h"
#include "utils/opencv.h"

class Photo: public Database {
private:
typedef std::vector<std::vector<cv::RotatedRect>> Page;
using LineIterator=Page::iterator;
enum {BL, TL, TR, BR};

int scale_to{0};
int num_chars{0};
const double INTER_LINE = 40.0;     // Примерный междустрочный интервал
size_t count {0};
Data data;
LineIterator line_iterator;
std::ostream& ostr;
cv::Mat input_image;    // Изображение по которому строится страница
cv::Point2f p1[4];  
cv::Point2f p2[4];
    
void points(const cv::RotatedRect& r1, cv::Point2f* pts) {

    r1.points(pts);
    if (r1.angle>=45.0) {
        std::swap(pts[TL].x, pts[BR].x);
        std::swap(pts[TR].y, pts[BL].y);
    }
}

Page make_page(const std::vector<cv::RotatedRect>& input) {
    
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
    // если истина мы имеем дело с одной строкой

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
                    page[i].push_back(std::move(word));
                
                page[j].resize(0);
            }
            break;
        }
        count+=page[i].size();
    } 
}

std::vector<Data> separate_word(const cv::Mat word) {
        
        cv::Point2f pts[4];
        cv::Mat binary = binarize(word);
        cv::Mat dilated;
        auto kernel = cv::getStructuringElement(cv::MORPH_CROSS, {3,3});
        cv::dilate(binary, dilated, kernel, {-1, -1}, 1);

        Contours contours;
        cv::findContours(dilated, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        std::vector<cv::Rect> chars;
        for (const auto& cont: contours) {
            
            cv::Rect rect=cv::boundingRect(cont);
            if (rect.area()>1000.0 and rect.area()<10000) { // TODO: Здесь нужен какой-то другой способ!

                chars.push_back(rect);
            }
        }
        sort(chars.begin(), chars.end(), 
            [] (const cv::Rect& r1, const cv::Rect& r2) {return r1.x<r2.x;});

        std::vector<Data> result;
        for (auto& ch: chars) {

            cv::Mat cropped_char;
            cv::Point2f center;
            center.x=(ch.br().x+ch.tl().x)/2;
            center.y=(ch.br().y+ch.tl().y)/2;
            LOG(cout, "Char: "<<num_chars++<<' '<<ch.area()<<' '<<center.x<<'x'<<center.y<<'\n')
            cv::getRectSubPix(word, ch.size(), center, cropped_char);
            cv::Mat scaled;
            cv::resize(cropped_char, scaled, {scale_to, scale_to}, 0.0, 0.0, cv::INTER_AREA);
            cv::Mat binary = binarize(scaled);
            result.push_back({binary});

#ifdef WRITEIMG
            stringstream charname;
            charname<<"./debug/chars/char_"<<num_chars-1<<".png";
            cv::imwrite(charname.str(), binary);
#endif
        }
        return result;
    }

public:
Page page;

Photo(std::ostream& ostr, const char* filename, int scale_to): ostr{ostr}, scale_to{scale_to} {

    ostr<<boolalpha;
    input_image = cv::imread(filename);
    if (!input_image.data) {
        error(ER_FILE, string(("Could not open file ")+string(filename)).c_str());
    }
    cv::Mat no_shadows = remove_shadows(input_image);
    cv::Mat binary = binarize(no_shadows);
    auto regions = text_areas(binary);
    page=make_page(regions);
    line_iterator=page.begin();
    if (page.size()<=0) error(ERROR, "Could not read page\n");
    LOGI(ostr, "Photo: "<<count<<" words\n")   
}

const Data get_next() final { 
    
    error(ERROR, "Not implemented const Data& Photo::get_next().");
    return data;
}

std::vector<Data> get_next(const cv::RotatedRect& rect) final {
    
    cv::Mat word=deskewAndcrop(input_image, rect);
    
    points(rect, p1);
    LOG(ostr,
        ", TL: "<<p1[TL].x<<'x'<<p1[TL].y<<
        " TR: "<<p1[TR].x<<'x'<<p1[TR].y<<
        ", WxH: "<<(rect.angle>45.0? rect.size.height:rect.size.width)<<'x'
                <<(rect.angle>45.0? rect.size.width:rect.size.height)<<
        ", a: "<<rect.angle<<'\n');

#ifdef WRITEIMG
    for (int j=0; j<4; j++) 
        cv::line(input_image, p1[j], p1[(j+1)%4], {0,255,0}, 2);
#endif 
    return separate_word(word);
}

inline size_t total() const final {return count;} 

~Photo() {
#ifdef WRITEIMG    
    cv::imwrite("./debug/word.jpg", input_image);
#endif     
}
};
