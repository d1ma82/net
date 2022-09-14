#pragma once

#include "repo/photo.h"
#include "repo/database.h"

namespace test {
bool photo() {


    Photo photo(cout, "D:/git/cpp/net/db/DCIM/IMG_20220603_142405.jpg");
    const Data data= photo.get_next();
    std::cout<<data.image;
    //data.print(std::cout);
    return data.image.data!=nullptr;
}
}