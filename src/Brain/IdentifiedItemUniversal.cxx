
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#define __IDENTIFIED_ITEM_UNIVERSAL_DECLARE__
#include "IdentifiedItemUniversal.h"
#undef __IDENTIFIED_ITEM_UNIVERSAL_DECLARE__

#include "CaretPreferences.h"
#include "FileInformation.h"
#include "IdentifiedItemNode.h"
#include "IdentifiedItemVoxel.h"
#include "SceneAttributes.h"
#include "SceneClassAssistant.h"
#include "SessionManager.h"

using namespace caret;


    
/**
 * \class caret::IdentifiedItem
 * \brief Describes an identified for all types (media, surface, volume)
 */

/**
 * Constructor of invalid instance.
 */
IdentifiedItemUniversal::IdentifiedItemUniversal()
: CaretObject(),
SceneableInterface()
{
    initializeInstance();
    m_voxelIJK.fill(-1);
    m_stereotaxicXYZ.fill(0.0f);
}

/**
 * Constructor.
 *
 *@param type
 *    Type of identified item
 * @param simpleText
 *    Text describing the identified item.
 * @param formattedText
 *    Formatted text describing the identified item.
 * @param dataFileName
 *    Name of data file on which identification was performed
 * @param structure
 *    Structure on which identification took place.
 * @param surfaceNumberOfVertices
 *    Number of vertices in surface.
 * @param surfaceVertexIndex
 *    Index of vertex that was identified.
 * @param pixelIndex
 *    Index of pixel.
 * @param voxelIJK
 *    The voxel IJK indices
 *@param stereotaxicXYZ
 *    The stereotaxic coordinate
 * @param stereotaxicXYZValidFlag
 *    True if the stereotaxic coordinate is valid
 */
IdentifiedItemUniversal::IdentifiedItemUniversal(const IdentifiedItemUniversalTypeEnum::Enum type,
                                                 const AString& simpleText,
                                                 const AString& formattedText,
                                                 const AString& dataFileName,
                                                 const StructureEnum::Enum structure,
                                                 const int32_t surfaceNumberOfVertices,
                                                 const int32_t surfaceVertexIndex,
                                                 const PixelIndex& pixelIndex,
                                                 const std::array<int64_t, 3>& voxelIJK,
                                                 const std::array<float, 3>& stereotaxicXYZ,
                                                 const bool stereotaxicXYZValidFlag)
: CaretObject(),
SceneableInterface(),
m_type(type),
m_simpleText(simpleText),
m_formattedText(formattedText),
m_dataFileName(FileInformation(dataFileName).getFileName()),
m_structure(structure),
m_contralateralStructure(StructureEnum::getContralateralStructure(structure)),
m_surfaceNumberOfVertices(surfaceNumberOfVertices),
m_surfaceVertexIndex(surfaceVertexIndex),
m_pixelIndex(pixelIndex),
m_voxelIJK(voxelIJK),
m_stereotaxicXYZ(stereotaxicXYZ),
m_stereotaxicXYZValidFlag(stereotaxicXYZValidFlag)
{
    initializeInstance();
}

/**
 * @return An invalid instance.  Typically used when restoring a scene.
 */
IdentifiedItemUniversal*
IdentifiedItemUniversal::newInstanceInvalidIdentification()
{
    return new IdentifiedItemUniversal();
}

/**
 * @return New instance for a pixel identification.
 *
 * @param simpleText
 *    Text describing the identified item.
 * @param formattedText
 *    Formatted text describing the identified item.
 */
IdentifiedItemUniversal*
IdentifiedItemUniversal::newInstanceTextNoSymbolIdentification(const AString& simpleText,
                                                               const AString& formattedText)
{
    AString dataFileName;
    PixelIndex pixelIndex;
    const std::array<float, 3> stereotaxicXYZ { 0.0f, 0.0f, 0.0f };
    const bool stereotaxicXYZValidFlag(false);
    const StructureEnum::Enum structure(StructureEnum::INVALID);
    const int32_t surfaceNumberOfVertices(-1);
    const int32_t surfaceVertexIndex(-1);
    std::array<int64_t, 3> voxelIJK;
    voxelIJK.fill(0);
    IdentifiedItemUniversal* item = new IdentifiedItemUniversal(IdentifiedItemUniversalTypeEnum::TEXT_NO_SYMBOL,
                                                                simpleText,
                                                                formattedText,
                                                                dataFileName,
                                                                structure,
                                                                surfaceNumberOfVertices,
                                                                surfaceVertexIndex,
                                                                pixelIndex,
                                                                voxelIJK,
                                                                stereotaxicXYZ,
                                                                stereotaxicXYZValidFlag);
    return item;
}


/**
 * @return New instance for a pixel identification.
 *
 * @param simpleText
 *    Text describing the identified item.
 * @param formattedText
 *    Formatted text describing the identified item.
 * @param dataFileName
 *    Name of data file on which identification was performed
 * @param pixelIndex
 *    The pixel index
 * @param stereotaxicXYZ
 *    Stereotaxic coordinate of vertex
 * @param stereotaxicXYZValidFlag
 *    True if the stereotaxic coordinate is valid
 *
 */
IdentifiedItemUniversal*
IdentifiedItemUniversal::newInstanceMediaIdentification(const AString& simpleText,
                                                        const AString& formattedText,
                                                        const AString& dataFileName,
                                                        const PixelIndex& pixelIndex,
                                                        const std::array<float, 3>& stereotaxicXYZ,
                                                        const bool stereotaxicXYZValidFlag)
{
    const StructureEnum::Enum structure(StructureEnum::INVALID);
    const int32_t surfaceNumberOfVertices(-1);
    const int32_t surfaceVertexIndex(-1);
    std::array<int64_t, 3> voxelIJK;
    voxelIJK.fill(0);
    IdentifiedItemUniversal* item = new IdentifiedItemUniversal(IdentifiedItemUniversalTypeEnum::MEDIA,
                                                                simpleText,
                                                                formattedText,
                                                                dataFileName,
                                                                structure,
                                                                surfaceNumberOfVertices,
                                                                surfaceVertexIndex,
                                                                pixelIndex,
                                                                voxelIJK,
                                                                stereotaxicXYZ,
                                                                stereotaxicXYZValidFlag);
    return item;
}

/**
 * @return New instance for a surface identification.
 *
 * @param simpleText
 *    Text describing the identified item.
 * @param formattedText
 *    Formatted text describing the identified item.
 * @param dataFileName
 *    Name of data file on which identification was performed
 * @param structure
 *    Structure on which identification took place.
 * @param surfaceNumberOfVertices
 *    Number of vertices in surface.
 * @param surfaceVertexIndex
 *    Index of vertex that was identified.
 * @param stereotaxicXYZ
 *    Stereotaxic coordinate of vertex
 *
 */
IdentifiedItemUniversal*
IdentifiedItemUniversal::newInstanceSurfaceIdentification(const AString& simpleText,
                                                          const AString& formattedText,
                                                          const AString& dataFileName,
                                                          const StructureEnum::Enum structure,
                                                          const int32_t surfaceNumberOfVertices,
                                                          const int32_t surfaceVertexIndex,
                                                          const std::array<float, 3>& stereotaxicXYZ)
{
    std::array<int64_t, 3> voxelIJK;
    voxelIJK.fill(0);
    const bool stereotaxicXYZValidFlag(true);
    IdentifiedItemUniversal* item = new IdentifiedItemUniversal(IdentifiedItemUniversalTypeEnum::SURFACE,
                                                                simpleText,
                                                                formattedText,
                                                                dataFileName,
                                                                structure,
                                                                surfaceNumberOfVertices,
                                                                surfaceVertexIndex,
                                                                PixelIndex(),
                                                                voxelIJK,
                                                                stereotaxicXYZ,
                                                                stereotaxicXYZValidFlag);
    return item;
}

/**
 * @return New instance for a volume identification.
 *
 * @param simpleText
 *    Text describing the identified item.
 * @param formattedText
 *    Formatted text describing the identified item.
 * @param dataFileName
 *    Name of data file on which identification was performed
 * @param voxelIJK
 *    The voxel IJK indices
 * @param stereotaxicXYZ
 *    Stereotaxic coordinate of vertex
 *
 */
IdentifiedItemUniversal*
IdentifiedItemUniversal::newInstanceVolumeIdentification(const AString& simpleText,
                                                         const AString& formattedText,
                                                         const AString& dataFileName,
                                                         const std::array<int64_t, 3>& voxelIJK,
                                                         const std::array<float, 3>& stereotaxicXYZ)
{
    const StructureEnum::Enum structure(StructureEnum::INVALID);
    const int32_t surfaceNumberOfVertices(-1);
    const int32_t surfaceVertexIndex(-1);
    const bool stereotaxicXYZValidFlag(true);
    IdentifiedItemUniversal* item = new IdentifiedItemUniversal(IdentifiedItemUniversalTypeEnum::VOLUME,
                                                                simpleText,
                                                                formattedText,
                                                                dataFileName,
                                                                structure,
                                                                surfaceNumberOfVertices,
                                                                surfaceVertexIndex,
                                                                PixelIndex(),
                                                                voxelIJK,
                                                                stereotaxicXYZ,
                                                                stereotaxicXYZValidFlag);
    return item;
}

/**
 * @return New instance from an older identification item (NULL if unable to convert)
 * @param oldItem
 *    The old identified item
 */
IdentifiedItemUniversal*
IdentifiedItemUniversal::newInstanceFromOldIdentification(const IdentifiedItemBase* oldItem)
{
    CaretAssert(oldItem);
    
    IdentifiedItemUniversal* item(NULL);
    
    const IdentifiedItemNode* vertexID(dynamic_cast<const IdentifiedItemNode*>(oldItem));
    const IdentifiedItemVoxel* voxelID(dynamic_cast<const IdentifiedItemVoxel*>(oldItem));
    
    AString dataFileName;
    if (vertexID != NULL) {
        const std::array<float, 3> stereotaxicXYZ { 0.0f, 0.0f, 0.0f };
        item = newInstanceSurfaceIdentification(vertexID->getSimpleText(),
                                                vertexID->getFormattedText(),
                                                dataFileName,
                                                vertexID->getStructure(),
                                                vertexID->getSurfaceNumberOfNodes(),
                                                vertexID->getNodeIndex(),
                                                stereotaxicXYZ);
        item->m_stereotaxicXYZValidFlag = false;
    }
    else if (voxelID != NULL) {
        std::array<float, 3> xyz;
        voxelID->getXYZ(xyz.data());
        std::array<int64_t, 3> voxelIJK = { -1, -1, -1 };
        item = newInstanceVolumeIdentification(voxelID->getSimpleText(),
                                               voxelID->getFormattedText(),
                                               dataFileName,
                                               voxelIJK,
                                               xyz);
    }
    else {
        item = newInstanceTextNoSymbolIdentification(oldItem->getSimpleText(),
                                                     oldItem->getFormattedText());
    }
    
    CaretAssert(item);

    
    return item;
}

/**
 * Destructor.
 */
IdentifiedItemUniversal::~IdentifiedItemUniversal()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
IdentifiedItemUniversal::IdentifiedItemUniversal(const IdentifiedItemUniversal& obj)
: CaretObject(obj),
SceneableInterface(obj),
m_type(obj.m_type)
{
    initializeInstance();
    this->copyHelperIdentifiedItemUniversal(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
IdentifiedItemUniversal&
IdentifiedItemUniversal::operator=(const IdentifiedItemUniversal& obj)
{
    if (this != &obj) {
        this->copyHelperIdentifiedItemUniversal(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
IdentifiedItemUniversal::copyHelperIdentifiedItemUniversal(const IdentifiedItemUniversal& obj)
{
    m_type = obj.m_type;
    m_simpleText = obj.m_simpleText;
    m_formattedText = obj.m_formattedText;
    m_dataFileName = obj.m_dataFileName;
    m_structure = obj.m_structure;
    m_contralateralStructure = obj.m_contralateralStructure;
    m_surfaceNumberOfVertices = obj.m_surfaceNumberOfVertices;
    m_surfaceVertexIndex = obj.m_surfaceVertexIndex;
    m_pixelIndex = obj.m_pixelIndex;
    m_voxelIJK = obj.m_voxelIJK;
    m_stereotaxicXYZ = obj.m_stereotaxicXYZ;
    m_stereotaxicXYZValidFlag = obj.m_stereotaxicXYZValidFlag;
    m_symbolColor = obj.m_symbolColor;
    m_contralateralSymbolColor = obj.m_contralateralSymbolColor;
    m_symbolSizeType = obj.m_symbolSizeType;
    m_symbolSize = obj.m_symbolSize;
}

/**
 * @return Type of universal item in this instance
 */
IdentifiedItemUniversalTypeEnum::Enum
IdentifiedItemUniversal::getType() const
{
    return m_type;
}

/**
 * Change the type of the item to text with no symbol
 */
void
IdentifiedItemUniversal::setTypeToTextNoSymbol()
{
    m_type = IdentifiedItemUniversalTypeEnum::TEXT_NO_SYMBOL;
}

/**
 * Clear all simple and formatted text
 */
void
IdentifiedItemUniversal::clearAllText()
{
    m_simpleText.clear();
    m_formattedText.clear();
}

/**
 * @return The simple text describing the identified item.
 */
AString
IdentifiedItemUniversal::getSimpleText() const
{
    return m_simpleText;
}

/**
 * @return The formatted text describing the identified item.
 */
AString
IdentifiedItemUniversal::getFormattedText() const
{
    return m_formattedText;
}

/**
 * Initialize the scene assistant
 */
void
IdentifiedItemUniversal::initializeInstance()
{
    /*
     * Note: Most items are initialized in the header file
     */
    
    /*
     * Generate the unique ID for this instance.
     * The unique identified is NOT saved to scenes.
     */
    m_uniqueIdentifier = ++s_uniqueIdentifierGenerator;
    
    /*
     * Add items to scene assistant
     */
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add<IdentifiedItemUniversalTypeEnum, IdentifiedItemUniversalTypeEnum::Enum>("m_type",
                                                                                                  &m_type);
    m_sceneAssistant->add("m_simpleText", &m_simpleText);
    m_sceneAssistant->add("m_formattedText", &m_formattedText);
    m_sceneAssistant->add("m_dataFileName", &m_dataFileName);
    m_sceneAssistant->add<StructureEnum>("m_structure", &m_structure);
    m_sceneAssistant->add<StructureEnum>("m_contralateralStructure", &m_contralateralStructure);
    m_sceneAssistant->add("m_surfaceNumberOfVertices", &m_surfaceNumberOfVertices);
    m_sceneAssistant->add("m_surfaceVertexIndex", &m_surfaceVertexIndex);
    m_sceneAssistant->add("m_pixelIndex", "PixelIndex", &m_pixelIndex);
    m_sceneAssistant->addArray("m_voxelIJK", m_voxelIJK.data(), m_voxelIJK.size(), -1);
    m_sceneAssistant->addArray("m_stereotaxicXYZ", m_stereotaxicXYZ.data(), m_stereotaxicXYZ.size(), 0.0);
    m_sceneAssistant->add("m_stereotaxicXYZValidFlag", &m_stereotaxicXYZValidFlag);
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_symbolColor", &m_symbolColor);
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_contralateralSymbolColor", &m_contralateralSymbolColor);
    m_sceneAssistant->add("m_symbolSize", &m_symbolSize);
    m_sceneAssistant->add<IdentificationSymbolSizeTypeEnum, IdentificationSymbolSizeTypeEnum::Enum>("m_symbolSizeType",
                                                                                                    &m_symbolSizeType);
}

/**
 * @return The tooltip for this instance
 */
AString
IdentifiedItemUniversal::getToolTip() const
{
    const AString xyzText(m_stereotaxicXYZValidFlag
                          ? ("<br>XYZ ("
                             + AString::fromNumbers(m_stereotaxicXYZ.data(), 3, ", ")
                             + ")")
                          : "");
    
    AString toolTipText;
    switch (m_type) {
        case IdentifiedItemUniversalTypeEnum::INVALID:
            break;
        case IdentifiedItemUniversalTypeEnum::MEDIA:
        {
            toolTipText = ("Media File: "
                           + m_dataFileName + "<br>"
                           "Pixel ("
                           + m_pixelIndex.toString()
                           + ")"
                           + xyzText);
        }
            break;
        case IdentifiedItemUniversalTypeEnum::SURFACE:
            toolTipText = ("Surface: "
                           + m_dataFileName + "<br>"
                           "Vertex: "
                           + AString::number(m_surfaceVertexIndex)
                           + ")"
                           + xyzText);
            break;
        case IdentifiedItemUniversalTypeEnum::TEXT_NO_SYMBOL:
            break;
        case IdentifiedItemUniversalTypeEnum::VOLUME:
            toolTipText = ("Volume: "
                           + m_dataFileName + "<br>"
                           "Index ("
                           + AString::fromNumbers(m_voxelIJK.data(), 3, ", ")
                           + ")"
                           + xyzText);
            break;
    }
    
    return toolTipText;
}


/**
 * @return Is this item valid?  Typically only used when restoring
 * from scene.
 */
bool
IdentifiedItemUniversal::isValid() const
{
    switch (m_type) {
        case IdentifiedItemUniversalTypeEnum::INVALID:
            CaretAssert(0);
            break;
        case IdentifiedItemUniversalTypeEnum::MEDIA:
            if (m_pixelIndex.isValid()) {
                return true;
            }
            break;
        case IdentifiedItemUniversalTypeEnum::SURFACE:
            if ((m_structure != StructureEnum::INVALID)
                && (m_surfaceVertexIndex >= 0)
                && (m_surfaceNumberOfVertices >= 0)) {
                return true;
            }
            break;
        case IdentifiedItemUniversalTypeEnum::TEXT_NO_SYMBOL:
            return true;
            break;
        case IdentifiedItemUniversalTypeEnum::VOLUME:
            if ((m_voxelIJK[0] >= 0)
                && (m_voxelIJK[1] >= 0)
                && (m_voxelIJK[2] >= 0)) {
                return true;
            }
            break;
    }
    
    return false;
}

/**
 * Append text to this item's text.
 * @param simpleText
 *    Text describing the identified item.
 * @param formattedText
 *    Formatted text describing the identified item.
 */
void
IdentifiedItemUniversal::appendText(const AString& simpleText,
                               const AString& formattedText)
{
    m_simpleText += simpleText;
    if ( ! m_formattedText.isEmpty()) {
        m_formattedText.append("\n");
    }
    m_formattedText += formattedText;
}

/**
 * @return The data file name
 */
AString
IdentifiedItemUniversal::getDataFileName() const
{
    return m_dataFileName;
}

/**
 * @return The structure for the identified vertex.
 */
StructureEnum::Enum
IdentifiedItemUniversal::getStructure() const
{
    return m_structure;
}

/**
 * @return The contralateral structure of the identified vertex.
 */
StructureEnum::Enum
IdentifiedItemUniversal::getContralateralStructure() const
{
    return m_contralateralStructure;
}

/**
 * Set the contralateral structure.
 * @param contralateralStructure
 *   The contralateral structure.
 */
void
IdentifiedItemUniversal::setContralateralStructure(const StructureEnum::Enum contralateralStructure)
{
    m_contralateralStructure = contralateralStructure;
}

/**
 * @return The number of vertices in the surface on which identification took place.
 */
int32_t
IdentifiedItemUniversal::getSurfaceNumberOfVertices() const
{
    return m_surfaceNumberOfVertices;
}

/**
 * @return The index of the surface vertex that was identified.
 */
int32_t
IdentifiedItemUniversal::getSurfaceVertexIndex() const
{
    return m_surfaceVertexIndex;
}

/**
 * @return The unique identifier
 */
int64_t
IdentifiedItemUniversal::getUniqueIdentifier() const
{
    CaretAssertMessage((m_uniqueIdentifier > 0), "Failed to initialize the unique identifier for this instance");
    return m_uniqueIdentifier;
}

/**
 * Reset the unique identifier generator
 */
void
IdentifiedItemUniversal::resetUniqueIdentifierGenerator()
{
    s_uniqueIdentifierGenerator = 0;
}

/**
 * @return The pixel index.
 */
PixelIndex
IdentifiedItemUniversal::getPixelIndex() const
{
    return m_pixelIndex;
}

/**
 * @return The voxel IJK indices
 */
std::array<int64_t, 3>
IdentifiedItemUniversal::getVoxelIJK() const
{
    return m_voxelIJK;
}

/**
 * @return The stereotaxic coordinate
 */
std::array<float, 3>
IdentifiedItemUniversal::getStereotaxicXYZ() const
{
    return m_stereotaxicXYZ;
}

/**
 * Set the stereotaxic coordinate
 * @param xyz
 *    new XYZ coordinate
 */
void
IdentifiedItemUniversal::setStereotaxicXYZ(const std::array<float, 3>& xyz)
{
    m_stereotaxicXYZ = xyz;
}

/**
 * @return Is the stereotaxic coordinate valid?
 */
bool
IdentifiedItemUniversal::isStereotaxicXYZValid() const
{
    return m_stereotaxicXYZValidFlag;
}

/**
 * @return True if this identified time is from a scene before this class was created
 */
bool
IdentifiedItemUniversal::isOldIdentification() const
{
    return m_oldIdentificationFlag;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
IdentifiedItemUniversal::toString() const
{
    const AString s = (", m_type=" + IdentifiedItemUniversalTypeEnum::toName(m_type)
                       + ", m_simpleText=" + m_simpleText
                       + ", m_formattedText=" + m_formattedText
                       + ", m_dataFileName" + m_dataFileName
                       + ", m_structure=" + StructureEnum::toName(m_structure)
                       + ", m_contralateralStructure=" + StructureEnum::toName(m_contralateralStructure)
                       + ", m_surfaceNumberOfVertices=" + AString::number(m_surfaceNumberOfVertices)
                       + ", m_surfaceVertexIndex=" + AString::number(m_surfaceVertexIndex)
                       + ", m_pixelIndex=" + m_pixelIndex.toString()
                       + ", m_voxelIJK=" + AString::fromNumbers(m_voxelIJK.data(), m_voxelIJK.size(), ", ")
                       + ", m_stereotaxicXYZ=" + AString::fromNumbers(m_stereotaxicXYZ.data(), m_stereotaxicXYZ.size(), ", ")
                       + ", m_stereotaxicXYZValidFlag=" + AString::fromBool(m_stereotaxicXYZValidFlag)
                       + ", m_symbolColor=" + CaretColorEnum::toName(m_symbolColor)
                       + ", m_contralateralSymbolColor=" + CaretColorEnum::toName(m_contralateralSymbolColor)
                       + ", m_symbolSize=" + AString::number(m_symbolSize)
                       + ", m_symbolSizeType=" + IdentificationSymbolSizeTypeEnum::toName(m_symbolSizeType)
                       + ", m_oldIdentificationFlag=" + AString::fromBool(m_oldIdentificationFlag)
                       + ", m_uniqueIdentifier=" + AString::number(m_uniqueIdentifier));
    return s;
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
IdentifiedItemUniversal::saveToScene(const SceneAttributes* sceneAttributes,
                            const AString& instanceName)
{
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "IdentifiedItemUniversal",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes, sceneClass);
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void
IdentifiedItemUniversal::restoreFromScene(const SceneAttributes* sceneAttributes,
                                 const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

