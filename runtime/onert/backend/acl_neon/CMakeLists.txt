# Unsupported architecture
nnfw_find_package(ARMCompute QUIET)
if(NOT ARMCompute_FOUND)
  return()
endif(NOT ARMCompute_FOUND)

set(LIB_ONERT_BACKEND_ACL_NEON onert_backend_acl_neon)

file(GLOB_RECURSE SOURCES "*.cc")

add_library(${LIB_ONERT_BACKEND_ACL_NEON} SHARED ${SOURCES})

target_link_libraries(${LIB_ONERT_BACKEND_ACL_NEON} PRIVATE ${LIB_ONERT_BACKEND_ACL_COMMON})
target_link_libraries(${LIB_ONERT_BACKEND_ACL_NEON} PRIVATE nnfw_common)
target_link_libraries(${LIB_ONERT_BACKEND_ACL_NEON} PRIVATE nnfw_coverage)

# Set install rpath to find onert_core and acl library
set_target_properties(${LIB_ONERT_BACKEND_ACL_NEON} PROPERTIES
  OUTPUT_NAME backend_acl_neon
  INSTALL_RPATH ${ONERT_RPATH_PLUGIN})

install(TARGETS ${LIB_ONERT_BACKEND_ACL_NEON} DESTINATION ${ONERT_INSTALL_BACKENDDIR})
