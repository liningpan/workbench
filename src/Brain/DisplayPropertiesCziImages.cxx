
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

#define __DISPLAY_PROPERTIES_CZI_IMAGES_DECLARE__
#include "DisplayPropertiesCziImages.h"
#undef __DISPLAY_PROPERTIES_CZI_IMAGES_DECLARE__

#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;
    
/**
 * \class DisplayPropertiesCziImages
 * \brief Display properties for volume slices.
 *
 * Display properties for volume slices.
 */


/**
 * Constructor.
 */
DisplayPropertiesCziImages::DisplayPropertiesCziImages()
: DisplayProperties()
{
    m_resolutionChangeMode.fill(CziImageResolutionChangeModeEnum::AUTO);
    m_sceneAssistant->addArray<CziImageResolutionChangeModeEnum,CziImageResolutionChangeModeEnum::Enum>("m_resolutionChangeMode",
                                                                                                        m_resolutionChangeMode.data(),
                                                                                                        m_resolutionChangeMode.size(),
                                                                                                        CziImageResolutionChangeModeEnum::MANUAL);
}

/**
 * Destructor.
 */
DisplayPropertiesCziImages::~DisplayPropertiesCziImages()
{
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void 
DisplayPropertiesCziImages::reset()
{
    m_resolutionChangeMode.fill(CziImageResolutionChangeModeEnum::MANUAL);
}

/**
 * Update due to changes in data.
 */
void 
DisplayPropertiesCziImages::update()
{
    
}


/**
 * Copy the display properties from one tab to another.
 * @param sourceTabIndex
 *    Index of tab from which properties are copied.
 * @param targetTabIndex
 *    Index of tab to which properties are copied.
 */
void 
DisplayPropertiesCziImages::copyDisplayProperties(const int32_t /*sourceTabIndex*/,
                                                    const int32_t /*targetTabIndex*/)
{
}

/**
 * @return Resolution change mode for the given tab
 * @param tabIndex
 *    Index of the tab.
 */
CziImageResolutionChangeModeEnum::Enum
DisplayPropertiesCziImages::getResolutionChangeMode(const int32_t tabIndex) const
{
    CaretAssertVectorIndex(m_resolutionChangeMode, tabIndex);
    return m_resolutionChangeMode[tabIndex];
}

/**
 * Set the overall surface opacity.
 *
 * @param opacity
 *    New value for opacity.
 */
void
DisplayPropertiesCziImages::setResolutionChangeMode(const int32_t tabIndex,
                             const CziImageResolutionChangeModeEnum::Enum changeMode)
{
    CaretAssertVectorIndex(m_resolutionChangeMode, tabIndex);
    m_resolutionChangeMode[tabIndex] = changeMode;
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
DisplayPropertiesCziImages::saveToScene(const SceneAttributes* sceneAttributes,
                   const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DisplayPropertiesCziImages",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
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
 *     SceneClass containing the state that was previously 
 *     saved and should be restored.
 */
void 
DisplayPropertiesCziImages::restoreFromScene(const SceneAttributes* sceneAttributes,
                        const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
}
