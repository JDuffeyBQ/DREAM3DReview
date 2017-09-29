/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _paradisreader_h_
#define _paradisreader_h_

#include <QtCore/QString>
#include <vector>
#include <QtCore/QFile>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/CoreFilters/FileReader.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/MeshStructs.h"
#include "SIMPLib/SIMPLib.h"

#include "DDDAnalysisToolbox/DDDAnalysisToolboxConstants.h"

/**
 * @class ParaDisReader ParaDisReader.h DREAM3DLib/IO/ParaDisReader.h
 * @brief
 * @author mjackson
 * @date Sep 28, 2011
 * @version $Revision$
 */
class  ParaDisReader : public FileReader
{
    Q_OBJECT
  public:
    SIMPL_SHARED_POINTERS(ParaDisReader)
    SIMPL_STATIC_NEW_MACRO(ParaDisReader)
    SIMPL_TYPE_MACRO_SUPER(ParaDisReader, FileReader)

    virtual ~ParaDisReader();
    SIMPL_FILTER_PARAMETER(QString, EdgeDataContainerName)
    Q_PROPERTY(QString EdgeDataContainerName READ getEdgeDataContainerName WRITE setEdgeDataContainerName)
    SIMPL_FILTER_PARAMETER(QString, VertexAttributeMatrixName)
    Q_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)
    SIMPL_FILTER_PARAMETER(QString, EdgeAttributeMatrixName)
    Q_PROPERTY(QString EdgeAttributeMatrixName READ getEdgeAttributeMatrixName WRITE setEdgeAttributeMatrixName)

    SIMPL_FILTER_PARAMETER(QString, InputFile)
    Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
    SIMPL_FILTER_PARAMETER(float, BurgersVector)
    Q_PROPERTY(float BurgersVector READ getBurgersVector WRITE setBurgersVector)

    SIMPL_FILTER_PARAMETER(QString, NumberOfArmsArrayName)
    Q_PROPERTY(QString NumberOfArmsArrayName READ getNumberOfArmsArrayName WRITE setNumberOfArmsArrayName)

    SIMPL_FILTER_PARAMETER(QString, NodeConstraintsArrayName)
    Q_PROPERTY(QString NodeConstraintsArrayName READ getNodeConstraintsArrayName WRITE setNodeConstraintsArrayName)

    SIMPL_FILTER_PARAMETER(QString, BurgersVectorsArrayName)
    Q_PROPERTY(QString BurgersVectorsArrayName READ getBurgersVectorsArrayName WRITE setBurgersVectorsArrayName)

    SIMPL_FILTER_PARAMETER(QString, SlipPlaneNormalsArrayName)
    Q_PROPERTY(QString SlipPlaneNormalsArrayName READ getSlipPlaneNormalsArrayName WRITE setSlipPlaneNormalsArrayName)

    SIMPL_FILTER_PARAMETER(QString, DomainBoundsArrayName)


    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getCompiledLibraryName();

    /**
     * @brief getBrandingString Returns the branding string for the filter, which is a tag
     * used to denote the filter's association with specific plugins
     * @return Branding string
    */
    virtual const QString getBrandingString();

    /**
     * @brief getFilterVersion Returns a version string for this filter. Default
     * value is an empty string.
     * @return
     */
    virtual const QString getFilterVersion();

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getGroupName();

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getSubGroupName();

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    virtual const QString getHumanLabel();

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters();

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
     * @brief execute Reimplemented from @see AbstractFilter class
     */
    virtual void execute();

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    virtual void preflight();

  protected:
    ParaDisReader();

    /**
     * @brief readHeader
     * @return
     */
    virtual int readHeader();

    /**
     * @brief readFile
     * @return
     */
    virtual int readFile();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();

    /**
     * @brief updateVertexInstancePointers
     */
    void updateVertexInstancePointers();

    /**
     * @brief updateEdgeInstancePointers
     */
    void updateEdgeInstancePointers();

  private:
    DEFINE_DATAARRAY_VARIABLE(int32_t, NumberOfArms)
    DEFINE_DATAARRAY_VARIABLE(int32_t, NodeConstraints)
    DEFINE_DATAARRAY_VARIABLE(float, BurgersVectors)
    DEFINE_DATAARRAY_VARIABLE(float, SlipPlaneNormals)
    DEFINE_DATAARRAY_VARIABLE(float, DomainBounds)
    QFile  m_InStream;

    int m_NumVerts;
    int m_NumEdges;

    int m_FileVersion;

    ParaDisReader(const ParaDisReader&); // Copy Constructor Not Implemented
    void operator=(const ParaDisReader&); // Operator '=' Not Implemented
};

#endif /* ParaDisREADER_H_ */





