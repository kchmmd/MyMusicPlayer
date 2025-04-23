cmake_minimum_required(VERSION 3.5)

project(MyMusicPlayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS REQUIRED Core Widgets Xml Multimedia)	

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Xml_INCLUDE_DIRS})
include_directories(${Qt5Multimedia_INCLUDE_DIRS})
link_libraries(${Qt5Core_LIBRARIES})
link_libraries(${Qt5Widgets_LIBRARIES})
link_libraries(${Qt5Xml_LIBRARIES})
link_libraries(${Qt5Multimedia_LIBRARIES})

include_directories(./HeaderBar)
include_directories(./MyWidget)

set(HeaderBar
        ./HeaderBar/HeaderBar.cpp
        ./HeaderBar/HeaderBar.h
)
source_group("HeaderBar" FILES ${HeaderBar})

set(MyWidget
        ./MyWidget/MyWidget.cpp
        ./MyWidget/MyWidget.h
)
source_group("MyWidget" FILES ${MyWidget})

set(Common
        ./Common.cpp
        ./Common.h
)
source_group("Common" FILES ${Common})

set(Source
        main.cpp
        mainwindow.cpp
        mainwindow.h
        ControlBar.h
        ControlBar.cpp
		LyricBar.cpp
		LyricBar.h
		LyricTableWidget.cpp
		LyricTableWidget.h
		MusicList.cpp
		MusicList.h
		PlayerAnimation.cpp
		PlayerAnimation.h
		SearchBar.cpp
		SearchBar.h
		VolumeControlBar.cpp
		VolumeControlBar.h
)
source_group("Source" FILES ${Source})

set(DataStruct
	DataStruct.h
)
source_group("DataStruct" FILES ${DataStruct})

set(Language 
	zh_CN.ts
	zh_EN.ts
) 
source_group("Language" FILES ${Language})

set(Style 
	"../resource/style_black.qss"
	"../resource/style_white.qss"
) 
source_group("Style" FILES ${Style})

set(ALL_FILES
	${DataStruct}
	${HeaderBar}
	${MyWidget}
	${Common}
	${Source}
	${Language}
	${Style}
)

set(RESOURCES_FILE "resource.qrc")  # 你的资源文件名称
qt5_add_resources(SOURCES ${RESOURCES_FILE}) 

add_compile_options(/Zi)
add_link_options(/DEBUG)

add_executable(${PROJECT_NAME}  WIN32 ${ALL_FILES} ${SOURCES})

#################################
#output directory
#################################
if(WIN32)
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../"
)
else()
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../${CMAKE_BUILD_TYPE}"
)
endif()

#################################
#target name
#################################
#(${PROJECT_NAME} PROPERTIES
#	TARGET_NAME_DEBUG ${PROJECT_NAME}
#	TARGET_NAME_RELEASE ${PROJECT_NAME}
#)
#copy FILES
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/../resource/ ${CMAKE_SOURCE_DIR}/../${CMAKE_BUILD_TYPE}/resource/)
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/../cli/ ${CMAKE_SOURCE_DIR}/../${CMAKE_BUILD_TYPE}/cli/)

set(LRELEASE_EXECUTABLE ${Qt5_DIR}/../../../bin/lrelease)
message("LRELEASE_EXECUTABLE:"${LRELEASE_EXECUTABLE})
#脚本执行期间才会执行，故可以先编辑写当前CMakeList.txt 构建时会执行该脚本
execute_process(
        COMMAND ${LRELEASE_EXECUTABLE}  ${CMAKE_SOURCE_DIR}/zh_CN.ts -qm ${CMAKE_SOURCE_DIR}/../${CMAKE_BUILD_TYPE}/resource/zh_CN.qm)
execute_process(
        COMMAND ${LRELEASE_EXECUTABLE}  ${CMAKE_SOURCE_DIR}/zh_EN.ts -qm ${CMAKE_SOURCE_DIR}/../${CMAKE_BUILD_TYPE}/resource/zh_EN.qm)