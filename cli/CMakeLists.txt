set (CLIROOT "${PROJECT_SOURCE_DIR}/cli")

set (CLI_SRC
	"${CLIROOT}/main.cpp"
	"${CLIROOT}/optionparser.h"
)
source_group ("cli" FILES ${CLI_SRC})

add_executable (zapcli ${CLI_SRC})

target_link_libraries (zapcli ZAP)

if (CMAKE_COMPILER_IS_GNUCXX)
	set_source_files_properties (${SRC} PROPERTIES COMPILE_FLAGS "-std=c++11")
endif ()
