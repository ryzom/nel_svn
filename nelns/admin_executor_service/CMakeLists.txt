FILE(GLOB SRC *.cpp *.h)

ADD_EXECUTABLE(admin_executor_service ${SRC})

INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR} ${NELMISC_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(admin_executor_service ${PLATFORM_LINKFLAGS} ${LIBXML2_LIBRARIES} ${NELMISC_LIBRARY} ${NELNET_LIBRARY})
IF(WIN32)
  SET_TARGET_PROPERTIES(admin_executor_service PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libcmt /SUBSYSTEM:WINDOWS")
ENDIF(WIN32)
ADD_DEFINITIONS(${LIBXML2_DEFINITIONS})

INSTALL(TARGETS admin_executor_service RUNTIME DESTINATION bin COMPONENT aes)
INSTALL(FILES admin_executor_service.cfg common.cfg DESTINATION etc/nel/nelns COMPONENT aes)
