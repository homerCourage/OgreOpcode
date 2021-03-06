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
#ifndef AE7C3C2E_35F8_44bc_AFC3_FEBF4CF6BC74
#define AE7C3C2E_35F8_44bc_AFC3_FEBF4CF6BC74

#include "OgreOpcodeExports.h"

/*
	This is the compliment to RefPtr. Instead of providing a simple reference
	counted pointer with automatic object deletion, this system provides
	a more flexible system where on the final lost reference a virtual
	"destroySelf" function is triggered. This allows the object to perform it's
	cleanup in any number of creative ways. Deletion is not implied. It could
	just as easily send a pointer to itself to a garbage collector for later
	deletion, or call any other sort of notification function to make the
	program aware that it is no longer referenced anywhere.

*/

namespace OgreOpcode
{
	namespace utility
	{
		template<typename> class RefObjHandle; //forward declaration

		//! Base abstract class for reference counted objects.
		class _OgreOpcode_Export RefObject
		{
			template<typename> friend class RefObjHandle;
		public:
			RefObject(): mRefCount( 0 ) {}
			virtual ~RefObject() {}
		protected:
			//! This is called when the last handle to this object is destroyed.
			virtual void finalize() = 0;
		private:
			void _reference()
			{
				mRefCount++;
			}
			void _unreference()
			{
				mRefCount--;
				if ( mRefCount == 0 )
					finalize();
			}
			size_t mRefCount;
		};


		//! Handle object for RefObject derived classes
		template<typename OBJECT>
		class RefObjHandle
		{
		private:
			//template<>
			RefObject* m_Handle;
		public:
			//! Create a handle from an existing object pointer, or empty
			RefObjHandle( RefObject* counted_object = 0 ): m_Handle( counted_object )
			{
				if ( m_Handle )
					m_Handle->_reference();
			}
			~RefObjHandle() {
				if ( m_Handle )
					m_Handle->_unreference();
			}
			//! Create a handle from another handle
			RefObjHandle( const RefObjHandle<OBJECT> &other_handle )
			{
				m_Handle = other_handle.m_Handle;
				if ( m_Handle )	m_Handle->_reference();
			}

			//! Assignment operator
			RefObjHandle<OBJECT>& operator=( const RefObjHandle<OBJECT>& right )
			{
				if ( m_Handle ) m_Handle->_unreference();
				m_Handle = right.m_Handle;
				if ( m_Handle ) m_Handle->_reference();
				return *this;
			}

			//! Return the total number of handles to the object, yourself included.
			unsigned int getHandleCount()
			{
				if ( m_Handle )
					return m_Handle->mRefCount;
				return 0;
			}

			//! The usual *Handle resolving
			OBJECT& operator*() const
			{
				return *( dynamic_cast<OBJECT*>( m_Handle ) );
			}
			//! The usual Handle->something resolving
			OBJECT* operator->() const
			{
				return dynamic_cast<OBJECT*>( m_Handle );
			}

			//! This makes "if( Handle )" work just like "if( !Handle.isNull() )"
			operator bool() const
			{
				return m_Handle == 0 ? false : true;
			}

			//! This allows us to use "if( Handle == NULL )" and "if( Handle == 0 )"
			bool operator==( int p ) const
			{
				//yep, they're testing for null, but we need to make sure
				if ( 0 == p )
					return operator ==(( OBJECT* )0 );
				//mmkay, i don't know of anyone that holds pointers inside of ints
				//so no, we don't equal that
				return false;
			}

			//! used for "if( Handle != NULL )" and "if(Handle != 0 )"
			bool operator!=( int p ) const
			{
				//yet another null test type
				return !operator==( p );
			}
			//! Used for "if( Handle == normal_pointer )"
			bool operator ==( const OBJECT* p ) const
			{
				return m_Handle == p;
			}

			//! Used for "if( Handle == Handle )"
			bool operator==( const RefObjHandle& r ) const
			{
				return m_Handle == r.m_Handle;
			}

			//! Used for "if( Handle != Handle )"
			bool operator!=( const RefObjHandle& r ) const
			{
				return !operator==( r );
			}
			//! Used for "if( Handle != normal_pointer )"
			bool operator!=( const OBJECT* p ) const
			{
				return !operator==( p );
			}

			//! Handle.isNull() returns true if RefPtr carries no pointer
			bool isNull()const
			{
				return m_Handle == 0;
			}
			//! Returns the carried pointer in raw form.
			OBJECT* get() const
			{
					return ( dynamic_cast<OBJECT*>( m_Handle ) );
			}
		};
	} // namespace utility
} //namespace OgreOpcode

#endif // AE7C3C2E_35F8_44bc_AFC3_FEBF4CF6BC74
