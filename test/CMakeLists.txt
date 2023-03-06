include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})
include_directories("/usr/local/include")
link_directories(${CHECK_LIBRARY_DIRS})

set(TEST_SOURCES
  test_gboy.c
)

add_executable(test_gboy ${TEST_SOURCES})
target_link_libraries(test_gboy gboy ${CHECK_LIBRARIES})
target_include_directories(test_gboy PRIVATE ${PROJECT_SOURCE_DIR}/include )

if (WIN32)
target_include_directories(gboy PUBLIC ${PROJECT_SOURCE_DIR}/windows/check )
endif()