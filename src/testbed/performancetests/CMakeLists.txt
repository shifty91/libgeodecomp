lgd_generate_sourcelists("./")

set(RELATIVE_PATH "")
include(auto.cmake)

if(WITH_INTRINSICS)
  add_executable(libgeodecomp_testbed_performancetests main.cpp)
  set_target_properties(libgeodecomp_testbed_performancetests PROPERTIES OUTPUT_NAME performancetests)
  target_link_libraries(libgeodecomp_testbed_performancetests PUBLIC ${LOCAL_LIBGEODECOMP_LINK_LIB})
  if(WITH_HPX)
    hpx_setup_target(libgeodecomp_testbed_performancetests)
  endif()
endif()

if(WITH_CUDA AND WITH_INTRINSICS)
  if(CMAKE_VERSION VERSION_LESS 3.10)
    cuda_add_library(libgeodecomp_testbed_performancetests_cudatests cudatests.cu)
  else()
    add_library(libgeodecomp_testbed_performancetests_cudatests cudatests.cu)
  endif()
  target_link_libraries(libgeodecomp_testbed_performancetests PUBLIC libgeodecomp_testbed_performancetests_cudatests)
  if(WITH_HPX)
    hpx_setup_target(libgeodecomp_testbed_performancetests_cudatests)
  endif()
endif()
