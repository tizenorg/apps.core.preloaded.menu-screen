CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(menu-screen C)

SET(PACKAGE_NAME "org.tizen.${PROJECT_NAME}")
SET(PREFIX "/usr/apps/${PACKAGE_NAME}")
SET(EXEC_PREFIX "${PREFIX}/bin")
SET(LIBDIR "${PREFIX}/lib")
SET(DATADIR "/opt${PREFIX}/data")
SET(RESDIR "${PREFIX}/res")
SET(ICONDIR "/usr/share/icons/default/small")
SET(IMAGEDIR "${RESDIR}/images")
SET(EDJEDIR "${RESDIR}/edje")
SET(LOCALEDIR "${RESDIR}/locale")
SET(VERSION 0.1.0)

#IF("${ARCH}" STREQUAL "arm")
#	OPTION(USE_ARM "Use Arm" ON)
#	ADD_DEFINITIONS("-DARM_TYPE")
#ELSE()
#	OPTION(USE_EMULATOR "Use Emulator" ON)
#	ADD_DEFINITIONS("-DEMULATOR_TYPE")
#ENDIF()
#MESSAGE("ARCH: ${ARCH}")

INCLUDE(FindPkgConfig)
pkg_check_modules(MENU_PKGS REQUIRED
	ail
	appcore-efl
	appsvc
	aul
	badge
	bundle
	capi-appfw-application
#	capi-appfw-preference
	capi-system-info
	capi-system-system-settings
	capi-system-info
	dlog
	ecore
	ecore-evas
	ecore-file
	ecore-imf
	ecore-input
#	ecore-x
	edje
	eet
	eina
	elementary
	evas
	pkgmgr
	pkgmgr-info
	shortcut
	syspopup-caller
#	utilX
)

IF(USE_ARM)
pkg_check_modules(ARM_PKGS REQUIRED
	bincfg
	factory
)
ENDIF()

ADD_DEFINITIONS("-DENABLE_BGIMAGE")
ADD_DEFINITIONS("-DLOG_TAG=\"MENU_SCREEN\"")
ADD_DEFINITIONS("-DPREFIX=\"${PREFIX}\"")
ADD_DEFINITIONS("-DPACKAGE=\"${PACKAGE_NAME}\"")
ADD_DEFINITIONS("-DVERSION=\"${VERSION}\"")
ADD_DEFINITIONS("-DDATADIR=\"${DATADIR}\"")
ADD_DEFINITIONS("-DRESDIR=\"${RESDIR}\"")
ADD_DEFINITIONS("-DIMAGEDIR=\"${IMAGEDIR}\"")
ADD_DEFINITIONS("-DEDJEDIR=\"${EDJEDIR}\"")
ADD_DEFINITIONS("-DLOCALEDIR=\"${LOCALEDIR}\"")

FOREACH (flag ${MENU_PKGS_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

FOREACH(flag ${ARM_PKGS_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} -fdata-sections -ffunction-sections -Wl,--gc-sections")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIE")
SET(CMAKE_C_FLAGS_PROFILING "-O0 -g -pg -fPIE")
SET(CMAKE_CXX_FLAGS_PROFILING "-O0 -std=c++0x -g -pg -fPIE")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g -fPIE")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -std=c++0x -g -fPIE")
SET(CMAKE_C_FLAGS_RELEASE "-O2 -g -fPIE")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 std=c++0x -g -fPIE")
SET(CMAKE_C_FLAGS_CCOV "-O2 -g --coverage -fPIE")
SET(CMAKE_CXX_FLAGS_CCOV "-O2 std=c++0x -g --coverage -fPIE")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -pie")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

ADD_EXECUTABLE(${PROJECT_NAME}
	src/db.c
	src/index.c
	src/item.c
	src/item_badge.c
	src/item_event.c
	src/key.c
	src/layout.c
	src/list.c
	src/mapbuf.c
	src/menu_screen.c
	src/mouse.c
	src/page.c
	src/page_scroller.c
	src/pkgmgr.c
	src/popup.c
	src/util.c
	src/all_apps/db.c
	src/all_apps/layout.c
	src/all_apps/list.c
	src/all_apps/shortcut.c
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${ARM_PKGS_LDFLAGS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${MENU_PKGS_LDFLAGS})

# Install
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${EXEC_PREFIX})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}.efl DESTINATION /etc/smack/accesses.d)

ADD_SUBDIRECTORY(data)

# End of a file
