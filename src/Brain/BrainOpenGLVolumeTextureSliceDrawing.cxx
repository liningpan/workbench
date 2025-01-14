
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include <cmath>
#include <limits>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define __BRAIN_OPEN_GL_VOLUME_TEXTURE_SLICE_DRAWING_DECLARE__
#include "BrainOpenGLVolumeTextureSliceDrawing.h"
#undef __BRAIN_OPEN_GL_VOLUME_TEXTURE_SLICE_DRAWING_DECLARE__

#include "AnnotationCoordinate.h"
#include "AnnotationPercentSizeText.h"
#include "BoundingBox.h"
#include "Brain.h"
#include "BrainOpenGLAnnotationDrawingFixedPipeline.h"
#include "BrainOpenGLPrimitiveDrawing.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLVolumeSliceDrawing.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOpenGLInclude.h"
#include "CaretPreferenceDataValue.h"
#include "CaretPreferences.h"
#include "CaretVolumeExtension.h"
#include "CiftiMappableDataFile.h"
#include "DeveloperFlagsEnum.h"
#include "DisplayPropertiesFoci.h"
#include "DisplayPropertiesLabels.h"
#include "DisplayPropertiesVolume.h"
#include "ElapsedTimer.h"
#include "EventOpenGLObjectToWindowTransform.h"
#include "FociFile.h"
#include "Focus.h"
#include "GapsAndMargins.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GroupAndNameHierarchyModel.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GraphicsPrimitiveV3fC4f.h"
#include "GraphicsPrimitiveV3fC4ub.h"
#include "GraphicsPrimitiveV3fT3f.h"
#include "GraphicsUtilitiesOpenGL.h"
#include "IdentificationWithColor.h"
#include "LabelDrawingProperties.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "NodeAndVoxelColoring.h"
#include "SelectionItemFocusVolume.h"
#include "SelectionItemVoxel.h"
#include "SelectionItemVoxelEditing.h"
#include "SelectionManager.h"
#include "SessionManager.h"
#include "SpacerTabIndex.h"
#include "Surface.h"
#include "SurfacePlaneIntersectionToContour.h"
#include "VolumeFile.h"
#include "VolumeSurfaceOutlineColorOrTabModel.h"
#include "VolumeSurfaceOutlineModel.h"
#include "VolumeSurfaceOutlineSetModel.h"

using namespace caret;

static const bool debugFlag = false;
    
/**
 * \class caret::BrainOpenGLVolumeTextureSliceDrawing
 * \brief Draws volume slices using OpenGL
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLVolumeTextureSliceDrawing::BrainOpenGLVolumeTextureSliceDrawing()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLVolumeTextureSliceDrawing::~BrainOpenGLVolumeTextureSliceDrawing()
{
}

/**
 * Draw Volume Slices or slices for ALL Stuctures View.
 *
 * @param fixedPipelineDrawing
 *    The OpenGL drawing.
 * @param browserTabContent
 *    Content of browser tab that is to be drawn.
 * @param volumeDrawInfo
 *    Info on each volume layers for drawing.
 * @param sliceDrawingType
 *    Type of slice drawing (montage, single)
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param obliqueSliceMaskingType
 *    Masking for oblique slice drawing
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                           BrowserTabContent* browserTabContent,
                                           std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                           const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                           const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                           const VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum obliqueSliceMaskingType,
                                           const int32_t viewport[4])
{
//    CaretAssert(sliceProjectionType == VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE);
    
    if (volumeDrawInfo.empty()) {
        return;
    }
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    drawPrivate(fixedPipelineDrawing,
                browserTabContent,
                volumeDrawInfo,
                sliceDrawingType,
                sliceProjectionType,
                obliqueSliceMaskingType,
                viewport);
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/**
 * Draw Volume Slices or slices for ALL Stuctures View.
 *
 * @param fixedPipelineDrawing
 *    The OpenGL drawing.
 * @param browserTabContent
 *    Content of browser tab that is to be drawn.
 * @param volumeDrawInfo
 *    Info on each volume layers for drawing.
 * @param sliceDrawingType
 *    Type of slice drawing (montage, single)
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param obliqueSliceMaskingType
 *    Masking for oblique slice drawing
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::drawPrivate(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                  BrowserTabContent* browserTabContent,
                                                  std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                                  const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                  const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                  const VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum obliqueSliceMaskingType,
                                                  const int32_t viewport[4])
{
    //    CaretAssert(sliceProjectionType == VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE);
    
    if (volumeDrawInfo.empty()) {
        return;
    }
    CaretAssert(fixedPipelineDrawing);
    CaretAssert(browserTabContent);
    m_browserTabContent = browserTabContent;    
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    m_obliqueSliceMaskingType = obliqueSliceMaskingType;
    /*
     * No lighting for drawing slices
     */
    m_fixedPipelineDrawing->disableLighting();
    
    /*
     * Initialize class members which help reduce the number of
     * parameters that are passed to methods.
     */
    m_brain           = NULL;
    m_modelVolume     = NULL;
    m_modelWholeBrain = NULL;
    m_modelType       = ModelTypeEnum::MODEL_TYPE_INVALID;
    if (m_browserTabContent->getDisplayedVolumeModel() != NULL) {
        m_modelVolume = m_browserTabContent->getDisplayedVolumeModel();
        m_brain = m_modelVolume->getBrain();
        m_modelType = m_modelVolume->getModelType();
    }
    else if (m_browserTabContent->getDisplayedWholeBrainModel() != NULL) {
        m_modelWholeBrain = m_browserTabContent->getDisplayedWholeBrainModel();
        m_brain = m_modelWholeBrain->getBrain();
        m_modelType = m_modelWholeBrain->getModelType();
    }
    else {
        CaretAssertMessage(0, "Invalid model for volume slice drawing.");
    }
    CaretAssert(m_brain);
    CaretAssert(m_modelType != ModelTypeEnum::MODEL_TYPE_INVALID);
    
    m_volumeDrawInfo = volumeDrawInfo;
    if (m_volumeDrawInfo.empty()) {
        return;
    }
    m_underlayVolume = m_volumeDrawInfo[0].volumeFile;
    
    const DisplayPropertiesLabels* dsl = m_brain->getDisplayPropertiesLabels();
    m_displayGroup = dsl->getDisplayGroupForTab(m_fixedPipelineDrawing->windowTabIndex);
    
    m_tabIndex = m_browserTabContent->getTabNumber();
    
    /*
     * Cifti files are slow at getting individual voxels since they
     * provide no access to individual voxels.  The reason is that
     * the data may be on a server (Dense data) and accessing a single
     * voxel would require requesting the entire map.  So, for
     * each Cifti file, get the enter map.  This also, eliminate multiple
     * requests for the same map when drawing an ALL view.
     */
//    const int32_t numVolumes = static_cast<int32_t>(m_volumeDrawInfo.size());
//    for (int32_t i = 0; i < numVolumes; i++) {
//        std::vector<float> ciftiMapData;
//        m_ciftiMappableFileData.push_back(ciftiMapData);
//
//        const CiftiMappableDataFile* ciftiMapFile = dynamic_cast<const CiftiMappableDataFile*>(m_volumeDrawInfo[i].volumeFile);
//        if (ciftiMapFile != NULL) {
//            ciftiMapFile->getMapData(m_volumeDrawInfo[i].mapIndex,
//                                     m_ciftiMappableFileData[i]);
//        }
//    }
    
    if (browserTabContent->getDisplayedVolumeModel() != NULL) {
        drawVolumeSliceViewPlane(sliceDrawingType,
                                 sliceProjectionType,
                                 browserTabContent->getSliceViewPlane(),
                                 browserTabContent->getSlicePlanesAllViewLayout(),
                                 viewport);
    }
    else if (browserTabContent->getDisplayedWholeBrainModel() != NULL) {
        drawVolumeSlicesForAllStructuresView(sliceProjectionType,
                                             viewport);
    }    
}

/**
 * Draw volume view slices for the given view plane.
 *
 * @param sliceDrawingType
 *    Type of slice drawing (montage, single)
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param allPlanesLayout
 *    The layout in ALL slices view.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::drawVolumeSliceViewPlane(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                        const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                        const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                        const VolumeSliceViewAllPlanesLayoutEnum::Enum allPlanesLayout,
                                                        const int32_t viewport[4])
{
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
        {
            int32_t paraVP[4]    = { 0, 0, 0, 0 };
            int32_t coronalVP[4] = { 0, 0, 0, 0 };
            int32_t axialVP[4]   = { 0, 0, 0, 0 };
            
            BrainOpenGLViewportContent::getSliceAllViewViewport(viewport,
                                                                   VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                                                   allPlanesLayout,
                                                                   paraVP);
            BrainOpenGLViewportContent::getSliceAllViewViewport(viewport,
                                                                   VolumeSliceViewPlaneEnum::CORONAL,
                                                                   allPlanesLayout,
                                                                   coronalVP);
            BrainOpenGLViewportContent::getSliceAllViewViewport(viewport,
                                                                   VolumeSliceViewPlaneEnum::AXIAL,
                                                                   allPlanesLayout,
                                                                   axialVP);
            
            /*
             * Draw parasagittal slice
             */
            glPushMatrix();
            drawVolumeSliceViewType(BrainOpenGLVolumeSliceDrawing::AllSliceViewMode::ALL_YES,
                                    sliceDrawingType,
                                    sliceProjectionType,
                                    VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                    paraVP);
            glPopMatrix();
            
            
            /*
             * Draw coronal slice
             */
            glPushMatrix();
            drawVolumeSliceViewType(BrainOpenGLVolumeSliceDrawing::AllSliceViewMode::ALL_YES,
                                    sliceDrawingType,
                                    sliceProjectionType,
                                    VolumeSliceViewPlaneEnum::CORONAL,
                                    coronalVP);
            glPopMatrix();
            
            
            /*
             * Draw axial slice
             */
            glPushMatrix();
            drawVolumeSliceViewType(BrainOpenGLVolumeSliceDrawing::AllSliceViewMode::ALL_YES,
                                    sliceDrawingType,
                                    sliceProjectionType,
                                    VolumeSliceViewPlaneEnum::AXIAL,
                                    axialVP);
            glPopMatrix();
            
            if (allPlanesLayout == VolumeSliceViewAllPlanesLayoutEnum::GRID_LAYOUT) {
                /*
                 * 4th quadrant is used for axis showing orientation
                 */
                int32_t allVP[4] = { 0, 0, 0, 0 };
                BrainOpenGLViewportContent::getSliceAllViewViewport(viewport,
                                                                       VolumeSliceViewPlaneEnum::ALL,
                                                                       allPlanesLayout,
                                                                       allVP);
                
                switch (sliceProjectionType) {
                    case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                        drawOrientationAxes(allVP);
                        break;
                    case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                        break;
                }
            }
        }
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
        case VolumeSliceViewPlaneEnum::CORONAL:
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            drawVolumeSliceViewType(BrainOpenGLVolumeSliceDrawing::AllSliceViewMode::ALL_NO,
                                    sliceDrawingType,
                                    sliceProjectionType,
                                    sliceViewPlane,
                                    viewport);
            break;
    }
}

/**
 * Draw slices for the all structures view.
 *
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param viewport
 *    The viewport.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::drawVolumeSlicesForAllStructuresView(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                                           const int32_t viewport[4])
{
    m_orthographicBounds[0] = m_fixedPipelineDrawing->orthographicLeft;
    m_orthographicBounds[1] = m_fixedPipelineDrawing->orthographicRight;
    m_orthographicBounds[2] = m_fixedPipelineDrawing->orthographicBottom;
    m_orthographicBounds[3] = m_fixedPipelineDrawing->orthographicTop;
    m_orthographicBounds[4] = m_fixedPipelineDrawing->orthographicNear;
    m_orthographicBounds[5] = m_fixedPipelineDrawing->orthographicFar;
    
    /*
     * Enlarge the region
     */
    {
        const float left   = m_fixedPipelineDrawing->orthographicLeft;
        const float right  = m_fixedPipelineDrawing->orthographicRight;
        const float bottom = m_fixedPipelineDrawing->orthographicBottom;
        const float top    = m_fixedPipelineDrawing->orthographicTop;
        
        const float scale = 2.0;
        
        const float centerX  = (left + right) / 2.0;
        const float dx       = (right - left) / 2.0;
        const float newLeft  = centerX - (dx * scale);
        const float newRight = centerX + (dx * scale);
        
        const float centerY    = (bottom + top) / 2.0;
        const float dy         = (top - bottom) / 2.0;
        const float newBottom  = centerY - (dy * scale);
        const float newTop     = centerY + (dy * scale);
        
        m_orthographicBounds[0] = newLeft;
        m_orthographicBounds[1] = newRight;
        m_orthographicBounds[2] = newBottom;
        m_orthographicBounds[3] = newTop;
    }
    
    const float sliceCoordinates[3] = {
        m_browserTabContent->getSliceCoordinateParasagittal(),
        m_browserTabContent->getSliceCoordinateCoronal(),
        m_browserTabContent->getSliceCoordinateAxial()
    };
    
    if (m_browserTabContent->isSliceAxialEnabled()) {
        glPushMatrix();
        drawVolumeSliceViewProjection(BrainOpenGLVolumeSliceDrawing::AllSliceViewMode::ALL_NO,
                                      VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE,
                                      sliceProjectionType,
                                      VolumeSliceViewPlaneEnum::AXIAL,
                                      sliceCoordinates,
                                      viewport);
        glPopMatrix();
    }
    
    if (m_browserTabContent->isSliceCoronalEnabled()) {
        glPushMatrix();
        drawVolumeSliceViewProjection(BrainOpenGLVolumeSliceDrawing::AllSliceViewMode::ALL_NO,
                                      VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE,
                                      sliceProjectionType,
                                      VolumeSliceViewPlaneEnum::CORONAL,
                                      sliceCoordinates,
                                      viewport);
        glPopMatrix();
    }
    
    if (m_browserTabContent->isSliceParasagittalEnabled()) {
        glPushMatrix();
        drawVolumeSliceViewProjection(BrainOpenGLVolumeSliceDrawing::AllSliceViewMode::ALL_NO,
                                      VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE,
                                      sliceProjectionType,
                                      VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                      sliceCoordinates,
                                      viewport);
        glPopMatrix();
    }
}

/**
 * Draw single or montage volume view slices.
 *
 * @param allSliceViewMode
 *    Indicates drawing of ALL slices volume view (axial, coronal, parasagittal in one view)
 * @param sliceDrawingType
 *    Type of slice drawing (montage, single)
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::drawVolumeSliceViewType(const BrainOpenGLVolumeSliceDrawing::AllSliceViewMode allSliceViewMode,
                                                              const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                              const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                              const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                              const int32_t viewport[4])
{
    switch (sliceDrawingType) {
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
            drawVolumeSliceViewTypeMontage(allSliceViewMode,
                                           sliceDrawingType,
                                           sliceProjectionType,
                                 sliceViewPlane,
                                 viewport);
            break;
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
        {
            const float sliceCoordinates[3] = {
                m_browserTabContent->getSliceCoordinateParasagittal(),
                m_browserTabContent->getSliceCoordinateCoronal(),
                m_browserTabContent->getSliceCoordinateAxial()
            };
            drawVolumeSliceViewProjection(allSliceViewMode,
                                          sliceDrawingType,
                                          sliceProjectionType,
                                sliceViewPlane,
                                sliceCoordinates,
                                viewport);
        }
            break;
    }
    
}

/**
 * Draw montage slices.
 *
 * @param allSliceViewMode
 *    Indicates drawing of ALL slices volume view (axial, coronal, parasagittal in one view)
 * @param sliceDrawingType
 *    Type of slice drawing (montage, single)
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::drawVolumeSliceViewTypeMontage(const BrainOpenGLVolumeSliceDrawing::AllSliceViewMode allSliceViewMode,
                                                                     const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                                     const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                                     const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                     const int32_t viewport[4])
{
    const int32_t numRows = m_browserTabContent->getMontageNumberOfRows();
    CaretAssert(numRows > 0);
    const int32_t numCols = m_browserTabContent->getMontageNumberOfColumns();
    CaretAssert(numCols > 0);
    
    const int32_t montageCoordPrecision = m_browserTabContent->getVolumeMontageCoordinatePrecision();
    
    const GapsAndMargins* gapsAndMargins = m_brain->getGapsAndMargins();
    
    const int32_t windowIndex = m_fixedPipelineDrawing->m_windowIndex;
    
    int32_t vpSizeY        = 0;
    int32_t verticalMargin = 0;
    BrainOpenGLFixedPipeline::createSubViewportSizeAndGaps(viewport[3],
                                                           gapsAndMargins->getVolumeMontageVerticalGapForWindow(windowIndex),
                                                           -1,
                                                           numRows,
                                                           vpSizeY,
                                                           verticalMargin);
    
    int32_t vpSizeX          = 0;
    int32_t horizontalMargin = 0;
    BrainOpenGLFixedPipeline::createSubViewportSizeAndGaps(viewport[2],
                                                           gapsAndMargins->getVolumeMontageHorizontalGapForWindow(windowIndex),
                                                           -1,
                                                           numCols,
                                                           vpSizeX,
                                                           horizontalMargin);
    
    /*
     * Voxel sizes for underlay volume
     */
    float originX, originY, originZ;
    float x1, y1, z1;
    m_underlayVolume->indexToSpace(0, 0, 0, originX, originY, originZ);
    m_underlayVolume->indexToSpace(1, 1, 1, x1, y1, z1);
    float sliceThickness = 0.0;
    float sliceOrigin    = 0.0;
    
    AString axisLetter = "";
    
    float sliceCoordinates[3] = {
        m_browserTabContent->getSliceCoordinateParasagittal(),
        m_browserTabContent->getSliceCoordinateCoronal(),
        m_browserTabContent->getSliceCoordinateAxial()
    };
    
    int32_t sliceIndex = -1;
    int32_t maximumSliceIndex = -1;
    int64_t dimI, dimJ, dimK, numMaps, numComponents;
    m_underlayVolume->getDimensions(dimI, dimJ, dimK, numMaps, numComponents);
    const int32_t sliceStep = m_browserTabContent->getMontageSliceSpacing();
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            sliceIndex = -1;
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            sliceIndex = m_browserTabContent->getSliceIndexAxial(m_underlayVolume);
            maximumSliceIndex = dimK;
            sliceThickness = z1 - originZ;
            sliceOrigin = originZ;
            axisLetter = "Z";
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            sliceIndex = m_browserTabContent->getSliceIndexCoronal(m_underlayVolume);
            maximumSliceIndex = dimJ;
            sliceThickness = y1 - originY;
            sliceOrigin = originY;
            axisLetter = "Y";
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            sliceIndex = m_browserTabContent->getSliceIndexParasagittal(m_underlayVolume);
            maximumSliceIndex = dimI;
            sliceThickness = x1 - originX;
            sliceOrigin = originX;
            axisLetter = "X";
            break;
    }
    
    /*
     * Foreground color for slice coordinate text
     */
    const CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    uint8_t foregroundRGBA[4];
    prefs->getBackgroundAndForegroundColors()->getColorForegroundVolumeView(foregroundRGBA);
    foregroundRGBA[3] = 255;
    uint8_t backgroundRGBA[4];
    prefs->getBackgroundAndForegroundColors()->getColorBackgroundVolumeView(backgroundRGBA);
    backgroundRGBA[3] = 255;
    const bool showCoordinates = m_browserTabContent->isVolumeMontageAxesCoordinatesDisplayed();
    
    /*
     * Determine a slice offset to selected slices is in
     * the center of the montage
     */
    const int32_t numSlicesViewed = (numCols * numRows);
    const int32_t sliceOffset = ((numSlicesViewed / 2)
                             * sliceStep);
    
    sliceIndex += sliceOffset;
    
    /*
     * Find first valid slice for montage
     */
    while (sliceIndex >= 0) {
        if (sliceIndex < maximumSliceIndex) {
            break;
        }
        sliceIndex -= sliceStep;
    }
    
    if (sliceIndex >= 0) {
        for (int32_t i = 0; i < numRows; i++) {
            for (int32_t j = 0; j < numCols; j++) {
                if ((sliceIndex >= 0)
                    && (sliceIndex < maximumSliceIndex)) {
                    const int32_t vpX = (j * (vpSizeX + horizontalMargin));
                    const int32_t vpY = ((numRows - i - 1) * (vpSizeY + verticalMargin));
                    int32_t vp[4] = {
                        viewport[0] + vpX,
                        viewport[1] + vpY,
                        vpSizeX,
                        vpSizeY
                    };
                    
                    if ((vp[2] <= 0)
                        || (vp[3] <= 0)) {
                        continue;
                    }
                    
                    const float sliceCoord = (sliceOrigin
                                              + sliceThickness * sliceIndex);
                    switch (sliceViewPlane) {
                        case VolumeSliceViewPlaneEnum::ALL:
                            break;
                        case VolumeSliceViewPlaneEnum::AXIAL:
                            sliceCoordinates[2] = sliceCoord;
                            break;
                        case VolumeSliceViewPlaneEnum::CORONAL:
                            sliceCoordinates[1] = sliceCoord;
                            break;
                        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                            sliceCoordinates[0] = sliceCoord;
                            break;
                    }
                    
                    drawVolumeSliceViewProjection(allSliceViewMode,
                                                  sliceDrawingType,
                                                  sliceProjectionType,
                                                  sliceViewPlane,
                                                  sliceCoordinates,
                                                  vp);
                    
                    if (showCoordinates) {
                        const AString coordText = (axisLetter
                                                   + "="
                                                   + AString::number(sliceCoord, 'f', montageCoordPrecision));
                        
                        AnnotationPercentSizeText annotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
                        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
                        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                        annotationText.setFontPercentViewportSize(10.0f);
                        annotationText.setLineColor(CaretColorEnum::NONE);
                        annotationText.setTextColor(CaretColorEnum::CUSTOM);
                        annotationText.setBackgroundColor(CaretColorEnum::CUSTOM);
                        annotationText.setCustomTextColor(foregroundRGBA);
                        annotationText.setCustomBackgroundColor(backgroundRGBA);
                        annotationText.setText(coordText);
                        m_fixedPipelineDrawing->drawTextAtViewportCoords((vpSizeX - 5),
                                                                         5.0,
                                                                         annotationText);
                    }
                }
                sliceIndex -= sliceStep;
            }
        }
    }
    
    /*
     * Draw the axes labels for the montage view
     */
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    if (m_browserTabContent->isVolumeAxesCrosshairLabelsDisplayed()) {
        drawAxesCrosshairsOblique(sliceViewPlane,
                                          sliceCoordinates,
                                          false,
                                          true);
    }
}

/**
 * Draw a slice for either projection mode (oblique, orthogonal)
 *
 * @param allSliceViewMode
 *    Indicates drawing of ALL slices volume view (axial, coronal, parasagittal in one view)
 * @param sliceDrawingType
 *    Type of slice drawing (montage, single)
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param sliceCoordinates
 *    Coordinates of the selected slice.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::drawVolumeSliceViewProjection(const BrainOpenGLVolumeSliceDrawing::AllSliceViewMode allSliceViewMode,
                                                                    const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                                    const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                                    const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                    const float sliceCoordinates[3],
                                                                    const int32_t viewport[4])
{
    bool twoDimSliceViewFlag = false;
    if (m_modelVolume != NULL) {
        twoDimSliceViewFlag = true;
    }
    else if (m_modelWholeBrain != NULL) {
        /* nothing */
    }
    else {
        CaretAssertMessage(0, "Invalid model type.");
    }
    
    if (twoDimSliceViewFlag) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glViewport(viewport[0],
                   viewport[1],
                   viewport[2],
                   viewport[3]);

        /*
         * Set the orthographic projection to fit the slice axis
         */
        setOrthographicProjection(allSliceViewMode,
                                  sliceViewPlane,
                                  viewport);
    }
    
    /*
     * Create the plane equation for the slice
     */
    Plane slicePlane;
    createSlicePlaneEquation(sliceProjectionType,
                             sliceViewPlane,
                             sliceCoordinates,
                             slicePlane);
    CaretAssert(slicePlane.isValidPlane());
    if (slicePlane.isValidPlane() == false) {
        return;
    }
    
    
    if (twoDimSliceViewFlag) {
        /*
         * Set the viewing transformation (camera position)
         */
        setVolumeSliceViewingAndModelingTransformations(sliceProjectionType,
                                                        sliceViewPlane,
                                                        slicePlane,
                                                        sliceCoordinates);
    }
    
    SelectionItemVoxel* voxelID = m_brain->getSelectionManager()->getVoxelIdentification();
    SelectionItemVoxelEditing* voxelEditingID = m_brain->getSelectionManager()->getVoxelEditingIdentification();
    
    m_fixedPipelineDrawing->applyClippingPlanes(BrainOpenGLFixedPipeline::CLIPPING_DATA_TYPE_VOLUME,
                                                StructureEnum::ALL);
    
    /*
     * Check for a 'selection' type mode
     */
    bool drawVolumeSlicesFlag = true;
    m_identificationModeFlag = false;
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (voxelID->isEnabledForSelection()
                || voxelEditingID->isEnabledForSelection()) {
                m_identificationModeFlag = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else {
                /*
                 * Don't return.  Allow other items (such as annotations) to be drawn.
                 */
                drawVolumeSlicesFlag = false;
            }
            break;
        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
            return;
            break;
    }
    
    GLboolean cullFaceOn = glIsEnabled(GL_CULL_FACE);
    
    if (drawVolumeSlicesFlag) {
        /*
         * Disable culling so that both sides of the triangles/quads are drawn.
         */
        glDisable(GL_CULL_FACE);
        
        Matrix4x4 obliqueTransformationMatrix;
        switch (sliceProjectionType) {
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            {
                /*
                 * Create the oblique slice transformation matrix
                 */
                createObliqueTransformationMatrix(sliceProjectionType,
                                                  sliceCoordinates,
                                                  obliqueTransformationMatrix);
                
            }
                break;
        }
        
        drawObliqueSliceWithPrimitive(sliceViewPlane,
                                      sliceProjectionType,
                                      obliqueTransformationMatrix);
    }
    
    if ( ! m_identificationModeFlag) {
        if (slicePlane.isValidPlane()) {
            drawLayers(sliceDrawingType,
                       sliceProjectionType,
                       sliceViewPlane,
                       slicePlane,
                       sliceCoordinates);
        }
    }
    
    /*
     * Draw model space annotaitons on the volume slice
     */
    float sliceThickness = 1.0;
    if ( ! m_volumeDrawInfo.empty()) {
        if (m_volumeDrawInfo[0].volumeFile != NULL) {
            float spaceX = 0.0, spaceY = 0.0, spaceZ = 0.0;
            m_volumeDrawInfo[0].volumeFile->getVoxelSpacing(spaceX, spaceY, spaceZ);
            
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    sliceThickness = spaceZ;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    sliceThickness = spaceY;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    sliceThickness = spaceX;
                    break;
            }
        }
    }
    const bool annotationModeFlag = (m_fixedPipelineDrawing->m_windowUserInputMode == UserInputModeEnum::Enum::ANNOTATIONS);
    const bool tileTabsEditModeFlag = (m_fixedPipelineDrawing->m_windowUserInputMode == UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING);
    std::set<AString> emptyMediaFileNames;
    BrainOpenGLAnnotationDrawingFixedPipeline::Inputs inputs(this->m_brain,
                                                             m_fixedPipelineDrawing->mode,
                                                             BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance,
                                                             m_fixedPipelineDrawing->m_windowIndex,
                                                             m_fixedPipelineDrawing->windowTabIndex,
                                                             SpacerTabIndex(),
                                                             BrainOpenGLAnnotationDrawingFixedPipeline::Inputs::WINDOW_DRAWING_NO,
                                                             emptyMediaFileNames,
                                                             annotationModeFlag,
                                                             tileTabsEditModeFlag);
    m_fixedPipelineDrawing->m_annotationDrawing->drawModelSpaceAnnotationsOnVolumeSlice(&inputs,
                                                                                        slicePlane,
                                                                                        sliceThickness);
    
    m_fixedPipelineDrawing->disableClippingPlanes();
    
    if (cullFaceOn) {
        glEnable(GL_CULL_FACE);
    }
}

/**
 * Create the equation for the slice plane
 *
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceViewPlane
 *    View plane that is displayed.
 * @param sliceCoordinates
 *    Slice coordinates
 * @param planeOut
 *    OUTPUT plane of slice after transforms.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::createSlicePlaneEquation(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                               const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                               const float sliceCoordinates[3],
                                                               Plane& planeOut)
{
    /*
     * Default the slice normal vector to an orthogonal view
     */
    float sliceNormalVector[3] = { 0.0, 0.0, 0.0 };
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
        case VolumeSliceViewPlaneEnum::AXIAL:
            sliceNormalVector[2] = 1.0;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            sliceNormalVector[1] = -1.0;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            sliceNormalVector[0] = -1.0;
            break;
    }
    
    switch (sliceProjectionType) {
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
        {
            /*
             * Transform the slice normal vector by the oblique rotation
             * matrix so that the normal vector points out of the slice
             */
            const Matrix4x4 obliqueRotationMatrix = m_browserTabContent->getObliqueVolumeRotationMatrix();
            obliqueRotationMatrix.multiplyPoint3(sliceNormalVector);
            MathFunctions::normalizeVector(sliceNormalVector);
        }
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            break;
    }
    
    Plane plane(sliceNormalVector,
                sliceCoordinates);
    planeOut = plane;
    
    m_lookAtCenter[0] = sliceCoordinates[0];
    m_lookAtCenter[1] = sliceCoordinates[1];
    m_lookAtCenter[2] = sliceCoordinates[2];
}

/**
 * Convert a Matrix4x4 to a glm::mat4 matrix
 */
glm::mat4
BrainOpenGLVolumeTextureSliceDrawing::convertMatrix4x4toGlmMat4(const Matrix4x4& matrix) const
{
    float m[16];
    matrix.getMatrixForOpenGL(m);
    
    glm::mat4 out(m[0], m[1], m[2], m[3],
                    m[4], m[5], m[6], m[7],
                    m[8], m[9], m[10], m[11],
                    m[12], m[13], m[14], m[15]);
    return out;
}

/**
 * Set the volume slice viewing transformation.  This sets the position and
 * orientation of the camera.
 *
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceViewPlane
 *    View plane that is displayed.
 * @param plane
 *    Plane equation of selected slice.
 * @param sliceCoordinates
 *    Coordinates of the selected slices.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::setVolumeSliceViewingAndModelingTransformations(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                                                      const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                                      const Plane& plane,
                                                                                      const float sliceCoordinates[3])
{
    
    /*
     * Move the camera with the user's translation
     */
    const float* userTranslation = m_browserTabContent->getTranslation();
    glm::vec3 translation(0.0);
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
        case VolumeSliceViewPlaneEnum::AXIAL:
            translation[0] = sliceCoordinates[0] + userTranslation[0];
            translation[1] = sliceCoordinates[1] + userTranslation[1];
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            translation[0] = sliceCoordinates[0] + userTranslation[0];
            translation[1] = sliceCoordinates[2] + userTranslation[2];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            translation[0] = -(sliceCoordinates[1] + userTranslation[1]);
            translation[1] =   sliceCoordinates[2] + userTranslation[2];
            break;
    }
    
    /*
     * Since an orthographic projection is used, the eye only needs
     * to be a little bit from the center along the plane's normal vector.
     */
    double planeNormal[3];
    plane.getNormalVector(planeNormal);
    glm::vec3 eye(m_lookAtCenter[0] + planeNormal[0] * BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance,
                  m_lookAtCenter[1] + planeNormal[1] * BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance,
                  m_lookAtCenter[2] + planeNormal[2] * BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance);
    
    /*
     * Set the up vector which indices which way is up (screen Y)
     */
    glm::vec4 up(0.0, 0.0, 0.0, 1.0);
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
        case VolumeSliceViewPlaneEnum::AXIAL:
            up[1] = 1.0;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            up[2] = 1.0;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            up[2] = 1.0;
            break;
    }
    
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
        {
            /*
             * For oblique viewing, the up vector needs to be rotated by the
             * oblique rotation matrix.
             */
            const Matrix4x4 obMat = m_browserTabContent->getObliqueVolumeRotationMatrix();
            const glm::mat4 matrix = convertMatrix4x4toGlmMat4(obMat);
            up = matrix * up;
            
            if (debugFlag) {
                float upTemp[3] = { up[0], up[1], up[2] };
                m_browserTabContent->getObliqueVolumeRotationMatrix().multiplyPoint3(upTemp);
                float upCopy[3] = { up[0], up[1], up[2] };
                const float dist = MathFunctions::distance3D(upTemp, upCopy);
                if (dist >= 0.01) {
                    std::cout << "Up vectors different by distance: " << dist << std::endl;
                }
            }
        }
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            break;
    }
    
    /*
     * Now set the camera to look at the selected coordinate (center)
     * with the camera offset a little bit from the center.
     * This allows the slice's voxels to be drawn in the actual coordinates.
     */
//    gluLookAt(eye[0], eye[1], eye[2],
//              m_lookAtCenter[0], m_lookAtCenter[1], m_lookAtCenter[2],
//              up[0], up[1], up[2]);
    glm::vec3 lookAt(m_lookAtCenter[0], m_lookAtCenter[1], m_lookAtCenter[2]);
    glm::mat4 lookAtMatrix = glm::lookAt(eye, lookAt, glm::vec3(up));

    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0), translation);
    
    glm::mat4 projMatrix = translationMatrix * lookAtMatrix;

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(projMatrix));
}

/**
 * Draw the layers type data.
 *
 * @param sliceDrawingType
 *    Type of slice drawing (montage, single)
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceViewPlane
 *    View plane that is displayed.
 * @param slicePlane
 *    Plane of the slice.
 * @param sliceCoordinates
 *    Coordinates of the selected slices.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::drawLayers(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                 const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                 const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                 const Plane& slicePlane,
                                                 const float sliceCoordinates[3])
{
    bool drawCrosshairsFlag = true;
    bool drawFibersFlag     = true;
    bool drawFociFlag       = true;
    bool drawOutlineFlag    = true;
    
    if (m_modelWholeBrain != NULL) {
        drawCrosshairsFlag = false;
        drawFibersFlag = false;
        drawFociFlag = false;
    }
    
    if ( ! m_identificationModeFlag) {
        if (slicePlane.isValidPlane()) {
            /*
             * Disable culling so that both sides of the triangles/quads are drawn.
             */
            GLboolean cullFaceOn = glIsEnabled(GL_CULL_FACE);
            glDisable(GL_CULL_FACE);
            
            glPushMatrix();
            
            GLboolean depthBufferEnabled = false;
            glGetBooleanv(GL_DEPTH_TEST,
                          &depthBufferEnabled);
            
            /*
             * Use some polygon offset that will adjust the depth values of the
             * layers so that the layers depth values place the layers in front of
             * the volume slice.
             */
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(0.0, 1.0);
            
            if (drawOutlineFlag) {
                BrainOpenGLVolumeSliceDrawing::drawSurfaceOutline(m_underlayVolume,
                                                                  m_modelType,
                                                                  sliceProjectionType,
                                                                  sliceViewPlane,
                                                                  sliceCoordinates,
                                                                  slicePlane,
                                                                  m_browserTabContent->getVolumeSurfaceOutlineSet(),
                                                                  m_fixedPipelineDrawing,
                                                                  true);
            }
            
            if (drawFibersFlag) {
                glDisable(GL_DEPTH_TEST);
                m_fixedPipelineDrawing->drawFiberOrientations(&slicePlane,
                                                              StructureEnum::ALL);
                m_fixedPipelineDrawing->drawFiberTrajectories(&slicePlane,
                                                              StructureEnum::ALL);
                if (depthBufferEnabled) {
                    glEnable(GL_DEPTH_TEST);
                }
                else {
                    glDisable(GL_DEPTH_TEST);
                }
            }
            if (drawFociFlag) {
                glDisable(GL_DEPTH_TEST);
                drawVolumeSliceFoci(slicePlane);
                if (depthBufferEnabled) {
                    glEnable(GL_DEPTH_TEST);
                }
                else {
                    glDisable(GL_DEPTH_TEST);
                }
            }
            
            glDisable(GL_POLYGON_OFFSET_FILL);
            
            if (drawCrosshairsFlag) {
                glPushMatrix();
                drawAxesCrosshairs(sliceProjectionType,
                                   sliceDrawingType,
                                   sliceViewPlane,
                                   sliceCoordinates);
                glPopMatrix();
                if (depthBufferEnabled) {
                    glEnable(GL_DEPTH_TEST);
                }
                else {
                    glDisable(GL_DEPTH_TEST);
                }
            }
            
            glPopMatrix();
            
            if (cullFaceOn) {
                glEnable(GL_CULL_FACE);
            }
        }
    }
}

/**
 * Draw foci on volume slice.
 *
 * @param plane
 *   Plane of the volume slice on which surface outlines are drawn.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::drawVolumeSliceFoci(const Plane& plane)
{
    SelectionItemFocusVolume* idFocus = m_brain->getSelectionManager()->getVolumeFocusIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (idFocus->isEnabledForSelection()) {
                isSelect = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else {
                return;
            }
            break;
        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
            return;
            break;
    }
    
    VolumeMappableInterface* underlayVolume = m_volumeDrawInfo[0].volumeFile;
    float minVoxelSpacing;
    float maxVoxelSpacing;
    if ( ! getMinMaxVoxelSpacing(underlayVolume, minVoxelSpacing, maxVoxelSpacing)) {
        return;
    }
    
    const float sliceThickness = maxVoxelSpacing;
    const float halfSliceThickness = sliceThickness * 0.5;
    
    
    const DisplayPropertiesFoci* fociDisplayProperties = m_brain->getDisplayPropertiesFoci();
    const DisplayGroupEnum::Enum displayGroup = fociDisplayProperties->getDisplayGroupForTab(m_fixedPipelineDrawing->windowTabIndex);
    
    if (fociDisplayProperties->isDisplayed(displayGroup,
                                           m_fixedPipelineDrawing->windowTabIndex) == false) {
        return;
    }
    const float focusDiameter = fociDisplayProperties->getFociSizeMillimeters(displayGroup,
                                                                   m_fixedPipelineDrawing->windowTabIndex);
    const FeatureColoringTypeEnum::Enum fociColoringType = fociDisplayProperties->getColoringType(displayGroup,
                                                                                                  m_fixedPipelineDrawing->windowTabIndex);
    
    const CaretColorEnum::Enum caretColor = fociDisplayProperties->getStandardColorType(displayGroup,
                                                                                        m_fixedPipelineDrawing->windowTabIndex);
    float caretColorRGBA[4];
    CaretColorEnum::toRGBAFloat(caretColor, caretColorRGBA);
    
    bool drawAsSpheres = false;
    switch (fociDisplayProperties->getDrawingType(displayGroup,
                                                  m_fixedPipelineDrawing->windowTabIndex)) {
        case FociDrawingTypeEnum::DRAW_AS_SPHERES:
            drawAsSpheres = true;
            break;
        case FociDrawingTypeEnum::DRAW_AS_SQUARES:
            break;
    }
    
    /*
     * Process each foci file
     */
    const int32_t numberOfFociFiles = m_brain->getNumberOfFociFiles();
    for (int32_t iFile = 0; iFile < numberOfFociFiles; iFile++) {
        FociFile* fociFile = m_brain->getFociFile(iFile);
        
        const GroupAndNameHierarchyModel* classAndNameSelection = fociFile->getGroupAndNameHierarchyModel();
        if (classAndNameSelection->isSelected(displayGroup,
                                              m_fixedPipelineDrawing->windowTabIndex) == false) {
            continue;
        }
        
        const GiftiLabelTable* classColorTable = fociFile->getClassColorTable();
        const GiftiLabelTable* nameColorTable = fociFile->getNameColorTable();
        
        const int32_t numFoci = fociFile->getNumberOfFoci();
        
        for (int32_t j = 0; j < numFoci; j++) {
            Focus* focus = fociFile->getFocus(j);
            
            const GroupAndNameHierarchyItem* groupNameItem = focus->getGroupNameSelectionItem();
            if (groupNameItem != NULL) {
                if (groupNameItem->isSelected(displayGroup,
                                              m_fixedPipelineDrawing->windowTabIndex) == false) {
                    continue;
                }
            }
            
            float rgba[4] = { 0.0, 0.0, 0.0, 1.0 };
            switch (fociColoringType) {
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_CLASS:
                    if (focus->isClassRgbaValid() == false) {
                        const GiftiLabel* colorLabel = classColorTable->getLabelBestMatching(focus->getClassName());
                        if (colorLabel != NULL) {
                            colorLabel->getColor(rgba);
                            focus->setClassRgba(rgba);
                        }
                        else {
                            focus->setClassRgba(rgba);
                        }
                    }
                    focus->getClassRgba(rgba);
                    break;
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_STANDARD_COLOR:
                    rgba[0] = caretColorRGBA[0];
                    rgba[1] = caretColorRGBA[1];
                    rgba[2] = caretColorRGBA[2];
                    rgba[3] = caretColorRGBA[3];
                    break;
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_NAME:
                    if (focus->isNameRgbaValid() == false) {
                        const GiftiLabel* colorLabel = nameColorTable->getLabelBestMatching(focus->getName());
                        if (colorLabel != NULL) {
                            colorLabel->getColor(rgba);
                            focus->setNameRgba(rgba);
                        }
                        else {
                            focus->setNameRgba(rgba);
                        }
                    }
                    focus->getNameRgba(rgba);
                    break;
            }
            
            const int32_t numProjections = focus->getNumberOfProjections();
            for (int32_t k = 0; k < numProjections; k++) {
                const SurfaceProjectedItem* spi = focus->getProjection(k);
                if (spi->isVolumeXYZValid()) {
                    float xyz[3];
                    spi->getVolumeXYZ(xyz);
                    
                    bool drawIt = false;
                    if (plane.absoluteDistanceToPlane(xyz) < halfSliceThickness) {
                        drawIt = true;
                    }
                    
                    if (drawIt) {
                        glPushMatrix();
                        glTranslatef(xyz[0], xyz[1], xyz[2]);
                        if (isSelect) {
                            uint8_t idRGBA[4];
                            m_fixedPipelineDrawing->colorIdentification->addItem(idRGBA,
                                                                                 SelectionItemDataTypeEnum::FOCUS_VOLUME,
                                                                                 iFile, // file index
                                                                                 j, // focus index
                                                                                 k);// projection index
                            idRGBA[3] = 255;
                            if (drawAsSpheres) {
                                m_fixedPipelineDrawing->drawSphereWithDiameter(idRGBA,
                                                                               focusDiameter);
                            }
                            else {
                                glColor4ubv(idRGBA);
                                drawSquare(focusDiameter);
                            }
                        }
                        else {
                            if (drawAsSpheres) {
                                m_fixedPipelineDrawing->drawSphereWithDiameter(rgba,
                                                                               focusDiameter);
                            }
                            else {
                                glColor3fv(rgba);
                                drawSquare(focusDiameter);
                            }
                        }
                        glPopMatrix();
                    }
                }
            }
        }
    }
    
    if (isSelect) {
        int32_t fociFileIndex = -1;
        int32_t focusIndex = -1;
        int32_t focusProjectionIndex = -1;
        float depth = -1.0;
        m_fixedPipelineDrawing->getIndexFromColorSelection(SelectionItemDataTypeEnum::FOCUS_VOLUME,
                                                           m_fixedPipelineDrawing->mouseX,
                                                           m_fixedPipelineDrawing->mouseY,
                                                           fociFileIndex,
                                                           focusIndex,
                                                           focusProjectionIndex,
                                                           depth);
        if (fociFileIndex >= 0) {
            if (idFocus->isOtherScreenDepthCloserToViewer(depth)) {
                Focus* focus = m_brain->getFociFile(fociFileIndex)->getFocus(focusIndex);
                idFocus->setBrain(m_brain);
                idFocus->setFocus(focus);
                idFocus->setFociFile(m_brain->getFociFile(fociFileIndex));
                idFocus->setFocusIndex(focusIndex);
                idFocus->setFocusProjectionIndex(focusProjectionIndex);
                idFocus->setVolumeFile(underlayVolume);
                idFocus->setScreenDepth(depth);
                float xyz[3];
                const SurfaceProjectedItem* spi = focus->getProjection(focusProjectionIndex);
                spi->getVolumeXYZ(xyz);
                m_fixedPipelineDrawing->setSelectedItemScreenXYZ(idFocus, xyz);
                CaretLogFine("Selected Volume Focus Identification Symbol: " + QString::number(focusIndex));
            }
        }
    }
}

/**
 * Draw the axes crosshairs.
 *
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceDrawingType
 *    Type of slice drawing (montage, single)
 * @param sliceViewPlane
 *    View plane that is displayed.
 * @param sliceCoordinates
 *    Coordinates of the selected slices.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::drawAxesCrosshairs(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                         const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                         const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                         const float sliceCoordinates[3])
{
    const bool drawCrosshairsFlag = m_browserTabContent->isVolumeAxesCrosshairsDisplayed();
    bool drawCrosshairLabelsFlag = m_browserTabContent->isVolumeAxesCrosshairLabelsDisplayed();
    
    switch (sliceDrawingType) {
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
            drawCrosshairLabelsFlag = false;
            break;
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
            break;
    }
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
//            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
        {
            glPushMatrix();
            glLoadIdentity();
            drawAxesCrosshairsOblique(sliceViewPlane,
                                      sliceCoordinates,
                                      drawCrosshairsFlag,
                                      drawCrosshairLabelsFlag);
            glPopMatrix();
        }
            break;
    }
}

/**
 * Draw the axes crosshairs for an orthogonal slice.
 *
 * @param sliceViewPlane
 *    The slice plane view.
 * @param sliceCoordinatesIn
 *    Coordinates of the selected slices.
 * @param drawCrosshairsFlag
 *    If true, draw the crosshairs.
 * @param drawCrosshairLabelsFlag
 *    If true, draw the crosshair labels.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::drawAxesCrosshairsOblique(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                const float sliceCoordinatesIn[3],
                                                                const bool drawCrosshairsFlag,
                                                                const bool drawCrosshairLabelsFlag)
{
    const float gapPercentViewportHeight = SessionManager::get()->getCaretPreferences()->getVolumeCrosshairGap();
    const float gapMM = GraphicsUtilitiesOpenGL::convertPercentageOfViewportHeightToMillimeters(gapPercentViewportHeight);
    
    const std::array<float, 3> sliceCoordinates = { sliceCoordinatesIn[0], sliceCoordinatesIn[1], sliceCoordinatesIn[2] };
    GLboolean depthEnabled = GL_FALSE;
    glGetBooleanv(GL_DEPTH_TEST,
                  &depthEnabled);
    glDisable(GL_DEPTH_TEST);
    
    const float bigValue = 10000.0 + gapMM;
    
    std::array<float, 3> horizontalAxisPosStartXYZ = sliceCoordinates;
    
    float trans[3];
    m_browserTabContent->getTranslation(trans);
    
    std::array<float, 3> horizTrans = { trans[0], trans[1], trans[2] };
    
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            horizontalAxisPosStartXYZ[0] = sliceCoordinates[0];
            horizontalAxisPosStartXYZ[1] = sliceCoordinates[2];
            horizontalAxisPosStartXYZ[2] = sliceCoordinates[1];
            
            horizTrans[0] = trans[0];
            horizTrans[1] = trans[2];
            horizTrans[2] = trans[1];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            horizontalAxisPosStartXYZ[0] = -sliceCoordinates[1];
            horizontalAxisPosStartXYZ[1] = sliceCoordinates[2];
            horizontalAxisPosStartXYZ[2] = sliceCoordinates[0];
            
            horizTrans[0] = -trans[1];
            horizTrans[1] = trans[2];
            horizTrans[2] = trans[0];
            break;
    }
    
    std::array<float, 3> horizontalAxisPosEndXYZ = horizontalAxisPosStartXYZ;
    std::array<float, 3> verticalAxisPosStartXYZ = horizontalAxisPosStartXYZ;
    std::array<float, 3> verticalAxisPosEndXYZ = horizontalAxisPosStartXYZ;
    
    std::array<float, 3> horizontalAxisNegStartXYZ = horizontalAxisPosStartXYZ;
    std::array<float, 3> horizontalAxisNegEndXYZ = horizontalAxisPosEndXYZ;
    std::array<float, 3> verticalAxisNegStartXYZ = verticalAxisPosStartXYZ;
    std::array<float, 3> verticalAxisNegEndXYZ = verticalAxisPosEndXYZ;
    
    std::array<float, 3> vertTrans = horizTrans;

    float axialRGBA[4];
    getAxesColor(VolumeSliceViewPlaneEnum::AXIAL,
                 axialRGBA);
    
    float coronalRGBA[4];
    getAxesColor(VolumeSliceViewPlaneEnum::CORONAL,
                 coronalRGBA);
    
    float paraRGBA[4];
    getAxesColor(VolumeSliceViewPlaneEnum::PARASAGITTAL,
                 paraRGBA);
    
    AString horizontalLeftText  = "";
    AString horizontalRightText = "";
    AString verticalBottomText  = "";
    AString verticalTopText     = "";
    
    float* horizontalAxisRGBA   = axialRGBA;
    float* verticalAxisRGBA     = axialRGBA;
    
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            horizontalLeftText   = "L";
            horizontalRightText  = "R";
            horizontalAxisRGBA = coronalRGBA;
            horizontalAxisPosStartXYZ[0] += gapMM;
            horizontalAxisPosEndXYZ[0]   += bigValue;
            horizontalAxisNegStartXYZ[0] -= gapMM;
            horizontalAxisNegEndXYZ[0]   -= bigValue;

            verticalBottomText = "P";
            verticalTopText    = "A";
            verticalAxisRGBA = paraRGBA;
            verticalAxisPosStartXYZ[1] += gapMM;
            verticalAxisPosEndXYZ[1]   += bigValue;
            verticalAxisNegStartXYZ[1] -= gapMM;
            verticalAxisNegEndXYZ[1]   -= bigValue;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            horizontalLeftText   = "L";
            horizontalRightText  = "R";
            horizontalAxisRGBA = axialRGBA;
            horizontalAxisPosStartXYZ[0] += gapMM;
            horizontalAxisPosEndXYZ[0]   += bigValue;
            horizontalAxisNegStartXYZ[0] -= gapMM;
            horizontalAxisNegEndXYZ[0]   -= bigValue;

            verticalBottomText = "I";
            verticalTopText    = "S";
            verticalAxisRGBA = paraRGBA;
            verticalAxisPosStartXYZ[1] += gapMM;
            verticalAxisPosEndXYZ[1]   += bigValue;
            verticalAxisNegStartXYZ[1] -= gapMM;
            verticalAxisNegEndXYZ[1]   -= bigValue;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            horizontalLeftText   = "A";
            horizontalRightText  = "P";
            horizontalAxisRGBA = axialRGBA;
            horizontalAxisPosStartXYZ[0] += gapMM;
            horizontalAxisPosEndXYZ[0]   += bigValue;
            horizontalAxisNegStartXYZ[0] -= gapMM;
            horizontalAxisNegEndXYZ[0]   -= bigValue;

            verticalBottomText = "I";
            verticalTopText    = "S";
            verticalAxisRGBA = coronalRGBA;
            verticalAxisPosStartXYZ[1] += gapMM;
            verticalAxisPosEndXYZ[1]   += bigValue;
            verticalAxisNegStartXYZ[1] -= gapMM;
            verticalAxisNegEndXYZ[1]   -= bigValue;
            break;
    }
    
    /*
     * Offset text labels be a percentage of viewort width/height
     */
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    const int textOffsetX = viewport[2] * 0.01f;
    const int textOffsetY = viewport[3] * 0.01f;
    const int textLeftWindowXY[2] = {
        textOffsetX,
        (viewport[3] / 2)
    };
    const int textRightWindowXY[2] = {
        viewport[2] - textOffsetX,
        (viewport[3] / 2)
    };
    const int textBottomWindowXY[2] = {
        viewport[2] / 2,
        textOffsetY
    };
    const int textTopWindowXY[2] = {
        (viewport[2] / 2),
        viewport[3] - textOffsetY
    };
    
    /*
     * Crosshairs
     */
    if (drawCrosshairsFlag) {
        glPushMatrix();
        glTranslatef(horizTrans[0], horizTrans[1], horizTrans[2]);
        std::unique_ptr<GraphicsPrimitiveV3fC4f> horizHairPrimitive(GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES));
        horizHairPrimitive->addVertex(&horizontalAxisPosStartXYZ[0], horizontalAxisRGBA);
        horizHairPrimitive->addVertex(&horizontalAxisPosEndXYZ[0], horizontalAxisRGBA);
        horizHairPrimitive->addVertex(&horizontalAxisNegStartXYZ[0], horizontalAxisRGBA);
        horizHairPrimitive->addVertex(&horizontalAxisNegEndXYZ[0], horizontalAxisRGBA);
        horizHairPrimitive->setLineWidth(GraphicsPrimitive::LineWidthType::PIXELS, 2.0f);
        GraphicsEngineDataOpenGL::draw(horizHairPrimitive.get());
        glPopMatrix();
        
        glPushMatrix();
        glTranslatef(vertTrans[0], vertTrans[1], vertTrans[2]);
        std::unique_ptr<GraphicsPrimitiveV3fC4f> vertHairPrimitive(GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES));
        vertHairPrimitive->addVertex(&verticalAxisPosStartXYZ[0], verticalAxisRGBA);
        vertHairPrimitive->addVertex(&verticalAxisPosEndXYZ[0], verticalAxisRGBA);
        vertHairPrimitive->addVertex(&verticalAxisNegStartXYZ[0], verticalAxisRGBA);
        vertHairPrimitive->addVertex(&verticalAxisNegEndXYZ[0], verticalAxisRGBA);
        vertHairPrimitive->setLineWidth(GraphicsPrimitive::LineWidthType::PIXELS, 2.0f);
        GraphicsEngineDataOpenGL::draw(vertHairPrimitive.get());
        glPopMatrix();
    }
    
    if (drawCrosshairLabelsFlag) {
        const AnnotationTextFontPointSizeEnum::Enum fontSize = AnnotationTextFontPointSizeEnum::SIZE18;
        
        const int textCenter[2] = {
            textLeftWindowXY[0],
            textLeftWindowXY[1]
        };
        const int halfFontSize = AnnotationTextFontPointSizeEnum::toSizeNumeric(fontSize) / 2;
        
        uint8_t backgroundRGBA[4] = {
            m_fixedPipelineDrawing->m_backgroundColorByte[0],
            m_fixedPipelineDrawing->m_backgroundColorByte[1],
            m_fixedPipelineDrawing->m_backgroundColorByte[2],
            m_fixedPipelineDrawing->m_backgroundColorByte[3]
        };
        
        GLint savedViewport[4];
        glGetIntegerv(GL_VIEWPORT, savedViewport);
        
        int vpLeftX   = savedViewport[0] + textCenter[0] - halfFontSize;
        int vpRightX  = savedViewport[0] + textCenter[0] + halfFontSize;
        int vpBottomY = savedViewport[1] + textCenter[1] - halfFontSize;
        int vpTopY    = savedViewport[1] + textCenter[1] + halfFontSize;
        MathFunctions::limitRange(vpLeftX,
                                  savedViewport[0],
                                  savedViewport[0] + savedViewport[2]);
        MathFunctions::limitRange(vpRightX,
                                  savedViewport[0],
                                  savedViewport[0] + savedViewport[2]);
        MathFunctions::limitRange(vpBottomY,
                                  savedViewport[1],
                                  savedViewport[1] + savedViewport[3]);
        MathFunctions::limitRange(vpTopY,
                                  savedViewport[1],
                                  savedViewport[1] + savedViewport[3]);
        
        const int vpSizeX = vpRightX - vpLeftX;
        const int vpSizeY = vpTopY - vpBottomY;
        glViewport(vpLeftX, vpBottomY, vpSizeX, vpSizeY);
        
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        
        std::vector<uint8_t> rgba;
        std::vector<float> coords, normals;
        
        coords.push_back(-1.0);
        coords.push_back(-1.0);
        coords.push_back( 0.0);
        normals.push_back(0.0);
        normals.push_back(0.0);
        normals.push_back(1.0);
        rgba.push_back(backgroundRGBA[0]);
        rgba.push_back(backgroundRGBA[1]);
        rgba.push_back(backgroundRGBA[2]);
        rgba.push_back(backgroundRGBA[3]);
        
        coords.push_back( 1.0);
        coords.push_back(-1.0);
        coords.push_back( 0.0);
        normals.push_back(0.0);
        normals.push_back(0.0);
        normals.push_back(1.0);
        rgba.push_back(backgroundRGBA[0]);
        rgba.push_back(backgroundRGBA[1]);
        rgba.push_back(backgroundRGBA[2]);
        rgba.push_back(backgroundRGBA[3]);
        
        coords.push_back( 1.0);
        coords.push_back( 1.0);
        coords.push_back( 0.0);
        normals.push_back(0.0);
        normals.push_back(0.0);
        normals.push_back(1.0);
        rgba.push_back(backgroundRGBA[0]);
        rgba.push_back(backgroundRGBA[1]);
        rgba.push_back(backgroundRGBA[2]);
        rgba.push_back(backgroundRGBA[3]);
        
        coords.push_back(-1.0);
        coords.push_back( 1.0);
        coords.push_back( 0.0);
        normals.push_back(0.0);
        normals.push_back(0.0);
        normals.push_back(1.0);
        rgba.push_back(backgroundRGBA[0]);
        rgba.push_back(backgroundRGBA[1]);
        rgba.push_back(backgroundRGBA[2]);
        rgba.push_back(backgroundRGBA[3]);
        
        
        BrainOpenGLPrimitiveDrawing::drawQuads(coords,
                                               normals,
                                               rgba);
        
        glPopMatrix();
        
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        
        glViewport(savedViewport[0],
                   savedViewport[1],
                   savedViewport[2],
                   savedViewport[3]);
        
        AnnotationPercentSizeText annotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
        annotationText.setBoldStyleEnabled(true);
        annotationText.setFontPercentViewportSize(5.0f);
        annotationText.setTextColor(CaretColorEnum::CUSTOM);
        annotationText.setBackgroundColor(CaretColorEnum::CUSTOM);
        annotationText.setCustomTextColor(horizontalAxisRGBA);
        annotationText.setCustomBackgroundColor(backgroundRGBA);
        
        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        annotationText.setText(horizontalLeftText);
        m_fixedPipelineDrawing->drawTextAtViewportCoords(textLeftWindowXY[0],
                                                         textLeftWindowXY[1],
                                                         annotationText);
        
        annotationText.setText(horizontalRightText);
        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        m_fixedPipelineDrawing->drawTextAtViewportCoords(textRightWindowXY[0],
                                                         textRightWindowXY[1],
                                                         annotationText);
        
        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
        annotationText.setCustomTextColor(verticalAxisRGBA);
        annotationText.setText(verticalBottomText);
        m_fixedPipelineDrawing->drawTextAtViewportCoords(textBottomWindowXY[0],
                                                         textBottomWindowXY[1],
                                                         annotationText);
        
        annotationText.setText(verticalTopText);
        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
        annotationText.getCoordinate()->setXYZ(textTopWindowXY[0], textTopWindowXY[1], 0.0);
        m_fixedPipelineDrawing->drawTextAtViewportCoords(textTopWindowXY[0],
                                                         textTopWindowXY[1],
                                                         annotationText);
    }
    
    if (depthEnabled) {
        glEnable(GL_DEPTH_TEST);
    }
}

/**
 * Get the RGBA coloring for a slice view plane.
 *
 * @param sliceViewPlane
 *    The slice view plane.
 * @param rgbaOut
 *    Output colors ranging 0.0 to 1.0
 */
void
BrainOpenGLVolumeTextureSliceDrawing::getAxesColor(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                   float rgbaOut[4]) const
{
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            rgbaOut[0] = 0.0;
            rgbaOut[1] = 0.0;
            rgbaOut[2] = 1.0;
            rgbaOut[3] = 1.0;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            rgbaOut[0] = 0.0;
            rgbaOut[1] = 1.0;
            rgbaOut[2] = 0.0;
            rgbaOut[3] = 1.0;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            rgbaOut[0] = 1.0;
            rgbaOut[1] = 0.0;
            rgbaOut[2] = 0.0;
            rgbaOut[3] = 1.0;
            break;
    }
}

/**
 * Draw a one millimeter square facing the user.
 * NOTE: This method will alter the current
 * modelviewing matrices so caller may need
 * to enclose the call to this method within
 * glPushMatrix() and glPopMatrix().
 *
 * @param size
 *     Size of square.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::drawSquare(const float size)
{
    const float length = size * 0.5;
    
    /*
     * Draw both front and back side since in some instances,
     * such as surface montage, we are viweing from the far
     * side (from back of monitor)
     */
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-length, -length, 0.0);
    glVertex3f( length, -length, 0.0);
    glVertex3f( length,  length, 0.0);
    glVertex3f(-length,  length, 0.0);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-length, -length, 0.0);
    glVertex3f(-length,  length, 0.0);
    glVertex3f( length,  length, 0.0);
    glVertex3f( length, -length, 0.0);
    glEnd();
}

/**
 * Get the minimum and maximum distance between adjacent voxels in all
 * slices planes.  Output spacing value are always non-negative even if
 * a right-to-left orientation.
 *
 * @param volume
 *    Volume for which min/max spacing is requested.
 * @param minSpacingOut
 *    Output minimum spacing.
 * @param maxSpacingOut
 *    Output maximum spacing.
 * @return
 *    True if min and max spacing are greater than zero.
 */
bool
BrainOpenGLVolumeTextureSliceDrawing::getMinMaxVoxelSpacing(const VolumeMappableInterface* volume,
                                                            float& minSpacingOut,
                                                            float& maxSpacingOut) const
{
    CaretAssert(volume);
    
    float originX, originY, originZ;
    float x1, y1, z1;
    volume->indexToSpace(0, 0, 0, originX, originY, originZ);
    volume->indexToSpace(1, 1, 1, x1, y1, z1);
    const float dx = std::fabs(x1 - originX);
    const float dy = std::fabs(y1 - originY);
    const float dz = std::fabs(z1 - originZ);
    
    minSpacingOut = std::min(std::min(dx, dy), dz);
    maxSpacingOut = std::max(std::max(dx, dy), dz);
    
    if ((minSpacingOut > 0.0)
        && (maxSpacingOut > 0.0)) {
        return true;
    }
    return false;
}

/**
 * Draw orientation axes
 *
 * @param viewport
 *    The viewport region for the orientation axes.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::drawOrientationAxes(const int viewport[4])
{
    const bool drawCylindersFlag = m_browserTabContent->isVolumeAxesCrosshairsDisplayed();
    const bool drawLabelsFlag = m_browserTabContent->isVolumeAxesCrosshairLabelsDisplayed();
    
    /*
     * Set the viewport
     */
    glViewport(viewport[0],
               viewport[1],
               viewport[2],
               viewport[3]);
    const double viewportWidth  = viewport[2];
    const double viewportHeight = viewport[3];
    
    /*
     * Determine bounds for orthographic projection
     */
    const double maxCoord = 100.0;
    const double minCoord = -maxCoord;
    double left   = 0.0;
    double right  = 0.0;
    double top    = 0.0;
    double bottom = 0.0;
    const double nearDepth = -1000.0;
    const double farDepth  =  1000.0;
    if (viewportHeight > viewportWidth) {
        left  = minCoord;
        right = maxCoord;
        const double aspectRatio = (viewportHeight
                                    / viewportWidth);
        top   = maxCoord * aspectRatio;
        bottom = minCoord * aspectRatio;
    }
    else {
        const double aspectRatio = (viewportWidth
                                    / viewportHeight);
        top   = maxCoord;
        bottom = minCoord;
        left  = minCoord * aspectRatio;
        right = maxCoord * aspectRatio;
    }
    
    /*
     * Set the orthographic projection
     */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
//    glLoadIdentity();
//    glOrtho(left, right,
//            bottom, top,
//            nearDepth, farDepth);
    glm::mat4 orthoMatrix = glm::ortho(left, right,
                                       bottom, top,
                                       nearDepth, farDepth);
    glLoadMatrixf(glm::value_ptr(orthoMatrix));
    
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    {
        /*
         * Set the viewing transformation, places 'eye' so that it looks
         * at the 'model' which is, in this case, the axes
         */
        const glm::vec3 eyeXYZ(0.0, 0.0, BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance);
        const glm::vec3 lookAtXYZ(0.0, 0.0, 0.0);
        const glm::vec3 upVector(0.0, 1.0, 0.0);
        glm::mat4 lookAtMatrix = glm::lookAt(eyeXYZ,
                                             lookAtXYZ,
                                             upVector);

        /*
         * Set the modeling transformation
         */
        const Matrix4x4 obliqueRotationMatrix = m_browserTabContent->getObliqueVolumeRotationMatrix();
        const glm::mat4 obliqueMat4 = convertMatrix4x4toGlmMat4(obliqueRotationMatrix);
        const glm::mat4 modelMatrix = lookAtMatrix * obliqueMat4;
        glLoadMatrixf(glm::value_ptr(modelMatrix));
        
        
        /*
         * Disable depth buffer.  Otherwise, when volume slices are drawn
         * black regions of the slices may set depth buffer and the occlude
         * the axes from display.
         */
        GLboolean depthBufferEnabled = false;
        glGetBooleanv(GL_DEPTH_TEST,
                      &depthBufferEnabled);
        glDisable(GL_DEPTH_TEST);
        const float red[4] = {
            1.0, 0.0, 0.0, 1.0
        };
        const float green[4] = {
            0.0, 1.0, 0.0, 1.0
        };
        const float blue[4] = {
            0.0, 0.0, 1.0, 1.0
        };
        
        const double axisMaxCoord = maxCoord * 0.8;
        const double axisMinCoord = -axisMaxCoord;
        const double textMaxCoord = maxCoord * 0.9;
        const double textMinCoord = -textMaxCoord;
        
        
        const float axialPlaneMin[3] = { 0.0, 0.0, (float)axisMinCoord };
        const float axialPlaneMax[3] = { 0.0, 0.0, (float)axisMaxCoord };
        const double axialTextMin[3]  = { 0.0, 0.0, (float)textMinCoord };
        const double axialTextMax[3]  = { 0.0, 0.0, (float)textMaxCoord };
        
        const float coronalPlaneMin[3] = { (float)axisMinCoord, 0.0, 0.0 };
        const float coronalPlaneMax[3] = { (float)axisMaxCoord, 0.0, 0.0 };
        const double coronalTextMin[3]  = { (float)textMinCoord, 0.0, 0.0 };
        const double coronalTextMax[3]  = { (float)textMaxCoord, 0.0, 0.0 };
        
        const float paraPlaneMin[3] = { 0.0, (float)axisMinCoord, 0.0 };
        const float paraPlaneMax[3] = { 0.0, (float)axisMaxCoord, 0.0 };
        const double paraTextMin[3]  = { 0.0, (float)textMinCoord, 0.0 };
        const double paraTextMax[3]  = { 0.0, (float)textMaxCoord, 0.0 };
        
        /*
         * Set radius as percentage of viewport height
         */
        float axesCrosshairRadius = 1.0;
        if (viewportHeight > 0) {
            const float percentageRadius = 0.005f;
            axesCrosshairRadius = percentageRadius * viewportHeight;
        }
        
        if (drawCylindersFlag) {
            m_fixedPipelineDrawing->drawCylinder(blue,
                                                 axialPlaneMin,
                                                 axialPlaneMax,
                                                 axesCrosshairRadius * 0.5f);
        }
        
        AnnotationPercentSizeText annotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        annotationText.setFontPercentViewportSize(5.0f);
        annotationText.setCoordinateSpace(AnnotationCoordinateSpaceEnum::STEREOTAXIC);
        annotationText.setTextColor(CaretColorEnum::CUSTOM);
        
        if (drawLabelsFlag) {
            annotationText.setCustomTextColor(blue);
            annotationText.setText("I");
            m_fixedPipelineDrawing->drawTextAtModelCoords(axialTextMin,
                                                          annotationText);
            annotationText.setText("S");
            m_fixedPipelineDrawing->drawTextAtModelCoords(axialTextMax,
                                                          annotationText);
        }
        
        
        if (drawCylindersFlag) {
            m_fixedPipelineDrawing->drawCylinder(green,
                                                 coronalPlaneMin,
                                                 coronalPlaneMax,
                                                 axesCrosshairRadius * 0.5f);
        }
        
        if (drawLabelsFlag) {
            annotationText.setCustomTextColor(green);
            annotationText.setText("L");
            m_fixedPipelineDrawing->drawTextAtModelCoords(coronalTextMin,
                                                          annotationText);
            annotationText.setText("R");
            m_fixedPipelineDrawing->drawTextAtModelCoords(coronalTextMax,
                                                          annotationText);
        }
        
        
        if (drawCylindersFlag) {
            m_fixedPipelineDrawing->drawCylinder(red,
                                                 paraPlaneMin,
                                                 paraPlaneMax,
                                                 axesCrosshairRadius * 0.5f);
        }
        
        if (drawLabelsFlag) {
            annotationText.setCustomTextColor(red);
            annotationText.setText("P");
            m_fixedPipelineDrawing->drawTextAtModelCoords(paraTextMin,
                                                          annotationText);

            annotationText.setText("A");
            m_fixedPipelineDrawing->drawTextAtModelCoords(paraTextMax,
                                                          annotationText);
        }
    }
    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
}

/**
 * Set the orthographic projection.
 *
 * @param allSliceViewMode
 *    Indicates drawing of ALL slices volume view (axial, coronal, parasagittal in one view)
 * @param sliceViewPlane
 *    View plane that is displayed.
 * @param viewport
 *    The viewport.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::setOrthographicProjection(const BrainOpenGLVolumeSliceDrawing::AllSliceViewMode allSliceViewMode,
                                                                const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                const int viewport[4])
{
    const bool useOrthosDrawingProjectionFlag = false; /* does not work as expected when oblique */
    if (useOrthosDrawingProjectionFlag) {
        /*
         * Determine model size in screen Y when viewed
         */
        BoundingBox boundingBox;
        m_volumeDrawInfo[0].volumeFile->getVoxelSpaceBoundingBox(boundingBox);
        
        const double zoomFactor = m_browserTabContent->getScaling();
        BrainOpenGLVolumeSliceDrawing::setOrthographicProjection(allSliceViewMode,
                                                                 sliceViewPlane,
                                                                 boundingBox,
                                                                 zoomFactor,
                                                                 viewport,
                                                                 m_orthographicBounds);

        return;
    }
    
    /*
     * Determine model size in screen Y when viewed
     */
    BoundingBox boundingBox;
    m_volumeDrawInfo[0].volumeFile->getVoxelSpaceBoundingBox(boundingBox);
    
    /*
     * Set top and bottom to the min/max coordinate
     * that runs vertically on the screen
     */
    double modelTop = 200.0;
    double modelBottom = -200.0;
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssertMessage(0, "Should never get here");
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            modelTop = boundingBox.getMaxY();
            modelBottom = boundingBox.getMinY();
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            modelTop = boundingBox.getMaxZ();
            modelBottom = boundingBox.getMinZ();
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            modelTop = boundingBox.getMaxZ();
            modelBottom = boundingBox.getMinZ();
            break;
    }
    
    switch (allSliceViewMode) {
        case BrainOpenGLVolumeSliceDrawing::AllSliceViewMode::ALL_YES:
        {
            /*
             * Parasagittal and Coronal Views have Brain's Z-axis in Screen Y
             * Axial View has Brain's Y-axis in Screen Y
             * So, use maximum of Brain's Y- and Z-axes for sizing height of slice
             * so that voxels are same size for each slice in each axis view
             */
            const float maxRangeYZ = std::max(boundingBox.getDifferenceY(),
                                              boundingBox.getDifferenceZ());
            const float range = modelTop - modelBottom;
            if (maxRangeYZ > range) {
                const float diff = maxRangeYZ - range;
                const float halfDiff = diff / 2.0;
                modelTop    += halfDiff;
                modelBottom -= halfDiff;
            }
        }
            break;
        case BrainOpenGLVolumeSliceDrawing::AllSliceViewMode::ALL_NO:
            break;
    }
    
    /*
     * Scale ratio makes region slightly larger than model
     */
    const double zoom = m_browserTabContent->getScaling();
    double scaleRatio = (1.0 / 0.98);
    if (zoom > 0.0) {
        scaleRatio /= zoom;
    }
    modelTop *= scaleRatio;
    modelBottom *= scaleRatio;
    
    /*
     * Determine aspect ratio of viewport
     */
    const double viewportWidth = viewport[2];
    const double viewportHeight = viewport[3];
    const double aspectRatio = (viewportWidth
                                / viewportHeight);
    
    /*
     * Set bounds of orthographic projection
     */
    const double halfModelY = ((modelTop - modelBottom) / 2.0);
    const double orthoBottom = modelBottom;
    const double orthoTop    = modelTop;
    const double orthoRight  =  halfModelY * aspectRatio;
    const double orthoLeft   = -halfModelY * aspectRatio;
    const double nearDepth = -1000.0;
    const double farDepth  =  1000.0;
    m_orthographicBounds[0] = orthoLeft;
    m_orthographicBounds[1] = orthoRight;
    m_orthographicBounds[2] = orthoBottom;
    m_orthographicBounds[3] = orthoTop;
    m_orthographicBounds[4] = nearDepth;
    m_orthographicBounds[5] = farDepth;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(m_orthographicBounds[0],
            m_orthographicBounds[1],
            m_orthographicBounds[2],
            m_orthographicBounds[3],
            m_orthographicBounds[4],
            m_orthographicBounds[5]);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Create the oblique transformation matrix.
 *
 * @parm sliceProjectionType
 *    The slice projection type
 * @param sliceCoordinates
 *    Slice that is being drawn.
 * @param obliqueTransformationMatrixOut
 *    OUTPUT transformation matrix for oblique viewing.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::createObliqueTransformationMatrix(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                                        const float sliceCoordinates[3],
                                                                        Matrix4x4& obliqueTransformationMatrixOut)
{
    /*
     * Initialize the oblique transformation matrix
     */
    obliqueTransformationMatrixOut.identity();
    
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
        {
            /*
             * Get the oblique rotation matrix
             */
            Matrix4x4 obliqueRotationMatrix = m_browserTabContent->getObliqueVolumeRotationMatrix();
            
            /*
             * Create the transformation matrix
             */
            obliqueTransformationMatrixOut.postmultiply(obliqueRotationMatrix);

        }
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            break;
    }
    
    /*
     * Translate to selected coordinate
     */
    obliqueTransformationMatrixOut.translate(sliceCoordinates[0],
                                             sliceCoordinates[1],
                                             sliceCoordinates[2]);
}

/* ======================================================================= */

/**
 * Get the texture coordinates for an XYZ-coordinate
 *
 * @param volumeMappableInterface
 *     The volume file
 * @param xyz
 *     The XYZ coordinate
 * @param maxStr
 *     The maximum texture str coordinate
 * @param strOut
 *     Output texture str coordinate
 * @return
 *     True if output coordinate is valid, else false.
 */
bool
BrainOpenGLVolumeTextureSliceDrawing::getTextureCoordinates(const VolumeMappableInterface* volumeMappableInterface,
                                                            const std::array<float, 3>& xyz,
                                                            const std::array<float, 3>& maxStr,
                                                            std::array<float, 3>& strOut) const
{
    std::vector<int64_t> dims(5);
    volumeMappableInterface->getDimensions(dims);
    
    
    {
        const VolumeSpace& volumeSpace = volumeMappableInterface->getVolumeSpace();
        std::array<float, 3> ijk;
        volumeSpace.spaceToIndex(xyz.data(), ijk.data());
        
        const std::array<float, 3> normalizedIJK {
            (ijk[0] / dims[0]),
            (ijk[1] / dims[1]),
            (ijk[2] / dims[2])
        };
        std::array<float, 3> str {
            (normalizedIJK[0] * maxStr[0]),
            (normalizedIJK[1] * maxStr[1]),
            (normalizedIJK[2] * maxStr[2])
        };
        
        strOut = str;
    }
    
    return true;
}

/**
 * Draw an oblique slice with support for outlining labels and thresholded palette data.
 *
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param sliceProjectionType
 *    The slice projection type
 * @param transformationMatrix
 *    The for oblique viewing.
 */
void
BrainOpenGLVolumeTextureSliceDrawing::drawObliqueSliceWithPrimitive(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                    const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                                    Matrix4x4& transformationMatrix)
{
    /*
     * When performing voxel identification for editing voxels,
     * we need to draw EVERY voxel since the user may click
     * regions where the voxels are "off".
     */
    // value unused at this time but will likely be needed later if this volume rendering is fully implemented
    //float voxelEditingValue = 1.0;
    VolumeFile* voxelEditingVolumeFile = NULL;
    if (m_identificationModeFlag) {
        SelectionItemVoxelEditing* voxelEditID = m_brain->getSelectionManager()->getVoxelEditingIdentification();
        if (voxelEditID->isEnabledForSelection()) {
            voxelEditingVolumeFile = voxelEditID->getVolumeFileForEditing();
            if (voxelEditingVolumeFile != NULL) {
                if (voxelEditingVolumeFile->isMappedWithLabelTable()) {
                    if (voxelEditingVolumeFile->getNumberOfMaps() > 0) {
                        //voxelEditingValue = voxelEditingVolumeFile->getMapLabelTable(0)->getUnassignedLabelKey();
                    }
                }
            }
        }
    }
    
    const int32_t numVolumes = static_cast<int32_t>(m_volumeDrawInfo.size());
    
    /*
     * Look at point is in center of volume
     */
    float translation[3];
    m_browserTabContent->getTranslation(translation);
    float viewOffsetX = 0.0;
    float viewOffsetY = 0.0;
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            viewOffsetX = (m_lookAtCenter[0] + translation[0]);
            viewOffsetY = (m_lookAtCenter[1] + translation[1]);
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            viewOffsetX = (m_lookAtCenter[0] + translation[0]);
            viewOffsetY = (m_lookAtCenter[2] + translation[2]);
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            viewOffsetX = (m_lookAtCenter[1] + translation[1]);
            viewOffsetY = (m_lookAtCenter[2] + translation[2]);
            break;
    }
    
    float minScreenX = m_orthographicBounds[0] - viewOffsetX;
    float maxScreenX = m_orthographicBounds[1] - viewOffsetX;
    float minScreenY = m_orthographicBounds[2] - viewOffsetY;
    float maxScreenY = m_orthographicBounds[3] - viewOffsetY;
    
    
    /*
     * Get origin voxel IJK
     */
    const float zeroXYZ[3] = { 0.0, 0.0, 0.0 };
    int64_t originIJK[3];
    m_volumeDrawInfo[0].volumeFile->enclosingVoxel(zeroXYZ[0], zeroXYZ[1], zeroXYZ[2],
                                                   originIJK[0], originIJK[1], originIJK[2]);
    
    
    /*
     * Get XYZ center of origin Voxel
     */
    float originVoxelXYZ[3];
    m_volumeDrawInfo[0].volumeFile->indexToSpace(originIJK, originVoxelXYZ);
    float actualOrigin[3];
    m_volumeDrawInfo[0].volumeFile->indexToSpace(originIJK, actualOrigin);
    
    /*
     * Set the corners of the screen for the respective view
     */
    std::array<float, 3> bottomLeft;
    std::array<float, 3> bottomRight;
    std::array<float, 3> topRight;
    std::array<float, 3> topLeft;
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            bottomLeft[0] = minScreenX;
            bottomLeft[1] = minScreenY;
            bottomLeft[2] = 0.0;
            bottomRight[0] = maxScreenX;
            bottomRight[1] = minScreenY;
            bottomRight[2] = 0.0;
            topRight[0] = maxScreenX;
            topRight[1] = maxScreenY;
            topRight[2] = 0.0;
            topLeft[0] = minScreenX;
            topLeft[1] = maxScreenY;
            topLeft[2] = 0.0;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            bottomLeft[0] = minScreenX;
            bottomLeft[1] = 0.0;
            bottomLeft[2] = minScreenY;
            bottomRight[0] = maxScreenX;
            bottomRight[1] = 0.0;
            bottomRight[2] = minScreenY;
            topRight[0] = maxScreenX;
            topRight[1] = 0.0;
            topRight[2] = maxScreenY;
            topLeft[0] = minScreenX;
            topLeft[1] = 0.0;
            topLeft[2] = maxScreenY;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            bottomLeft[0] = 0.0;
            bottomLeft[1] = minScreenX;
            bottomLeft[2] = minScreenY;
            bottomRight[0] = 0.0;
            bottomRight[1] = maxScreenX;
            bottomRight[2] = minScreenY;
            topRight[0] = 0.0;
            topRight[1] = maxScreenX;
            topRight[2] = maxScreenY;
            topLeft[0] = 0.0;
            topLeft[1] = minScreenX;
            topLeft[2] = maxScreenY;
            break;
    }
    
    
    /*
     * Transform the corners of the screen into model coordinates
     */
    transformationMatrix.multiplyPoint3(bottomLeft.data());
    transformationMatrix.multiplyPoint3(bottomRight.data());
    transformationMatrix.multiplyPoint3(topRight.data());
    transformationMatrix.multiplyPoint3(topLeft.data());
        
    if (debugFlag) {
        m_fixedPipelineDrawing->setLineWidth(3.0);
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINE_LOOP);
        glVertex3fv(bottomLeft.data());
        glVertex3fv(bottomRight.data());
        glVertex3fv(topRight.data());
        glVertex3fv(topLeft.data());
        glEnd();
    }
    
    /*
     * Unit vector and distance in model coords along left side of screen
     */
    double bottomLeftToTopLeftUnitVector[3] = {
        topLeft[0] - bottomLeft[0],
        topLeft[1] - bottomLeft[1],
        topLeft[2] - bottomLeft[2],
    };
    MathFunctions::normalizeVector(bottomLeftToTopLeftUnitVector);
    const double bottomLeftToTopLeftDistance = MathFunctions::distance3D(bottomLeft.data(),
                                                                         topLeft.data());
    
    /*
     * Unit vector and distance in model coords along right side of screen
     */
    double bottomRightToTopRightUnitVector[3] = {
        topRight[0] - bottomRight[0],
        topRight[1] - bottomRight[1],
        topRight[2] - bottomRight[2]
    };
    MathFunctions::normalizeVector(bottomRightToTopRightUnitVector);
    const double bottomRightToTopRightDistance = MathFunctions::distance3D(bottomRight.data(),
                                                                           topRight.data());
    
    if ((bottomLeftToTopLeftDistance > 0)
        && (bottomRightToTopRightDistance > 0)) {
        const DisplayPropertiesVolume* dsv = m_brain->getDisplayPropertiesVolume();
        const bool allowBlendingFlag(dsv->getOpacity() >= 1.0);
        
        glPushAttrib(GL_COLOR_BUFFER_BIT);
        if (allowBlendingFlag) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        
        if (m_modelWholeBrain != NULL) {
            glAlphaFunc(GL_GEQUAL, 0.95);
            glEnable(GL_ALPHA_TEST);
            glEnable(GL_DEPTH_TEST);
        }
        else {
            glDisable(GL_DEPTH_TEST);
        }
        
        bool firstFlag(true);
        for (int32_t iVol = 0; iVol < numVolumes; iVol++) {
            const BrainOpenGLFixedPipeline::VolumeDrawInfo& vdi = m_volumeDrawInfo[iVol];
            VolumeMappableInterface* volumeInterface = vdi.volumeFile;
            if (volumeInterface != NULL) {
                if (debugFlag) {
                    //std::cout << "Vol: " << iVol << ": " << vf->getFileNameNoPath() << std::endl;
                }
                
                if (firstFlag) {
                    /*
                     * Using GL_ONE prevents an edge artifact
                     * (narrow line on texture edges).
                     */
                    if (allowBlendingFlag) {
                        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                    }
                    firstFlag = false;
                }
                else {
                    if (allowBlendingFlag) {
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    }
                }
                
                GraphicsPrimitiveV3fT3f* primitive(volumeInterface->getVolumeDrawingPrimitive(vdi.mapIndex,
                                                                                        DisplayGroupEnum::DISPLAY_GROUP_TAB,
                                                                                        m_tabIndex));
                
                if (primitive != NULL) {
                    std::array<float, 3> maxStr = { 1.0, 1.0, 1.0 };
                    std::array<float, 3> textureBottomLeft;
                    getTextureCoordinates(volumeInterface, bottomLeft, maxStr, textureBottomLeft);
                    std::array<float, 3> textureBottomRight;
                    getTextureCoordinates(volumeInterface, bottomRight, maxStr, textureBottomRight);
                    std::array<float, 3> textureTopLeft;
                    getTextureCoordinates(volumeInterface, topLeft, maxStr, textureTopLeft);
                    std::array<float, 3> textureTopRight;
                    getTextureCoordinates(volumeInterface, topRight, maxStr, textureTopRight);
                    
                    primitive->replaceVertexFloatXYZ(0, bottomLeft.data());
                    primitive->replaceVertexFloatXYZ(1, bottomRight.data());
                    primitive->replaceVertexFloatXYZ(2, topLeft.data());
                    primitive->replaceVertexFloatXYZ(3, topRight.data());
                    primitive->replaceVertexTextureSTR(0, textureBottomLeft.data());
                    primitive->replaceVertexTextureSTR(1, textureBottomRight.data());
                    primitive->replaceVertexTextureSTR(2, textureTopLeft.data());
                    primitive->replaceVertexTextureSTR(3, textureTopRight.data());
                    
                    bool discreteFlag(false);
                    bool magNearestFlag(false);
                    bool magSmoothFlag(false);
                    switch (sliceProjectionType) {
                        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                            if (vdi.mapFile->isMappedWithLabelTable()
                                || (vdi.mapFile->isMappedWithRGBA())) {
                                /*
                                 * Use "discrete" with Labels or RGBA since no interpolation
                                 */
                                discreteFlag = true;
                            }
                            else if (vdi.mapFile->isMappedWithPalette()) {
                                bool anatomyFlag(false);
                                bool functionalFlag(false);
                                if (DataFileTypeEnum::isConnectivityDataType(vdi.mapFile->getDataFileType())) {
                                    /*
                                     * CIFTI is always a functional type
                                     */
                                    functionalFlag = true;
                                }
                                else {
                                    /*
                                     * Must be a volume file
                                     */
                                    const VolumeFile* vf(dynamic_cast<VolumeFile*>(vdi.mapFile));
                                    CaretAssert(vf);
                                    switch (vf->getType()) {
                                        case SubvolumeAttributes::ANATOMY:
                                            anatomyFlag = true;
                                            break;
                                        case SubvolumeAttributes::FUNCTIONAL:
                                            functionalFlag = true;
                                            break;
                                        case SubvolumeAttributes::LABEL:
                                            CaretAssert(0);
                                            break;
                                        case SubvolumeAttributes::RGB:
                                            CaretAssert(0);
                                            break;
                                        case SubvolumeAttributes::SEGMENTATION:
                                            anatomyFlag = true;
                                            break;
                                        case SubvolumeAttributes::UNKNOWN:
                                            anatomyFlag = true;
                                            break;
                                        case SubvolumeAttributes::VECTOR:
                                            CaretAssert(0);
                                            break;
                                    }
                                }
                                
                                if (anatomyFlag) {
                                    if (DeveloperFlagsEnum::isFlag(DeveloperFlagsEnum::DELELOPER_FLAG_TEXTURE_ANATOMY_VOLUME_SMOOTH)) {
                                        magSmoothFlag = true;
                                    }
                                    else {
                                        magNearestFlag = true;
                                    }
                                }
                                else if (functionalFlag) {
                                    if (DeveloperFlagsEnum::isFlag(DeveloperFlagsEnum::DELELOPER_FLAG_TEXTURE_FUNCTIONAL_VOLUME_SMOOTH)) {
                                        magSmoothFlag = true;
                                    }
                                    else {
                                        magNearestFlag = true;
                                    }
                                }
                            }
                            break;
                        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                            /*
                             * Always use NEAREST for orthogonal
                             */
                            magNearestFlag = true;
                            break;
                    }
                    
                    if (discreteFlag) {
                        primitive->setTextureMinificationFilter(GraphicsTextureMinificationFilterEnum::NEAREST);
                        primitive->setTextureMagnificationFilter(GraphicsTextureMagnificationFilterEnum::NEAREST);
                    }
                    else if (magNearestFlag) {
                        /* Use Linear for Minification, Nearest for Magnification */
                        primitive->setTextureMinificationFilter(GraphicsTextureMinificationFilterEnum::LINEAR);
                        primitive->setTextureMagnificationFilter(GraphicsTextureMagnificationFilterEnum::NEAREST);
                    }
                    else if (magSmoothFlag) {
                        /* Use Linear for both Minification and Magnification */
                        primitive->setTextureMinificationFilter(GraphicsTextureMinificationFilterEnum::LINEAR);
                        primitive->setTextureMagnificationFilter(GraphicsTextureMagnificationFilterEnum::LINEAR);
                    }
                    else {
                        CaretAssert(0);
                    }

                    GraphicsEngineDataOpenGL::draw(primitive);
                    
                    if (m_identificationModeFlag) {
                        performIdentification(volumeInterface,
                                              primitive,
                                              sliceViewPlane,
                                              bottomLeft,
                                              bottomRight,
                                              topRight,
                                              topLeft,
                                              m_fixedPipelineDrawing->mouseX,
                                              m_fixedPipelineDrawing->mouseY);
                    }
                }
            }
        }
        
        glPopAttrib();
    }
}

/**
 * Perform identification procesing
 * @param volumeInterface
 *    The volume
 * @param primitive
 *    Graphics primitive used to draw volume
 * @param sliceViewPlane
 *    The slice view plane
 * @param bottomLeft
 *    Bottom left corner of viewport in model coordinates
 * @param bottomRight
 *    Bottom right corner of viewport in model coordinates
 * @param topRight
 *    Top right corner of viewport in model coordinates
 * @param topLeft
 *    Top left corner of viewport in model coordinates
 * @param mouseX
 *    X location of mouse click
 * @param mouseY
 *    Y location of mouse click
 * @return
 *    True if identification is valid, else false.
*
 */
void
BrainOpenGLVolumeTextureSliceDrawing::performIdentification(VolumeMappableInterface* volumeInterface,
                                                            const GraphicsPrimitiveV3fT3f* primitive,
                                                            const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                            const std::array<float, 3> bottomLeft,
                                                            const std::array<float, 3> bottomRight,
                                                            const std::array<float, 3> topRight,
                                                            const std::array<float, 3> topLeft,
                                                            const float mouseX,
                                                            const float mouseY)
{
    CaretAssert(volumeInterface);
    CaretAssert(primitive);
    
    
    int32_t viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const float vpMinX(viewport[0]);
    const float vpMaxX(viewport[0] + viewport[2]);
    const float vpRangeX(vpMaxX - vpMinX);
    const float vpMinY(viewport[1]);
    const float vpMaxY(viewport[1] + viewport[3]);
    const float vpRangeY(vpMaxY - vpMinY);
    
    bool swapNormXValueFlag(false);
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            swapNormXValueFlag = true;
            break;
    }
    const float mouseNormX(swapNormXValueFlag
                           ? (1.0 - ((mouseX - vpMinX) / vpRangeX))
                           : ((mouseX - vpMinX) / vpRangeX));
    const float mouseNormY((mouseY - vpMinY) / vpRangeY);
    if (debugFlag) {
        std::cout << "Mouse Norm X/Y: " << mouseNormX << ", " << mouseNormY << std::endl << std::flush;
    }
    
    if ((mouseNormX >= 0.0)
        && (mouseNormX <= 1.0)
        && (mouseNormY >= 0.0)
        && (mouseNormY <= 1.0)) {
        float deltaTopXYZ[3];
        MathFunctions::subtractVectors(topRight.data(), topLeft.data(), deltaTopXYZ);
        const float topXYZ[3] {
            topLeft[0] + (deltaTopXYZ[0] * mouseNormX),
            topLeft[1] + (deltaTopXYZ[1] * mouseNormX),
            topLeft[2] + (deltaTopXYZ[2] * mouseNormX),
        };
        
        float deltaBottomXYZ[3];
        MathFunctions::subtractVectors(bottomRight.data(), bottomLeft.data(), deltaBottomXYZ);
        const float bottomXYZ[3] {
            bottomLeft[0] + (deltaBottomXYZ[0] * mouseNormX),
            bottomLeft[1] + (deltaBottomXYZ[1] * mouseNormX),
            bottomLeft[2] + (deltaBottomXYZ[2] * mouseNormX),
        };
        
        const float weightTop(mouseNormY);
        const float weightBottom(1.0 - weightTop);
        
        const float pickXYZ[3] {
            (topXYZ[0] * weightTop) + (bottomXYZ[0] * weightBottom),
            (topXYZ[1] * weightTop) + (bottomXYZ[1] * weightBottom),
            (topXYZ[2] * weightTop) + (bottomXYZ[2] * weightBottom),
        };
        
        float depth(0.0);
        
        int64_t ijk[3];
        volumeInterface->enclosingVoxel(pickXYZ[0], pickXYZ[1], pickXYZ[2], ijk[0], ijk[1], ijk[2]);

        
        if (volumeInterface->indexValid(ijk[0], ijk[0], ijk[2])) {
            SelectionItemVoxel* voxelID = m_brain->getSelectionManager()->getVoxelIdentification();
            if (voxelID->isEnabledForSelection()) {
                if ( ! voxelID->isValid()) {
                    if (voxelID->isOtherScreenDepthCloserToViewer(depth)) {
                        voxelID->setVoxelIdentification(m_brain,
                                                        volumeInterface,
                                                        ijk,
                                                        depth);
                        
                        m_fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelID,
                                                                         pickXYZ);
                        CaretLogFinest("Selected Voxel (3D): " + AString::fromNumbers(ijk, 3, ","));
                        
                        if (debugFlag) {
                            std::cout << "Mouse XYZ: " << AString::fromNumbers(pickXYZ, 3, ", ") << std::endl << std::flush;
                            std::cout << "      IJK: " << AString::fromNumbers(ijk, 3, ", ") << std::endl << std::flush;
                        }
                    }
                }
            }
            
            SelectionItemVoxelEditing* voxelEditID = m_brain->getSelectionManager()->getVoxelEditingIdentification();
            if (voxelEditID->isEnabledForSelection()) {
                if (voxelEditID->getVolumeFileForEditing() == volumeInterface) {
                    if ( ! voxelEditID->isValid()) {
                        if (voxelEditID->isOtherScreenDepthCloserToViewer(depth)) {
                            voxelEditID->setVoxelIdentification(m_brain,
                                                                volumeInterface,
                                                                ijk,
                                                                depth);
                            const float floatDiffXYZ[3] = { 1.0, 1.0, 1.0 };
                            voxelEditID->setVoxelDiffXYZ(floatDiffXYZ);
                            
                            m_fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelEditID,
                                                                             pickXYZ);
                            CaretLogFinest("Selected Voxel Editing (3D): Indices ("
                                           + AString::fromNumbers(ijk, 3, ",")
                                           + ") Diff XYZ ("
                                           + AString::fromNumbers(floatDiffXYZ, 3, ",")
                                           + ")");
                        }
                    }
                }
            }
        }
    }
}
