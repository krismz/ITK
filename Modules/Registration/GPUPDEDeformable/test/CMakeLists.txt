if (ITK_USE_GPU)

# With MS compilers on Win64, we need the /bigobj switch, else generated
# code results in objects with number of sections exceeding object file
# format.
# see http://msdn.microsoft.com/en-us/library/ms173499.aspx
if(CMAKE_CL_64 OR CMAKE_COMPILER_2005)
add_definitions(/bigobj)
endif()

itk_module_test()

set(ITKGPUPDEDeformable-tests
# itkGPUDemonsRegistrationFilterTest.cxx
 )

CreateTestDriver(ITKGPUPDEDeformable "${ITKGPUPDEDeformable-Test_LIBRARIES}" "${ITKGPUPDEDeformable-tests}")

#itk_add_test(NAME itkGPUDemonsRegistrationFilterTest
#        COMMAND ITKGPUPDEDeformableTestDriver itkGPUDemonsRegistrationFilterTest
#        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
#        ${ITK_TEST_OUTPUT_DIR}/gpuImageFilterTest.png
#        ${ITK_TEST_OUTPUT_DIR}/output)


endif(ITK_USE_GPU)
