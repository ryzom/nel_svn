SUBDIRS(	build_coarse_mesh
		build_far_bank
		build_smallbank
		ig_lighter
		zone_dependencies
		zone_ig_lighter 
		zone_lighter
		zone_welder
		animation_set_builder
		anim_builder
		build_clod_bank
		build_clodtex
		build_interface
		build_shadow_skin
		cluster_viewer
		file_info
		get_neighbors
		ig_add
		ig_info
		tga_cut
		tga_resize
		zone_check_bind
		zone_dump
		zviewer)
IF(WIN32) 
  ADD_SUBDIRECTORY(object_viewer)
ENDIF(WIN32)

IF(WITH_QT)
  ADD_SUBDIRECTORY(tile_edit_qt)
ENDIF(WITH_QT)

IF(WITH_MAXPLUGIN)
  IF(MAXSDK_FOUND)
    ADD_SUBDIRECTORY(plugin_max)
    ADD_SUBDIRECTORY(ligo)
  ENDIF(MAXSDK_FOUND)
ENDIF(WITH_MAXPLUGIN)

# These use WIndows-specific things that need to be fixed.
#ig_elevation
#lightmap_optimizer
#ligo
#object_viewer_exe
#panoply_maker
#hls_bank_maker
#s3tc_compressor_lib
#tga_2_dds
#plugin_max
