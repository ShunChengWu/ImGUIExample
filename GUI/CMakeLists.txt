# GUI #
#######
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseImGUI.cmake)

SET(sources
        GUI.cpp
        )
SET(headers
        GUI.h
        )

ADD_LIBRARY(GUI ${sources} ${headers})
target_link_libraries(GUI PUBLIC imgui)