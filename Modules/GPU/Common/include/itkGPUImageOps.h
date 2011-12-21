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
#ifndef __itkGPUImageOps_h
#define __itkGPUImageOps_h

#include "itkMacro.h"

namespace itk
{
/** \class GPUImageOps
 *
 * \brief Provides the kernels for some basic GPU Image Operations
 *
 * \ingroup ITKGPUCommon
 */

/** Create a helper GPU Kernel class for GPUImageOps */
itkGPUKernelClassMacro(GPUImageOpsKernel);

/** GPUImageOps class definition */
class ITK_EXPORT GPUImageOps
{
public:
  /** Standard class typedefs. */
  typedef GPUImageOps Self;

  /** Get OpenCL Kernel source as a string, creates a GetOclSource method */
  itkGetOclSourceFromKernelMacro(GPUImageOpsKernel);

private:
  GPUImageOps();
  virtual ~GPUImageOps();

  GPUImageOps(const Self &);                   //purposely not implemented
  void operator=(const Self &);                //purposely not implemented

};


} // end of namespace itk

#endif
