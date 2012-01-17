set(DOCUMENTATION "This module contains the GPU implementation of classes
for deformable image registration based on intensity differences by solving
the PDE, optical flow problem.  This includes Thirion's popular \"demons\"
algorithm.")

itk_module(ITKGPUPDEDeformableRegistration
  DEPENDS
    ITKCommon
    ITKGPUCommon
    ITKPDEDeformableRegistration
    ITKGPUFiniteDifference
  TEST_DEPENDS
    ITKTestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
