function(add_lit_testsuite)
# since we do not have AddLLVM cmake module, make a stub here
endfunction(add_lit_testsuite)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(LIBCXXABI_LIBCXX_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/libcxx/include)
  add_subdirectory(libcxxabi)
  link_directories(${CMAKE_CURRENT_BINARY_DIR}/libcxxabi/lib)

  set(LIBCXX_CXX_ABI libcxxabi)
  set(LIBCXX_LIBCXXABI_INCLUDE_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/libcxxabi/include)
  add_subdirectory(libcxx)

  add_dependencies(cxx cxxabi-static)
endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

add_subdirectory(gmock-1.6.0)
