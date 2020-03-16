# GUI3D #
#########
find_package(GLM REQUIRED)



SET(sources
        GUI3D.cpp
        )
SET(headers
        GUI3D.h
        glShader.hpp
        glCamera.hpp
        )

ADD_LIBRARY(GUI3D ${sources} ${headers})
target_link_libraries(GUI3D PUBLIC GUI)
TARGET_INCLUDE_DIRECTORIES(GUI3D PUBLIC ${glm_INCLUDE_DIRS})
message("glm_INCLUDE_DIRS: " ${glm_INCLUDE_DIRS})