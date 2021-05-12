
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

#define __VIEWING_TRANSFORMATIONS_DECLARE__
#include "ViewingTransformations.h"
#undef __VIEWING_TRANSFORMATIONS_DECLARE__

#include "CaretLogger.h"
#include "GraphicsObjectToWindowTransform.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;



/**
 * \class caret::ViewingTransformations
 * \brief Viewing transformations (pan/rotate/zoom).
 * \ingroup Brain
 */

/**
 * Constructor.
 */
ViewingTransformations::ViewingTransformations()
: CaretObject()
{
    m_sceneAssistant = new SceneClassAssistant();
    
    m_rotationMatrix = new Matrix4x4();
    m_translation[0] = 0.0;
    m_translation[1] = 0.0;
    m_translation[2] = 0.0;
    m_scaling = 1.0;
    m_flatRotationMatrix = new Matrix4x4();
    
    m_rightCortexFlatMapOffset[0] = 0.0;
    m_rightCortexFlatMapOffset[1] = 0.0;
    m_rightCortexFlatMapZoomFactor = 1.0;
    
    m_sceneAssistant->addArray("m_translation",
                               m_translation,
                               3,
                               0.0);
    m_sceneAssistant->add("m_scaling",
                          &m_scaling);
    
    m_sceneAssistant->addArray("m_rightCortexFlatMapOffset",
                               m_rightCortexFlatMapOffset,
                               2,
                               0.0);
    m_sceneAssistant->add("m_rightCortexFlatMapZoomFactor",
                          &m_rightCortexFlatMapZoomFactor);
}

/**
 * Destructor.
 */
ViewingTransformations::~ViewingTransformations()
{
    delete m_rotationMatrix;
    delete m_flatRotationMatrix;
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ViewingTransformations::ViewingTransformations(const ViewingTransformations& obj)
: CaretObject(obj), SceneableInterface(obj)
{
    this->copyHelperViewingTransformations(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
ViewingTransformations&
ViewingTransformations::operator=(const ViewingTransformations& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperViewingTransformations(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
ViewingTransformations::copyHelperViewingTransformations(const ViewingTransformations& obj)
{
    *m_rotationMatrix = *obj.m_rotationMatrix;
    m_translation[0]  = obj.m_translation[0];
    m_translation[1]  = obj.m_translation[1];
    m_translation[2]  = obj.m_translation[2];
    m_scaling         = obj.m_scaling;
    *m_flatRotationMatrix = *obj.m_flatRotationMatrix;
    m_rightCortexFlatMapOffset[0]  = obj.m_rightCortexFlatMapOffset[0];
    m_rightCortexFlatMapOffset[1]  = obj.m_rightCortexFlatMapOffset[1];
    m_rightCortexFlatMapZoomFactor = obj.m_rightCortexFlatMapZoomFactor;
}

/**
 * @return The viewing translation.
 */
const float*
ViewingTransformations::getTranslation() const
{
    return m_translation;
}

/**
 * Get the viewing translation.
 *
 * @param translationOut
 *    Translation values output.
 */
void
ViewingTransformations::getTranslation(float translationOut[3]) const
{
    translationOut[0] = m_translation[0];
    translationOut[1] = m_translation[1];
    translationOut[2] = m_translation[2];
}

/**
 * Set the viewing translation.
 *
 * @param translation
 *    New translation values.
 */
void
ViewingTransformations::setTranslation( const float translation[3])
{
    m_translation[0] = translation[0];
    m_translation[1] = translation[1];
    m_translation[2] = translation[2];
}

/**
 * Set the viewing translation.
 *
 * @param translationX
 *    New translation X-value.
 * @param translationY
 *    New translation Y-value.
 * @param translationZ
 *    New translation Z-value.
 */
void
ViewingTransformations::setTranslation(const float translationX,
                                       const float translationY,
                                       const float translationZ)
{
    m_translation[0] = translationX;
    m_translation[1] = translationY;
    m_translation[2] = translationZ;
}

/**
 * @return The viewing scaling.
 */
float
ViewingTransformations::getScaling() const
{
    return m_scaling;
}

/**
 * Set the viewing scaling.
 * @param scaling
 *    New value for scaling.
 */
void
ViewingTransformations::setScaling(const float scaling)
{
    m_scaling = scaling;
}

/**
 * Scale about the position of the mouse
 * @param transform
 *    Graphics object to window transform
 * @param mousePressX
 *    X-Location of where mouse was pressed
 * @param mousePressX
 *    Y-Location of where mouse was pressed
 * @param mouseDY
 *    Change in mouse Y
 * @param dataX
 *    X-coordinate of data where mouse was pressed
 * @param dataY
 *    Y-coordinate of data where mouse was pressed
 * @param dataXYValidFlag
 *    True if dataX and dataY are valid
 */
void
ViewingTransformations::scaleAboutMouse(const GraphicsObjectToWindowTransform* transform,
                                        const int32_t mousePressX,
                                        const int32_t mousePressY,
                                        const int32_t mouseDY,
                                        const float dataX,
                                        const float dataY,
                                        const bool dataXYValidFlag)
{
    if (mouseDY == 0) {
        return;
    }
    
    if (transform == NULL) {
        CaretLogSevere("Object to window transform is NULL");
        return;
    }
    if ( ! transform->isValid()) {
        CaretLogSevere("Object to window transform is INVALID");
        return;
    }
    
    if ( ! dataXYValidFlag) {
        std::cout << "Data XY not valid; cannot zoom about mouse" << std::endl;
        return;
    }
    
    const bool debugFlag(false);
    const float mousePressXYZ[3] { static_cast<float>(mousePressX), static_cast<float>(mousePressY), 0.0f };
    if (debugFlag) {
        std::cout << "Window X Y: " << mousePressXYZ[0] << ", " << mousePressXYZ[1] << std::endl;
        std::cout << "   Data X Y: " << dataX << ", " << dataY << std::endl;
    }
    
    
    {
        /*
         * Works with scaling and translation
         */
        
        /*
         * Update the scaling but don't let it get to zero or negative
         */
        const float minimumValidScaleValue(0.001);
        const float newScale((1.0f + (mouseDY * 0.01)));
        const float oldScale(getScaling());
        const float totalScale(std::max((newScale * oldScale),
                                        minimumValidScaleValue));
        setScaling(totalScale);
        
        /*
         * Get viewing coordinate at the location of mouse when mouse was first pressed
         */
        Matrix4x4 identityMatrix;
        auto newXform = transform->cloneWithNewModelViewMatrix(identityMatrix);
        float identityXYZ[3];
        newXform->inverseTransformPoint(mousePressXYZ, identityXYZ);
        if (debugFlag) {
            std::cout << "   Identity X Y: " << identityXYZ[0] << ", " << identityXYZ[1] << std::endl;
        }
        
        /*
         * Need to offset model origin from where the mouse was pressed
         * so that the image pixel under mouse press stays at the same
         * location on the screen
         */
        float tx = -((dataX * totalScale) - identityXYZ[0]);
        float ty = -((dataY * totalScale) - identityXYZ[1]);
        if (debugFlag) {
            std::cout << "   Ident Trans XY: " << tx << ", " << ty << std::endl;
        }
        
        setTranslation(tx, ty, 0.0);
        
        return;
    }
    
    {
        /*
         * Works with scaling and translation
         */
        const float newScale((1.0f + (mouseDY * 0.01)));
        const float oldScale(getScaling());
        const float totalScale(newScale * oldScale);
        
        float ct[3];
        getTranslation(ct);
        
        Matrix4x4 m;
        m.translate(-dataX, -dataY, 0.0);
        m.scale(totalScale, totalScale, 1.0);
        m.translate(dataX, dataY, 0.0);

        float t[3];
        m.getTranslation(t);
        setTranslation(t);
        std::cout << "  Trans X Y: " << t[0] << ", " << t[1] << std::endl;

        double sx(0.0), sy(0.0), sz(0.0);
        m.getScale(sx, sy, sz);
        setScaling(sx);
        std::cout << "  Scale: " << sx << std::endl;
        
        
        Matrix4x4 identityMatrix;
        auto newXform = transform->cloneWithNewModelViewMatrix(identityMatrix);
        float identityXYZ[3];
        newXform->inverseTransformPoint(mousePressXYZ, identityXYZ);
        std::cout << "   Identity X Y: " << identityXYZ[0] << ", " << identityXYZ[1] << std::endl;
        const float diffXY[2] { identityXYZ[0] - dataX, identityXYZ[1] - dataY };
        std::cout << "   Diff Model X Y: " << diffXY[0] << ", " << diffXY[1] << std::endl;

        float tx = -((dataX * sx) - identityXYZ[0]);
        float ty = -((dataY * sy) - identityXYZ[1]);
        std::cout << "   Ident Trans XY: " << tx << ", " << ty << std::endl;
        
        setTranslation(tx, ty, 0.0);
        
        return;
    }
    
    {
        float modelXYZ[3];
        transform->inverseTransformPoint(mousePressXYZ,
                                         modelXYZ);
        //    std::cout << "   Model X Y: " << modelXYZ[0] << ", " << modelXYZ[1] << std::endl;
        /*
         * This works as long as there is no previous translation/scaling (image at origin)
         */
        const float newScale((1.0f + (mouseDY * 0.01)));
        const float oldScale(getScaling());
        const float totalScale(newScale * oldScale);
  
        float ct[3];
        getTranslation(ct);
        
        Matrix4x4 m;
        m.translate(-modelXYZ[0], -modelXYZ[1], 0.0);
        m.scale(totalScale, totalScale, 1.0);
        m.translate(modelXYZ[0], modelXYZ[1], 0.0);

        float t[3];
        m.getTranslation(t);
        setTranslation(t);
        
        double sx(0.0), sy(0.0), sz(0.0);
        m.getScale(sx, sy, sz);
        setScaling(sx);

        return;
    }
    
}

/**
 * @return The rotation matrix.
 */
Matrix4x4
ViewingTransformations::getRotationMatrix() const
{
    return *m_rotationMatrix;
}

/**
 * Set the rotation matrix.
 *
 * @param rotationMatrix
 *    The new rotation matrix.
 */
void
ViewingTransformations::setRotationMatrix(const Matrix4x4& rotationMatrix)
{
    *m_rotationMatrix = rotationMatrix;
}

/**
 * @return The flat rotation matrix
 */
Matrix4x4
ViewingTransformations::getFlatRotationMatrix() const
{
    return *m_flatRotationMatrix;
}

/**
 * Set the flat rotation matrix
 * @param flatRotationMatrix
 *    New  flat rotation matrix
 */
void
ViewingTransformations::setFlatRotationMatrix(const Matrix4x4& flatRotationMatrix)
{
    *m_flatRotationMatrix = flatRotationMatrix;
}

/**
 * Get the offset for the right cortex flat map.
 *
 * @param rightCortexFlatMapOffsetX
 *    Output with X offset.
 * @param rightCortexFlatMapOffsetY
 *    Output with Y offset.
 */
void
ViewingTransformations::getRightCortexFlatMapOffset(float& rightCortexFlatMapOffsetX,
                                                    float& rightCortexFlatMapOffsetY) const
{
    rightCortexFlatMapOffsetX = m_rightCortexFlatMapOffset[0];
    rightCortexFlatMapOffsetY = m_rightCortexFlatMapOffset[1];
}

/**
 * Set the offset for the right cortex flat map.
 *
 * @param rightCortexFlatMapOffsetX
 *    New X offset.
 * @param rightCortexFlatMapOffsetY
 *    New Y offset.
 */
void
ViewingTransformations::setRightCortexFlatMapOffset(const float rightCortexFlatMapOffsetX,
                                                    const float rightCortexFlatMapOffsetY)
{
    m_rightCortexFlatMapOffset[0] = rightCortexFlatMapOffsetX;
    m_rightCortexFlatMapOffset[1] = rightCortexFlatMapOffsetY;
}

/**
 * @return The right flat cortex flat map offset.
 */
float
ViewingTransformations::getRightCortexFlatMapZoomFactor() const
{
    return m_rightCortexFlatMapZoomFactor;
}

/**
 * Set the right flat cortex flat map offset.
 *
 * @param rightCortexFlatMapZoomFactor
 *     The right flat cortex flat map offset.
 */
void
ViewingTransformations::setRightCortexFlatMapZoomFactor(const float rightCortexFlatMapZoomFactor)
{
    m_rightCortexFlatMapZoomFactor = rightCortexFlatMapZoomFactor;
}


/**
 * Reset the view to the default view for a SURFACE
 */
void
ViewingTransformations::resetView()
{
    setTranslation(0.0, 0.0, 0.0);
    m_rotationMatrix->identity();
    setScaling(1.0);
    m_flatRotationMatrix->identity();
    setRightCortexFlatMapOffset(0.0, 0.0);
    setRightCortexFlatMapZoomFactor(1.0);
    leftView();
}

/**
 * Set to a right side view.
 */
void
ViewingTransformations::rightView()
{
    m_rotationMatrix->identity();
    m_rotationMatrix->rotateY(-90.0);
    m_rotationMatrix->rotateZ(-90.0);
}

/**
 * set to a left side view.
 */
void
ViewingTransformations::leftView()
{
    m_rotationMatrix->identity();
    m_rotationMatrix->rotateY(90.0);
    m_rotationMatrix->rotateZ(90.0);
}

/**
 * set to a anterior view.
 */
void
ViewingTransformations::anteriorView()
{
    m_rotationMatrix->identity();
    m_rotationMatrix->setRotation(90.0, 0.0, -180.0);
}

/**
 * set to a posterior view.
 */
void
ViewingTransformations::posteriorView()
{
    m_rotationMatrix->identity();
    m_rotationMatrix->setRotation(-90.0, 0.0, 0.0);
}

/**
 * set to a dorsal view.
 */
void
ViewingTransformations::dorsalView()
{
    m_rotationMatrix->setRotation(0.0, 0.0, 90.0);
}

/**
 * set to a ventral view.
 */
void
ViewingTransformations::ventralView()
{
    m_rotationMatrix->setRotation(0.0, 180.0, 90.0);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
ViewingTransformations::toString() const
{
    return "ViewingTransformations";
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
ViewingTransformations::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ViewingTransformations",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    /*
     * Save rotation matrices.
     */
    float matrix[4][4];
    m_rotationMatrix->getMatrix(matrix);
    sceneClass->addFloatArray("m_rotationMatrix", (float*)matrix, 16);
    
    /*
     * Save flat rotation matrices.
     */
    m_flatRotationMatrix->getMatrix(matrix);
    sceneClass->addFloatArray("m_flatRotationMatrix", (float*)matrix, 16);
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
ViewingTransformations::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    /*
     * Restore rotation matrices.
     */
    float matrix[4][4];
    if (sceneClass->getFloatArrayValue("m_rotationMatrix", (float*)matrix, 16) == 16) {
        m_rotationMatrix->setMatrix(matrix);
    }
    else {
        m_rotationMatrix->identity();
    }
    
    /*
     * Restore flat rotation matrices.
     */
    if (sceneClass->getFloatArrayValue("m_flatRotationMatrix", (float*)matrix, 16) == 16) {
        m_flatRotationMatrix->setMatrix(matrix);
    }
    else {
        m_flatRotationMatrix->identity();
    }
    
}


