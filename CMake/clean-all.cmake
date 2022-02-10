set(binary_dir "${CMAKE_CURRENT_SOURCE_DIR}/build")
if (EXISTS ${binary_dir})
    message(STATUS "Cleaning ${binary_dir}")
    file(REMOVE_RECURSE ${binary_dir})
endif()
message(STATUS "Cleaning build done.")

if (${CLEAN_ALL_DEPS})
    file(GLOB deps_dirs "${CMAKE_CURRENT_SOURCE_DIR}/deps/*")
    foreach(dir ${deps_dirs})
        file(GLOB build_dirs "${dir}/build/*")
        file(GLOB install_dirs "${dir}/install/*")
        foreach(file ${build_dirs} ${install_dirs})
            if (EXISTS ${file})
                message(STATUS "cleaning ${file}")
                file(REMOVE_RECURSE ${file})
            endif()
        endforeach(file)
    endforeach(dir)
    message(STATUS "Cleaning deps done.")
endif()