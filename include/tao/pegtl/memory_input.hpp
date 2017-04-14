// Copyright (c) 2014-2017 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/PEGTL/

#ifndef TAOCPP_PEGTL_INCLUDE_MEMORY_INPUT_HPP
#define TAOCPP_PEGTL_INCLUDE_MEMORY_INPUT_HPP

#include <cstddef>
#include <cstring>
#include <string>
#include <utility>

#include "config.hpp"
#include "eol.hpp"
#include "position.hpp"
#include "position_tracking.hpp"

#include "internal/action_input.hpp"
#include "internal/bump_impl.hpp"
#include "internal/iterator.hpp"
#include "internal/marker.hpp"

namespace tao
{
   namespace TAOCPP_PEGTL_NAMESPACE
   {
      namespace internal
      {
         template< typename Eol, position_tracking >
         class basic_memory_input_base;

         template< typename Eol >
         class basic_memory_input_base< Eol, position_tracking::IMMEDIATE >
         {
         public:
            basic_memory_input_base( const internal::iterator& in_iter, const char* in_end, const char* in_source ) noexcept
               : m_data( in_iter ),
                 m_end( in_end ),
                 m_source( in_source )
            {
            }

            basic_memory_input_base( const char* in_begin, const char* in_end, const char* in_source ) noexcept
               : basic_memory_input_base( internal::iterator( in_begin ), in_end, in_source )
            {
            }

            basic_memory_input_base( const char* in_begin, const char* in_end, const char* in_source, const std::size_t in_byte, const std::size_t in_line, const std::size_t in_byte_in_line ) noexcept
               : basic_memory_input_base( { in_byte, in_line, in_byte_in_line, in_begin }, in_end, in_source )
            {
            }

            const char* begin() const noexcept
            {
               return m_data.data;
            }

            const char* end( const std::size_t ) const noexcept
            {
               return m_end;
            }

            std::size_t byte() const noexcept
            {
               return m_data.byte;
            }

            std::size_t line() const noexcept
            {
               return m_data.line;
            }

            std::size_t byte_in_line() const noexcept
            {
               return m_data.byte_in_line;
            }

            const char* source() const noexcept
            {
               return m_source;
            }

            void bump( const std::size_t in_count = 1 ) noexcept
            {
               internal::bump( m_data, in_count, Eol::ch );
            }

            void bump_in_this_line( const std::size_t in_count = 1 ) noexcept
            {
               internal::bump_in_this_line( m_data, in_count );
            }

            void bump_to_next_line( const std::size_t in_count = 1 ) noexcept
            {
               internal::bump_to_next_line( m_data, in_count );
            }

            template< rewind_mode M >
            marker< internal::iterator, M > mark() noexcept
            {
               return marker< internal::iterator, M >( m_data );
            }

            TAOCPP_PEGTL_NAMESPACE::position position() const noexcept
            {
               return TAOCPP_PEGTL_NAMESPACE::position( m_data, m_source );
            }

            const internal::iterator& iterator() const noexcept
            {
               return m_data;
            }

         private:
            internal::iterator m_data;
            const char* const m_end;
            const char* const m_source;
         };

         template< typename Eol >
         class basic_memory_input_base< Eol, position_tracking::LAZY >
         {
         public:
            basic_memory_input_base( const char* in_begin, const char* in_end, const char* in_source ) noexcept
               : m_all( in_begin ),
                 m_run( in_begin ),
                 m_end( in_end ),
                 m_source( in_source )
            {
            }

            const char* begin() const noexcept
            {
               return m_run;
            }

            const char* end( const std::size_t ) const noexcept
            {
               return m_end;
            }

            std::size_t byte() const noexcept
            {
               return std::size_t( begin() - m_all );
            }

            const char* source() const noexcept
            {
               return m_source;
            }

            void bump( const std::size_t in_count = 1 ) noexcept
            {
               m_run += in_count;
            }

            void bump_in_this_line( const std::size_t in_count = 1 ) noexcept
            {
               m_run += in_count;
            }

            void bump_to_next_line( const std::size_t in_count = 1 ) noexcept
            {
               m_run += in_count;
            }

            template< rewind_mode M >
            marker< const char*, M > mark() noexcept
            {
               return marker< const char*, M >( m_run );
            }

            TAOCPP_PEGTL_NAMESPACE::position position() const noexcept
            {
               internal::iterator c( m_all );
               internal::bump( c, byte(), Eol::ch );
               return TAOCPP_PEGTL_NAMESPACE::position( c, m_source );
            }

            const char* iterator() const noexcept
            {
               return m_run;
            }

         private:
            const char* const m_all;
            const char* m_run;
            const char* const m_end;
            const char* const m_source;
         };

      }  // namespace internal

      template< typename Eol, position_tracking P >
      class basic_memory_input
         : public internal::basic_memory_input_base< Eol, P >
      {
      public:
         using eol_t = Eol;
         using memory_t = basic_memory_input;
         using action_t = internal::action_input< Eol, P >;

         using internal::basic_memory_input_base< Eol, P >::basic_memory_input_base;

         basic_memory_input( const char* in_begin, const std::size_t in_size, const char* in_source ) noexcept
            : basic_memory_input( in_begin, in_begin + in_size, in_source )
         {
         }

         basic_memory_input( const std::string& in_string, const char* in_source = "std::string" ) noexcept
            : basic_memory_input( in_string.data(), in_string.size(), in_source )
         {
         }

         basic_memory_input( const char* in_begin, const char* in_source = "const char*" ) noexcept
            : basic_memory_input( in_begin, std::strlen( in_begin ), in_source )
         {
         }

         bool empty() const noexcept
         {
            return this->begin() == this->end( 0 );
         }

         std::size_t size( const std::size_t ) const noexcept
         {
            return std::size_t( this->end( 0 ) - this->begin() );
         }

         char peek_char( const std::size_t offset = 0 ) const noexcept
         {
            return this->begin()[ offset ];
         }

         unsigned char peek_byte( const std::size_t offset = 0 ) const noexcept
         {
            return static_cast< unsigned char >( peek_char( offset ) );
         }

         void discard() const noexcept
         {
         }

         void require( const std::size_t ) const noexcept
         {
         }
      };

      template< position_tracking P = position_tracking::IMMEDIATE >
      using memory_input = basic_memory_input< lf_crlf_eol, P >;

   }  // namespace TAOCPP_PEGTL_NAMESPACE

}  // namespace tao

#endif
