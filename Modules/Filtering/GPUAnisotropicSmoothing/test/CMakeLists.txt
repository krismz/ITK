if (ITK_USE_GPU)

# With MS compilers on Win64, we need the /bigobj switch, else generated
# code results in objects with number of sections exceeding object file
# format.
# see http://msdn.microsoft.com/en-us/library/ms173499.aspx
if(CMAKE_CL_64 OR CMAKE_COMPILER_2005)
add_definitions(/bigobj)
endif()

itk_module_test()

set(ITKGPUAnisotropicSmoothing-tests
 itkGPUGradientAnisotropicDiffusionImageFilterTest.cxx
)

CreateTestDriver(ITKGPUAnisotropicSmoothing "${ITKGPUAnisotropicSmoothing-Test_LIBRARIES}" "${ITKGPUAnisotropicSmoothing-tests}")

itk_add_test(NAME itkGPUGradientAnisotropicDiffusionImageFilterTest2D
        COMMAND ITKGPUAnisotropicSmoothingTestDriver itkGPUGradientAnisotropicDiffusionImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${ITK_TEST_OUTPUT_DIR}/gpuGradientAnisotropicDiffusionImageFilterTest2D.mha 2)

itk_add_test(NAME itkGPUGradientAnisotropicDiffusionImageFilterTest3D
        COMMAND ITKGPUAnisotropicSmoothingTestDriver itkGPUGradientAnisotropicDiffusionImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${ITK_TEST_OUTPUT_DIR}/gpuGradientAnisotropicDiffusionImageFilterTest3D.mha 3)

endif(ITK_USE_GPU)
