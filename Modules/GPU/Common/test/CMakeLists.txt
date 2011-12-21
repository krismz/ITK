if (ITK_USE_GPU)

# With MS compilers on Win64, we need the /bigobj switch, else generated
# code results in objects with number of sections exceeding object file
# format.
# see http://msdn.microsoft.com/en-us/library/ms173499.aspx
if(CMAKE_CL_64 OR CMAKE_COMPILER_2005)
add_definitions(/bigobj)
endif()

itk_module_test()

set(ITKGPUCommon-tests
 itkGPUImageTest.cxx
itkGPUImageFilterTest.cxx
 #itkGPUImageFilterTestTemp.cxx
 itkGPUMeanImageFilterTest.cxx
 itkGPUBinaryThresholdImageFilterTest.cxx
 itkGPUNeighborhoodOperatorImageFilterTest.cxx
 itkGPUDiscreteGaussianImageFilterTest.cxx
 itkGPUGradientAnisotropicDiffusionImageFilterTest.cxx
# itkGPUDemonsRegistrationFilterTest.cxx
 )

CreateTestDriver(ITKGPUCommon "${ITKGPUCommon-Test_LIBRARIES}" "${ITKGPUCommon-tests}")

itk_add_test(NAME itkGPUImageFilterTest
        COMMAND ITKGPUCommonTestDriver itkGPUImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${ITK_TEST_OUTPUT_DIR}/gpuImageFilterTest.png)

#itk_add_test(NAME itkGPUImageTest COMMAND ITKGPUCommonTestDriver itkGPUImageTest)
#
#temporary test
#itk_add_test(NAME itkGPUImageFilterTestTemp
#        COMMAND ITKGPUCommonTestDriver itkGPUImageFilterTestTemp
#        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
#        ${ITK_TEST_OUTPUT_DIR}/gpuImageFilterTest.png)

itk_add_test(NAME itkGPUMeanImageFilterTest
        COMMAND ITKGPUCommonTestDriver itkGPUMeanImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${ITK_TEST_OUTPUT_DIR}/gpuMeanImageFilterTest.png)

itk_add_test(NAME itkGPUBinaryThresholdImageFilterTest
        COMMAND ITKGPUCommonTestDriver itkGPUBinaryThresholdImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${ITK_TEST_OUTPUT_DIR}/gpuBinaryThresholdImageFilterTest.mha)

itk_add_test(NAME itkGPUNeighborhoodOperatorImageFilterTest
        COMMAND ITKGPUCommonTestDriver itkGPUNeighborhoodOperatorImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${ITK_TEST_OUTPUT_DIR}/gpuNeighborhoodOperatorImageFilterTest.mha)

itk_add_test(NAME itkGPUDiscreteGaussianImageFilterTest
        COMMAND ITKGPUCommonTestDriver itkGPUDiscreteGaussianImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${ITK_TEST_OUTPUT_DIR}/gpuGradientDiscreteGaussianImageFilterTest.mha)

itk_add_test(NAME itkGPUGradientAnisotropicDiffusionImageFilterTest
        COMMAND ITKGPUCommonTestDriver itkGPUGradientAnisotropicDiffusionImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${ITK_TEST_OUTPUT_DIR}/gpuGradientAnisotropicDiffusionImageFilterTest.mha)

#itk_add_test(NAME itkGPUDemonsRegistrationFilterTest
#        COMMAND ITKGPUCommonTestDriver itkGPUDemonsRegistrationFilterTest
#        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
#        ${ITK_TEST_OUTPUT_DIR}/gpuImageFilterTest.png
#        ${ITK_TEST_OUTPUT_DIR}/output)


endif(ITK_USE_GPU)
