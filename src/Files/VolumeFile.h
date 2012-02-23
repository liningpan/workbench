
#ifndef __VOLUME_FILE_H__
#define __VOLUME_FILE_H__

/*LICENSE_START*/
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

#include "VolumeBase.h"
#include "CaretMappableDataFile.h"
#include "CaretVolumeExtension.h"
#include "StructureEnum.h"
#include "GiftiMetaData.h"
#include "BoundingBox.h"
#include "DescriptiveStatistics.h"

namespace caret {

    class VolumeFile : public VolumeBase, public CaretMappableDataFile
    {
        CaretVolumeExtension m_caretVolExt;
        
        void parseExtensions();//called after reading a file, in order to populate m_caretVolExt with best guesses
        
        void validateMembers();//called to ensure extension agrees with number of subvolumes
        
        void updateCaretExtension();//called before writing a file, erases all existing caret extensions from m_extensions, and rebuilds one from m_caretVolExt
        
        struct BrickAttributes//for storing ONLY stuff that doesn't get saved to the caret extension
        {
            CaretPointer<DescriptiveStatistics> m_statistics;
            CaretPointer<DescriptiveStatistics> m_statisticsLimitedValues;
            CaretPointer<GiftiMetaData> m_metadata;//NOTE: does not get saved currently!
        };
        
        mutable std::vector<BrickAttributes> m_brickAttributes;//because statistics and metadata construct lazily
        
        bool m_brickStatisticsValid;//so that setModified() doesn't do something slow
        
    public:
        
        enum InterpType
        {
            ENCLOSING_VOXEL,
            TRILINEAR
        };
        
        const static float INVALID_INTERP_VALUE;
        
        VolumeFile();
        VolumeFile(const std::vector<int64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const int64_t numComponents = 1, SubvolumeAttributes::VolumeType whatType = SubvolumeAttributes::ANATOMY);
        //convenience method for unsigned
        VolumeFile(const std::vector<uint64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const uint64_t numComponents = 1, SubvolumeAttributes::VolumeType whatType = SubvolumeAttributes::ANATOMY);
        ~VolumeFile();
        
        ///recreates the volume file storage with new size and spacing
        void reinitialize(const std::vector<int64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const int64_t numComponents = 1, SubvolumeAttributes::VolumeType whatType = SubvolumeAttributes::ANATOMY);
        void reinitialize(const std::vector<uint64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const uint64_t numComponents = 1, SubvolumeAttributes::VolumeType whatType = SubvolumeAttributes::ANATOMY);
        
        void setType(SubvolumeAttributes::VolumeType whatType);
        
        SubvolumeAttributes::VolumeType getType() const;

        float interpolateValue(const float* coordIn, InterpType interp = TRILINEAR, bool* validOut = NULL, const int64_t brickIndex = 0, const int64_t component = 0);

        float interpolateValue(const float coordIn1, const float coordIn2, const float coordIn3, InterpType interp = TRILINEAR, bool* validOut = NULL, const int64_t brickIndex = 0, const int64_t component = 0);

        ///returns true if volume space matches in spatial dimensions and sform
        bool matchesVolumeSpace(const VolumeFile* right) const;
        
        void readFile(const AString& filename) throw (DataFileException);

        void writeFile(const AString& filename) throw (DataFileException);

        bool isEmpty() const { return VolumeBase::isEmpty(); }
        
        virtual void setModified();
        
        virtual void clearModified() { CaretMappableDataFile::clearModified(); VolumeBase::clearModified(); }
        
        virtual bool isModified() const { return (CaretMappableDataFile::isModified() || VolumeBase::isModified()); }
        
        BoundingBox getSpaceBoundingBox() const;
        
        /**
         * @return The structure for this file.
         */
        StructureEnum::Enum getStructure() const;
        
        /**
         * Set the structure for this file.
         * @param structure
         *   New structure for this file.
         */
        void setStructure(const StructureEnum::Enum structure);
        
        /**
         * @return Get access to the file's metadata.
         */
        GiftiMetaData* getFileMetaData() { return NULL; }//doesn't seem to be a spot for generic metadata in the nifti caret extension
        
        /**
         * @return Get access to unmodifiable file's metadata.
         */
        const GiftiMetaData* getFileMetaData() const { return NULL; }
        
        bool isSurfaceMappable() const { return false; }
        
        bool isVolumeMappable() const { return true; }
        
        int32_t getNumberOfMaps() const { return m_dimensions[3]; }
        
        AString getMapName(const int32_t mapIndex) const;
        
        void setMapName(const int32_t mapIndex,
                                const AString& mapName);
        
        const GiftiMetaData* getMapMetaData(const int32_t mapIndex) const;
        
        GiftiMetaData* getMapMetaData(const int32_t mapIndex);
        
        const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex);
        
        const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex,
                                                      const float mostPositiveValueInclusive,
                                                      const float leastPositiveValueInclusive,
                                                      const float leastNegativeValueInclusive,
                                                      const float mostNegativeValueInclusive,
                                                      const bool includeZeroValues);
        
        bool isMappedWithPalette() const;
        
        PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex);
        
        const PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) const;
        
        bool isMappedWithLabelTable() const;
        
        GiftiLabelTable* getMapLabelTable(const int32_t mapIndex);
        
        const GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) const;

        AString getMapUniqueID(const int32_t mapIndex) const;
    };

}

#endif //__VOLUME_FILE_H__
