FILE(GLOB SRC *.cpp *.h)

DECORATE_NEL_LIB("nelmisc")
SET(NLMISC_LIB ${LIBNAME})

ADD_EXECUTABLE(xml_packer ${SRC})

INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(xml_packer ${LIBXML2_LIBRARIES} ${PLATFORM_LINKFLAGS} ${NLMISC_LIB})
IF(WIN32)
  SET_TARGET_PROPERTIES(xml_packer PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libcmt")
ENDIF(WIN32)
ADD_DEFINITIONS(${LIBXML2_DEFINITIONS})

INSTALL(TARGETS xml_packer RUNTIME DESTINATION bin COMPONENT toolsmisc)
