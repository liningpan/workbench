
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __PREFERENCES_DEVELOP_OPTIONS_WIDGET_DECLARE__
#include "PreferencesDevelopOptionsWidget.h"
#undef __PREFERENCES_DEVELOP_OPTIONS_WIDGET_DECLARE__

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "Brain.h"
#include "BrainOpenGLMediaDrawing.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GraphicsTextureMagnificationFilterEnum.h"
#include "GraphicsTextureMinificationFilterEnum.h"
#include "GuiManager.h"
#include "PreferencesDialog.h"
#include "SessionManager.h"
#include "BrainOpenGLVolumeObliqueSliceDrawing.h"
#include "WuQTrueFalseComboBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::PreferencesDevelopOptionsWidget 
 * \brief Widget for Develop Preferences
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
PreferencesDevelopOptionsWidget::PreferencesDevelopOptionsWidget(QWidget* parent)
: QWidget(parent)
{
    std::vector<DeveloperFlagsEnum::Enum> developerFlags;
    DeveloperFlagsEnum::getAllEnums(developerFlags);
    
    QGridLayout* gridLayout = new QGridLayout();
    if ( ! developerFlags.empty()) {
        for (const auto flag : developerFlags) {
            if (DeveloperFlagsEnum::isCheckable(flag)) {
                if (flag == DeveloperFlagsEnum::DEVELOPER_FLAG_UNUSED) {
                    continue;
                }

                WuQTrueFalseComboBox* comboBox = new WuQTrueFalseComboBox("On",
                                                                          "Off",
                                                                          this);
                QObject::connect(comboBox, &WuQTrueFalseComboBox::statusChanged,
                                 [=](const bool status) { this->developerFlagSelected(flag,
                                                                                      status); } );
                const AString toolTip(WuQtUtilities::createWordWrappedToolTipText(DeveloperFlagsEnum::toToolTip(flag)));
                comboBox->getWidget()->setToolTip(toolTip);
                
                QLabel* label = PreferencesDialog::addWidgetToLayout(gridLayout,
                                                                     DeveloperFlagsEnum::toGuiName(flag),
                                                                     comboBox->getWidget());
                label->setToolTip(toolTip);

                m_developerFlagsMap.insert(std::make_pair(comboBox,
                                                          flag));
            }
        }
    }

    /*
     * Crop scene images
     */
    m_cropSceneImagesComboBox = new WuQTrueFalseComboBox("On",
                                                         "Off",
                                                         this);
    QObject::connect(m_cropSceneImagesComboBox, &WuQTrueFalseComboBox::statusChanged,
                     this, &PreferencesDevelopOptionsWidget::cropSceneImagesStatusChanged);
    const AString cropSceneToolTip("Disabling of image cropping may be useful when "
                                   "debugging scenes where the content occupies a "
                                   "portion of the viewing region (data may be "
                                   "panned/zoomed");
    WuQtUtilities::setWordWrappedToolTip(m_cropSceneImagesComboBox->getWidget(),
                                         cropSceneToolTip);
    QLabel* cropSceneImagesLabel = PreferencesDialog::addWidgetToLayout(gridLayout,
                                                                        "Crop Scene Images",
                                                                        m_cropSceneImagesComboBox->getWidget());
    WuQtUtilities::setWordWrappedToolTip(cropSceneImagesLabel,
                                         cropSceneToolTip);

    /*
     * Image texture magnification filter
     */
    m_graphicsTextureMagnificationFilterEnumComboBox = new EnumComboBoxTemplate(this);
    m_graphicsTextureMagnificationFilterEnumComboBox->setup<GraphicsTextureMagnificationFilterEnum,GraphicsTextureMagnificationFilterEnum::Enum>();
    QObject::connect(m_graphicsTextureMagnificationFilterEnumComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &PreferencesDevelopOptionsWidget::graphicsTextureMagnificationFilterEnumComboBoxItemActivated);
    WuQtUtilities::setWordWrappedToolTip(m_graphicsTextureMagnificationFilterEnumComboBox->getWidget(),
                                         GraphicsTextureMagnificationFilterEnum::toToolTip());
    QLabel* magFilterLabel = PreferencesDialog::addWidgetToLayout(gridLayout,
                                         "Image Magnification Filter (Zoomed In)",
                                         m_graphicsTextureMagnificationFilterEnumComboBox->getWidget());
    WuQtUtilities::setWordWrappedToolTip(magFilterLabel,
                                         GraphicsTextureMagnificationFilterEnum::toToolTip());
    
    /*
     * Image texture minification filter
     */
    m_graphicsTextureMinificationFilterEnumComboBox = new EnumComboBoxTemplate(this);
    m_graphicsTextureMinificationFilterEnumComboBox->setup<GraphicsTextureMinificationFilterEnum,GraphicsTextureMinificationFilterEnum::Enum>();
    QObject::connect(m_graphicsTextureMinificationFilterEnumComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &PreferencesDevelopOptionsWidget::graphicsTextureMinificationFilterEnumComboBoxItemActivated);
    WuQtUtilities::setWordWrappedToolTip(m_graphicsTextureMinificationFilterEnumComboBox->getWidget(),
                                         GraphicsTextureMinificationFilterEnum::toToolTip());
    QLabel* minFilterLabel = PreferencesDialog::addWidgetToLayout(gridLayout,
                                                                  "Image Minification Filter (Zoomed Out)",
                                                                  m_graphicsTextureMinificationFilterEnumComboBox->getWidget());
    WuQtUtilities::setWordWrappedToolTip(minFilterLabel,
                                         GraphicsTextureMinificationFilterEnum::toToolTip());

    /*
     * Oblique voxel interpolation type
     */
    m_obliqueVolumeInterpolationTypeComboBox = new EnumComboBoxTemplate(this);
    m_obliqueVolumeInterpolationTypeComboBox->setup<VoxelInterpolationTypeEnum,VoxelInterpolationTypeEnum::Enum>();
    QObject::connect(m_obliqueVolumeInterpolationTypeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &PreferencesDevelopOptionsWidget::obliqueVolumeInterpolationTypeComboBoxActivated);
    WuQtUtilities::setWordWrappedToolTip(m_obliqueVolumeInterpolationTypeComboBox->getWidget(),
                                         "Voxel Interpolation Type ");
    PreferencesDialog::addWidgetToLayout(gridLayout,
                                         "Oblique Interpolation Type",
                                         m_obliqueVolumeInterpolationTypeComboBox->getWidget());

    /*
     * Oblique voxel scaling
     */
    m_obliqueVoxelScalingSpinBox = new QDoubleSpinBox();
    m_obliqueVoxelScalingSpinBox->setRange(0.25, 100.0);
    m_obliqueVoxelScalingSpinBox->setSingleStep(0.25);
    m_obliqueVoxelScalingSpinBox->setToolTip("Scale oblique voxel size");
    PreferencesDialog::addWidgetToLayout(gridLayout, "Oblique Voxel Scaling", m_obliqueVoxelScalingSpinBox);
    QObject::connect(m_obliqueVoxelScalingSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, &PreferencesDevelopOptionsWidget::obliqueVoxelScalingSpinBoxValueChanged);
    
    QLabel* notesLabel = new QLabel("Note: These developer options are NOT saved in the user's preferences.  "
                                    "Therefore, any desired changes to these selections must be made each time "
                                    "wb_view is started.");
    notesLabel->setWordWrap(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(gridLayout);
    layout->addStretch(5);
    layout->addWidget(notesLabel);
    layout->addStretch();
}

/**
 * Destructor.
 */
PreferencesDevelopOptionsWidget::~PreferencesDevelopOptionsWidget()
{
}

/**
 * Called when a developer flag is changed
 * @param flag
 *    Developer flag that is changed
 * @param status
 *    New on/off status
 */
void
PreferencesDevelopOptionsWidget::developerFlagSelected(const DeveloperFlagsEnum::Enum flag,
                                                       const bool status)
{
    DeveloperFlagsEnum::setFlag(flag,
                                status);
    updateGraphicsAndUserInterface();
}

/**
 * Update the graphics and the user interface
 */
void
PreferencesDevelopOptionsWidget::updateGraphicsAndUserInterface()
{
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when crop scene image combo box changec
 * @param status
 *    New status
 */
void
PreferencesDevelopOptionsWidget::cropSceneImagesStatusChanged(const bool status)
{
    CaretAssert(m_preferences);
    m_preferences->setCropSceneImagesEnabled(status);
}

/**
 * Update the widget
 * @param preferences
 *    The preferences
 */
void
PreferencesDevelopOptionsWidget::updateContent(CaretPreferences* preferences)
{
    m_preferences = preferences;
    CaretAssert(m_preferences);
    
    for (auto& comboBoxFlag : m_developerFlagsMap) {
        WuQTrueFalseComboBox* comboBox(comboBoxFlag.first);
        DeveloperFlagsEnum::Enum flag(comboBoxFlag.second);
        
        comboBox->setStatus(DeveloperFlagsEnum::isFlag(flag));
    }
    
    m_cropSceneImagesComboBox->setStatus(m_preferences->isCropSceneImagesEnabled());
    
    const GraphicsTextureMinificationFilterEnum::Enum minFilter  = BrainOpenGLMediaDrawing::getTextureMinificationFilter();
    const GraphicsTextureMagnificationFilterEnum::Enum magFilter = BrainOpenGLMediaDrawing::getTextureMagnificationFilter();
    m_graphicsTextureMagnificationFilterEnumComboBox->setSelectedItem<GraphicsTextureMagnificationFilterEnum,GraphicsTextureMagnificationFilterEnum::Enum>(magFilter);
    m_graphicsTextureMinificationFilterEnumComboBox->setSelectedItem<GraphicsTextureMinificationFilterEnum,GraphicsTextureMinificationFilterEnum::Enum>(minFilter);
    
    m_obliqueVolumeInterpolationTypeComboBox->setSelectedItem<VoxelInterpolationTypeEnum,VoxelInterpolationTypeEnum::Enum>(BrainOpenGLVolumeObliqueSliceDrawing::getVoxelInterpolationType());
    QSignalBlocker voxelScaleBlocker(m_obliqueVoxelScalingSpinBox);
    m_obliqueVoxelScalingSpinBox->setValue(BrainOpenGLVolumeObliqueSliceDrawing::getVoxelStepScaling());
}

/**
 * Called when graphics magnification filter changed
 */
void
PreferencesDevelopOptionsWidget::graphicsTextureMagnificationFilterEnumComboBoxItemActivated()
{
    const GraphicsTextureMagnificationFilterEnum::Enum magFilter = m_graphicsTextureMagnificationFilterEnumComboBox->getSelectedItem<GraphicsTextureMagnificationFilterEnum,GraphicsTextureMagnificationFilterEnum::Enum>();
    BrainOpenGLMediaDrawing::setTextureMagnificationFilter(magFilter);
    updateGraphicsAndUserInterface();

}

/**
 * Called when graphics minification filter changed
 */
void
PreferencesDevelopOptionsWidget::graphicsTextureMinificationFilterEnumComboBoxItemActivated()
{
    const GraphicsTextureMinificationFilterEnum::Enum minFilter = m_graphicsTextureMinificationFilterEnumComboBox->getSelectedItem<GraphicsTextureMinificationFilterEnum,GraphicsTextureMinificationFilterEnum::Enum>();
    BrainOpenGLMediaDrawing::setTextureMinificationFilter(minFilter);
    updateGraphicsAndUserInterface();
}

/**
 * Called when oblique volume interpolation type changed
 */
void
PreferencesDevelopOptionsWidget::obliqueVolumeInterpolationTypeComboBoxActivated()
{
    const VoxelInterpolationTypeEnum::Enum interpType = m_obliqueVolumeInterpolationTypeComboBox->getSelectedItem<VoxelInterpolationTypeEnum,VoxelInterpolationTypeEnum::Enum>();
    BrainOpenGLVolumeObliqueSliceDrawing::setVoxelInterpolationType(interpType);
    updateGraphicsAndUserInterface();
}

/**
 * Called when oblique voxel scaling value changed
 * @param value
 *    New value
 */
void
PreferencesDevelopOptionsWidget::obliqueVoxelScalingSpinBoxValueChanged(double value)
{
    BrainOpenGLVolumeObliqueSliceDrawing::setVoxelStepScaling(value);
    updateGraphicsAndUserInterface();
}
