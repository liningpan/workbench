#ifndef __CZI_UTILITIES_H__
#define __CZI_UTILITIES_H__

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

#include <QRectF>
#include <QString>

#include "libCZI_Pixels.h"


namespace caret {

    class CziUtilities {
        
    public:
        virtual ~CziUtilities();
        
        CziUtilities(const CziUtilities&) = delete;

        CziUtilities& operator=(const CziUtilities&) = delete;
        
        static QRectF intRectToQRect(const libCZI::IntRect& intRect);
        
        static libCZI::IntRect qRectToIntRect(const QRectF& qRect);
        
        static QString intRectToString(const libCZI::IntRect& intRect);
        
        static QString qRectToString(const QRectF& qRect);
        
        static QString qRectToLrbtString(const QRectF& qRect);
        
        // ADD_NEW_METHODS_HERE

    private:
        CziUtilities();
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CZI_UTILITIES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CZI_UTILITIES_DECLARE__

} // namespace
#endif  //__CZI_UTILITIES_H__
