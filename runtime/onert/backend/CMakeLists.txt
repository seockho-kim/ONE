# Backend common libs
set(LIB_ONERT_BACKEND_ACL_COMMON onert_backend_acl_common)
set(LIB_ONERT_BACKEND_CL_COMMON onert_backend_cl_common)
add_subdirectory(cl_common)
add_subdirectory(acl_common)

# Backends
set(LIB_ONERT_BACKEND_CPU onert_backend_cpu)
add_subdirectory(cpu)
add_subdirectory(acl_cl)
add_subdirectory(acl_neon)
add_subdirectory(ruy)
add_subdirectory(trix)

# Backend to train
add_subdirectory(train)
