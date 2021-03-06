/// ////////////////////////////////////////////////////////////////////////////
///  
///  This file is part of OgreOpcode.
///  
///
/// Copyright (c) 2009 The OgreOpcode Team.
///
/// This software is provided 'as-is', without any express or implied
/// warranty. In no event will the authors be held liable for any damages
/// arising from the use of this software.
///
/// Permission is granted to anyone to use this software for any purpose,
/// including commercial applications, and to alter it and redistribute it
/// freely, subject to the following restrictions:
///
/// 1. The origin of this software must not be misrepresented; you must not
/// claim that you wrote the original software. If you use this software
/// in a product, an acknowledgment in the product documentation would be
/// appreciated but is not required.
///
/// 2. Altered source versions must be plainly marked as such, and must not be
/// misrepresented as being the original software.
///
/// 3. This notice may not be removed or altered from any source
/// distribution.
///
///  
/// ////////////////////////////////////////////////////////////////////////////
// OpenGUI (http://opengui.sourceforge.net)
// This source code is release under the BSD License
// See LICENSE.TXT for details

#ifndef CB91D9E4_B0AA_4118_ACA8_58BE4754BE6D
#define CB91D9E4_B0AA_4118_ACA8_58BE4754BE6D

//Enable the following #define to make each refptr output to its own log.
//Great way to find out if all refptrs are actually getting destructed correctly.
//#define REFPTR_DEBUG

// I didn't like the ones that were out there, so I wrote my own. :-)
// Unlike the rest of OpenGUI, the immediate contents of this file are hereby PUBLIC DOMAIN.
// Do what you want with it. I think everyone should have a decent RefPtr class.
//             - Eric Shorkey (August 7th, 2006)

#include "OgreOpcodeExports.h"


namespace OgreOpcode
{
	namespace utility
	{
		template<typename T>
		class RefPtr;

		template<typename T>
		class __RefObj {
			friend class RefPtr<T>;
		private:

			void _OgreOpcode_Export __suicide()
			{
				delete this;
			}
			void _OgreOpcode_Export __killPayload()
			{
				if ( m_objPtr )
					delete m_objPtr;
				m_objPtr = 0;
			}

			__RefObj( T *payload )
			{
				m_objPtr = payload;
				refcount = 1;
	#ifdef REFPTR_DEBUG
				std::stringstream ss;
				ss << "REFPTR_" << m_objPtr << ".log";
				std::ofstream out;
				out.open( ss.str().c_str(), std::ios::app );
				out << "[+] NEW RefObj: 0x" << this << " : (0x" << m_objPtr << ") {" << refcount << "}\n";
				out.close();
	#endif
			}
			~__RefObj()
			{
	#ifdef REFPTR_DEBUG
				std::stringstream ss;
				ss << "REFPTR_" << m_objPtr << ".log";
				std::ofstream out;
				out.open( ss.str().c_str(), std::ios::app );
				out << "[-] DEL RefObj: 0x" << this << " : (0x" << m_objPtr << ") {" << refcount << "}\n";
				out.close();
	#endif
				__killPayload();
			}
			void ref()
			{
				refcount++;
	#ifdef REFPTR_DEBUG
				std::stringstream ss;
				ss << "REFPTR_" << m_objPtr << ".log";
				std::ofstream out;
				out.open( ss.str().c_str(), std::ios::app );
				out << "[R] REF RefObj: 0x" << this << " : (0x" << m_objPtr << ") {" << refcount << "}\n";
				out.close();
	#endif
			}
			void unref()
			{
				refcount--;
	#ifdef REFPTR_DEBUG
				std::stringstream ss;
				ss << "REFPTR_" << m_objPtr << ".log";
				std::ofstream out;
				out.open( ss.str().c_str(), std::ios::app );
				out << "[U] URF RefObj: 0x" << this << " : (0x" << m_objPtr << ") {" << refcount << "}\n";
				out.close();
	#endif
				if ( refcount == 0 )
					__suicide();
			}
			T* m_objPtr;
			unsigned int refcount;
		};

		//! This is the base template used for any reference counted pointers in %OgreOpcode
		template<typename T>
		class RefPtr
		{
		public:
			//! Create a RefPtr from an existing pointer, or empty
			RefPtr( T *ptr = 0 ) {
				m_refObjPtr = 0;
				if ( ptr )
					m_refObjPtr = new __RefObj<T>( ptr );
			}
			//! Create a RefPtr from another RefPtr
			RefPtr( const RefPtr<T> &ptr ) {
				m_refObjPtr = ptr.m_refObjPtr;
				if ( m_refObjPtr )	m_refObjPtr->ref();
			}
			//! Destructor does the usual unreferencing operation
			~RefPtr() {
				if ( m_refObjPtr )
					m_refObjPtr->unref();
			}

			//! Return the total number of references to the stored pointer, yourself included.
			unsigned int getRefCount() {
				if ( m_refObjPtr )
					return m_refObjPtr->refcount;
				return 0;
			}

			//! Assignment operator
			RefPtr<T>& operator=( const RefPtr<T>& ptr ) {
				if ( m_refObjPtr ) m_refObjPtr->unref();
				m_refObjPtr = ptr.m_refObjPtr;
				if ( m_refObjPtr ) m_refObjPtr->ref();
				return *this;
			}

			//! The usual *RefPtr resolving
			T& operator*() const {
				return *( m_refObjPtr->m_objPtr );
			}
			//! The usual RefPtr->somthing resolving
			T* operator->() const {
				return ( m_refObjPtr->m_objPtr );
			}
			//! This makes "if( RefPtr )" work just like "if( !RefPtr.isNull() )"
			operator bool() const {
				return m_refObjPtr == 0 ? false : true;
			}

			//! This allows us to use "if( RefPtr == NULL )" and "if( RefPtr == 0 )"
			bool operator==( int p ) const {
				//yep, they're testing for null, but we need to make sure
				if ( 0 == p )
					return operator ==(( T* )0 );
				//mmkay, i don't know of anyone that holds pointers inside of ints
				//so no, we don't equal that
				return false;
			}
			//! used for "if( RefPtr != NULL )" and "if(RefPtr != 0 )"
			bool operator!=( int p ) const {
				//yet another null test type
				return !operator==( p );
			}
			//! Used for "if( RefPtr == normal_pointer )"
			bool operator ==( const T* p ) const {
				if ( m_refObjPtr )
					return m_refObjPtr->m_objPtr  == p;
				return p == 0;
			}
			//! Used for "if( RefPtr == RefPtr )"
			bool operator==( const RefPtr& r ) const {
				return m_refObjPtr == r.m_refObjPtr;
			}

			//! Used for "if( RefPtr != RefPtr )"
			bool operator!=( const RefPtr& r ) const {
				return !operator==( r );
			}
			//! Used for "if( RefPtr != normal_pointer )"
			bool operator!=( const T* p ) const {
				return !operator==( p );
			}

			//! RefPtr.isNull() returns true if RefPtr carries no pointer
			bool isNull()const {
				return m_refObjPtr == 0;
			}
			//! Returns the carried pointer in raw form.
			T* get() const {
					return ( m_refObjPtr->m_objPtr );
				}
		protected:
			__RefObj<T> *m_refObjPtr;
		};
	}; // namespace utility
}; //namespace OgreOpcode
#endif // CB91D9E4_B0AA_4118_ACA8_58BE4754BE6D
