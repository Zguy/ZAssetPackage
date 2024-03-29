cmake_minimum_required(VERSION 3.20)

project(ZAssetPackage VERSION 0.1.0)

set(CMAKE_DEBUG_POSTFIX d)

#option(BUILD_SHARED_LIBS "Build shared libs" OFF)
option(ZAP_BUILD_DOC "Generate documentation" OFF)
option(ZAP_BUILD_CLI_TOOL "Build CLI tool" ON)

# Compression support
option(ZAP_COMPRESS_LZ4 "LZ4 compression support" ON)

set(INCROOT "${PROJECT_SOURCE_DIR}/include/ZAP")
set(SRCROOT "${PROJECT_SOURCE_DIR}/src")
set(DEPROOT "${PROJECT_SOURCE_DIR}/deps")

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${DEPROOT}")

configure_file("${INCROOT}/Config.h.in" "${PROJECT_BINARY_DIR}/include/Config.h")
include_directories("${PROJECT_BINARY_DIR}/include")

set(SRC_LIB
	"${SRCROOT}/Archive.cpp"
	"${INCROOT}/Archive.h"
	"${SRCROOT}/ArchiveBuilder.cpp"
	"${INCROOT}/ArchiveBuilder.h"
	"${SRCROOT}/Compression.cpp"
	"${INCROOT}/Compression.h"
	"${INCROOT}/Version.h"
)
source_group("zap" FILES ${SRC_LIB})

set(SRC ${SRC_LIB})

if (ZAP_COMPRESS_LZ4)
	set(SRC_LZ4
		"${DEPROOT}/lz4/lz4.c"
		"${DEPROOT}/lz4/lz4.h"
		"${DEPROOT}/lz4/lz4hc.c"
		"${DEPROOT}/lz4/lz4hc.h"
	)
	set(SRC ${SRC} ${SRC_LZ4})
	source_group("lz4" FILES ${SRC_LZ4})
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

add_library(ZAP STATIC ${SRC})

if (CMAKE_COMPILER_IS_GNUCXX)
	set_source_files_properties(${SRC_LIB} PROPERTIES COMPILE_FLAGS "-std=c++11 -Wno-multichar")
endif()

if (ZAP_BUILD_DOC)
	add_subdirectory("doc")
endif()

if (ZAP_BUILD_CLI_TOOL)
	add_subdirectory("cli")
endif()
