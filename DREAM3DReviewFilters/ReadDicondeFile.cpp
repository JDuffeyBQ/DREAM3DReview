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

#include "ReadDicondeFile.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "DREAM3DReview/DREAM3DReviewConstants.h"
#include "DREAM3DReview/DREAM3DReviewVersion.h"

#include <QtCore/QFileInfo>

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmimgle/dcmimage.h"

namespace
{
enum createdPathID : RenameDataPath::DataID_t
{
  DataContainerID = 1
};

size_t sizeOfRepresentation(EP_Representation representation)
{
  switch(representation)
  {
  case EP_Representation::EPR_Uint8:
    return sizeof(uint8_t);
  case EP_Representation::EPR_Sint8:
    return sizeof(int8_t);
  case EP_Representation::EPR_Uint16:
    return sizeof(uint16_t);
  case EP_Representation::EPR_Sint16:
    return sizeof(int16_t);
  case EP_Representation::EPR_Uint32:
    return sizeof(uint32_t);
  case EP_Representation::EPR_Sint32:
    return sizeof(int32_t);
  default:
    return 0;
  }
}

bool getSpacing(DcmDataset& dataset, FloatVec3Type& spacing)
{
  OFString dcmSpacing;

  OFCondition found = dataset.findAndGetOFStringArray(DCM_PixelSpacing, dcmSpacing, 1);

  if(found.bad())
  {
    return false;
  }

  QString spacingStr(dcmSpacing.c_str());

  QStringList spacingParts = spacingStr.split('\\');

  if(spacingParts.size() != 2)
  {
    return false;
  }

  bool ok = false;

  float xSpacing = spacingParts[0].toFloat(&ok);

  if(!ok)
  {
    return false;
  }

  float ySpacing = spacingParts[1].toFloat(&ok);

  if(!ok)
  {
    return false;
  }

  spacing.setX(xSpacing);
  spacing.setY(ySpacing);
  spacing.setZ(1.0f);

  return true;
}
} // namespace

struct ReadDicondeFile::Impl
{
  IDataArray::WeakPointer m_DataArray;

  void reset()
  {
    m_DataArray.reset();
  }
};

// -----------------------------------------------------------------------------
ReadDicondeFile::ReadDicondeFile()
: AbstractFilter()
, p_Impl(std::make_unique<Impl>())
, m_DataContainerPath(SIMPL::Defaults::DataContainerName)
{
  initialize();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
ReadDicondeFile::~ReadDicondeFile() = default;

// -----------------------------------------------------------------------------
void ReadDicondeFile::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
void ReadDicondeFile::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFilePath, FilterParameter::Category::Parameter, ReadDicondeFile, "*.dcm", "DICONDE"));
  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Created Geometry", DataContainerPath, FilterParameter::Category::CreatedArray, ReadDicondeFile));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void ReadDicondeFile::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
void ReadDicondeFile::dataCheck()
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

  DcmFileFormat file;

  OFCondition status = file.loadFile(m_InputFilePath.toStdString().c_str());

  if(status.bad())
  {
    QString ss = QObject::tr("Unable to open DICONDE file");
    setErrorCondition(-4, ss);
    return;
  }

  DcmDataset* dataset = file.getDataset();

  if(dataset == nullptr)
  {
    QString ss = QObject::tr("Unable to get DICONDE dataset");
    setErrorCondition(-5, ss);
    return;
  }

  FloatVec3Type spacing(1.0f, 1.0f, 1.0f);

  if(!::getSpacing(*dataset, spacing))
  {
    QString ss = QObject::tr("Unable to get spacing from DICONDE dataset. Assuming [1.0f, 1.0f]");
    setWarningCondition(-6, ss);
  }

  DicomImage image(&file, E_TransferSyntax::EXS_Unknown);
  if(image.getStatus() != EI_Status::EIS_Normal)
  {
    QString ss = QObject::tr("Unable to open image from DICONDE dataset");
    setErrorCondition(-7, ss);
    return;
  }

  const DiPixel* pixels = image.getInterData();

  if(pixels == nullptr)
  {
    QString ss = QObject::tr("Unable to get pixel representation from DICONDE dataset");
    setErrorCondition(-8, ss);
    return;
  }

  uint32_t height = image.getHeight();
  uint32_t width = image.getWidth();
  EP_Representation representation = pixels->getRepresentation();

  std::vector<size_t> dims{static_cast<size_t>(width), static_cast<size_t>(height)};

  ImageGeom::Pointer imageGeom = ImageGeom::CreateGeometry("ImageGeom");

  if(imageGeom == nullptr)
  {
    QString ss = QObject::tr("Unable to create ImageGeometry");
    setErrorCondition(-9, ss);
    return;
  }

  imageGeom->setDimensions(dims[0], dims[1], 1);
  imageGeom->setOrigin(0.0f, 0.0f, 0.0f);
  imageGeom->setSpacing(spacing);

  imageGeom->setUnits(IGeometry::LengthUnit::Millimeter);

  dc->setGeometry(imageGeom);

  auto matrix = dc->createNonPrereqAttributeMatrix(this, SIMPL::Defaults::CellAttributeMatrixName, dims, AttributeMatrix::Type::Cell);
  if(matrix == nullptr)
  {
    return;
  }

  QString dataArrayName("DicondeData");
  std::vector<size_t> cDims{1};

  switch(representation)
  {
  case EP_Representation::EPR_Uint8:
    p_Impl->m_DataArray = matrix->createNonPrereqArray<UInt8ArrayType, AbstractFilter, uint8_t>(this, dataArrayName, 0, cDims);
    break;
  case EP_Representation::EPR_Sint8:
    p_Impl->m_DataArray = matrix->createNonPrereqArray<Int8ArrayType, AbstractFilter, int8_t>(this, dataArrayName, 0, cDims);
    break;
  case EP_Representation::EPR_Uint16:
    p_Impl->m_DataArray = matrix->createNonPrereqArray<UInt16ArrayType, AbstractFilter, uint16_t>(this, dataArrayName, 0, cDims);
    break;
  case EP_Representation::EPR_Sint16:
    p_Impl->m_DataArray = matrix->createNonPrereqArray<Int16ArrayType, AbstractFilter, int16_t>(this, dataArrayName, 0, cDims);
    break;
  case EP_Representation::EPR_Uint32:
    p_Impl->m_DataArray = matrix->createNonPrereqArray<UInt32ArrayType, AbstractFilter, uint32_t>(this, dataArrayName, 0, cDims);
    break;
  case EP_Representation::EPR_Sint32:
    p_Impl->m_DataArray = matrix->createNonPrereqArray<Int32ArrayType, AbstractFilter, int32_t>(this, dataArrayName, 0, cDims);
    break;
  default: {
    QString ss = QObject::tr("Invalid image representation type");
    setErrorCondition(-10, ss);
  }
    return;
  }
}

// -----------------------------------------------------------------------------
void ReadDicondeFile::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
void ReadDicondeFile::execute()
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

  DicomImage image(m_InputFilePath.toStdString().c_str());
  if(image.getStatus() != EI_Status::EIS_Normal)
  {
    QString ss = QObject::tr("Unable to open image from DICONDE dataset");
    setErrorCondition(-12, ss);
    return;
  }

  const DiPixel* pixels = image.getInterData();

  if(pixels == nullptr)
  {
    QString ss = QObject::tr("Unable to get pixel representation from DICONDE dataset");
    setErrorCondition(-13, ss);
    return;
  }

  size_t count = pixels->getCount();
  EP_Representation representation = pixels->getRepresentation();
  size_t typeSize = sizeOfRepresentation(representation);

  if(dataPtr->getSize() != count)
  {
    QString ss = QObject::tr("Size of image and size of DataArray do not match");
    setErrorCondition(-14, ss);
    return;
  }

  if(dataPtr->getTypeSize() != typeSize)
  {
    QString ss = QObject::tr("Size of pixel and size of DataArray type do not match");
    setErrorCondition(-15, ss);
    return;
  }

  const void* pixelData = pixels->getData();

  void* data = dataPtr->getVoidPointer(0);

  std::memcpy(data, pixelData, count * typeSize);
}

// -----------------------------------------------------------------------------
AbstractFilter::Pointer ReadDicondeFile::newFilterInstance(bool copyFilterParameters) const
{
  ReadDicondeFile::Pointer filter = ReadDicondeFile::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
QString ReadDicondeFile::getCompiledLibraryName() const
{
  return DREAM3DReviewConstants::DREAM3DReviewBaseName;
}

// -----------------------------------------------------------------------------
QString ReadDicondeFile::getBrandingString() const
{
  return DREAM3DReviewConstants::DREAM3DReviewBaseName;
}

// -----------------------------------------------------------------------------
QString ReadDicondeFile::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << DREAM3DReview::Version::Major() << "." << DREAM3DReview::Version::Minor() << "." << DREAM3DReview::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
QString ReadDicondeFile::getGroupName() const
{
  return DREAM3DReviewConstants::FilterGroups::DREAM3DReviewFilters;
}

// -----------------------------------------------------------------------------
QString ReadDicondeFile::getSubGroupName() const
{
  return "IO";
}

// -----------------------------------------------------------------------------
QString ReadDicondeFile::getHumanLabel() const
{
  return "Read DICONDE file";
}

// -----------------------------------------------------------------------------
QUuid ReadDicondeFile::getUuid() const
{
  return QUuid("{9f4a5610-067f-5042-a661-d3f51111e980}");
}

// -----------------------------------------------------------------------------
ReadDicondeFile::Pointer ReadDicondeFile::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
ReadDicondeFile::Pointer ReadDicondeFile::New()
{
  struct make_shared_enabler : public ReadDicondeFile
  {
  };
  return std::make_shared<make_shared_enabler>();
}

// -----------------------------------------------------------------------------
QString ReadDicondeFile::getNameOfClass() const
{
  return ClassName();
}

// -----------------------------------------------------------------------------
QString ReadDicondeFile::ClassName()
{
  return QString("ReadDicondeFile");
}

// -----------------------------------------------------------------------------
QString ReadDicondeFile::getInputFilePath() const
{
  return m_InputFilePath;
}

// -----------------------------------------------------------------------------
void ReadDicondeFile::setInputFilePath(const QString& value)
{
  m_InputFilePath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ReadDicondeFile::getDataContainerPath() const
{
  return m_DataContainerPath;
}

// -----------------------------------------------------------------------------
void ReadDicondeFile::setDataContainerPath(const DataArrayPath& value)
{
  m_DataContainerPath = value;
}
