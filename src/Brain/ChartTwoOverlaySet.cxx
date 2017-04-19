
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __CHART_TWO_OVERLAY_SET_DECLARE__
#include "ChartTwoOverlaySet.h"
#undef __CHART_TWO_OVERLAY_SET_DECLARE__

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "ChartTwoCartesianAxis.h"
#include "ChartTwoOverlay.h"
#include "EventManager.h"
#include "EventMapYokingSelectMap.h"
#include "EventMapYokingValidation.h"
#include "PlainTextStringBuilder.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "SceneObjectMapIntegerKey.h"


using namespace caret;


    
/**
 * \class caret::ChartTwoOverlaySet 
 * \brief A set of chart overlays.
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param chartDataType
 *     Type of charts allowed in this overlay
 * @param name
 *     Name of the overlay set.
 * @param tabIndex
 *     Index of tab in which this overlay set is used.
 */
ChartTwoOverlaySet::ChartTwoOverlaySet(const ChartTwoDataTypeEnum::Enum chartDataType,
                                 const AString& name,
                                 const int32_t tabIndex)
: CaretObject(),
m_chartDataType(chartDataType),
m_name(name),
m_tabIndex(tabIndex)
{
    m_numberOfDisplayedOverlays = BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS;
    
    m_chartAxisLeft   = std::unique_ptr<ChartTwoCartesianAxis>(new ChartTwoCartesianAxis(ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT));
    m_chartAxisRight  = std::unique_ptr<ChartTwoCartesianAxis>(new ChartTwoCartesianAxis(ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT));
    m_chartAxisBottom = std::unique_ptr<ChartTwoCartesianAxis>(new ChartTwoCartesianAxis(ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM));
    
    m_chartAxisLeft->setVisible(false);
    m_chartAxisRight->setVisible(false);
    m_chartAxisBottom->setVisible(false);
    switch (m_chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
        {
            m_chartAxisLeft->setVisible(true);
            m_chartAxisLeft->setAxisTitle("Counts");
            m_chartAxisLeft->setUnits(ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
            float rangeMin = 0.0, rangeMax = 0.0;
            m_chartAxisLeft->getRange(rangeMin, rangeMax);
            rangeMin = 0.0;
            m_chartAxisLeft->setRange(rangeMin, rangeMax);
            
            m_chartAxisRight->setVisible(false);
            m_chartAxisRight->setAxisTitle("Counts");
            m_chartAxisRight->setUnits(ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
            m_chartAxisRight->getRange(rangeMin, rangeMax);
            rangeMin = 0.0;
            m_chartAxisRight->setRange(rangeMin, rangeMax);
            
            m_chartAxisBottom->setVisible(true);
            m_chartAxisBottom->setAxisTitle("Data");
            m_chartAxisBottom->setUnits(ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
        }
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            m_chartAxisLeft->setVisible(true);
            m_chartAxisBottom->setVisible(true);
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            break;
    }
    
    m_sceneAssistant  = new SceneClassAssistant();
    m_sceneAssistant->add("m_chartAxisLeft",
                          "ChartTwoCartesianAxis",
                          m_chartAxisLeft.get());
    m_sceneAssistant->add("m_chartAxisRight",
                          "ChartTwoCartesianAxis",
                          m_chartAxisRight.get());
    m_sceneAssistant->add("m_chartAxisBottom",
                          "ChartTwoCartesianAxis",
                          m_chartAxisBottom.get());
    m_sceneAssistant->add("m_numberOfDisplayedOverlays",
                          &m_numberOfDisplayedOverlays);
    
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        m_overlays[i] = new ChartTwoOverlay(this,
                                         m_chartDataType,
                                         i);
    }
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_VALIDATION);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP);
}

/**
 * Destructor.
 */
ChartTwoOverlaySet::~ChartTwoOverlaySet()
{
    EventManager::get()->removeAllEventsFromListener(this);
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        delete m_overlays[i];
    }
    delete m_sceneAssistant;
}

/**
 * Copy the given overlay set to this overlay set.
 * @param overlaySet
 *    Overlay set that is copied.
 */
void
ChartTwoOverlaySet::copyOverlaySet(const ChartTwoOverlaySet* overlaySet)
{
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, i);
        m_overlays[i]->copyData(overlaySet->getOverlay(i));
    }
    *m_chartAxisLeft   = *overlaySet->m_chartAxisLeft;
    *m_chartAxisRight  = *overlaySet->m_chartAxisRight;
    *m_chartAxisBottom = *overlaySet->m_chartAxisBottom;
    
    m_numberOfDisplayedOverlays = overlaySet->m_numberOfDisplayedOverlays;
}

/**
 * @return Returns the top-most overlay regardless of its enabled status.
 */
ChartTwoOverlay*
ChartTwoOverlaySet::getPrimaryOverlay()
{
    return m_overlays[0];
}

/**
 * Get the overlay at the specified index.
 * @param overlayNumber
 *   Index of the overlay.
 * @return Overlay at the given index.
 */
const ChartTwoOverlay*
ChartTwoOverlaySet::getOverlay(const int32_t overlayNumber) const
{
    CaretAssertArrayIndex(m_overlays,
                          BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS,
                          overlayNumber);
    return m_overlays[overlayNumber];
}

/**
 * Get the overlay at the specified index.
 * @param overlayNumber
 *   Index of the overlay.
 * @return Overlay at the given index.
 */
ChartTwoOverlay*
ChartTwoOverlaySet::getOverlay(const int32_t overlayNumber)
{
    CaretAssertArrayIndex(m_overlays,
                          BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS,
                          overlayNumber);
    return m_overlays[overlayNumber];
}

/**
 * Get a text description of the window's content.
 *
 * @param descriptionOut
 *    Description of the window's content.
 */
void
ChartTwoOverlaySet::getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const
{
    descriptionOut.addLine("Overlay Set");
    
    const int numOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numOverlays; i++) {
        if (getOverlay(i)->isEnabled()) {
            descriptionOut.pushIndentation();
            
            descriptionOut.pushIndentation();
            getOverlay(i)->getDescriptionOfContent(descriptionOut);
            descriptionOut.popIndentation();
            
            descriptionOut.popIndentation();
        }
    }
}

/**
 * Find the top-most displayed and enabled overlay containing the given data file.
 *
 * @param mapFile
 *     File for which overlay is requested.
 */
ChartTwoOverlay*
ChartTwoOverlaySet::getDisplayedOverlayContainingDataFile(const CaretMappableDataFile* mapFile)
{
    if (mapFile == NULL) {
        return NULL;
    }
    
    ChartTwoOverlay* overlayOut = NULL;
    
    const int numOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numOverlays; i++) {
        ChartTwoOverlay* overlay = getOverlay(i);
        if (overlay->isEnabled()) {
            CaretMappableDataFile* overlayMapFile = NULL;
            ChartTwoOverlay::SelectedIndexType selectionType = ChartTwoOverlay::SelectedIndexType::INVALID;
            int32_t dataFileIndex = -1;
            overlay->getSelectionData(overlayMapFile,
                                            selectionType,
                                            dataFileIndex);
            if (mapFile == overlayMapFile) {
                overlayOut = overlay;
            }
        }
    }
    
    return overlayOut;
}

/**
 * Add a displayed overlay.  If the maximum
 * number of surface overlays is reached,
 * this method has no effect.
 */
void
ChartTwoOverlaySet::addDisplayedOverlay()
{
    m_numberOfDisplayedOverlays++;
    if (m_numberOfDisplayedOverlays > BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays = BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS;
    }
}

/**
 * @return Returns the number of displayed overlays.
 */
int32_t
ChartTwoOverlaySet::getNumberOfDisplayedOverlays() const
{
    return m_numberOfDisplayedOverlays;
}

/**
 * Sets the number of displayed overlays.
 * @param numberOfDisplayedOverlays
 *   Number of overlays for display.
 */
void
ChartTwoOverlaySet::setNumberOfDisplayedOverlays(const int32_t numberOfDisplayedOverlays)
{
    const int32_t oldNumberOfDisplayedOverlays = m_numberOfDisplayedOverlays;
    m_numberOfDisplayedOverlays = numberOfDisplayedOverlays;
    if (m_numberOfDisplayedOverlays < BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays = BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS;
    }
    if (m_numberOfDisplayedOverlays > BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays = BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS;
    }
    
    /*
     * If one overlay added (probably through GUI),
     * shift all overlays down one position so that
     * new overlay appears at the top
     */
    const int32_t numberOfOverlaysAdded = m_numberOfDisplayedOverlays - oldNumberOfDisplayedOverlays;
    if (numberOfOverlaysAdded == 1) {
        for (int32_t i = (m_numberOfDisplayedOverlays - 1); i >= 0; i--) {
            moveDisplayedOverlayDown(i);
        }
    }
}

/**
 * Insert an overlay below this overlay
 * @param overlayIndex
 *     Index of overlay for which an overlay is added below
 */
void
ChartTwoOverlaySet::insertOverlayAbove(const int32_t overlayIndex)
{
    if (m_numberOfDisplayedOverlays < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays++;
        
        for (int32_t i = (m_numberOfDisplayedOverlays - 2); i >= overlayIndex; i--) {
            moveDisplayedOverlayDown(i);
        }
    }
}

/**
 * Insert an overlay above this overlay
 * @param overlayIndex
 *     Index of overlay for which an overlay is added above
 */
void
ChartTwoOverlaySet::insertOverlayBelow(const int32_t overlayIndex)
{
    if (m_numberOfDisplayedOverlays < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays++;
        
        for (int32_t i = (m_numberOfDisplayedOverlays - 2); i > overlayIndex; i--) {
            moveDisplayedOverlayDown(i);
        }
    }
}


/**
 * Remove a displayed overlay.  This method will have
 * no effect if the minimum number of overlays are
 * displayed
 *
 * @param overlayIndex
 *    Index of overlay for removal from display.
 */
void
ChartTwoOverlaySet::removeDisplayedOverlay(const int32_t overlayIndex)
{
    CaretAssertArrayIndex(m_overlays,
                          BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS,
                          overlayIndex);
    m_overlays[overlayIndex]->setMapYokingGroup(MapYokingGroupEnum::MAP_YOKING_GROUP_OFF);
    
    if (m_numberOfDisplayedOverlays > BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays--;
        for (int32_t i = overlayIndex; i < m_numberOfDisplayedOverlays; i++) {
            CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, i+1);
            m_overlays[i]->copyData(m_overlays[i+1]);
        }
    }
}

/**
 * Move the overlay at the given index up one level
 * (swap it with overlayIndex - 1).  This method will
 * have no effect if the overlay is the top-most overlay.
 *
 * @param overlayIndex
 *    Index of overlay that is to be moved up.
 */
void
ChartTwoOverlaySet::moveDisplayedOverlayUp(const int32_t overlayIndex)
{
    if (overlayIndex > 0) {
        CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, overlayIndex);
        m_overlays[overlayIndex]->swapData(m_overlays[overlayIndex - 1]);
    }
}

/**
 * Move the overlay at the given index down one level
 * (swap it with overlayIndex + 1).  This method will
 * have no effect if the overlay is the bottom-most overlay.
 *
 * @param overlayIndex
 *    Index of overlay that is to be moved down.
 */
void
ChartTwoOverlaySet::moveDisplayedOverlayDown(const int32_t overlayIndex)
{
    const int32_t nextOverlayIndex = overlayIndex + 1;
    if (nextOverlayIndex < m_numberOfDisplayedOverlays) {
        CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, nextOverlayIndex);
        m_overlays[overlayIndex]->swapData(m_overlays[nextOverlayIndex]);
    }
}

/**
 * Initialize the overlays.
 */
void
ChartTwoOverlaySet::initializeOverlays()
{
    /*
     * This method could be used to choose specific file types
     * for the default overlays similar to that in OverlaySet.cxx.
     */
}

/**
 * Called by first overlay when the first overlay's selection changes.
 * All other overlays are set to use the same chart compound data type
 * so that the charts in the tab are compatible
 */
void
ChartTwoOverlaySet::firstOverlaySelectionChanged()
{
    if (m_inFirstOverlayChangedMethodFlag) {
        return;
    }
    
    m_inFirstOverlayChangedMethodFlag = true;
    
    ChartTwoCompoundDataType cdt = m_overlays[0]->getChartTwoCompoundDataType();
    
    for (int32_t i = 1; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, i);
        m_overlays[i]->setChartTwoCompoundDataType(cdt);
    }
    
    PlainTextStringBuilder description;
    getDescriptionOfContent(description);
    
    m_inFirstOverlayChangedMethodFlag = false;
}


/**
 * Reset the yoking status of all overlays to off.
 */
void
ChartTwoOverlaySet::resetOverlayYokingToOff()
{
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, i);
        m_overlays[i]->setMapYokingGroup(MapYokingGroupEnum::MAP_YOKING_GROUP_OFF);
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartTwoOverlaySet::toString() const
{
    return "ChartTwoOverlaySet";
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ChartTwoOverlaySet::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_MAP_YOKING_VALIDATION) {
        /*
         * The events intended for overlays are received here so that
         * only DISPLAYED overlays are updated.
         */
        EventMapYokingValidation* mapYokeEvent = dynamic_cast<EventMapYokingValidation*>(event);
        CaretAssert(mapYokeEvent);
        
        const MapYokingGroupEnum::Enum requestedYokingGroup = mapYokeEvent->getMapYokingGroup();
        if (requestedYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
            
            /*
             * Find all overlays with the requested yoking
             */
            const int32_t overlayCount = getNumberOfDisplayedOverlays();
            for (int32_t j = 0; j < overlayCount; j++) {
                ChartTwoOverlay* overlay = getOverlay(j);
                if (overlay->isMapYokingSupported()) {
                    CaretMappableDataFile* mapFile = NULL;
                    ChartTwoOverlay::SelectedIndexType indexType = ChartTwoOverlay::SelectedIndexType::INVALID;
                    int32_t mapIndex = -1;
                    overlay->getSelectionData(mapFile,
                                              indexType,
                                              mapIndex);
                    if (mapFile != NULL) {
                        if (overlay->isMapYokingSupported()) {
                            mapYokeEvent->addMapYokedFile(mapFile, overlay->getMapYokingGroup(), m_tabIndex);
                        }
                    }
                }
            }
        }
        
        mapYokeEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP) {
        /*
         * The events intended for overlays are received here so that
         * only DISPLAYED overlays are updated.
         */
        EventMapYokingSelectMap* selectMapEvent = dynamic_cast<EventMapYokingSelectMap*>(event);
        CaretAssert(selectMapEvent);
        const MapYokingGroupEnum::Enum eventYokingGroup = selectMapEvent->getMapYokingGroup();
        if (eventYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
            const int32_t yokingGroupMapIndex = MapYokingGroupEnum::getSelectedMapIndex(eventYokingGroup);
            const bool yokingGroupSelectedStatus = MapYokingGroupEnum::isEnabled(eventYokingGroup);
            const CaretMappableDataFile* eventMapFile = selectMapEvent->getCaretMappableDataFile();
            
            /*
             * Find all overlays with the requested yoking
             */
            const int32_t overlayCount = getNumberOfDisplayedOverlays();
            for (int32_t j = 0; j < overlayCount; j++) {
                ChartTwoOverlay* overlay = getOverlay(j);
                if (overlay->isMapYokingSupported()) {
                    if (overlay->getMapYokingGroup() == selectMapEvent->getMapYokingGroup()) {
                        CaretMappableDataFile* mapFile = NULL;
                        ChartTwoOverlay::SelectedIndexType indexType = ChartTwoOverlay::SelectedIndexType::INVALID;
                        int32_t mapIndex = -1;
                        overlay->getSelectionData(mapFile,
                                                  indexType,
                                                  mapIndex);
                        
                        if (mapFile != NULL) {
                            if (overlay->isMapYokingSupported()) {
                                if (yokingGroupMapIndex < mapFile->getNumberOfMaps()) {
                                    overlay->setSelectionData(mapFile,
                                                              yokingGroupMapIndex);
                                }
                                
                                if (mapFile == eventMapFile) {
                                    overlay->setEnabled(yokingGroupSelectedStatus);
                                }
                            }
                        }
                    }
                }
            }
            
            selectMapEvent->setEventProcessed();
        }
    }
}

/**
 * @return The chart left-axis.
 */
ChartTwoCartesianAxis*
ChartTwoOverlaySet::getChartAxisLeft()
{
    return m_chartAxisLeft.get();
}

/**
 * @return The chart left-axis (const method)
 */
const ChartTwoCartesianAxis*
ChartTwoOverlaySet::getChartAxisLeft() const
{
    return m_chartAxisLeft.get();
}

/**
 * @return The chart right-axis.
 */
ChartTwoCartesianAxis*
ChartTwoOverlaySet::getChartAxisRight()
{
    return m_chartAxisRight.get();
}

/**
 * @return The chart right-axis (const method)
 */
const ChartTwoCartesianAxis*
ChartTwoOverlaySet::getChartAxisRight() const
{
    return m_chartAxisRight.get();
}

/**
 * @return The chart bottom-axis.
 */
ChartTwoCartesianAxis*
ChartTwoOverlaySet::getChartAxisBottom()
{
    return m_chartAxisBottom.get();
}

std::vector<ChartTwoCartesianAxis*>
ChartTwoOverlaySet::getDisplayedChartAxes() const
{
    std::vector<ChartTwoCartesianAxis*> axes;
    
    bool showAxesFlag = false;
    switch (m_chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            showAxesFlag = true;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            showAxesFlag = true;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            break;
    }

    bool showBottomFlag = false;
    bool showLeftFlag   = false;
    bool showRightFlag  = false;
    
    if (showAxesFlag) {
        for (auto overlay : m_overlays) {
            if (overlay->isEnabled()) {
                CaretMappableDataFile* mapFile = NULL;
                ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
                int32_t selectedIndex = -1;
                overlay->getSelectionData(mapFile,
                                          selectedIndexType,
                                          selectedIndex);
                if (mapFile != NULL) {
                    showBottomFlag = true;
                    switch (overlay->getCartesianVerticalAxisLocation()) {
                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                            CaretAssert(0);
                            break;
                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                            showLeftFlag = true;
                            break;
                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                            showRightFlag = true;
                            break;
                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                            CaretAssert(0);
                            break;
                    }
                }
            }
        }
    }
    
    m_chartAxisBottom->setVisible(showBottomFlag);
    m_chartAxisLeft->setVisible(showLeftFlag);
    m_chartAxisRight->setVisible(showRightFlag);
    
    if (m_chartAxisBottom->isVisible()) {
        axes.push_back(m_chartAxisBottom.get());
    }
    if (m_chartAxisLeft->isVisible()) {
        axes.push_back(m_chartAxisLeft.get());
    }
    if (m_chartAxisRight->isVisible()) {
        axes.push_back(m_chartAxisRight.get());
    }
    
    return axes;
}


/**
 * @return The chart bottom-axis (const method)
 */
const ChartTwoCartesianAxis*
ChartTwoOverlaySet::getChartAxisBottom() const
{
    return m_chartAxisBottom.get();
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
ChartTwoOverlaySet::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartTwoOverlaySet",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    const int32_t numOverlaysToSave = getNumberOfDisplayedOverlays();
    
    std::vector<SceneClass*> overlayClassVector;
    for (int i = 0; i < numOverlaysToSave; i++) {
        overlayClassVector.push_back(m_overlays[i]->saveToScene(sceneAttributes, "m_overlays"));
    }
    
    SceneClassArray* overlayClassArray = new SceneClassArray("m_overlays",
                                                             overlayClassVector);
    sceneClass->addChild(overlayClassArray);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
ChartTwoOverlaySet::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    const SceneClassArray* overlayClassArray = sceneClass->getClassArray("m_overlays");
    if (overlayClassArray != NULL) {
        const int32_t numOverlays = std::min(overlayClassArray->getNumberOfArrayElements(),
                                             (int32_t)BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS);
        for (int32_t i = 0; i < numOverlays; i++) {
            CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, i);
            m_overlays[i]->restoreFromScene(sceneAttributes,
                                            overlayClassArray->getClassAtIndex(i));
        }
    }
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}
