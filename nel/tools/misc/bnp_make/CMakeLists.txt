FILE(GLOB SRC *.cpp *.h)

DECORATE_NEL_LIB("nelmisc")
SET(NLMISC_LIB ${LIBNAME})

ADD_EXECUTABLE(bnp_make ${SRC})

INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(bnp_make ${LIBXML2_LIBRARIES} ${PLATFORM_LINKFLAGS} ${NLMISC_LIB})
IF(WIN32)
  SET_TARGET_PROPERTIES(bnp_make PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libcmt")
ENDIF(WIN32)
ADD_DEFINITIONS(${LIBXML2_DEFINITIONS})

INSTALL(TARGETS bnp_make RUNTIME DESTINATION bin COMPONENT toolsmisc)
