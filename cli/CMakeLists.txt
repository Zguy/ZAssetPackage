set (CLIROOT "${PROJECT_SOURCE_DIR}/cli")

set (SRC_CLI
	"${CLIROOT}/main.cpp"
	"${CLIROOT}/extract.h"
	"${CLIROOT}/extract.cpp"
	"${CLIROOT}/optionparser.h"
	"${CLIROOT}/options.h"
	"${CLIROOT}/pack.h"
	"${CLIROOT}/pack.cpp"
	"${CLIROOT}/path.h"
	"${CLIROOT}/path.cpp"
	"${CLIROOT}/pretty.h"
	"${CLIROOT}/pretty.cpp"
)
source_group ("cli" FILES ${SRC_CLI})

add_executable (zapcli ${SRC_CLI})

target_link_libraries (zapcli ZAP)

if (CMAKE_COMPILER_IS_GNUCXX)
	set_source_files_properties (${SRC_CLI} PROPERTIES COMPILE_FLAGS "-std=c++11")
endif ()
