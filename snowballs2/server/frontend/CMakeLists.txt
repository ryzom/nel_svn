ADD_SUBDIRECTORY(src)

INSTALL(FILES frontend_service.cfg DESTINATION ${SNOWBALLS_CONFIG_FILE} COMPONENT fesservice)
