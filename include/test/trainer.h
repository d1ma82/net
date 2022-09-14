#pragma once

#include <opencv2/highgui.hpp>
#include "manager/repo.h"
#include "repo/photo.h"

namespace test {

    bool read() {
        try{
            Photo photo(cout, "./db/DCIM/IMG_20220603_142405.jpg");
            
            std::vector<Data> chars;
            //for (const auto& line: photo.page)
                for (const auto& word: photo.page.at(2)) 
                    chars=photo.get_next(word); 
                //chars[2].print(cout);
        } catch(runtime_error& ex) {cout<<ex.what()<<'\n';}
        return false;
    }
    //TODO: Натринеровать сеть буквами, прочитать.
}