set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
AUX_SOURCE_DIRECTORY(. SOURCE_LIST_PROTOCOL)
add_library(protocol_driver
        SHARED
        ${SOURCE_LIST_PROTOCOL}
        serialFactory.h serialInfantry.h)
target_include_directories(protocol_driver PUBLIC ./)
