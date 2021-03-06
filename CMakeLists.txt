CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(menu-screen C)

SET(PACKAGE_NAME "org.tizen.${PROJECT_NAME}")
SET(PREFIX "/opt/apps/${PACKAGE_NAME}")
SET(EXEC_PREFIX "${PREFIX}/bin")
SET(LIBDIR "${PREFIX}/lib")
SET(DATADIR "${PREFIX}/data")
SET(RESDIR "${PREFIX}/res")
SET(IMAGEDIR "${RESDIR}/images")
SET(EDJEDIR "${RESDIR}/edje")
SET(LOCALEDIR "${RESDIR}/locale")
SET(VERSION 0.1.0)

INCLUDE(FindPkgConfig)
pkg_check_modules(MENU_PKGS REQUIRED
	ail
	aul
	bundle
	capi-appfw-application
	dlog
	ecore
	ecore-evas
	ecore-file
	ecore-imf
	ecore-input
	ecore-x
	edje
	eet
	eina
	elementary
	evas
	heynoti
	pkgmgr
	sysman
	syspopup-caller
	utilX
)

ADD_DEFINITIONS("-DENABLE_BGIMAGE")
ADD_DEFINITIONS("-DLOG_TAG=\"menu-screen\"")
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

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

ADD_EXECUTABLE(${PROJECT_NAME}
	src/index.c
	src/item.c
	src/item_event.c
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
	src/all_apps/layout.c
	src/all_apps/list.c
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS} -lm)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${MENU_PKGS_LDFLAGS})

# Install 
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${EXEC_PREFIX})

ADD_SUBDIRECTORY(data)

# End of a file
