SET(MANIFESTDIR "/usr/share/packages")

# manifest
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/main_operation.launch DESTINATION /usr/apps/org.tizen.menu-screen/res/data)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.menu-screen.xml DESTINATION /usr/share/packages)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/${PACKAGE_NAME}.png DESTINATION /usr/apps/org.tizen.menu-screen/shared/res)

ADD_SUBDIRECTORY(edje)
ADD_SUBDIRECTORY(images)
ADD_SUBDIRECTORY(po)

# End of a file
