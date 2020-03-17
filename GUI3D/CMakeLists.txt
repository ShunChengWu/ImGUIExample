# GUI3D #
#########
find_package(GLM REQUIRED)
FIND_PACKAGE(FreeType2 REQUIRED)


SET(sources
        GUI3D.cpp
        glUtils.cpp
        projection_control.cpp
        )
SET(headers
        GUI3D.h
        glShader.hpp
        glCamera.hpp
#        glMesh.hpp
        glUtils.hpp
        )

ADD_LIBRARY(GUI3D ${sources} ${headers})
target_link_libraries(GUI3D
        PUBLIC GUI
        )
TARGET_INCLUDE_DIRECTORIES(GUI3D
        PUBLIC ${glm_INCLUDE_DIRS}
        )
IF(FreeType2_FOUND)
    message("FreeType2_LIBRARIES: " ${FreeType2_LIBRARIES})
    TARGET_INCLUDE_DIRECTORIES(GUI3D PUBLIC ${FreeType2_INCLUDE_DIRS})
    TARGET_LINK_LIBRARIES(GUI3D PUBLIC ${FreeType2_LIBRARIES} )
    TARGET_LINK_LIBRARIES(GUI3D PUBLIC freetype)
    target_compile_definitions(GUI3D PUBLIC -DWITH_FREETYPE)
ENDIF()

TARGET_COMPILE_DEFINITIONS(GUI3D PUBLIC GUI_FOLDER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/")