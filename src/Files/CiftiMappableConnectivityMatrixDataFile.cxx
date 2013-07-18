
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __CIFTI_MAPPABLE_CONNECTIVITY_MATRIX_DATA_FILE_DECLARE__
#include "CiftiMappableConnectivityMatrixDataFile.h"
#undef __CIFTI_MAPPABLE_CONNECTIVITY_MATRIX_DATA_FILE_DECLARE__

#include "CaretAssert.h"
#include "CiftiFacade.h"
#include "CaretLogger.h"
#include "CiftiInterface.h"
#include "EventManager.h"
#include "EventProgressUpdate.h"
#include "SceneClass.h"

using namespace caret;


    
/**
 * \class caret::CiftiMappableConnectivityMatrixDataFile 
 * \brief Data file for Cifti Connectivity Matrix Files.
 * \ingroup Files
 */

/**
 * Constructor.
 */
CiftiMappableConnectivityMatrixDataFile::CiftiMappableConnectivityMatrixDataFile(const DataFileTypeEnum::Enum dataFileType,
                                                                                 const FileReading fileReading,
                                                                                 const IndicesMapToDataType rowIndexType,
                                                                                 const IndicesMapToDataType columnIndexType,
                                                                                 const DataAccess brainordinateMappedDataAccess,
                                                                                 const DataAccess seriesDataAccess)
: CiftiMappableDataFile(dataFileType,
                        fileReading,
                        rowIndexType,
                        columnIndexType,
                        brainordinateMappedDataAccess,
                        seriesDataAccess)
{
    clearPrivate();
}

/**
 * Destructor.
 */
CiftiMappableConnectivityMatrixDataFile::~CiftiMappableConnectivityMatrixDataFile()
{
    clearPrivate();
}

/**
 * Clear the contents of the file.
 */
void
CiftiMappableConnectivityMatrixDataFile::clear()
{
    CiftiMappableDataFile::clear();
    clearPrivate();
}

/**
 * Clear the contents of the file.
 * Note that "clear()" is virtual and cannot be called from destructor.
 */
void
CiftiMappableConnectivityMatrixDataFile::clearPrivate()
{
    m_loadedRowData.clear();
    m_rowLoadedTextForMapName = "";
    m_rowLoadedText = "";
    m_dataLoadingEnabled = true;
}

int64_t
CiftiMappableConnectivityMatrixDataFile::getRowLoadedIndex() const
{
	return m_selectionIndex;
}



/**
 * @return Is this file empty?
 */
bool
CiftiMappableConnectivityMatrixDataFile::isEmpty() const
{
    if (CiftiMappableDataFile::isEmpty()) {
        return true;
    }

//    if (m_loadedRowData.empty()) {
//        return true;
//    }
    
    return false;
}

/**
 * @return Is loading of data enabled.  Note that if
 * disabled, any previously loaded data is NOT removed
 * so that it can still be displayed but not updated.
 */
bool
CiftiMappableConnectivityMatrixDataFile::isMapDataLoadingEnabled(const int32_t /*mapIndex*/) const
{
    return m_dataLoadingEnabled;
}

/**
 * Set loading of data enabled.  Note that if
 * disabled, any previously loaded data is NOT removed
 * so that it can still be displayed but not updated.
 *
 * @param dataLoadingEnabled
 *   New data loading enabled status.
 */
void
CiftiMappableConnectivityMatrixDataFile::setMapDataLoadingEnabled(const int32_t /*mapIndex*/,
                                                          const bool dataLoadingEnabled)
{
    m_dataLoadingEnabled = dataLoadingEnabled;
}

/**
 * Get the data for the given map index.
 *
 * @param mapIndex
 *     Index of the map.
 * @param dataOut
 *     A vector that will contain the data for the map upon exit.
 */
void
CiftiMappableConnectivityMatrixDataFile::getMapData(const int32_t mapIndex,
                                  std::vector<float>& dataOut) const
{
    //int nCols = m_ciftiInterface->getNumberOfColumns();
    //dataOut.resize(nCols);
    //m_ciftiInterface->getColumn(&dataOut[0],mapIndex);
    dataOut = m_loadedRowData;
}

///**
// * For the given row index, find its corresponding surface structure and
// * node index.
// *
// * @param rowIndex
// *    The file's row index.
// * @param structureOut
// *    Output structure corresponding to row index.
// * @param nodeIndexOut
// *    Output node index corresponding to row index.
// * @return true if the row corresponds to a surface structure and node index
// *    else false.
// */
//bool
//CiftiMappableConnectivityMatrixDataFile::getStructureAndNodeIndexFromRowIndex(const int64_t rowIndex,
//                                                                      StructureEnum::Enum& structureOut,
//                                                                      int64_t& nodeIndexOut) const
//{
//    CaretAssert(m_ciftiFacade);
//    
//    const int64_t numberOfRows = m_ciftiFacade->getNumberOfRows();
//    if ((rowIndex >= 0)
//        && (rowIndex < numberOfRows)) {
//        std::vector<StructureEnum::Enum> surfaceStructures;
//        std::vector<StructureEnum::Enum> volumeStructures;
//        
//        const CiftiXML& ciftiXML = m_ciftiInterface->getCiftiXML();
//        ciftiXML.getStructureListsForColumns(surfaceStructures,
//                                             volumeStructures);
//        
//        /*
//         * Try to find the CIFTI row in the available surface structures.
//         */
//        for (std::vector<StructureEnum::Enum>::iterator structureIter = surfaceStructures.begin();
//             structureIter != surfaceStructures.end();
//             structureIter++) {
//            const StructureEnum::Enum structure = *structureIter;
//            std::vector<CiftiSurfaceMap> surfaceMaps;
//            ciftiXML.getSurfaceMapForColumns(surfaceMaps, structure);
//            
//            /*
//             * Search the surface maps for the structure.
//             */
//            for (std::vector<CiftiSurfaceMap>::iterator surfaceMapIter = surfaceMaps.begin();
//                 surfaceMapIter != surfaceMaps.end();
//                 surfaceMapIter++) {
//                CiftiSurfaceMap& csm = *surfaceMapIter;
//                if (csm.m_ciftiIndex == rowIndex) {
//                    structureOut = structure;
//                    nodeIndexOut = csm.m_surfaceNode;
//                    return true;
//                }
//            }
//        }
//    }
//    
//    return false;
//}
//
///**
// * For the given row index, find its corresponding voxel indices and
// * coordinate.
// *
// * @param rowIndex
// *    The file's row index.
// * @param ijkOut
// *    Output voxel indices corresponding to row index.
// * @param xyzOut
// *    Output voxel coordinates corresponding to row index.
// * @return true if the row corresponds to a voxel else false.
// */
//bool
//CiftiMappableConnectivityMatrixDataFile::getVoxelIndexAndCoordinateFromRowIndex(const int64_t rowIndex,
//                                                                                int64_t ijkOut[3],
//                                                                                float xyzOut[3]) const
//{
//    CaretAssert(m_ciftiFacade);
//    
//    const int64_t numberOfRows = m_ciftiFacade->getNumberOfRows();
//    if ((rowIndex >= 0)
//        && (rowIndex < numberOfRows)) {
//        std::vector<CiftiVolumeMap> volumeMaps;
//        
//        const CiftiXML& ciftiXML = m_ciftiInterface->getCiftiXML();
//        ciftiXML.getVolumeMapForColumns(volumeMaps);
//        
//        /*
//         * Search the surface maps for the structure.
//         */
//        for (std::vector<CiftiVolumeMap>::iterator volumeMapIter = volumeMaps.begin();
//             volumeMapIter != volumeMaps.end();
//             volumeMapIter++) {
//            CiftiVolumeMap& cvm = *volumeMapIter;
//            if (cvm.m_ciftiIndex == rowIndex) {
//                ijkOut[0] = cvm.m_ijk[0];
//                ijkOut[1] = cvm.m_ijk[1];
//                ijkOut[2] = cvm.m_ijk[2];
//                
//                indexToSpace(ijkOut,
//                             xyzOut);
//                return true;
//            }
//        }
//        
//    }
//    return false;
//}

/**
 * Get the index of a row when loading data for a surface node.
 * @param structure
 *    Structure of the surface.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @param nodeIndex
 *    Index of the node.
 * @return
 *    Index of row corresponding to node or -1 if no row in the
 *    matrix corresponds to the node.
 */
int64_t
CiftiMappableConnectivityMatrixDataFile::getRowIndexForNodeWhenLoading(const StructureEnum::Enum structure,
                                                               const int64_t surfaceNumberOfNodes,
                                                               const int64_t nodeIndex) 
{
    if (isCiftiInterfaceValid() == false) {
        return -1;
    }
    
    int64_t rowIndex = -1;
    
    const CiftiXML& ciftiXML = m_ciftiInterface->getCiftiXML();
    
    /*
     * Get the mapping type
     */
    const IndicesMapToDataType rowMappingType = ciftiXML.getMappingType(CiftiXML::ALONG_COLUMN);
    
    if (ciftiXML.getSurfaceNumberOfNodes(CiftiXML::ALONG_COLUMN,
                                         structure) == surfaceNumberOfNodes) {
        bool isBrainModels = false;
        bool isParcels     = false;
        switch (rowMappingType) {
            case CIFTI_INDEX_TYPE_BRAIN_MODELS:
                isBrainModels = true;
                break;
            case CIFTI_INDEX_TYPE_FIBERS:
                break;
            case CIFTI_INDEX_TYPE_INVALID:
                break;
            case CIFTI_INDEX_TYPE_LABELS:
                break;
            case CIFTI_INDEX_TYPE_PARCELS:
                isParcels = true;
                break;
            case CIFTI_INDEX_TYPE_SCALARS:
                break;
            case CIFTI_INDEX_TYPE_TIME_POINTS:
                break;
        }
        
        if (isBrainModels) {
            rowIndex = ciftiXML.getRowIndexForNode(nodeIndex, structure);
        }
        else if (isParcels) {
            rowIndex = ciftiXML.getColumnParcelForNode(nodeIndex,
                                                          structure);
        }
        else {
            CaretAssert(0);
            CaretLogSevere("Invalid row mapping type for connectivity file "
                           + DataFileTypeEnum::toName(getDataFileType()));
        }
    }
    
    return (m_selectionIndex = rowIndex);
}


/**
 * Get the index of a row when loading data for a voxel at a coordinate.
 * @param xyz
 *    Coordinate of the voxel.
 * @return
 *    Index of row corresponding to voxel or negative if no row in the
 *    matrix corresponds to the voxel.
 */
int64_t
CiftiMappableConnectivityMatrixDataFile::getRowIndexForVoxelAtCoordinateWhenLoading(const float xyz[3])
{
    if (isCiftiInterfaceValid() == false) {
        return -1;
    }
    
    int64_t rowIndex = -1;
    
    const CiftiXML& ciftiXML = m_ciftiInterface->getCiftiXML();
    const IndicesMapToDataType rowMappingType = ciftiXML.getMappingType(CiftiXML::ALONG_COLUMN);
    
    /*
     * Get the mapping type
     */
    bool isBrainModels = false;
    bool isParcels     = false;
    switch (rowMappingType) {
        case CIFTI_INDEX_TYPE_BRAIN_MODELS:
            isBrainModels = true;
            break;
        case CIFTI_INDEX_TYPE_FIBERS:
            break;
        case CIFTI_INDEX_TYPE_INVALID:
            break;
        case CIFTI_INDEX_TYPE_LABELS:
            break;
        case CIFTI_INDEX_TYPE_PARCELS:
            isParcels = true;
            break;
        case CIFTI_INDEX_TYPE_SCALARS:
            break;
        case CIFTI_INDEX_TYPE_TIME_POINTS:
            break;
    }
    
    if (isBrainModels) {
        rowIndex = ciftiXML.getRowIndexForVoxelCoordinate(xyz);
        
        if (rowIndex < 0) {
            std::vector<CiftiParcelElement> parcels;
            ciftiXML.getParcelsForColumns(parcels);
        }
    }
    else if (isParcels) {
        int64_t ijk[3];
        enclosingVoxel(xyz[0], xyz[1], xyz[2], ijk[0], ijk[1], ijk[2]);
        if (indexValid(ijk[0], ijk[1], ijk[2])) {
            rowIndex = ciftiXML.getColumnParcelForVoxel(ijk);
        }
    }
    else {
        CaretAssert(0);
        CaretLogSevere("Invalid row mapping type for connectivity file "
                       + DataFileTypeEnum::toName(getDataFileType()));
    }
    
    return (m_selectionIndex = rowIndex);
    
}

/**
 * Get the index of a row when loading data for a voxel index.
 *
 * @param ijk
 *    Indices of the voxel.
 * @return
 *    Index of row corresponding to voxel or negative if no row in the
 *    matrix corresponds to the voxel.
 */
int64_t
CiftiMappableConnectivityMatrixDataFile::getRowIndexForVoxelIndexWhenLoading(const int64_t ijk[3])
{  
    if (isCiftiInterfaceValid() == false) {
        return -1;
    }
    
    int64_t rowIndex = -1;
    
    const CiftiXML& ciftiXML = m_ciftiInterface->getCiftiXML();
    const IndicesMapToDataType rowMappingType = ciftiXML.getMappingType(CiftiXML::ALONG_COLUMN);
    
    /*
     * Get the mapping type
     */
    bool isBrainModels = false;
    bool isParcels     = false;
    switch (rowMappingType) {
        case CIFTI_INDEX_TYPE_BRAIN_MODELS:
            isBrainModels = true;
            break;
        case CIFTI_INDEX_TYPE_FIBERS:
            break;
        case CIFTI_INDEX_TYPE_INVALID:
            break;
        case CIFTI_INDEX_TYPE_LABELS:
            break;
        case CIFTI_INDEX_TYPE_PARCELS:
            isParcels = true;
            break;
        case CIFTI_INDEX_TYPE_SCALARS:
            break;
        case CIFTI_INDEX_TYPE_TIME_POINTS:
            break;
    }
    
    if (indexValid(ijk[0], ijk[1], ijk[2])) {
        if (isBrainModels) {
            rowIndex = ciftiXML.getRowIndexForVoxel(ijk);
        }
        else if (isParcels) {
            rowIndex = ciftiXML.getColumnParcelForVoxel(ijk);
        }
        else {
            CaretAssert(0);
            CaretLogSevere("Invalid row mapping type for connectivity file "
                           + DataFileTypeEnum::toName(getDataFileType()));
        }
    }
    
    m_selectionIndex = rowIndex;
    return rowIndex;
}

/**
 * Set the loaded row data to zeros.
 */
void
CiftiMappableConnectivityMatrixDataFile::setLoadedRowDataToAllZeros()
{
    if (m_loadedRowData.empty() == false){
        std::fill(m_loadedRowData.begin(),
                  m_loadedRowData.end(),
                  0.0);
    }
    if (m_mapContent.empty() == false) {
        m_mapContent[0]->invalidateColoring();
    }
}


/**
 * Load connectivity data for the surface's node.
 *
 * @param mapIndex
 *    Index of map.
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param structure
 *    Surface's structure.
 * @param nodeIndex
 *    Index of node number.
 * @return
 *    Index of row that was loaded or -1 if no data was loaded.
 * @throw
 *    DataFileException if there is an error.
 */
int64_t
CiftiMappableConnectivityMatrixDataFile::loadMapDataForSurfaceNode(const int32_t /*mapIndex*/,
                                                           const int32_t surfaceNumberOfNodes,
                                                           const StructureEnum::Enum structure,
                                                           const int32_t nodeIndex) throw (DataFileException)
{
    if (isCiftiInterfaceValid() == false) {
        setLoadedRowDataToAllZeros();
        return -1;
    }
    
    /*
     * Loading of data disabled?
     */
    if (m_dataLoadingEnabled == false) {
        return -1;
    }
    
    setLoadedRowDataToAllZeros();
    
    int64_t rowIndex = -1;
    
    try {
        bool dataWasLoaded = false;
        
        rowIndex = getRowIndexForNodeWhenLoading(structure,
                                                 surfaceNumberOfNodes,
                                                 nodeIndex);
        
        if (rowIndex >= 0) {
            const int64_t dataCount = m_ciftiInterface->getNumberOfColumns();
            if (dataCount > 0) {
                m_rowLoadedTextForMapName = ("Row: "
                                         + AString::number(rowIndex)
                                         + ", Node Index: "
                                         + AString::number(nodeIndex)
                                         + ", Structure: "
                                         + StructureEnum::toName(structure));

                m_rowLoadedText = ("Row_"
                                   + AString::number(rowIndex)
                                   + "_Node_Index_"
                                   + AString::number(nodeIndex)
                                   + "_Structure_"
                                   + StructureEnum::toGuiName(structure));
                CaretAssert((rowIndex >= 0) && (rowIndex < m_ciftiInterface->getNumberOfRows()));
                m_loadedRowData.resize(dataCount);
                m_ciftiInterface->getRow(&m_loadedRowData[0],
                                         rowIndex);
                
                CaretLogFine("Read row for node " + AString::number(nodeIndex));
                
                dataWasLoaded = true;
            }
        }
        
        if (dataWasLoaded == false) {
            CaretLogFine("FAILED to read row for node " + AString::number(nodeIndex));
        }
    }
    catch (CiftiFileException& e) {
        throw DataFileException(e.whatString());
    }
    
    CaretAssertVectorIndex(m_mapContent, 0);
    m_mapContent[0]->invalidateColoring();
    
    return (m_selectionIndex = rowIndex);
}


/**
 * Load connectivity data for the given map index.
 *
 * @param mapIndex
 *    Index of map.
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param structure
 *    Surface's structure.
 * @param nodeIndex
 *    Index of node number.
 * @return
 *    Index of row that was loaded or -1 if no data was loaded.
 * @throw
 *    DataFileException if there is an error.
 */
bool 
CiftiMappableConnectivityMatrixDataFile::loadMapData(const int32_t selectionIndex) throw (DataFileException)
{
    if (isCiftiInterfaceValid() == false) {
        return false;
    }
    
    /*
     * Loading of data disabled?
     */
    if (m_dataLoadingEnabled == false) {
        return false;
    }
    
    
    
    try {
        bool dataWasLoaded = false;
        
        if (selectionIndex >= 0) {
            const int64_t dataCount = m_ciftiInterface->getNumberOfColumns();
            if (dataCount > 0) {
                m_rowLoadedTextForMapName = ("Row: "
                                         + AString::number(selectionIndex+1)                                         
                                         );

                m_rowLoadedText = ("Row_"
                                   + AString::number(selectionIndex+1)
                                   );
                CaretAssert((selectionIndex >= 0) && (selectionIndex < m_ciftiInterface->getNumberOfRows()));
                m_loadedRowData.resize(dataCount);
                m_ciftiInterface->getRow(&m_loadedRowData[0],
                                         selectionIndex);
                
                CaretLogFine("Read row " + AString::number(selectionIndex+1));
                
                dataWasLoaded = true;
            }
        }
        
        if (dataWasLoaded == false) {
            CaretLogFine("FAILED to read row " + AString::number(selectionIndex+1));
        }
    }
    catch (CiftiFileException& e) {
        throw DataFileException(e.whatString());
    }
    
    CaretAssertVectorIndex(m_mapContent, 0);
    m_mapContent[0]->invalidateColoring();
    
    m_selectionIndex = selectionIndex;
    return true;
}

/**
 * Load connectivity data for the surface's nodes and then average the data.
 *
 * @param mapIndex
 *    Index of map.
 * @param surfaceFile
 *    Surface file used for structure.
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param structure
 *    Surface's structure.
 * @param nodeIndices
 *    Indices of nodes.
 * @throw
 *    DataFileException if there is an error.
 */
void
CiftiMappableConnectivityMatrixDataFile::loadMapAverageDataForSurfaceNodes(const int32_t /*mapIndex*/,
                                                                   const int32_t surfaceNumberOfNodes,
                                                                   const StructureEnum::Enum structure,
                                                                   const std::vector<int32_t>& nodeIndices) throw (DataFileException)
{
    if (isCiftiInterfaceValid() == false) {
        setLoadedRowDataToAllZeros();
        return;
    }
    
    /*
     * Loading of data disabled?
     */
    if (m_dataLoadingEnabled == false) {
        return;
    }
    
    setLoadedRowDataToAllZeros();
    
    const int32_t numberOfNodeIndices = static_cast<int32_t>(nodeIndices.size());
    if (numberOfNodeIndices <= 0) {
        return;
    }
    
    const bool isDenseMatrix = (getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE);
    const int32_t progressUpdateInterval = 1;
    
    try {
        bool dataWasLoaded = false;
        
        const int64_t dataCount = m_ciftiInterface->getNumberOfColumns();
        if (dataCount > 0) {
            /*
             * Contains the average row
             */
            std::vector<float> dataAverageVector(dataCount, 0.0);
            float* dataAverage = &dataAverageVector[0];
            
            /*
             * Contains row for a node
             */
            std::vector<float> dataRowVector(dataCount, 0.0);
            float* dataRow = &dataRowVector[0];
            
            int64_t rowSuccessCount = 0;
            
            bool userCancelled = false;
            EventProgressUpdate progressEvent(0,
                                              numberOfNodeIndices,
                                              0,
                                              ("Loading data for "
                                               + QString::number(numberOfNodeIndices)
                                               + " vertices in file ")
                                              + getFileNameNoPath());
            EventManager::get()->sendEvent(progressEvent.getPointer());
            
            /*
             * Read rows for each node
             */
            for (int32_t i = 0; i < numberOfNodeIndices; i++) {
                const int32_t nodeIndex = nodeIndices[i];
                
                if (isDenseMatrix) {
                    if ((i % progressUpdateInterval) == 0) {
                        progressEvent.setProgress(i,
                                                  "");
                        EventManager::get()->sendEvent(progressEvent.getPointer());
                        if (progressEvent.isCancelled()) {
                            userCancelled = true;
                            break;
                        }
                    }
                }
                
                const int64_t rowIndex = getRowIndexForNodeWhenLoading(structure,
                                                                       surfaceNumberOfNodes,
                                                                       nodeIndex);
                
                if (rowIndex >= 0) {
                    CaretAssert((rowIndex >= 0) && (rowIndex < m_ciftiInterface->getNumberOfRows()));
                    m_ciftiInterface->getRow(dataRow, rowIndex);
                    
                    for (int64_t j = 0; j < dataCount; j++) {
                        dataAverage[j] += dataRow[j];
                    }
                    rowSuccessCount++;
                    
                    CaretLogFine("Read row for node " + AString::fromNumbers(nodeIndices, ","));
                }
                else {
                    CaretLogFine("Failed reading row for node " + AString::number(nodeIndex));
                }
            }
            
            if (userCancelled) {
                m_loadedRowData.clear();
                m_loadedRowData.resize(dataCount, 0.0);
            }
            else if (rowSuccessCount > 0) {
                /*
                 * Average the data
                 */
                for (int64_t i = 0; i < dataCount; i++) {
                    dataAverage[i] /= rowSuccessCount;
                }
                
                m_rowLoadedTextForMapName = ("Structure: "
                                         + StructureEnum::toName(structure)
                                         + ", Averaged Node Count: "
                                         + AString::number(numberOfNodeIndices));
                m_rowLoadedText =  ("Structure_"
                                    + StructureEnum::toGuiName(structure)
                                    + "_Averaged_Node_Count_"
                                    + AString::number(numberOfNodeIndices));
                /*
                 * Update the viewed data
                 */
                m_loadedRowData = dataAverageVector;
                
                dataWasLoaded = true;
            }
        }
        
        if (dataWasLoaded == false) {
            CaretLogFine("FAILED to read rows for node average" + AString::fromNumbers(nodeIndices, ","));
        }

        CaretAssertVectorIndex(m_mapContent, 0);
        m_mapContent[0]->invalidateColoring();
    }
    catch (CiftiFileException& e) {
        throw DataFileException(e.whatString());
    }
}

/**
 * Load data for a voxel at the given coordinate.
 *
 * @param mapIndex
 *    Index of map.
 * @param xyz
 *    Coordinate of voxel.
 * @return 
 *    Index of row that was loaded or -1 if no data was loaded.
 * @throw
 *    DataFileException if there is an error.
 */
int64_t
CiftiMappableConnectivityMatrixDataFile::loadMapDataForVoxelAtCoordinate(const int32_t mapIndex,
                                                                 const float xyz[3]) throw (DataFileException)
{
    CaretAssert(mapIndex == 0);
    
    if (isCiftiInterfaceValid() == false) {
        setLoadedRowDataToAllZeros();
        return -1;
    }
    
    /*
     * Loading of data disabled?
     */
    if (m_dataLoadingEnabled == false) {
        return -1;
    }
    
    setLoadedRowDataToAllZeros();
    
    /*
     * Get content for map.
     */
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    /*
     * Loading of data disabled?
     */
    if (m_dataLoadingEnabled == false) {
        return -1;
    }
    
    
    int64_t rowIndex = -1;
    
    try {
        bool dataWasLoaded = false;
        
        rowIndex = getRowIndexForVoxelAtCoordinateWhenLoading(xyz);
        
        if (rowIndex >= 0) {
            const int64_t dataCount = m_ciftiInterface->getNumberOfColumns();
            if (dataCount > 0) {
                m_loadedRowData.resize(dataCount);
                CaretAssert((rowIndex >= 0) && (rowIndex < m_ciftiInterface->getNumberOfRows()));
                m_ciftiInterface->getRow(&m_loadedRowData[0], rowIndex);
                
                m_rowLoadedTextForMapName = ("Row: "
                                         + AString::number(rowIndex)
                                         + ", Voxel XYZ: ("
                                         + AString::fromNumbers(xyz, 3, ",")
                                         + ")");
                
                m_rowLoadedText = ("Row_"
                                   + AString::number(rowIndex)
                                   + "_Voxel_XYZ_"
                                   + AString::fromNumbers(xyz, 3, "_").replace('-', 'm'));
                
                CaretLogFine("Read row for voxel " + AString::fromNumbers(xyz, 3, ","));
                
                dataWasLoaded = true;
            }
        }
        
        if (dataWasLoaded == false) {
            CaretLogFine("FAILED to read row for voxel " + AString::fromNumbers(xyz, 3, ","));
        }

        CaretAssertVectorIndex(m_mapContent, 0);
        m_mapContent[0]->invalidateColoring();
    }
    catch (CiftiFileException& e) {
        throw DataFileException(e.whatString());
    }
    
    return (m_selectionIndex = rowIndex);
}

/**
 * Load connectivity data for the voxel indices and then average the data.
 *
 * @param mapIndex
 *    Index of map.
 * @param volumeDimensionIJK
 *    Dimensions of the volume.
 * @param voxelIndices
 *    Indices of voxels.
 * @throw
 *    DataFileException if there is an error.
 */
bool
CiftiMappableConnectivityMatrixDataFile::loadMapAverageDataForVoxelIndices(const int32_t mapIndex,
                                                                           const int64_t volumeDimensionIJK[3],
                                                                           const std::vector<VoxelIJK>& voxelIndices) throw (DataFileException)
{
    CaretAssert(mapIndex == 0);
    
    if (isCiftiInterfaceValid() == false) {
        setLoadedRowDataToAllZeros();
        return false;
    }
    
    /*
     * Loading of data disabled?
     */
    if (m_dataLoadingEnabled == false) {
        return false;
    }
    
    setLoadedRowDataToAllZeros();
    
    /*
     * Match dimensions
     */
    std::vector<int64_t> volumeDimensions;
    getDimensions(volumeDimensions);
    if (volumeDimensions.size() < 3) {
        return false;
    }
    if ((volumeDimensions[0] != volumeDimensionIJK[0])
        || (volumeDimensions[1] != volumeDimensionIJK[1])
        || (volumeDimensions[2] != volumeDimensionIJK[2])) {
        return false;
    }
    
    /*
     * Get content for map.
     */
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    
    const int64_t dataCount = m_ciftiInterface->getNumberOfColumns();
    if (dataCount <= 0) {
        return false;
    }
    
    const int64_t numberOfVoxelIndices = static_cast<int64_t>(voxelIndices.size());
    
    bool userCancelled = false;
    const int32_t progressUpdateInterval = 1;
    EventProgressUpdate progressEvent(0,
                                      numberOfVoxelIndices,
                                      0,
                                      ("Loading data for "
                                       + QString::number(numberOfVoxelIndices)
                                       + " voxels in file ")
                                      + getFileNameNoPath());
    EventManager::get()->sendEvent(progressEvent.getPointer());
    
    std::vector<float> rowData(dataCount);
    std::vector<double> rowSum(dataCount, 0.0);
    
    /*
     * Load and sum the data for all rows
     */
    int64_t numberOfRowsLoaded = 0;
    for (int64_t i = 0; i < numberOfVoxelIndices; i++) {
        if ((i % progressUpdateInterval) == 0) {
            progressEvent.setProgress(i,
                                      "");
            EventManager::get()->sendEvent(progressEvent.getPointer());
            if (progressEvent.isCancelled()) {
                userCancelled = true;
                break;
            }
        }
        
       const VoxelIJK& voxelIJK = voxelIndices[i];
        
        const int64_t rowIndex = getRowIndexForVoxelIndexWhenLoading(voxelIJK.m_ijk);
        if (rowIndex >= 0) {
            CaretAssert((rowIndex >= 0) && (rowIndex < m_ciftiInterface->getNumberOfRows()));
            m_ciftiInterface->getRow(&rowData[0],
                                     rowIndex);
            
            for (int64_t j = 0; j < dataCount; j++) {
                rowSum[j] += rowData[j];
            }
            
            numberOfRowsLoaded++;
        }
    }
    
    if (userCancelled) {
        m_loadedRowData.clear();
        m_loadedRowData.resize(dataCount, 0.0);
    }
    else if (numberOfRowsLoaded > 0) {
        progressEvent.setProgress(numberOfVoxelIndices - 1,
                                  "Averaging voxel data");
        EventManager::get()->sendEvent(progressEvent.getPointer());
        
        m_loadedRowData.resize(dataCount, 0.0);
        for (int64_t j = 0; j < dataCount; j++) {
            m_loadedRowData[j] = rowSum[j] / numberOfRowsLoaded;
        }
        
        m_rowLoadedTextForMapName = ("Averaged Voxel Count: "
                                     + AString::number(numberOfVoxelIndices));
        
        return true;
    }
    
    return false;
}

/**
 * @return Text describing row loaded that uses
 * underscores as separators.
 */
AString
CiftiMappableConnectivityMatrixDataFile::getRowLoadedText() const
{
    return m_rowLoadedText;
}


/**
 * Get the name of the map at the given index.  For connectivity matrix
 * files this always returns a description of the last data row that 
 * was loaded.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Name of the map.
 */
AString
CiftiMappableConnectivityMatrixDataFile::getMapName(const int32_t /*mapIndex*/) const
{
    return m_rowLoadedTextForMapName;
}

AString
CiftiMappableConnectivityMatrixDataFile::getRowName(const int32_t rowIndex) const
{
    const CiftiXML xml = m_ciftiInterface->getCiftiXML();
    std::vector<CiftiParcelElement> plist;
    xml.getParcelsForColumns(plist);
    return plist[rowIndex].m_parcelName;
}

AString
CiftiMappableConnectivityMatrixDataFile::getColumnName(const int32_t columnIndex) const
{
    const CiftiXML xml = m_ciftiInterface->getCiftiXML();
    std::vector<CiftiParcelElement> plist;
    xml.getParcelsForRows(plist);
    return plist[columnIndex].m_parcelName;
}

/**
 * Save file data from the scene.  For subclasses that need to
 * save to a scene, this method should be overriden.  sceneClass
 * will be valid and any scene data should be added to it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.
 */
void
CiftiMappableConnectivityMatrixDataFile::saveFileDataToScene(const SceneAttributes* /*sceneAttributes*/,
                                                             SceneClass* sceneClass)
{
    sceneClass->addBoolean("m_dataLoadingEnabled",
                           m_dataLoadingEnabled);
    sceneClass->addInteger("m_currentRowLoadedIndex",
                           m_selectionIndex);
}

/**
 * Restore file data from the scene.  For subclasses that need to
 * restore from a scene, this method should be overridden. The scene class
 * will be valid and any scene data may be obtained from it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
CiftiMappableConnectivityMatrixDataFile::restoreFileDataFromScene(const SceneAttributes* /*sceneAttributes*/,
                                                                  const SceneClass* sceneClass)
{
    m_dataLoadingEnabled = sceneClass->getBooleanValue("m_dataLoadingEnabled",
                                                       true);
    m_selectionIndex = sceneClass->getIntegerValue("m_currentRowLoadedIndex",-1);
}


