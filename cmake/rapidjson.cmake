include(FetchContent)
FetchContent_Declare(rapidjson
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/rapidjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
    GIT_TAG 06d58b9e848c650114556a23294d0b6440078c61
    GIT_SHALLOW FALSE
    FIND_PACKAGE_ARGS)
FetchContent_Populate(rapidjson)

include_directories(BEFORE ${rapidjson_SOURCE_DIR}/include)
