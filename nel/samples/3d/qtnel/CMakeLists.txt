INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR}/samples/3d/qtnel/ )
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/samples/3d/qtnel/ )
INCLUDE( ${QT_USE_FILE} )

FILE(GLOB QTNEL_SRC *.cpp)
SET(QTNEL_HDR qnelwindow.h qnelwidget.h)
#SET(TILE_EDIT_UIS tile_widget_qt.ui tile_rotation_qt.ui qtnel.ui tile_browser_qt.ui items_edit_qt.ui)
#SET(TILE_EDIT_RCS qtnel.qrc)

DECORATE_NEL_LIB("nel3d")
SET(NL3D_LIB ${LIBNAME})

SET( QT_USE_QTGUI TRUE)
SET( QT_USE_QT3SUPPORT TRUE)
SET( QT_USE_QTXML TRUE)
SET( QT_USE_QTOPENGL TRUE)

#QT4_ADD_RESOURCES( TILE_EDIT_RC_SRCS ${TILE_EDIT_RCS} )
#QT4_WRAP_UI( TILE_EDIT_UI_HDRS ${TILE_EDIT_UIS} )
QT4_WRAP_CPP( QTNEL_MOC_SRCS ${QTNEL_HDR})

#ADD_EXECUTABLE(qtnel ${TILE_EDIT_SRC} ${TILE_EDIT_MOC_SRCS} ${TILE_EDIT_RC_SRCS} ${TILE_EDIT_UI_HDRS})
ADD_EXECUTABLE(qtnel ${QTNEL_SRC} ${QTNEL_MOC_SRCS})

INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR} ${QT_INCLUDES})
TARGET_LINK_LIBRARIES(qtnel ${LIBXML2_LIBRARIES} ${QT_LIBRARIES} ${QT_QTOPENGL_LIBRARY} ${PLATFORM_LINKFLAGS} ${NL3D_LIB})
ADD_DEFINITIONS(${LIBXML2_DEFINITIONS} ${QT_DEFINITIONS})

INSTALL(TARGETS qtnel RUNTIME DESTINATION bin COMPONENT samples3d)

