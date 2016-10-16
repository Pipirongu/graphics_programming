cmake_minimum_required(VERSION 2.8)
project(S0017D_phivuu-2_assignment_1)
  
#///////////////////////////////////////////////////////////////////////////

#Auto-generate moc files if Q_OBJECT is found in header files
SET(CMAKE_AUTOMOC ON)
#Runs UIC if ui_XXX.h is included in headers and expects XXX.ui
SET(CMAKE_AUTOUIC ON)
#QT Resource files
SET(CMAKE_AUTORCC ON)
#Look for headers in cmake source dir and build dir
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5OpenGL REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

#Include Directories(where to look for headers) (probably just the code folder)
INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDES}
					${CMAKE_SOURCE_DIR}/code
					${CMAKE_SOURCE_DIR}/extralibs
					${CMAKE_CURRENT_BINARY_DIR} #UI headers are generated here
					${PROJECT_SOURCE_DIR}
					${PROJECT_BINARY_DIR})
			
# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
# ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS})
# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
# SET(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

# if (Qt5_POSITION_INDEPENDENT_CODE)
# 	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# endif()

#Find header and source files					
#//Code HEADER AND SOURCE
FILE(GLOB HEAD "code/*.h")
FILE(GLOB SRC "code/*.cpp")
LIST(APPEND FILES_CODE ${HEAD} ${SRC})
SOURCE_GROUP("code" FILES ${FILES_CODE})

#Find qrc add to list and add to executable
FILE(GLOB QRC "resources/*.qrc")
LIST(APPEND FILES_RESOURCES ${QRC})
SOURCE_GROUP("resources" FILES ${FILES_RESOURCES})
  
#Main SOURCE
FILE(GLOB HEAD "code/*.h")
FILE(GLOB SRC "code/*.cpp")
LIST(APPEND FILES_PROJECT ${HEAD} ${SRC})
SOURCE_GROUP("myproject" FILES ${FILES_PROJECT})

LIST(APPEND ALL_FILES 
	 ${FILES_CODE}
	 ${FILES_PROJECT})

#Personal Libraries
ADD_SUBDIRECTORY(extralibs/mypersonalmathlib)
SET(EXTRA_LIBS mypersonalmathlibrary)

#Generate ui headers so custom classes can use them to display the ui
# QT5_WRAP_UI(uis code/renderscene.ui)
ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${ALL_FILES} ${FILES_RESOURCES})# ${uis})
			

TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${OPENGL_LIBRARIES} ${EXTRA_LIBS} ${Qt5Widgets_LIBRARIES} ${Qt5OpenGL_LIBRARIES})