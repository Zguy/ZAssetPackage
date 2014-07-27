set (SRC_CLI
	"main.cpp"
	"extract.h"
	"extract.cpp"
	"optionparser.h"
	"options.h"
	"pack.h"
	"pack.cpp"
	"path.h"
	"path.cpp"
	"pretty.h"
	"pretty.cpp"
)
source_group ("cli" FILES ${SRC_CLI})

add_executable (zapcli ${SRC_CLI})

target_link_libraries (zapcli ZAP)

if (CMAKE_COMPILER_IS_GNUCXX)
	set_source_files_properties (${SRC_CLI} PROPERTIES COMPILE_FLAGS "-std=c++11")
endif ()
