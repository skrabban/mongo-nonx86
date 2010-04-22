/** jsobjManipulator.h */

/**
 *    Copyright (C) 2009 10gen Inc.
 *
 *    This program is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "jsobj.h"

namespace mongo {

    /** Manipulate the binary representation of a BSONElement in-place.
        Careful, this casts away const.
    */
    class BSONElementManipulator {
    public:
        BSONElementManipulator( const BSONElement &element ) :
            _element( element ) {
            assert( !_element.eoo() );
        }
        /** Replace a Timestamp type with a Date type initialized to
            OpTime::now().asDate()
        */
        void initTimestamp();
        
        /** Change the value, in place, of the number. */
        void setNumber(double d) {
            if ( _element.type() == NumberDouble ) copyLE< double >( value(), d );
            else if ( _element.type() == NumberInt ) copyLE< int >( value(), d );
            else assert(0);
        }
        void setLong(long long n) { 
            assert( _element.type() == NumberLong );
            copyLE<long long>( value(), n );
        }
        void setInt(int n) { 
            assert( _element.type() == NumberInt );
            copyLE<int>( value(), n );
        }

        
        /** Replace the type and value of the element with the type and value of e,
            preserving the original fieldName */
        void replaceTypeAndValue( const BSONElement &e ) {
            *data() = e.type();
            memcpy( value(), e.value(), e.valuesize() );
        }
        
        static void lookForTimestamps( const BSONObj& obj ){
            // If have a Timestamp field as the first or second element,
            // update it to a Date field set to OpTime::now().asDate().  The
            // replacement policy is a work in progress.
            
            BSONObjIterator i( obj );
            for( int j = 0; i.moreWithEOO() && j < 2; ++j ) {
                BSONElement e = i.next();
                if ( e.eoo() )
                    break;
                if ( e.type() == Timestamp ){
                    BSONElementManipulator( e ).initTimestamp();
                    break;
                }
            }
        }
    private:
        char *data() { return nonConst( _element.rawdata() ); }
        char *value() { return nonConst( _element.value() ); }
        static char *nonConst( const char *s ) { return const_cast< char * >( s ); }

        const BSONElement _element;
    };

} // namespace mongo
