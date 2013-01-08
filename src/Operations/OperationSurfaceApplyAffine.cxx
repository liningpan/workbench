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

#include "AffineFile.h"
#include "FloatMatrix.h"
#include "OperationSurfaceApplyAffine.h"
#include "OperationException.h"
#include "SurfaceFile.h"
#include "Vector3D.h"

using namespace caret;
using namespace std;

AString OperationSurfaceApplyAffine::getCommandSwitch()
{
    return "-surface-apply-affine";
}

AString OperationSurfaceApplyAffine::getShortDescription()
{
    return "APPLY AFFINE TRANSFORM TO SURFACE FILE";
}

OperationParameters* OperationSurfaceApplyAffine::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "in-surf", "the surface to transform");
    ret->addStringParameter(2, "affine", "the affine file");
    ret->addSurfaceOutputParameter(3, "out-surf", "the output transformed surface");
    OptionalParameter* flirtOpt = ret->createOptionalParameter(4, "-flirt", "MUST be used if affine is a flirt affine");
    flirtOpt->addStringParameter(1, "source-volume", "the source volume used when generating the affine");
    flirtOpt->addStringParameter(2, "target-volume", "the target volume used when generating the affine");
    ret->setHelpText(
        AString("For flirt matrices, you must use the -flirt option, because flirt matrices are not a complete description of the coordinate transform they represent.  ") +
        "If the -flirt option is not present, the affine must be a 'world' affine, which can be obtained with the -convert-affine command, or aff_conv from the 4dfp suite."
    );
    return ret;
}

void OperationSurfaceApplyAffine::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SurfaceFile* mySurf = myParams->getSurface(1);
    AString affineName = myParams->getString(2);
    SurfaceFile* mySurfOut = myParams->getOutputSurface(3);
    OptionalParameter* flirtOpt = myParams->getOptionalParameter(4);//gets optional parameter with key 3
    AffineFile myAffine;
    if (flirtOpt->m_present)
    {
        myAffine.readFlirt(affineName, flirtOpt->getString(1), flirtOpt->getString(2));
    } else {
        myAffine.readWorld(affineName);
    }
    *mySurfOut = *mySurf;//copy rather than initialize, don't currently have much in the way of modification functions
    const FloatMatrix& affMat = myAffine.getMatrix();
    Vector3D xvec, yvec, zvec, translate;
    xvec[0] = affMat[0][0]; xvec[1] = affMat[1][0]; xvec[2] = affMat[2][0];
    yvec[0] = affMat[0][1]; yvec[1] = affMat[1][1]; yvec[2] = affMat[2][1];
    zvec[0] = affMat[0][2]; zvec[1] = affMat[1][2]; zvec[2] = affMat[2][2];
    translate[0] = affMat[0][3]; translate[1] = affMat[1][3]; translate[2] = affMat[2][3];
    int numNodes = mySurf->getNumberOfNodes();
    const float* coordData = mySurf->getCoordinateData();
    vector<float> coordsOut(numNodes * 3);
    for (int i = 0; i < numNodes; ++i)
    {
        int base = i * 3;
        Vector3D transformed = coordData[base] * xvec + coordData[base + 1] * yvec + coordData[base + 2] * zvec + translate;
        coordsOut[base] = transformed[0];
        coordsOut[base + 1] = transformed[1];
        coordsOut[base + 2] = transformed[2];
    }
    mySurfOut->setCoordinates(coordsOut.data(), numNodes);
}
