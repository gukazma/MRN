set(TARGET_NAME MRNApp)

# for qt
# just for qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        OpenGL
        REQUIRED)

file(GLOB_RECURSE CPP_HEAD_FILES "*.h")
file(GLOB_RECURSE CPP_SOUCE_FILES  "*.cpp")
file(GLOB_RECURSE UI_FILES "*.ui")
set(QRC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Resources/resources.qrc)
set(APPRC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Resources/appicon.rc)
set(Translatefile ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Translations/zh_CN.ts)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${CPP_HEAD_FILES} ${CPP_SOUCE_FILES} ${UI_FILES} ${Translatefile})

add_executable(${TARGET_NAME} ${CPP_HEAD_FILES} ${CPP_SOUCE_FILES} ${UI_FILES} ${QRC_FILE} ${APPRC_FILE} ${Translatefile})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17 OUTPUT_NAME ${TARGET_NAME})
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER Apps)

target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")

target_link_libraries(${TARGET_NAME} PRIVATE 
${PROJECT_NAME}::Primitives
Qt5::Core
Qt5::Gui
Qt5::Widgets
Qt5::OpenGL
MRN::MRN
)

target_include_directories(
  ${TARGET_NAME} 
  PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
)

find_program(LUPDATE_EXE lupdate)
add_custom_target(
      MRNTranslateUpdate
      COMMAND ${LUPDATE_EXE} -recursive ./Widgets/ -ts ./Resources/Translations/zh_CN.ts
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      VERBATIM)

find_program(LRELEASE_EXE lrelease)
add_custom_target(
      MRNTranslateRelease
      COMMAND ${LRELEASE_EXE} ./Resources/Translations/zh_CN.ts
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      VERBATIM)
set_target_properties(MRNTranslateUpdate PROPERTIES FOLDER Apps)
set_target_properties(MRNTranslateRelease PROPERTIES FOLDER Apps)
