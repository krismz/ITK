/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#include "itkGradientDescentObjectOptimizer.h"
#include "itkDemonsImageToImageObjectMetric.h"
#include "itkRegistrationParameterScalesFromShift.h"
#include "itkRegistrationParameterScalesFromJacobian.h"

#include "itkSize.h"
#include "itkImageToData.h"
#include "itkExceptionObject.h"
#include "itkImageRegistrationMethodImageSource.h"

#include "itkIdentityTransform.h"
#include "itkAffineTransform.h"
#include "itkTranslationTransform.h"

/**
 *  This is a test using GradientDescentObjectOptimizer and parameter scales
 *  estimator. The scales are estimated before the first iteration by
 *  RegistrationParameterScalesFromShift. The learning rates are estimated
 *  at each iteration according to the shift of each step.
 */

template< class TMovingTransform >
int itkAutoScaledGradientDescentRegistrationTestTemplated(int numberOfIterations,
                                                          double shiftOfStep,
                                                          std::string scalesOption,
                                                          bool usePhysicalSpaceForShift = true)
{
  const unsigned int Dimension = TMovingTransform::SpaceDimension;
  typedef double PixelType;

  // Fixed Image Type
  typedef itk::Image<PixelType,Dimension>               FixedImageType;

  // Moving Image Type
  typedef itk::Image<PixelType,Dimension>               MovingImageType;

  // Size Type
  typedef typename MovingImageType::SizeType            SizeType;

  // ImageSource
  typedef typename itk::testhelper::ImageRegistrationMethodImageSource<
                                  typename FixedImageType::PixelType,
                                  typename MovingImageType::PixelType,
                                  Dimension >         ImageSourceType;

  typename FixedImageType::ConstPointer    fixedImage;
  typename MovingImageType::ConstPointer   movingImage;
  typename ImageSourceType::Pointer        imageSource;

  imageSource   = ImageSourceType::New();

  SizeType size;
  size[0] = 100;
  size[1] = 100;

  imageSource->GenerateImages( size );

  fixedImage    = imageSource->GetFixedImage();
  movingImage   = imageSource->GetMovingImage();

  // Transform for the moving image
  typedef TMovingTransform MovingTransformType;
  typename MovingTransformType::Pointer movingTransform = MovingTransformType::New();
  movingTransform->SetIdentity();

  // Transform for the fixed image
  typedef itk::IdentityTransform<double, Dimension> FixedTransformType;
  typename FixedTransformType::Pointer fixedTransform = FixedTransformType::New();
  fixedTransform->SetIdentity();

  // ParametersType for the moving transform
  typedef typename MovingTransformType::ParametersType ParametersType;

  // Metric
  typedef itk::DemonsImageToImageObjectMetric
    < FixedImageType, MovingImageType, FixedImageType > MetricType;
  typename MetricType::Pointer metric = MetricType::New();

  // Assign images and transforms to the metric.
  metric->SetFixedImage( fixedImage );
  metric->SetMovingImage( movingImage );
  metric->SetVirtualDomainImage( const_cast<FixedImageType *>(fixedImage.GetPointer()) );

  metric->SetFixedTransform( fixedTransform );
  metric->SetMovingTransform( movingTransform );

  // Initialize the metric to prepare for use
  metric->Initialize();

  // Optimizer
  typedef itk::GradientDescentObjectOptimizer  OptimizerType;
  OptimizerType::Pointer optimizer = OptimizerType::New();

  optimizer->SetMetric( metric );
  optimizer->SetNumberOfIterations( numberOfIterations );

  // Instantiate an Observer to report the progress of the Optimization
  typedef itk::CommandIterationUpdate< OptimizerType >  CommandIterationType;
  CommandIterationType::Pointer iterationCommand = CommandIterationType::New();
  iterationCommand->SetOptimizer( optimizer.GetPointer() );

  // Optimizer parameter scales estimator
  typename itk::OptimizerParameterScalesEstimator::Pointer scalesEstimator;

  typedef itk::RegistrationParameterScalesFromShift< MetricType > ShiftScalesEstimatorType;
  typedef itk::RegistrationParameterScalesFromJacobian< MetricType > JacobianScalesEstimatorType;

  if (scalesOption.compare("shift") == 0)
    {
    std::cout << "Testing RegistrationParameterScalesFromShift" << std::endl;
    typename ShiftScalesEstimatorType::Pointer shiftScalesEstimator
      = ShiftScalesEstimatorType::New();
    shiftScalesEstimator->SetMetric(metric);
    shiftScalesEstimator->SetTransformForward(true); //default
    shiftScalesEstimator->SetUsePhysicalSpaceForShift(usePhysicalSpaceForShift); //true by default
    shiftScalesEstimator->SetSamplingStrategy(ShiftScalesEstimatorType::CornerSampling);
    scalesEstimator = shiftScalesEstimator;
    }
  else
    {
    std::cout << "Testing RegistrationParameterScalesFromJacobian" << std::endl;
    typename JacobianScalesEstimatorType::Pointer jacobianScalesEstimator
      = JacobianScalesEstimatorType::New();
    jacobianScalesEstimator->SetMetric(metric);
    jacobianScalesEstimator->SetTransformForward(true); //default
    jacobianScalesEstimator->SetSamplingStrategy(JacobianScalesEstimatorType::RandomSampling);
    scalesEstimator = jacobianScalesEstimator;
    }

  optimizer->SetScalesEstimator(scalesEstimator);
  // If SetTrustedStepScale is not called, it will use voxel spacing.
  optimizer->SetTrustedStepScale(shiftOfStep);

  std::cout << "Start optimization..." << std::endl
            << "Number of iterations: " << numberOfIterations << std::endl;

  try
    {
    optimizer->StartOptimization();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cout << "Exception thrown ! " << std::endl;
    std::cout << "An error ocurred during Optimization:" << std::endl;
    std::cout << e.GetLocation() << std::endl;
    std::cout << e.GetDescription() << std::endl;
    std::cout << e.what()    << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "...finished. " << std::endl
            << "StopCondition: " << optimizer->GetStopConditionDescription()
            << std::endl
            << "Metric: NumberOfValidPoints: "
            << metric->GetNumberOfValidPoints()
            << std::endl;

  //
  // results
  //
  ParametersType finalParameters  = movingTransform->GetParameters();
  ParametersType fixedParameters  = movingTransform->GetFixedParameters();
  std::cout << "Estimated scales = " << optimizer->GetScales() << std::endl;
  std::cout << "finalParameters = " << finalParameters << std::endl;
  std::cout << "fixedParameters = " << fixedParameters << std::endl;
  bool pass = true;

  ParametersType actualParameters = imageSource->GetActualParameters();
  std::cout << "actualParameters = " << actualParameters << std::endl;
  const unsigned int numbeOfParameters = actualParameters.Size();

  // We know that for the Affine transform the Translation parameters are at
  // the end of the list of parameters.
  const unsigned int offsetOrder = finalParameters.Size()-actualParameters.Size();

  const double tolerance = 1.0;  // equivalent to 1 pixel.

  for(unsigned int i=0; i<numbeOfParameters; i++)
    {
    // the parameters are negated in order to get the inverse transformation.
    // this only works for comparing translation parameters....
    std::cout << finalParameters[i+offsetOrder] << " == " << -actualParameters[i] << std::endl;
    if( vnl_math_abs ( finalParameters[i+offsetOrder] - (-actualParameters[i]) ) > tolerance )
      {
      std::cout << "Tolerance exceeded at component " << i << std::endl;
      pass = false;
      }
    }

  if( !pass )
    {
    std::cout << "Test FAILED." << std::endl;
    return EXIT_FAILURE;
    }
  else
    {
    std::cout << "Test PASSED." << std::endl;
    return EXIT_SUCCESS;
    }
}

int itkAutoScaledGradientDescentRegistrationTest(int argc, char ** const argv)
{
  if( argc > 3 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " [numberOfIterations=50 shiftOfStep=0.5] ";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }
  unsigned int numberOfIterations = 50;
  double shiftOfStep = 0.5;

  if( argc >= 2 )
    {
    numberOfIterations = atoi( argv[1] );
    }
  if (argc >= 3)
    {
    shiftOfStep = atof( argv[2] );
    }

  const unsigned int Dimension = 2;
  int ret1, ret2, ret3, ret4;

  std::cout << std::endl << "Optimizing affine transform with shift scales" << std::endl;
  typedef itk::AffineTransform<double, Dimension> AffineTransformType;
  ret1 = itkAutoScaledGradientDescentRegistrationTestTemplated<AffineTransformType>(numberOfIterations, shiftOfStep, "shift");

  std::cout << std::endl<< "Optimizing affine transform with Jacobian scales" << std::endl;
  typedef itk::AffineTransform<double, Dimension> AffineTransformType;
  ret3 = itkAutoScaledGradientDescentRegistrationTestTemplated<AffineTransformType>(numberOfIterations, shiftOfStep, "jacobian");

  std::cout << std::endl << "Optimizing translation transform with shift scales" << std::endl;
  typedef itk::TranslationTransform<double, Dimension> TranslationTransformType;
  bool usePhysicalSpaceForShift = false;
  ret4 = itkAutoScaledGradientDescentRegistrationTestTemplated<TranslationTransformType>(numberOfIterations, shiftOfStep, "shift", usePhysicalSpaceForShift);

  std::cout << std::endl << "Optimizing translation transform with Jacobian scales" << std::endl;
  typedef itk::TranslationTransform<double, Dimension> TranslationTransformType;
  ret2 = itkAutoScaledGradientDescentRegistrationTestTemplated<TranslationTransformType>(numberOfIterations, 0.0, "jacobian");

  if ( ret1 == EXIT_SUCCESS && ret2 == EXIT_SUCCESS
    && ret3 == EXIT_SUCCESS && ret4 == EXIT_SUCCESS )
    {
    return EXIT_SUCCESS;
    }
  else
    {
    return EXIT_FAILURE;
    }
}
