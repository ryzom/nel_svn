FILE(GLOB SRC *.cpp *.h)

DECORATE_NEL_LIB("nelmisc")
SET(NLMISC_LIB ${LIBNAME})

ADD_EXECUTABLE(make_sheet_id ${SRC})

ADD_DEFINITIONS(-DNL_MK_SH_ID_CFG="\\"${NL_ETC_PREFIX}/\\"")

INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(make_sheet_id ${LIBXML2_LIBRARIES} ${PLATFORM_LINKFLAGS} ${NLMISC_LIB})
IF(WIN32)
  SET_TARGET_PROPERTIES(make_sheet_id PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libcmt")
ENDIF(WIN32)
ADD_DEFINITIONS(${LIBXML2_DEFINITIONS})

INSTALL(TARGETS make_sheet_id RUNTIME DESTINATION bin COMPONENT toolsmisc)
INSTALL(FILES make_sheet_id.cfg DESTINATION etc/nel COMPONENT toolsmisc)
