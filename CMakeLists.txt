cmake_minimum_required(VERSION 3.1)
project(GJKPractice)

# Set the output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

file(GLOB_RECURSE HEADER_FILES "include/*.h" "include/*.hpp")
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.c")

source_group(src FILES ${SOURCE_FILES})
source_group(header FILES ${HEADER_FILES})

add_executable(GJKPractice ${SOURCE_FILES} ${HEADER_FILES})

# ignore freetype install
set(SKIP_INSTALL_HEADERS ON CACHE BOOL "FreeType Ignore Install Headers")
set(SKIP_INSTALL_ALL ON CACHE BOOL "FreeType Ignore Install All")
set(BUILD_SHARED_LIBS ON CACHE BOOL "FreeType Build for DLL")
set(GLFW_BUILD_SHARED_LIBRARY ON CACHE BOOL "GLFW Build for DLL")
set(GLFW_INSTALL OFF CACHE BOOL "GLFW Ignore Install")

add_subdirectory(lib/freetype)
add_subdirectory(lib/glfw)



target_include_directories(GJKPractice PRIVATE include)
target_link_libraries(GJKPractice freetype)
target_link_libraries(GJKPractice glfw)

add_dependencies(GJKPractice freetype glfw)


if(MSVC)
set(INSTALL_ADDITIONAL_PATH "$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>")
# https://stackoverflow.com/questions/23950887/does-cmake-offer-a-method-to-set-the-working-directory-for-a-given-build-system
set_property(TARGET GJKPractice PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INSTALL_ADDITIONAL_PATH}")
else()
set(INSTALL_ADDITIONAL_PATH "")
endif()

install(FILES "fonts/arial.ttf" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INSTALL_ADDITIONAL_PATH}")