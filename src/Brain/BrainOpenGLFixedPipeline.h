
#ifndef __BRAIN_OPENGL_FIXED_PIPELINE_H__
#define __BRAIN_OPENGL_FIXED_PIPELINE_H__

/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <stdint.h>

#include "BrainConstants.h"
#include "BrainOpenGL.h"
#include "BrainOpenGLTextRenderInterface.h"
#include "CaretVolumeExtension.h"
#include "IdentificationItemDataTypeEnum.h"
#include "StructureEnum.h"
#include "SurfaceNodeColoring.h"
#include "VolumeSliceViewPlaneEnum.h"

class QGLWidget;

namespace caret {
    
    class Brain;
    class BrainOpenGLViewportContent;
    class BrowserTabContent;
    class FastStatistics;
    class DescriptiveStatistics;
    class IdentificationItem;
    class IdentificationManager;
    class IdentificationWithColor;
    class Surface;
    class Model;
    class ModelSurface;
    class ModelSurfaceMontage;
    class ModelVolume;
    class ModelWholeBrain;
    class Palette;
    class PaletteColorMapping;
    class PaletteFile;
    class SphereOpenGL;
    class VolumeFile;
    
    /**
     * Performs drawing of graphics using OpenGL.
     */
    class BrainOpenGLFixedPipeline : public BrainOpenGL {
        
    private:
        enum Mode {
            MODE_DRAWING,
            MODE_IDENTIFICATION,
            MODE_PROJECTION
        };
        
        BrainOpenGLFixedPipeline(const BrainOpenGLFixedPipeline&);
        BrainOpenGLFixedPipeline& operator=(const BrainOpenGLFixedPipeline&);
        
    public:
        BrainOpenGLFixedPipeline(BrainOpenGLTextRenderInterface* textRenderer);

        ~BrainOpenGLFixedPipeline();
        
        void drawModels(std::vector<BrainOpenGLViewportContent*>& viewportContents);
        
        void selectModel(BrainOpenGLViewportContent* viewportContent,
                         const int32_t mouseX,
                         const int32_t mouseY);
        
        void projectToModel(BrainOpenGLViewportContent* viewportContent,
                            const int32_t mouseX,
                            const int32_t mouseY,
                            SurfaceProjectedItem& projectionOut);
        
        void initializeOpenGL();
        
    private:
        class VolumeDrawInfo {
        public:
            VolumeDrawInfo(VolumeFile* volumeFile,
                           Brain* brain,
                           Palette* palette,
                           PaletteColorMapping* paletteColorMapping,
                           const FastStatistics* statistics,
                           const int32_t mapIndex,
                           const float opacity);
            Brain* brain;
            VolumeFile* volumeFile;
            SubvolumeAttributes::VolumeType volumeType;
            Palette* palette;
            PaletteColorMapping* paletteColorMapping;
            const FastStatistics* statistics;
            int32_t mapIndex;
            float opacity;
        };
        
        void colorizeVoxels(const VolumeDrawInfo& volumeDrawInfo,
                            const float* scalarValues,
                            const float* thresholdValues,
                            const int32_t numberOfScalars,
                            float* rgbaOut,
                            const bool ignoreThresholding);

        void drawModelInternal(Mode mode,
                               BrainOpenGLViewportContent* viewportContent);
        
        void initializeMembersBrainOpenGL();
        
        void drawSurfaceController(ModelSurface* surfaceController,
                                   const int32_t viewport[4]);
        
        void drawSurface(Surface* surface,
                         const float* nodeColoringRGBA);
        
        void drawSurfaceNodes(Surface* surface,
                              const float* nodeColoringRGBA);
        
        void drawSurfaceTrianglesWithVertexArrays(const Surface* surface,
                                                  const float* nodeColoringRGBA);
        
        void drawSurfaceTriangles(Surface* surface,
                                  const float* nodeColoringRGBA);
        
        void drawSurfaceNodeAttributes(Surface* surface);
        
        void drawSurfaceBorderBeingDrawn(const Surface* surface);
        
        void drawSurfaceBorders(Surface* surface);
        
        void drawBorder(const Surface* surface,
                        const Border* border,
                        const int32_t borderFileIndex,
                        const int32_t borderIndex,
                        const bool isSelect,
                        const bool isContralateralEnabled);
        
        void drawVolumeController(BrowserTabContent* browserTabContent,
                                  ModelVolume* volumeController,
                                  const int32_t viewport[4]);
        
        void drawVolumeAxesCrosshairs(
                                  const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                  const float voxelXYZ[3]);
        
        void drawVolumeAxesLabels(const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                      const int32_t viewport[4]);
        
        void drawVolumeOrthogonalSlice(const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                       const int64_t sliceIndex,
                                       std::vector<VolumeDrawInfo>& volumeDrawInfo);
        
        void drawVolumeOrthogonalSliceVolumeViewer(const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                       const int64_t sliceIndex,
                                       std::vector<VolumeDrawInfo>& volumeDrawInfo);
        
        void drawVolumeSurfaceOutlines(Brain* brain,
                                       Model* modelDisplayController,
                                       const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                       const int64_t sliceIndex,
                                       VolumeFile* underlayVolume);
        
        void setupVolumeDrawInfo(BrowserTabContent* browserTabContent,
                                 Brain* brain,
                                 std::vector<VolumeDrawInfo>& volumeDrawInfoOut);
        
        void drawWholeBrainController(BrowserTabContent* browserTabContent,
                                      ModelWholeBrain* wholeBrainController,
                                      const int32_t viewport[4]);
        
        void drawSurfaceMontageModel(BrowserTabContent* browserTabContent,
                                     ModelSurfaceMontage* surfaceMontageModel,
                                     const int32_t viewport[4]);
        
        void setOrthographicProjection(const int32_t viewport[4],
                                       const bool isRightSurfaceLateralMedialYoked);
        
        void checkForOpenGLError(const Model* modelController,
                                 const AString& msg);
        
        void enableLighting();
        
        void disableLighting();
        
        void enableLineAntiAliasing();
        void disableLineAntiAliasing();
        
        void getIndexFromColorSelection(const IdentificationItemDataTypeEnum::Enum dataType,
                                           const int32_t x,
                                           const int32_t y,
                                           int32_t& indexOut,
                                           float& depthOut);
        
        void getIndexFromColorSelection(const IdentificationItemDataTypeEnum::Enum dataType,
                                        const int32_t x,
                                        const int32_t y,
                                        int32_t& index1Out,
                                        int32_t& index2Out,
                                        int32_t& index3Out,
                                        float& depthOut);
        
        void setIdentifiedItemScreenXYZ(IdentificationItem* item,
                                        const float itemXYZ[3]);
        
        void setViewportAndOrthographicProjection(const int32_t viewport[4],
                                                  const bool isRightSurfaceLateralMedialYoked = false);
        
        void applyViewingTransformations(const Model* modelDisplayController,
                                         const int32_t tabIndex,
                                         const float objectCenterXYZ[3],
                                         const bool isRightSurfaceLateralMedialYoked);
        
        void applyViewingTransformationsVolumeSlice(const ModelVolume* modelDisplayControllerVolume,
                                         const int32_t tabIndex,
                                         const VolumeSliceViewPlaneEnum::Enum viewPlane);
        
        void drawSurfaceAxes();
        
        void drawSphere(const double radius);
        
        void drawTextWindowCoords(const int windowX,
                                  const int windowY,
                                  const QString& text,
                                  const BrainOpenGLTextRenderInterface::TextAlignmentX alignmentX,
                                  const BrainOpenGLTextRenderInterface::TextAlignmentY alignmentY);
        
        void drawTextModelCoords(const double modelX,
                                 const double modelY,
                                 const double modelZ,
                                 const QString& text);
        
        void drawAllPalettes(Brain* brain);
        
        void drawPalette(const Palette* palette,
                         const PaletteColorMapping* paletteColorMapping,
                         const DescriptiveStatistics* statistics,
                         const int paletteDrawingIndex);
        
        void drawPalette(const Palette* palette,
                         const PaletteColorMapping* paletteColorMapping,
                         const FastStatistics* statistics,
                         const int paletteDrawingIndex);
        
        float modelSizeToPixelSize(const float modelSize);
        
        void setProjectionModeData(const float screenDepth,
                                          const float xyz[3],
                                          const StructureEnum::Enum structure,
                                          const float barycentricAreas[3],
                                          const int barycentricNodes[3],
                                          const int numberOfNodes);
        
        /** Indicates OpenGL has been initialized */
        bool initializedOpenGLFlag;
        
        /** Content of browser tab being drawn */
        BrowserTabContent* browserTabContent;
        
        /** Index of window tab */
        int32_t windowTabIndex;
        
        /** mode of operation draw/select/etc*/
        Mode mode;
        
        /** Identification manager */
        IdentificationManager* identificationManager;
        
        int32_t mouseX;
        int32_t mouseY;
        
        /** Identify using color */
        IdentificationWithColor* colorIdentification;

        SurfaceProjectedItem* modeProjectionData;
        
        /** Screen depth when projecting to surface mode */
        double modeProjectionScreenDepth;
        
        /** Performs node coloring */
        SurfaceNodeColoring* surfaceNodeColoring;
         
        uint32_t sphereDisplayList;
        
        SphereOpenGL* sphereOpenGL;
        
        double orthographicLeft; 
        double orthographicRight;
        double orthographicBottom;
        double orthographicTop;
        double orthographicFar; 
        double orthographicNear; 
    };

#ifdef __BRAIN_OPENGL_FIXED_PIPELINE_DEFINE_H
#endif //__BRAIN_OPENGL_FIXED_PIPELINE_DEFINE_H

} // namespace


#endif // __BRAIN_OPENGL_FIXED_PIPELINE_H__
