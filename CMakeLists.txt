cmake_minimum_required (VERSION 2.6)

project (ZAssetPackage)

# Version number
set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
set (VERSION_PATCH 0)

#option (BUILD_SHARED_LIBS "Build shared libs" OFF)
option (ZAP_BUILD_DOC "Generate documentation" OFF)

# Compression support
option (ZAP_COMPRESS_LZ4 "LZ4 compression support" ON)

set (INCROOT "${PROJECT_SOURCE_DIR}/include/ZAP")
set (SRCROOT "${PROJECT_SOURCE_DIR}/src")
set (DEPROOT "${PROJECT_SOURCE_DIR}/deps")

include_directories ("${PROJECT_SOURCE_DIR}/include")
include_directories ("${DEPROOT}")

configure_file ("${INCROOT}/Config.h.in" "${PROJECT_BINARY_DIR}/include/Config.h")
include_directories ("${PROJECT_BINARY_DIR}/include")

set (SRC
	"${SRCROOT}/Archive.cpp"
	"${INCROOT}/Archive.h"
	"${SRCROOT}/ArchiveBuilder.cpp"
	"${INCROOT}/ArchiveBuilder.h"
	"${SRCROOT}/Compression.cpp"
	"${INCROOT}/Compression.h"
	"${INCROOT}/DataPointer.h"
	"${INCROOT}/Version.h"
)
source_group ("zap" FILES ${SRC})

if (ZAP_COMPRESS_LZ4)
	set (SRC_LZ4
		"${DEPROOT}/lz4/lz4.c"
		"${DEPROOT}/lz4/lz4.h"
		"${DEPROOT}/lz4/lz4hc.c"
		"${DEPROOT}/lz4/lz4hc.h"
	)
	set (SRC ${SRC} ${SRC_LZ4})
	source_group ("lz4" FILES ${SRC_LZ4})
endif ()

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

add_library (ZAP STATIC ${SRC})

if (CMAKE_COMPILER_IS_GNUCXX)
	set_target_properties (ZAP PROPERTIES COMPILE_FLAGS "-std=c++11 -Wno-multichar")
endif ()

if (ZAP_BUILD_DOC)
	add_subdirectory (doc)
endif ()
