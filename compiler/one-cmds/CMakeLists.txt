# NOTE find_package will try to use at least python3.10 as follows depending on platform version
#   Ubuntu20.04; explictly installed python3.10 (default is python3.8)
#   Ubuntu22.04; default python3.10
#   Ubuntu24.04; explicitly installed python3.8 (default is python3.12)
#   refer https://github.com/Samsung/ONE/issues/9962
#   refer https://github.com/Samsung/ONE/issues/15226
find_package(Python 3.10 EXACT COMPONENTS Interpreter QUIET)
if(NOT Python_FOUND)
  find_package(Python 3.10 COMPONENTS Interpreter QUIET)
endif()

if(NOT Python_Interpreter_FOUND)
  message(STATUS "Build one-cmds: FAILED (Python3 is missing)")
  return()
endif()

# NOTE assume only use 3.10.x or 3.12.x
# NOTE PYTHON_VERSION_MINOR is not used but added for consistancy with common-artifacts and dalgona
if((Python_VERSION VERSION_GREATER_EQUAL 3.10) AND (Python_VERSION VERSION_LESS 3.11))
  set(PYTHON_VERSION_MINOR 10)
elseif((Python_VERSION VERSION_GREATER_EQUAL 3.12) AND (Python_VERSION VERSION_LESS 3.13))
  set(PYTHON_VERSION_MINOR 12)
else()
  # TODO support more
  message(STATUS "Build one-cmds: FAILED (Unsupported python: ${Python_VERSION})")
  return()
endif()

set(PYTHON_EXECUTABLE ${Python_EXECUTABLE})

# NOTE these files should not have extensions.
#      below code will remove extension when copy and install.
set(ONE_COMMAND_FILES
    one-build
    one-create-quant-dataset
    one-import
    one-import-bcq
    one-import-tf
    one-import-tflite
    one-import-onnx
    one-import-pytorch
    one-optimize
    one-quantize
    one-pack
    one-partition
    one-profile
    one-infer
    one-codegen
    one-prepare-venv
    onecc
)

foreach(ONE_COMMAND IN ITEMS ${ONE_COMMAND_FILES})

  set(ONE_COMMAND_FILE ${ONE_COMMAND})
  set(ONE_COMMAND_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${ONE_COMMAND_FILE}")
  # strip extension from the name
  get_filename_component(ONE_COMMNAD_FILE_NAME ${ONE_COMMAND} NAME_WE)
  set(ONE_COMMAND_BIN "${CMAKE_CURRENT_BINARY_DIR}/${ONE_COMMNAD_FILE_NAME}")
  set(ONE_COMMAND_TARGET "${ONE_COMMAND}_target")

  add_custom_command(OUTPUT ${ONE_COMMAND_BIN}
    COMMAND ${CMAKE_COMMAND} -E copy "${ONE_COMMAND_SRC}" "${ONE_COMMAND_BIN}"
    DEPENDS ${ONE_COMMAND_SRC}
    COMMENT "Generate ${ONE_COMMAND_BIN}"
  )

  add_custom_target(${ONE_COMMAND_TARGET} ALL DEPENDS ${ONE_COMMAND_BIN})

  install(FILES ${ONE_COMMAND_BIN}
          PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE
                      GROUP_READ GROUP_EXECUTE
                      WORLD_READ WORLD_EXECUTE
          DESTINATION bin)

endforeach(ONE_COMMAND)

unset(ONNX2CIRCLE_SOURCE_PATH)
# if ONE_ONNX2CIRCLE_PATH is set, install that file to bin folder
if(DEFINED ENV{ONE_ONNX2CIRCLE_PATH})
  set(ONNX2CIRCLE_SOURCE_PATH $ENV{ONE_ONNX2CIRCLE_PATH})
else()
  # if local build/install of circle-mlir exist, use it
  set(ONNX2CIRCLE_PATH "${NNAS_PROJECT_SOURCE_DIR}/circle-mlir/build/install/bin/onnx2circle")
  if (EXISTS ${ONNX2CIRCLE_PATH})
    set(ONNX2CIRCLE_SOURCE_PATH ${ONNX2CIRCLE_PATH})
  else()
    # if circletools/onnx2circle package is installed, use it
    set(ONNX2CIRCLE_PATH "/usr/share/circletools/bin/onnx2circle")
    if (EXISTS ${ONNX2CIRCLE_PATH})
      set(ONNX2CIRCLE_SOURCE_PATH ${ONNX2CIRCLE_PATH})
    endif()
  endif()
endif()
if(DEFINED ONNX2CIRCLE_SOURCE_PATH AND EXISTS ${ONNX2CIRCLE_SOURCE_PATH})
  message(STATUS "ONNX2CIRCLE from ${ONNX2CIRCLE_SOURCE_PATH}")
  install(FILES ${ONNX2CIRCLE_SOURCE_PATH}
          PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE
                      GROUP_READ GROUP_EXECUTE
                      WORLD_READ WORLD_EXECUTE
          DESTINATION bin)
endif()

set(ONE_UTILITY_FILES
    one-build.template.cfg
    onecc.template.cfg
    onnx_legalizer.py
)

foreach(ONE_UTILITY IN ITEMS ${ONE_UTILITY_FILES})

  set(ONE_UTILITY_FILE ${ONE_UTILITY})
  set(ONE_UTILITY_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${ONE_UTILITY_FILE}")
  set(ONE_UTILITY_BIN "${CMAKE_CURRENT_BINARY_DIR}/${ONE_UTILITY_FILE}")
  set(ONE_UTILITY_TARGET "${ONE_UTILITY}_target")

  add_custom_command(OUTPUT ${ONE_UTILITY_BIN}
    COMMAND ${CMAKE_COMMAND} -E copy "${ONE_UTILITY_SRC}" "${ONE_UTILITY_BIN}"
    DEPENDS ${ONE_UTILITY_SRC}
    COMMENT "Generate ${ONE_UTILITY_BIN}"
  )

  add_custom_target(${ONE_UTILITY_TARGET} ALL DEPENDS ${ONE_UTILITY_BIN})

  install(FILES ${ONE_UTILITY}
          PERMISSIONS OWNER_WRITE OWNER_READ
                      GROUP_READ
                      WORLD_READ
          DESTINATION bin)

endforeach(ONE_UTILITY)

# one-pack internally uses model2nnpkg tool
set(MODEL2NNPKG "${NNAS_PROJECT_SOURCE_DIR}/tools/nnpackage_tool/model2nnpkg/model2nnpkg.py")
install(FILES ${MODEL2NNPKG}
              PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE
                          GROUP_READ GROUP_EXECUTE
                          WORLD_READ WORLD_EXECUTE
              DESTINATION bin
              RENAME "model2nnpkg")

# make python directory
set(ONE_PYTHON_FILES argumentparse.py
                     backends.py
                     constant.py
                     export_constant.py
                     make_cmd.py
                     CfgRunner.py
                     Command.py
                     OptionBuilder.py
                     TopologicalSortHelper.py
                     WorkflowRunner.py
                     utils.py)

foreach(ONE_PYTHON_FILE IN ITEMS ${ONE_PYTHON_FILES})

  set(ONE_PYTHON_DIR "onelib")
  set(ONE_PYTHON_DIR_BIN "${CMAKE_CURRENT_BINARY_DIR}/${ONE_PYTHON_DIR}")
  set(ONE_PYTHON_FILE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${ONE_PYTHON_DIR}/${ONE_PYTHON_FILE}")
  set(ONE_PYTHON_FILE_BIN "${CMAKE_CURRENT_BINARY_DIR}/${ONE_PYTHON_DIR}/${ONE_PYTHON_FILE}")
  set(ONE_PYTHON_TARGET "${ONE_PYTHON_FILE}_target")

  add_custom_command(OUTPUT ${ONE_PYTHON_DIR_BIN}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ONE_PYTHON_DIR_BIN}"
    COMMENT "Generate ${ONE_PYTHON_DIR_BIN}"
  )

  add_custom_command(OUTPUT ${ONE_PYTHON_FILE_BIN}
    COMMAND ${CMAKE_COMMAND} -E copy "${ONE_PYTHON_FILE_SRC}" "${ONE_PYTHON_FILE_BIN}"
    DEPENDS ${ONE_PYTHON_SRC}
    COMMENT "Generate ${ONE_PYTHON_FILE_BIN}"
  )

  add_custom_target(${ONE_PYTHON_TARGET} ALL DEPENDS ${ONE_PYTHON_DIR_BIN} ${ONE_PYTHON_FILE_BIN})

  install(DIRECTORY ${ONE_PYTHON_DIR}
          FILE_PERMISSIONS OWNER_WRITE OWNER_READ
                           GROUP_READ
                           WORLD_READ
          DESTINATION bin)

endforeach(ONE_PYTHON_FILE)

set(CONSTANT_EXPORTING_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/${ONE_PYTHON_DIR}/export_constant.py")
set(CONSTANT_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/${ONE_PYTHON_DIR}/constant.py")
set(O1_OPTION "O1")
set(O1_CFG_FILE "${O1_OPTION}.cfg")
set(O1_CFG_FILE_BIN "${CMAKE_CURRENT_BINARY_DIR}/${ONE_PYTHON_DIR}/${O1_CFG_FILE}")

add_custom_command(OUTPUT ${O1_CFG_FILE_BIN}
  COMMAND ${PYTHON_EXECUTABLE} ${CONSTANT_EXPORTING_SCRIPT}
          --constant ${O1_OPTION}
          --format cfg
          --output_path ${O1_CFG_FILE_BIN}
  DEPENDS ${CONSTANT_EXPORTING_SCRIPT} ${CONSTANT_SCRIPT} ${ONE_PYTHON_DIR_BIN}
  COMMENT "Generate ${O1_CFG_FILE}"
)

add_custom_target("O1_cfg_target" ALL DEPENDS ${O1_CFG_FILE_BIN})

install(FILES ${O1_CFG_FILE_BIN}
        PERMISSIONS OWNER_WRITE OWNER_READ
                    GROUP_READ
                    WORLD_READ
        DESTINATION optimization)

set(ONE_DOCUMENT_FILES
    how-to-use-one-commands.txt
    how-to-prepare-virtualenv.txt
    how-to-create-hdf5-dataset.txt
)

foreach(ONE_DOCUMENT IN ITEMS ${ONE_DOCUMENT_FILES})

  install(FILES ${ONE_DOCUMENT} DESTINATION doc)

endforeach(ONE_DOCUMENT)

if(NOT ENABLE_TEST)
  return()
endif(NOT ENABLE_TEST)

add_subdirectory(dummy-driver)
add_subdirectory(tests)
add_subdirectory(validate-onnx2circle)
