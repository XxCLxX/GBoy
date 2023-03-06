

file (GLOB sources CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/lib/*.c")

file (GLOB headers CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h")

add_library(gboy STATIC ${sources} ${headers})

target_include_directories(gboy PUBLIC ${PROJECT_SOURCE_DIR}/include )


if (WIN32)
  target_include_directories(gboy PUBLIC "${PROJECT_SOURCE_DIR}/windows/sdl2/include" )
  target_include_directories(gboy PUBLIC ${PROJECT_SOURCE_DIR}/windows/sdl2_ttf/include )
else()
  target_include_directories(gboy PUBLIC ${SDL2_INCLUDE_DIR})
  target_link_libraries(gboy PUBLIC ${SDL2_LIBRARY}) 
  target_link_libraries(gboy PUBLIC ${SDL2_TTF_LIBRARY}) 
endif()

include_directories("/usr/local/include")
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(gboy ${SDL2_LIBRARIES})

include_directories(${SDL2_TTF_INCLUDE_DIRS})
target_link_libraries(gboy ${SDL2_TTF_LIBRARIES})