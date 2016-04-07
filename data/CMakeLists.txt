SET(MANIFESTDIR "/usr/share/packages")

# manifest
CONFIGURE_FILE(${PACKAGE_NAME}.xml.in ${CMAKE_BINARY_DIR}/${PACKAGE_NAME}.xml)
INSTALL(FILES ${CMAKE_BINARY_DIR}/${PACKAGE_NAME}.xml DESTINATION ${MANIFESTDIR})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/main_operation.launch DESTINATION /usr/apps/org.tizen.menu-screen/res/data)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/${PACKAGE_NAME}.png DESTINATION /usr/apps/org.tizen.menu-screen/shared/res)

ADD_SUBDIRECTORY(edje)
ADD_SUBDIRECTORY(images)
ADD_SUBDIRECTORY(po)

# End of a file
