project(ITKGPUCommon)

if (ITK_USE_GPU)
  find_package ( OpenCL REQUIRED )
  if ( NOT ${OPENCL_FOUND} )
     message ( FATAL "Could not find OpenCL" )
  endif ()

  file(GLOB itkGPUSRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cl)
  file(GLOB itkGPUHDR ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
  file(GLOB itkTxxSRC ${CMAKE_CURRENT_SOURCE_DIR}/include/*.txx)

  source_group("OpenCL Source Files" FILES ${itkGPUSRC})

  file(COPY ${itkGPUSRC} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/OpenCL)
  message( "OpenCL files to copy " ${itkGPUSRC} )


  #configure_file(
  #  ${CMAKE_CURRENT_SOURCE_DIR}/src/pathToOpenCLSourceCode.h.in
  #  pathToOpenCLSourceCode.h)

  set(ITKGPUCommon_LIBRARIES ITKGPUCommon)
  set(ITKGPUCommon_SYSTEM_INCLUDE_DIRS ${OPENCL_INCLUDE_DIRS})
  set(ITKGPUCommon_SYSTEM_LIBRARY_DIRS ${OPENCL_LIBRARIES})
endif(ITK_USE_GPU)

itk_module_impl()
