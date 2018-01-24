/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _applytransformationtogeometry_h_
#define _applytransformationtogeometry_h_

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"
/**
 * @brief The ApplyTransformationToGeometry class. See [Filter documentation](@ref applytransformationtogeometry) for details.
 */
class ApplyTransformationToGeometry : public AbstractFilter
{
  Q_OBJECT

public:
  SIMPL_SHARED_POINTERS(ApplyTransformationToGeometry)
  SIMPL_STATIC_NEW_MACRO(ApplyTransformationToGeometry)
   SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ApplyTransformationToGeometry, AbstractFilter)

  virtual ~ApplyTransformationToGeometry();

  SIMPL_FILTER_PARAMETER(DynamicTableData, ManualTransformationMatrix)
  Q_PROPERTY(DynamicTableData ManualTransformationMatrix READ getManualTransformationMatrix WRITE setManualTransformationMatrix)

  SIMPL_FILTER_PARAMETER(DataArrayPath, ComputedTransformationMatrix)
  Q_PROPERTY(DataArrayPath ComputedTransformationMatrix READ getComputedTransformationMatrix WRITE setComputedTransformationMatrix)

  SIMPL_FILTER_PARAMETER(QString, GeometryToTransform)
  Q_PROPERTY(QString GeometryToTransform READ getGeometryToTransform WRITE setGeometryToTransform)

  SIMPL_FILTER_PARAMETER(int, TransformationMatrixType)
  Q_PROPERTY(int TransformationMatrixType READ getTransformationMatrixType WRITE setTransformationMatrixType)

  SIMPL_FILTER_PARAMETER(FloatVec3_t, RotationAxis)
  Q_PROPERTY(FloatVec3_t RotationAxis READ getRotationAxis WRITE setRotationAxis)

  SIMPL_FILTER_PARAMETER(float, RotationAngle)
  Q_PROPERTY(float RotationAngle READ getRotationAngle WRITE setRotationAngle)

  SIMPL_FILTER_PARAMETER(FloatVec3_t, Translation)
  Q_PROPERTY(FloatVec3_t Translation READ getTranslation WRITE setTranslation)

  SIMPL_FILTER_PARAMETER(FloatVec3_t, Scale)
  Q_PROPERTY(FloatVec3_t Scale READ getScale WRITE setScale)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  virtual const QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
  */
  virtual const QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  virtual const QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  virtual const QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  virtual const QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  virtual const QUuid getUuid() override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  virtual const QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  virtual void setupFilterParameters() override;

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  virtual void execute() override;

  /**
  * @brief preflight Reimplemented from @see AbstractFilter class
  */
  virtual void preflight() override;

signals:
  /**
   * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
   * be pushed from a user-facing control (such as a widget)
   * @param filter Filter instance pointer
   */
  void updateFilterParameters(AbstractFilter* filter);

  /**
   * @brief parametersChanged Emitted when any Filter parameter is changed internally
   */
  void parametersChanged();

  /**
   * @brief preflightAboutToExecute Emitted just before calling dataCheck()
   */
  void preflightAboutToExecute();

  /**
   * @brief preflightExecuted Emitted just after calling dataCheck()
   */
  void preflightExecuted();

protected:
  ApplyTransformationToGeometry();

  /**
   * @brief applyTransformation
   */
  void applyTransformation();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

private:
  DEFINE_DATAARRAY_VARIABLE(float, TransformationMatrix)

  FloatArrayType::Pointer m_TransformationReference;

  ApplyTransformationToGeometry(const ApplyTransformationToGeometry&) = delete; // Copy Constructor Not Implemented
  void operator=(const ApplyTransformationToGeometry&);                // Operator '=' Not Implemented
};

#endif /* _ApplyTransformationToGeometry_H_ */
