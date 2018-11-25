project(waveviewer)

set(include_dir ${CMAKE_SOURCE_DIR}/include/waveviewer)

set(MOC_WAVEPLOTTER_INC "${include_dir}/waveplotter.h")
set(MOC_WAVEPRODLG_INC "${include_dir}/waveprodlg.h")
set(INCS	
		${include_dir}/datatype.h
		${include_dir}/utility.h
		${include_dir}/numericaldiff.h
		${include_dir}/filter1d.h
		${include_dir}/wavepro.h)
set(SRCS
		utility.cpp 
		numericaldiff.cpp 
		filter1d.cpp 
		wavepro.cpp 
		waveplotter.cpp 
		waveprodlg.cpp
		main.cpp)

file(GLOB UI_WAVE_DLG "./*.ui")

file(GLOB qt_res_files "./*.qrc")

qt5_wrap_ui(UI_WAVE_DLG_INC ${UI_WAVE_DLG})
qt5_wrap_cpp(MOC_WAVEPLOTTER_SRC "${MOC_WAVEPLOTTER_INC}")
qt5_wrap_cpp(MOC_WAVEPRODLG_SRC "${MOC_WAVEPRODLG_INC}")
qt5_add_resources(res_files waveprodlg.qrc)

list(APPEND INCS "${MOC_WAVEPLOTTER_INC}" "${MOC_WAVEPRODLG_INC}" "${UI_WAVE_DLG_INC}")
list(APPEND SRCS "${MOC_WAVEPLOTTER_SRC}" "${MOC_WAVEPRODLG_SRC}")

source_group("Generated Files" FILES ${UI_WAVE_DLG_INC} ${MOC_WAVEPLOTTER_SRC} ${MOC_WAVEPRODLG_SRC} ${res_files})
source_group("Form Files" FILES ${UI_WAVE_DLG})
source_group("Resource Files" FILES ${qt_res_files})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${EIGEN_INCLUDE_DIRS})
include_directories(${include_dir})
add_executable(${PROJECT_NAME} ${SRCS} ${INCS} ${res_files})
target_link_libraries(${PROJECT_NAME} liblas Qt5::Widgets Qt5::Core Qt5::Gui)

#qt5_use_modules(${PROJECT_NAME} Widgets Core Gui)