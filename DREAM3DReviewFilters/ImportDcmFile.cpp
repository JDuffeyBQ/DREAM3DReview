/* ============================================================================
 * Copyright (c) 2020 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ImportDcmFile.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "DREAM3DReview/DREAM3DReviewConstants.h"
#include "DREAM3DReview/DREAM3DReviewVersion.h"

#include <QtCore/QFileInfo>

#include "gdcmImageReader.h"

namespace
{
enum createdPathID : RenameDataPath::DataID_t
{
  DataContainerID = 1
};
} // namespace

struct ImportDcmFile::Impl
{
  IDataArray::WeakPointer m_DataArray;

  void reset()
  {
    m_DataArray.reset();
  }
};

// -----------------------------------------------------------------------------
ImportDcmFile::ImportDcmFile()
: AbstractFilter()
, p_Impl(std::make_unique<Impl>())
, m_DataContainerPath(SIMPL::Defaults::DataContainerName)
{
  initialize();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
ImportDcmFile::~ImportDcmFile() = default;

// -----------------------------------------------------------------------------
void ImportDcmFile::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
void ImportDcmFile::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFilePath, FilterParameter::Category::Parameter, ImportDcmFile, "*.dcm", "DICOM/DICONDE"));
  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Created Geometry", DataContainerPath, FilterParameter::Category::CreatedArray, ImportDcmFile));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void ImportDcmFile::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
void ImportDcmFile::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  p_Impl->reset();

  QFileInfo fileInfo(m_InputFilePath);

  if(m_InputFilePath.isEmpty())
  {
    QString ss = QObject::tr("Input file path must not be empty");
    setErrorCondition(-1, ss);
    return;
  }
  else if(!fileInfo.exists())
  {
    QString ss = QObject::tr("Input file path does not exist");
    setErrorCondition(-2, ss);
    return;
  }

  auto dca = getDataContainerArray();

  if(dca == nullptr)
  {
    QString ss = QObject::tr("Unable to obatin DataContainerArray");
    setErrorCondition(-3, ss);
    return;
  }

  auto dc = dca->createNonPrereqDataContainer(this, m_DataContainerPath, createdPathID::DataContainerID);
  if(dc == nullptr)
  {
    return;
  }

  gdcm::ImageReader imageReader;

  imageReader.SetFileName(m_InputFilePath.toStdString().c_str());

  if(!imageReader.Read())
  {
    QString ss = QObject::tr("Unable to open .DCM file");
    setErrorCondition(-4, ss);
    return;
  }

  gdcm::Image image = imageReader.GetImage();

  size_t size = image.GetBufferLength();

  gdcm::PixelFormat::ScalarType type = image.GetPixelFormat().GetScalarType();

  FloatVec3Type spacing;
  FloatVec3Type origin;
  std::vector<size_t> dims(3, 0);

  for(size_t i = 0; i < 3; i++)
  {
    spacing[i] = static_cast<float>(image.GetSpacing(i));
    origin[i] = static_cast<float>(image.GetOrigin(i));
    dims[i] = static_cast<size_t>(image.GetDimension(i));
  }

  ImageGeom::Pointer imageGeom = ImageGeom::CreateGeometry("ImageGeom");

  if(imageGeom == nullptr)
  {
    QString ss = QObject::tr("Unable to create ImageGeometry");
    setErrorCondition(-6, ss);
    return;
  }

  imageGeom->setDimensions(dims);
  imageGeom->setOrigin(origin);
  imageGeom->setSpacing(spacing);

  imageGeom->setUnits(IGeometry::LengthUnit::Millimeter);

  dc->setGeometry(imageGeom);

  auto matrix = dc->createNonPrereqAttributeMatrix(this, SIMPL::Defaults::CellAttributeMatrixName, dims, AttributeMatrix::Type::Cell);
  if(matrix == nullptr)
  {
    return;
  }

  const QString dataArrayName("Data");
  const std::vector<size_t> cDims{1};

  switch(type)
  {
  case gdcm::PixelFormat::ScalarType::UINT8:
    p_Impl->m_DataArray = matrix->createNonPrereqArray<UInt8ArrayType>(this, dataArrayName, 0, cDims);
    break;
  case gdcm::PixelFormat::ScalarType::INT8:
    p_Impl->m_DataArray = matrix->createNonPrereqArray<Int8ArrayType>(this, dataArrayName, 0, cDims);
    break;
  case gdcm::PixelFormat::ScalarType::UINT16:
    p_Impl->m_DataArray = matrix->createNonPrereqArray<UInt16ArrayType>(this, dataArrayName, 0, cDims);
    break;
  case gdcm::PixelFormat::ScalarType::INT16:
    p_Impl->m_DataArray = matrix->createNonPrereqArray<Int16ArrayType>(this, dataArrayName, 0, cDims);
    break;
  case gdcm::PixelFormat::ScalarType::UINT32:
    p_Impl->m_DataArray = matrix->createNonPrereqArray<UInt32ArrayType>(this, dataArrayName, 0, cDims);
    break;
  case gdcm::PixelFormat::ScalarType::INT32:
    p_Impl->m_DataArray = matrix->createNonPrereqArray<Int32ArrayType>(this, dataArrayName, 0, cDims);
    break;
  default: {
    QString ss = QObject::tr("Invalid image representation type");
    setErrorCondition(-7, ss);
  }
    return;
  }
}

// -----------------------------------------------------------------------------
void ImportDcmFile::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  if(getCancel())
  {
    return;
  }

  auto dataPtr = p_Impl->m_DataArray.lock();

  if(dataPtr == nullptr)
  {
    QString ss = QObject::tr("Unable to obtain DataArray");
    setErrorCondition(-11, ss);
    return;
  }

  gdcm::ImageReader imageReader;

  imageReader.SetFileName(m_InputFilePath.toStdString().c_str());

  if(!imageReader.Read())
  {
    QString ss = QObject::tr("Unable to open .DCM file");
    setErrorCondition(-8, ss);
    return;
  }

  gdcm::Image image = imageReader.GetImage();

  size_t bufferSize = image.GetBufferLength();

  size_t bufferTypeSize = image.GetPixelFormat().GetPixelSize();

  if(dataPtr->getTypeSize() != bufferTypeSize)
  {
    QString ss = QObject::tr("Size of pixel and size of DataArray type do not match");
    setErrorCondition(-9, ss);
    return;
  }

  if(dataPtr->getSize() * dataPtr->getTypeSize() != bufferSize)
  {
    QString ss = QObject::tr("Size of image and size of DataArray do not match");
    setErrorCondition(-10, ss);
    return;
  }

  void* data = dataPtr->getVoidPointer(0);

  if(!image.GetBuffer(reinterpret_cast<char*>(data)))
  {
    QString ss = QObject::tr("Unable to get image data");
    setErrorCondition(-11, ss);
    return;
  }
}

// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportDcmFile::newFilterInstance(bool copyFilterParameters) const
{
  ImportDcmFile::Pointer filter = ImportDcmFile::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
QString ImportDcmFile::getCompiledLibraryName() const
{
  return DREAM3DReviewConstants::DREAM3DReviewBaseName;
}

// -----------------------------------------------------------------------------
QString ImportDcmFile::getBrandingString() const
{
  return DREAM3DReviewConstants::DREAM3DReviewBaseName;
}

// -----------------------------------------------------------------------------
QString ImportDcmFile::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << DREAM3DReview::Version::Major() << "." << DREAM3DReview::Version::Minor() << "." << DREAM3DReview::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
QString ImportDcmFile::getGroupName() const
{
  return DREAM3DReviewConstants::FilterGroups::DREAM3DReviewFilters;
}

// -----------------------------------------------------------------------------
QString ImportDcmFile::getSubGroupName() const
{
  return "IO";
}

// -----------------------------------------------------------------------------
QString ImportDcmFile::getHumanLabel() const
{
  return "Import DCM File";
}

// -----------------------------------------------------------------------------
QUuid ImportDcmFile::getUuid() const
{
  return QUuid("{4ab56073-cc79-56f4-83d7-7575d08ab47c}");
}

// -----------------------------------------------------------------------------
ImportDcmFile::Pointer ImportDcmFile::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
ImportDcmFile::Pointer ImportDcmFile::New()
{
  struct make_shared_enabler : public ImportDcmFile
  {
  };
  return std::make_shared<make_shared_enabler>();
}

// -----------------------------------------------------------------------------
QString ImportDcmFile::getNameOfClass() const
{
  return ClassName();
}

// -----------------------------------------------------------------------------
QString ImportDcmFile::ClassName()
{
  return QString("ImportDcmFile");
}

// -----------------------------------------------------------------------------
QString ImportDcmFile::getInputFilePath() const
{
  return m_InputFilePath;
}

// -----------------------------------------------------------------------------
void ImportDcmFile::setInputFilePath(const QString& value)
{
  m_InputFilePath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ImportDcmFile::getDataContainerPath() const
{
  return m_DataContainerPath;
}

// -----------------------------------------------------------------------------
void ImportDcmFile::setDataContainerPath(const DataArrayPath& value)
{
  m_DataContainerPath = value;
}
