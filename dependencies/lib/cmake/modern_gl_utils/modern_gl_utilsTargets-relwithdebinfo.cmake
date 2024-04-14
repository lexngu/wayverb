#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "modern_gl_utils" for configuration "RelWithDebInfo"
set_property(TARGET modern_gl_utils APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(modern_gl_utils PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libmodern_gl_utils.a"
  )

list(APPEND _cmake_import_check_targets modern_gl_utils )
list(APPEND _cmake_import_check_files_for_modern_gl_utils "${_IMPORT_PREFIX}/lib/libmodern_gl_utils.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
