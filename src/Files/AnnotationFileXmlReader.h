#ifndef __ANNOTATION_FILE_XML_READER_H__
#define __ANNOTATION_FILE_XML_READER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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


#include "AnnotationFileXmlFormatBase.h"
#include "CaretPointer.h"

class QXmlStreamAttributes;
class QXmlStreamReader;

namespace caret {

    class Annotation;
    class AnnotationCoordinate;
    class AnnotationFile;
    class AnnotationOneDimensionalShape;
    class AnnotationText;
    class AnnotationTwoDimensionalShape;
    
    class AnnotationFileXmlReader : public AnnotationFileXmlFormatBase {
        
    public:
        AnnotationFileXmlReader();
        
        virtual ~AnnotationFileXmlReader();

        void readFile(const AString& filename,
                      AnnotationFile* annotationFile);

        // ADD_NEW_METHODS_HERE

    private:
        AnnotationFileXmlReader(const AnnotationFileXmlReader&);

        AnnotationFileXmlReader& operator=(const AnnotationFileXmlReader&);
        
        void readVersionOne();
        
        void readOneDimensionalAnnotation(const QString& annotationElementName,
                                          AnnotationOneDimensionalShape* annotation);

        void readTwoDimensionalAnnotation(const QString& annotationElementName,
                                          AnnotationTwoDimensionalShape* annotation);
        
        void readCoordinate(const QString& coordinateElementName,
                            AnnotationCoordinate* coordinate);
        
        void readAnnotationAttributes(Annotation* annotation,
                                      const QString& annotationElementName,
                                      const QXmlStreamAttributes& attributes);
        
        void readTextDataElement(AnnotationText* textAnnotation);
        
        
        QString getRequiredAttributeStringValue(const QXmlStreamAttributes& attributes,
                                                const QString& elementName,
                                                const QString& attributeName);
        
        int getRequiredAttributeIntValue(const QXmlStreamAttributes& attributes,
                                                const QString& elementName,
                                                const QString& attributeName);
        
        float getRequiredAttributeFloatValue(const QXmlStreamAttributes& attributes,
                                         const QString& elementName,
                                         const QString& attributeName);
        
        bool getRequiredAttributeBoolValue(const QXmlStreamAttributes& attributes,
                                             const QString& elementName,
                                             const QString& attributeName);
        
        void throwDataFileException(const QString message);
        
        CaretPointer<QXmlStreamReader> m_stream;
        
        QString m_filename;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_FILE_XML_READER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_FILE_XML_READER_DECLARE__

} // namespace
#endif  //__ANNOTATION_FILE_XML_READER_H__