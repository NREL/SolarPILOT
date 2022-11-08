#####################################################################################################################
#
# CMake and System Settings
#
#####################################################################################################################
cmake_minimum_required(VERSION 3.11)

set(CMAKE_VERBOSE_MAKEFILE ON)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OS X deployment version")
endif()

if (UNIX AND NOT CMAKE_C_COMPILER)
	set(CMAKE_C_COMPILER gcc)
	set(CMAKE_CXX_COMPILER g++)
endif()

#####################################################################################################################
#
# Project Settings 
#
#####################################################################################################################

Project(SolarPILOT)

if ( NOT APPLE)
	set(CURL_DIR build_resources/libcurl_ssl_x64)
endif()

include_directories(. $ENV{WEXDIR}/include $ENV{LKDIR}/include $ENV{CORETRACEDIR} $ENV{SSCDIR}/shared $ENV{SSCDIR}/solarpilot)

set(SOLARPILOT_SRC
	app/FieldPlot.cpp                         app/FieldPlot.h
	app/FluxPlot.cpp                          app/FluxPlot.h
	app/gui_about_dialog.cpp                  app/gui_about_dialog.h
	app/GUI_ClimatePage.cpp
	app/GUI_CostPage.cpp
	app/GUI_HeliostatPage.cpp
	app/GUI_LayoutPage.cpp
	app/GUI_LayoutSimulationPage.cpp
	app/GUI_main.cpp                          app/GUI_main.h
	app/GUI_MarketPage.cpp
	app/GUI_OptimizationPage.cpp
	app/GUI_ParametricSimulationPage.cpp
	app/GUI_PerformanceSimulationPage.cpp
	app/GUI_PlotFieldPage.cpp
	app/GUI_PlotFluxPage.cpp
	app/GUI_ReceiverPage.cpp
	app/GUI_ResultsPage.cpp
	app/GUI_SimulationsPage.cpp
	app/gui_util.cpp                          app/gui_util.h
	app/help.cpp                              app/help.h
	app/ImageCanvas.cpp                       app/ImageCanvas.h
	app/InputControl.cpp                      app/InputControl.h
	app/kml_select_dialog.cpp                 app/kml_select_dialog.h
	app/layout_sim_export_dialog.cpp          app/layout_sim_export_dialog.h
	app/numericvareditform.cpp                app/numericvareditform.h
	app/OutputControl.cpp                     app/OutputControl.h
	app/pagepanel.cpp                         app/pagepanel.h
	app/par_edit_ctrls.cpp                    app/par_edit_ctrls.h
	app/param_dialog.cpp                      app/param_dialog.h
	app/parse_kml.cpp                         app/parse_kml.h
	app/plot_base.cpp                         app/plot_base.h
	app/plot_contourf.cpp                     app/plot_contourf.h
	app/plot_scatter.cpp                      app/plot_scatter.h
	app/plot_select_dialog.cpp                app/plot_select_dialog.h
	app/rec_select_dialog.cpp                 app/rec_select_dialog.h
	app/rename_dialog.cpp                     app/rename_dialog.h
	app/sandbox.cpp
	app/scripting.cpp                         app/scripting.h
	app/settings_dialog.cpp                   app/settings_dialog.h)


#####################################################################################################################
#
# Compile Options per Platform
#
#####################################################################################################################

cmake_policy(SET CMP0043 NEW)

if(MSVC)
	add_compile_options(/W3 /MP)
        add_compile_definitions( __64BIT__ LK_USE_WXWIDGETS _CRT_SECURE_NO_DEPRECATE=1 )
	add_compile_definitions(_CRT_NON_CONFORMING_SWPRINTFS=1 _SCL_SECURE_NO_WARNINGS=1)
	add_compile_definitions(__WXMSW__ _UNICODE NOPCH )
	add_compile_definitions( SP_USE_SOLTRACE SP_USE_THREADS )
	foreach (flag_var CMAKE_C_FLAGS_DEBUG CMAKE_CXX_FLAGS_DEBUG)
		set(${flag_var} "${${flag_var}} /D_DEBUG" CACHE STRING "compile flags" FORCE)
	endforeach()
else(MSVC)
	if (APPLE)
		set(CMAKE_POSITION_INDEPENDENT_CODE ON)
		add_compile_options(-arch x86_64)
		add_definitions(-D__WXOSX__)
	else()
		add_definitions(-D__64BIT__)
	endif()
	add_compile_options(-Wall -Wno-deprecated -Wno-unknown-pragmas -Wno-overloaded-virtual -fno-common -std=c++11)
	add_definitions(-DLK_USE_WXWIDGETS -DWX_PRECOMP )
	add_definitions(-DSP_USE_SOLTRACE -DSP_USE_THREADS )
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_compile_definitions(_DEBUG)
	else()
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3" )
	endif()
endif(MSVC)


#####################################################################################################################
#
# Packages
#
#####################################################################################################################

if (UNIX)
	set(wxWidgets_CONFIG_EXECUTABLE /usr/local/bin/wx-config-3)
	find_package(wxWidgets COMPONENTS qa aui richtext html propgrid adv net stc core xml base REQUIRED)
else()
	set(wxWidgets_ROOT_DIR $ENV{WXMSW3})
	find_package(wxWidgets COMPONENTS qa webview aui richtext html propgrid adv net stc core xml base scintilla REQUIRED)
endif()

include(${wxWidgets_USE_FILE})


#####################################################################################################################
# 
# CMake Targets
#
#####################################################################################################################

# Setup Application
if (APPLE)
	set(MACOSX_BUNDLE_ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/build_resources/SolarPILOT-UI.icns)
	set(SOLARPILOT_ICON ${CMAKE_CURRENT_SOURCE_DIR}/build_resources/SolarPILOT-UI.icns)
	set_source_files_properties(${SOLARPILOT_ICON} PROPERTIES
		MACOSX_PACKAGE_LOCATION "Resources")
elseif(WIN32)
	set(SOLARPILOT_ICON ${CMAKE_CURRENT_SOURCE_DIR}/build_resources/SolarPILOT.rc)
endif()

# Setup Target
add_executable(SolarPILOT
		WIN32
		MACOSX_BUNDLE 
		${SOLARPILOT_SRC}
		${SOLARPILOT_ICON})
set_target_properties(SolarPILOT
    PROPERTIES
    DEBUG_POSTFIX "d"
	MINSIZEREL_POSTFIX "min"
	RELWITHDEBINFO_POSTFIX "rdbg"
	PREFIX ""
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/deploy/x64
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/deploy/x64
	RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}/deploy/x64
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/deploy/x64
	)

if (APPLE)
	set_target_properties(SolarPILOT PROPERTIES
		MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/../build_resources/Info-SolarPILOT.plist
	)
endif()

if (MSVC)
	set_target_properties(SolarPILOT PROPERTIES
        LINK_FLAGS /SUBSYSTEM:WINDOWS)
endif()

#####################################################################################################################
#
# Link Libraries and Options
#
#####################################################################################################################

if (${CMAKE_PROJECT_NAME} STREQUAL solarpilot_ui)
	target_link_libraries(SolarPILOT solarpilot_core coretrace shared nlopt lpsolve wex lk)
else()
	unset(LK_LIB CACHE)
	unset(WEX_LIB CACHE)
	unset(LPSOLVE_LIB CACHE)
	unset(NLOPT_LIB CACHE)
	unset(SHARED_LIB CACHE)
	unset(CORETRACE_LIB CACHE)
	unset(SPCORE_LIB CACHE)
	find_library( LK_LIB
		NAMES lk.a lk.lib
		PATHS $ENV{LK_LIB} $ENV{LKDIR}/build $ENV{LKDIR}/build/Release)
	find_library( WEX_LIB
		NAMES wex.a wex.lib
		PATHS $ENV{WEX_LIB} $ENV{WEXDIR}/build $ENV{WEXDIR}/build/Release)
	find_library( LPSOLVE_LIB
		NAMES lpsolve.a lpsolve.lib
		PATHS $ENV{LPSOLVE_LIB} $ENV{SSCDIR}/build/lpsolve $ENV{SSCDIR}/build/lpsolve/Release)
	find_library( NLOPT_LIB
		NAMES nlopt.a nlopt.lib
		PATHS $ENV{NLOPT_LIB} $ENV{SSCDIR}/build/nlopt $ENV{SSCDIR}/build/nlopt/Release)
	find_library( SHARED_LIB
		NAMES shared.a shared.lib
		PATHS $ENV{SHARED_LIB} $ENV{SSCDIR}/build/shared $ENV{SSCDIR}/build/shared/Release)
	find_library( CORETRACE_LIB
		NAMES coretrace.a coretrace.lib
		PATHS $ENV{CORETRACE_LIB} $ENV{CORETRACEDIR}/build $ENV{CORETRACEDIR}/build/Release)
	find_library( SPCORE_LIB
		NAMES solarpilot_core.a solarpilot_core.lib
		PATHS $ENV{SPCORE_LIB} $ENV{SSCDIR}/build/solarpilot $ENV{SSCDIR}/build/solarpilot/Release) 

	target_link_libraries(SolarPILOT optimized ${SPCORE_LIB} optimized ${CORETRACE_LIB} optimized ${SHARED_LIB} optimized ${NLOPT_LIB} optimized ${LPSOLVE_LIB} optimized ${WEX_LIB} optimized ${LK_LIB})
	
	if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR MSVC)
		unset(LK_LIB CACHE)
		unset(WEX_LIB CACHE)
		unset(LPSOLVE_LIB CACHE)
		unset(NLOPT_LIB CACHE)
		unset(SHARED_LIB CACHE)
		unset(CORETRACE_LIB CACHE)
		unset(SPCORE_LIB CACHE)
		find_library( LK_LIB
			NAMES lkd.a lkd.lib
			PATHS $ENV{LK_LIB} $ENV{LKDIR}/build $ENV{LKDIR}/build/Debug)
		find_library( WEX_LIB
			NAMES wexd.a wexd.lib
			PATHS $ENV{WEX_LIB} $ENV{WEXDIR}/build $ENV{WEXDIR}/build/Debug)
		find_library( LPSOLVE_LIB
			NAMES lpsolved.a lpsolved.lib
			PATHS $ENV{LPSOLVE_LIB} $ENV{SSCDIR}/build/lpsolve $ENV{SSCDIR}/build/lpsolve/Debug)
		find_library( NLOPT_LIB
			NAMES nloptd.a nloptd.lib
			PATHS $ENV{NLOPT_LIB} $ENV{SSCDIR}/build/nlopt $ENV{SSCDIR}/build/nlopt/Debug)
		find_library( SHARED_LIB
			NAMES sharedd.a sharedd.lib
			PATHS $ENV{SHARED_LIB} $ENV{SSCDIR}/build/shared $ENV{SSCDIR}/build/shared/Debug)
		find_library( CORETRACE_LIB
			NAMES coretraced.a coretraced.lib
			PATHS $ENV{CORETRACE_LIB} $ENV{CORETRACEDIR}/build $ENV{CORETRACEDIR}/build/Debug)
		find_library( SPCORE_LIB
			NAMES solarpilot_cored.a solarpilot_cored.lib
			PATHS $ENV{SPCORE_LIB} $ENV{SSCDIR}/build/solarpilot $ENV{SSCDIR}/build/solarpilot/Debug) 

		target_link_libraries(SolarPILOT debug ${SPCORE_LIB} debug ${CORETRACE_LIB} debug ${SHARED_LIB} debug ${NLOPT_LIB} debug ${LPSOLVE_LIB} debug ${WEX_LIB} debug ${LK_LIB})
	
	endif()
endif()

target_link_libraries(SolarPILOT ${wxWidgets_LIBRARIES})

if (UNIX AND NOT APPLE)
	target_link_libraries(SolarPILOT -ldl -lfontconfig)
endif()

if (UNIX)
	target_link_libraries(SolarPILOT -lm -lcurl)
elseif (MSVC)
	find_library( CURL_LIB
			NAMES libcurl.lib
			PATHS $ENV{WEXDIR}/build_resources/libcurl_ssl_x64/lib)
	target_link_libraries(SolarPILOT ${CURL_LIB})
	target_link_libraries(SolarPILOT Winhttp)
endif()

#####################################################################################################################
#
# Target Installation
#
#####################################################################################################################

if (MSVC)
	set( LIBCURL_FILES
	   dbghelp.dll
	   libcurl.dll
	   libeay32.dll
	   libssh2.dll
	   msvcp140.dll
	   openssl.exe
	   ssleay32.dll
	   vcruntime140.dll
	)

	set(SOLARPILOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deploy/x64)

	foreach( file_i ${LIBCURL_FILES})
		add_custom_command(
		TARGET SolarPILOT
		POST_BUILD
		COMMAND ${CMAKE_COMMAND}
		ARGS -E copy $ENV{WEXDIR}/build_resources/libcurl_ssl_x64/bin/${file_i} ${SOLARPILOT_DIR}
	)
	endforeach( file_i )
endif()


