include_directories("${PROJECT_SOURCE_DIR}/src")

find_package(Boost COMPONENTS program_options)
find_package(OpenGL)
find_package(GLUT)

if (NOT Boost_FOUND)
    message("Boost not found")
endif()

if(NOT ${OPENGL_FOUND})
    message("OPENGL not found")
endif()

if(NOT ${GLUT_FOUND})
    message("GLUT not found")
endif()

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLUT_INCLUDE_DIRS})
add_executable(csim csim.cpp Gfx.cpp P_IndCtrl.cpp Player.cpp PlayerView.cpp Quadtree.cpp Unit.cpp UnitTypes.cpp World.cpp W_Plain.cpp)
target_link_libraries(csim ${Boost_LIBRARIES})
target_link_libraries(csim ${OPENGL_LIBRARIES})
target_link_libraries(csim ${GLUT_LIBRARIES})
