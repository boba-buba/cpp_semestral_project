
# CMakeList.txt : CMake project for tools, include source and define
# project specific logic here.
#

# Set the Vcpkg directory
set(VCPKG_ROOT "C:/vcpkg")


# Find and configure the desired package(s)
find_package(curl QUIET)
find_package(gumbo QUIET)

# Check if the package was found
if(NOT curl_FOUND)
    message(STATUS "curl package not found. Installing with Vcpkg...")
    execute_process(
        COMMAND "${VCPKG_ROOT}/vcpkg" install curl:x64-windows
        RESULT_VARIABLE VCPKG_RESULT
    )
    #nejspis je potreba zmeit vcpkg na .\...\vcpkg.exe pro windows urcite, mnevim jestli to budu delat pro linux
    # Check the installation result
    if(NOT ${VCPKG_RESULT} EQUAL 0)
        message(FATAL_ERROR "Failed to install curl package with Vcpkg. " ${VCPKG_ROOT})
    endif()

    if(${VCPKG_RESULT} EQUAL 0)
        message("OK with curl " ${VCPKG_ROOT})
    endif()

    # Find the package again after installation
    find_package(CURL REQUIRED)
endif()

if(NOT gumbo_FOUND)
    message(STATUS "Gumbo package not found. Installing with Vcpkg...")
    execute_process(
        COMMAND "${VCPKG_ROOT}/vcpkg" install gumbo:x64-windows
        RESULT_VARIABLE VCPKG_RESULT
    )

    # Check the installation result
    if(NOT ${VCPKG_RESULT} EQUAL 0)
        message(FATAL_ERROR "Failed to install Gumbo package with Vcpkg. " ${VCPKG_ROOT})
    endif()

    # Find the package again after installation
    find_package(unofficial-gumbo REQUIRED)
endif()




# Add source to this project's executable.
add_executable (tools "data.h" "DataRetrieval.cpp" "DataRetrieval.h" "ECalculator.h" "ECalculator.cpp" "GumboParser.cpp"
	"GumboParser.h" "main_tools.cpp" "Utilities.h")

# Link your executable with the package(s)
target_link_libraries(tools PRIVATE CURL::libcurl)
target_link_libraries(tools PRIVATE unofficial::gumbo::gumbo)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET tools PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
