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
#ifndef __itkRegistrationParameterScalesEstimator_hxx
#define __itkRegistrationParameterScalesEstimator_hxx

#include "itkRegistrationParameterScalesEstimator.h"

namespace itk
{

template< class TMetric >
RegistrationParameterScalesEstimator< TMetric >
::RegistrationParameterScalesEstimator()
{
  // estimate paramter scales of the moving transform
  m_TransformForward = true;

  // number for random sampling
  m_NumberOfRandomSamples = 0;

  // default sampling strategy
  m_SamplingStrategy = FullDomainSampling;

  // the metric object must be set before EstimateScales()
}

/** Estimate the trusted scale for steps. It returns the voxel spacing. */
template< class TMetric >
typename RegistrationParameterScalesEstimator< TMetric >::FloatType
RegistrationParameterScalesEstimator< TMetric >
::EstimateTrustedStepScale()
{
  const typename VirtualImageType::SpacingType & spacing
    = this->m_VirtualImage->GetSpacing();

  const SizeValueType dim = this->GetImageDimension();

  FloatType minSpacing = NumericTraits<FloatType>::max();

  for (SizeValueType d=0; d<dim; d++)
    {
    if (minSpacing > spacing[d])
      {
      minSpacing = spacing[d];
      }
    }

  return minSpacing;
}

/** Validate and set metric and its transforms and images. */
template< class TMetric >
bool
RegistrationParameterScalesEstimator< TMetric >
::CheckAndSetInputs()
{
if (m_Metric.IsNull())
    {
    itkExceptionMacro("RegistrationParameterScalesEstimator: the metric is NULL");
    }

  this->m_FixedImage = m_Metric->GetFixedImage();
  this->m_MovingImage = m_Metric->GetMovingImage();
  this->m_VirtualImage = m_Metric->GetVirtualDomainImage();

  if (this->m_FixedImage.GetPointer() == NULL
    || this->m_MovingImage.GetPointer() == NULL
    || this->m_VirtualImage.GetPointer() == NULL)
    {
    itkExceptionMacro("RegistrationParameterScalesEstimator: the image(s) in the metric is NULL");
    }

  this->m_MovingTransform = m_Metric->GetMovingTransform();
  this->m_FixedTransform = m_Metric->GetFixedTransform();

  if (this->m_MovingTransform.GetPointer() == NULL
    || this->m_FixedTransform.GetPointer() == NULL)
    {
    itkExceptionMacro("RegistrationParameterScalesEstimator: the transform(s) in the metric is NULL.");
    }

  const ParametersType &parameters = this->GetTransform()->GetParameters();
  for (SizeValueType p = 0; p < parameters.size(); p++)
    {
    // check for NaN
    if (parameters[p] != parameters[p])
      {
      itkExceptionMacro("RegistrationParameterScalesEstimator: a transform parameter is not a number.");
      }
    }

  return true;
}

/** Get the transform being estimated scales for. */
template< class TMetric >
const TransformBase *
RegistrationParameterScalesEstimator< TMetric >
::GetTransform()
{
  if (m_TransformForward)
    {
    return m_MovingTransform.GetPointer();
    }
  else
    {
    return m_FixedTransform.GetPointer();
    }
}

/** Get the dimension of the target image transformed to. */
template< class TMetric >
itk::SizeValueType
RegistrationParameterScalesEstimator< TMetric >
::GetImageDimension()
{
  if (m_TransformForward)
    {
    return MovingImageDimension;
    }
  else
    {
    return FixedImageDimension;
    }
}

/** Check if the transform being optimized has local support. */
template< class TMetric >
bool
RegistrationParameterScalesEstimator< TMetric >
::HasLocalSupport()
{
  if (this->m_TransformForward)
    {
    return this->m_MovingTransform->HasLocalSupport();
    }
  else
    {
    return this->m_FixedTransform->HasLocalSupport();
    }
}

/** Get the number of scales. */
template< class TMetric >
SizeValueType
RegistrationParameterScalesEstimator< TMetric >
::GetNumberOfScales()
{
  if (this->GetTransformForward())
    {
    return this->GetMovingTransform()->GetNumberOfLocalParameters();
    }
  else
    {
    return this->GetFixedTransform()->GetNumberOfLocalParameters();
    }
}

/** Update the transform with a change in parameters. */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::UpdateTransformParameters(const ParametersType &deltaParameters)
{
  // Apply the delta parameters to the transform
  if (this->m_TransformForward)
    {
    typename MovingTransformType::Pointer movingTransform =
      const_cast<MovingTransformType *>(this->GetMovingTransform());
    ParametersType &step = const_cast<ParametersType &>(deltaParameters);
    movingTransform->UpdateTransformParameters(step);
    }
  else
    {
    typename FixedTransformType::Pointer fixedTransform =
      const_cast<FixedTransformType *>(this->GetFixedTransform());
    ParametersType &step = const_cast<ParametersType &>(deltaParameters);
    fixedTransform->UpdateTransformParameters(step);
    }
}

/** Transform a physical point to a new physical point.
 *  We want to compute shift in physical space so that the scales is not
 *  sensitive to spacings and directions of image voxel sampling.
 */
template< class TMetric >
template< class TTargetPointType >
void
RegistrationParameterScalesEstimator< TMetric >
::TransformPoint(const VirtualPointType &point,
                 TTargetPointType &mappedPoint)
{
  if (this->GetTransformForward())
    {
    mappedPoint = this->GetMovingTransform()->TransformPoint(point);
    }
  else
    {
    mappedPoint = this->GetFixedTransform()->TransformPoint(point);
    }
}

/** Transform a physical point to its continuous index */
template< class TMetric >
template< class TContinuousIndexType >
void
RegistrationParameterScalesEstimator< TMetric >
::TransformPointToContinuousIndex(const VirtualPointType &point,
                                  TContinuousIndexType &mappedIndex)
{
  if (this->GetTransformForward())
    {
    MovingPointType mappedPoint;
    mappedPoint = this->GetMovingTransform()->TransformPoint(point);
    this->GetMovingImage()->TransformPhysicalPointToContinuousIndex(mappedPoint, mappedIndex);
    }
  else
    {
    FixedPointType mappedPoint;
    mappedPoint = this->GetFixedTransform()->TransformPoint(point);
    this->GetFixedImage()->TransformPhysicalPointToContinuousIndex(mappedPoint, mappedIndex);
    }
}

/** Get the squared norms of the transform Jacobians w.r.t parameters at a point */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::ComputeSquaredJacobianNorms( const VirtualPointType  & point,
                                     ParametersType & squareNorms )
{
  JacobianType jacobian;
  const SizeValueType numPara = this->GetNumberOfScales();
  const SizeValueType dim = this->GetImageDimension();

  if (this->GetTransformForward())
    {
    this->GetMovingTransform()->ComputeJacobianWithRespectToParameters(point, jacobian);

    for (SizeValueType p=0; p<numPara; p++)
      {
      squareNorms[p] = NumericTraits< typename ParametersType::ValueType >::Zero;
      for (SizeValueType d=0; d<dim; d++)
        {
        squareNorms[p] += jacobian[d][p] * jacobian[d][p];
        }
      }
    }
  else
    {
    this->GetFixedTransform()->ComputeJacobianWithRespectToParameters(point, jacobian);

    for (SizeValueType p=0; p<numPara; p++)
      {
      squareNorms[p] = NumericTraits< typename ParametersType::ValueType >::Zero;
      for (SizeValueType d=0; d<dim; d++)
        {
        squareNorms[p] += jacobian[d][p] * jacobian[d][p];
        }
      }
    }
}

/** Sample the virtual image domain with phyical points
 *  and store the results into m_ImageSamples.
 */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::SampleImageDomain()
{
  if (this->HasLocalSupport())
    {
    // Have to use FullDomainSampling for a transform with local support
    this->SetSamplingStrategy(FullDomainSampling);
    }

  if ( !(this->m_SamplingTime < this->GetTimeStamp())
    && !(this->m_SamplingTime < this->m_VirtualImage->GetTimeStamp()) )
    {
    // No modification since last sampling
    return;
    }

  if (m_SamplingStrategy == CornerSampling)
    {
    this->SampleImageDomainWithCorners();
    }
  else if (m_SamplingStrategy == RandomSampling)
    {
    this->SampleImageDomainRandomly();
    }
  else
    {
    this->SampleImageDomainFully();
    }

  this->Modified();
  this->m_SamplingTime = this->GetTimeStamp();
}

/**
 *  Sample the virtual domain with the points at image corners.
 *  and store the results into m_ImageSamples.
 */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::SampleImageDomainWithCorners()
{
  VirtualImageConstPointer image = this->m_VirtualImage;

  VirtualRegionType region = this->m_Metric->GetVirtualDomainRegion();
  VirtualIndexType firstCorner = region.GetIndex();
  VirtualIndexType corner;
  VirtualPointType point;

  VirtualSizeType size = region.GetSize();
  const int cornerNumber = 1 << VirtualImageDimension; // 2^ImageDimension

  m_ImageSamples.resize(cornerNumber);

  for(int i=0; i<cornerNumber; i++)
    {
    int bit;
    for (int d=0; d<VirtualImageDimension; d++)
      {
      bit = (int) (( i & (1 << d) ) != 0); // 0 or 1
      corner[d] = firstCorner[d] + bit * (size[d] - 1);
      }

    image->TransformIndexToPhysicalPoint(corner, point);
    m_ImageSamples[i] = point;
    }
}

/**
 * Sample the physical points of the virtual image in a uniform random distribution.
 */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::SampleImageDomainRandomly()
{
  VirtualImageConstPointer image = this->GetVirtualImage();

  if (m_NumberOfRandomSamples == 0)
    {
    const SizeValueType total = this->m_Metric->GetVirtualDomainRegion().GetNumberOfPixels();
    if (total <= SizeOfSmallDomain)
      {
      m_NumberOfRandomSamples = total;
      }
    else
      {
      FloatType ratio = 1 + vcl_log((FloatType)total/SizeOfSmallDomain);
      //ratio >= 1 since total/SizeOfSmallDomain > 1

      m_NumberOfRandomSamples = static_cast<int>(SizeOfSmallDomain * ratio);
      if (m_NumberOfRandomSamples > total)
        {
        m_NumberOfRandomSamples = total;
        }
      }
    }

  m_ImageSamples.resize(m_NumberOfRandomSamples);

  // Set up a random iterator within the user specified virtual image region.
  typedef ImageRandomConstIteratorWithIndex<VirtualImageType> RandomIterator;
  RandomIterator randIter( image, this->m_Metric->GetVirtualDomainRegion() );

  VirtualPointType point;

  randIter.SetNumberOfSamples( m_NumberOfRandomSamples );
  randIter.GoToBegin();
  for (SizeValueType i=0; i<m_NumberOfRandomSamples; i++)
    {
    image->TransformIndexToPhysicalPoint( randIter.GetIndex(), point );
    m_ImageSamples[i] = point;
    ++randIter;
    }
}

/**
 * Sample the virtual image domain fully with all pixels.
 */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::SampleImageDomainFully()
{

  VirtualImageConstPointer image = this->m_VirtualImage;
  const SizeValueType total = this->m_Metric->GetVirtualDomainRegion().GetNumberOfPixels();
  m_ImageSamples.resize(total);

  /* Set up an iterator within the user specified virtual image region. */
  typedef ImageRegionConstIteratorWithIndex<VirtualImageType> RegionIterator;
  RegionIterator regionIter( image, this->m_Metric->GetVirtualDomainRegion() );

  VirtualPointType point;

  /* Iterate over the image */
  SizeValueType count = 0;
  regionIter.GoToBegin();
  while( !regionIter.IsAtEnd() )
    {
    image->TransformIndexToPhysicalPoint( regionIter.GetIndex(), point );
    m_ImageSamples[count] = point;
    ++regionIter;
    ++count;
    }
}

/**
 * Print the information about this class.
 */
template< class TMetric >
void
RegistrationParameterScalesEstimator< TMetric >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "MetricType   = " << std::endl;
  os << indent << typeid(MetricType).name()  << std::endl;

  os << indent << "FixedTransformType   = " << std::endl;
  os << indent << typeid(FixedTransformType).name()  << std::endl;

  os << indent << "MovingTransformType  = " << std::endl;
  os << indent << typeid(MovingTransformType).name()  << std::endl;

  os << indent << "FixedImageType    = " << std::endl;
  os << indent << typeid(FixedImageType).name()  << std::endl;

  os << indent << "MovingImageType   = " << std::endl;
  os << indent << typeid(MovingImageType).name()  << std::endl;

  os << indent << "VirtualImageType  = " << std::endl;
  os << indent << typeid(VirtualImageType).name()  << std::endl;

  os << indent << "m_ImageSamples.size = " << std::endl;
  os << indent << this->m_ImageSamples.size()  << std::endl;

  os << indent << "m_TransformForward = " << this->m_TransformForward << std::endl;
  os << indent << "m_SamplingStrategy = " << this->m_SamplingStrategy << std::endl;

}

}  // namespace itk

#endif /* __itkRegistrationParameterScalesEstimator_txx */
