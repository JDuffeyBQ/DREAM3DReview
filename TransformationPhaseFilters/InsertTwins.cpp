/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
 * All rights reserved.
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *  This code was written under United States Air Force Contract number
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "InsertTwins.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

const static float m_pi = static_cast<float>(M_PI);


#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InsertTwins::InsertTwins() :
  AbstractFilter(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_ActiveArrayName(DREAM3D::FieldData::Active),
  m_CentroidsArrayName(DREAM3D::FieldData::Centroids),
  m_EquivalentDiametersArrayName(DREAM3D::FieldData::EquivalentDiameters),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_TwinThickness(0.5f),
  m_GrainIds(NULL),
  m_AvgQuats(NULL),
  m_Active(NULL),
  m_Centroids(NULL),
  m_EquivalentDiameters(NULL),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InsertTwins::~InsertTwins()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTwins::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Twin Thickness");
    option->setPropertyName("TwinThickness");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("Equivalent Diameters");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTwins::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setTwinThickness( reader->readValue("TwinThickness", getTwinThickness()) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int InsertTwins::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("TwinThickness", getTwinThickness() );
    writer->closeFilterGroup();
    return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTwins::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  // Cell Data
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -302, float, FloatArrayType, fields, 4)

  // Field Data
  GET_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, -302, float, FloatArrayType, fields, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Centroids, ss, -304, float, FloatArrayType, fields, 3)

  // Ensemble Data
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, true, fields, 1)

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -305, unsigned int, XTalStructArrayType, ensembles, 1)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTwins::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTwins::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  setErrorCondition(0);

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  insert_twins();

  notifyStatusMessage("Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTwins::insert_twins()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  DREAM3D_RANDOMNG_NEW()

  size_t totalFields = m->getNumFieldTuples();
  int64_t totalPoints = m->getTotalPoints();
  int xPoints = static_cast<int>(m->getXPoints());
  int yPoints = static_cast<int>(m->getYPoints());
  int zPoints = static_cast<int>(m->getZPoints());
  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

  float sample111[3] = {0.0f,0.0f,0.0f};
  float crystal111[3] = {0.0f,0.0f,0.0f};
  QuatF q1;
  QuatF q2;
  float g[3][3], gT[3][3], rotMat[3][3], newMat[3][3];
  float plateThickness = 0.0f;
  size_t numGrains = totalFields;
  float x, y, z, d, d2, D, D2, random;
  float sig3 = 60.0f * (m_pi/180.0f);
  float e[3];

  for (size_t curGrain = 1; curGrain < numGrains; ++curGrain)
  {
    QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

	// randomly pick from the {111} family of planes
	for (int i = 0; i < 3; ++i)
	{
      random = static_cast<float>(rg.genrand_res53());
	  crystal111[i] = 1.0f;
	  if (random < 0.5f) crystal111[i] = -1.0f;
	}

	// find where {111} points
	QuaternionMathF::Copy(avgQuats[curGrain], q1);
    OrientationMath::QuattoMat(q1, g);
	MatrixMath::Transpose3x3(g, gT);
    MatrixMath::Multiply3x3with3x1(g, crystal111, sample111);

	// define plate = user input fraction of eq dia centered at centroid
	plateThickness = m_EquivalentDiameters[curGrain] * m_TwinThickness * 0.5f;

	// set the origin of the plane
	d = -sample111[0] * m_Centroids[3*curGrain+0] - sample111[1] * m_Centroids[3*curGrain+1] - sample111[2] * m_Centroids[3*curGrain+2];

	d2 = -sample111[0] * (m_Centroids[3*curGrain+0] + plateThickness*2) - sample111[1] * (m_Centroids[3*curGrain+1] + plateThickness*2) - sample111[2] * (m_Centroids[3*curGrain+2] + plateThickness*2);

	// generate twin orientation with a 60 degree rotation about the {111}
	OrientationMath::AxisAngletoMat(sig3, 1, 1, 1, rotMat);
	MatrixMath::Multiply3x3with3x3(g, rotMat, newMat);
	OrientationMath::MatToEuler(newMat, e[0], e[1], e[2]);
	OrientationMath::EulertoQuat(q2, e[0], e[1], e[2]);

	// loop through all cells to find matching grain IDs
	int zStride, yStride;
	for(int i = 0; i < zPoints; i++)
	{
	  zStride = i * xPoints * yPoints;
	  for (int j = 0 ; j < yPoints; j++)
	  {
		yStride = j * xPoints;
		for(int k = 0; k < xPoints; k++)
		{
		  int gnum = m_GrainIds[zStride+yStride+k];
		  x = float(k) * xRes;
		  y = float(j) * yRes;
		  z = float(i) * zRes;

		  // if the grain IDs match...
		  if (gnum == curGrain)
		  {
			// calculate the distance between the cell and the plane
			float denom = 1 / sqrtf(sample111[0] * sample111[0] + sample111[1] * sample111[1] + sample111[2] * sample111[2]);
			D = sample111[0] * x * denom + sample111[1] * y * denom + sample111[2] * z * denom + d * denom;  
			D2 = sample111[0] * x * denom + sample111[1] * y * denom + sample111[2] * z * denom + d2 * denom;  
			int stop = 0;

			// if the cell-plane distance is less than the plate thickness then place a twin voxel
			if(fabs(D) < plateThickness || fabs(D2) < plateThickness)
			{
			  m_GrainIds[zStride+yStride+k] = totalFields;
			}
		  }
		}
	  }
	}
  }
  totalFields = transfer_attributes(totalFields, totalPoints, q2);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t InsertTwins::transfer_attributes(size_t totalFields, size_t totalPoints, QuatF q)
{
  VoxelDataContainer* m = getVoxelDataContainer();
  m->resizeFieldDataArrays(totalFields + 1);
  dataCheck(false, totalPoints, totalFields + 1, m->getNumEnsembleTuples());
  m_AvgQuats[4*totalFields+0] = q.x;
  m_AvgQuats[4*totalFields+1] = q.y;
  m_AvgQuats[4*totalFields+2] = q.z;
  m_AvgQuats[4*totalFields+3] = q.w;
  ++totalFields;
  return totalFields;
}