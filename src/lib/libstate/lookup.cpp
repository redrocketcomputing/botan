/*
* Algorithm Retrieval
* (C) 1999-2007 Jack Lloyd
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#include <botan/lookup.h>
#include <botan/libstate.h>
#include <botan/engine.h>

namespace Botan {

/*
* Get a PBKDF algorithm by name
*/
PBKDF* get_pbkdf(const std::string& algo_spec)
   {
   Algorithm_Factory& af = global_state().algorithm_factory();

   if(PBKDF* pbkdf = af.make_pbkdf(algo_spec))
      return pbkdf;

   throw Algorithm_Not_Found(algo_spec);
   }

/*
* Query if an algorithm exists
*/
bool have_algorithm(const std::string& name)
   {
   Algorithm_Factory& af = global_state().algorithm_factory();

   if(af.prototype_block_cipher(name))
      return true;
   if(af.prototype_stream_cipher(name))
      return true;
   if(af.prototype_hash_function(name))
      return true;
   if(af.prototype_mac(name))
      return true;
   return false;
   }

/*
* Query the block size of a cipher or hash
*/
size_t block_size_of(const std::string& name)
   {
   Algorithm_Factory& af = global_state().algorithm_factory();

   if(const BlockCipher* cipher = af.prototype_block_cipher(name))
      return cipher->block_size();

   if(const HashFunction* hash = af.prototype_hash_function(name))
      return hash->hash_block_size();

   throw Algorithm_Not_Found(name);
   }

/*
* Query the output_length() of a hash or MAC
*/
size_t output_length_of(const std::string& name)
   {
   Algorithm_Factory& af = global_state().algorithm_factory();

   if(const HashFunction* hash = af.prototype_hash_function(name))
      return hash->output_length();

   if(const MessageAuthenticationCode* mac = af.prototype_mac(name))
      return mac->output_length();

   throw Algorithm_Not_Found(name);
   }

/*
* Get a cipher object
*/
Keyed_Filter* get_cipher(const std::string& algo_spec,
                         Cipher_Dir direction)
   {
   Algorithm_Factory& af = global_state().algorithm_factory();

   Algorithm_Factory::Engine_Iterator i(af);

   while(Engine* engine = i.next())
      {
      if(Keyed_Filter* algo = engine->get_cipher(algo_spec, direction, af))
         return algo;
      }

   throw Algorithm_Not_Found(algo_spec);
   }

/*
* Get a cipher object
*/
Keyed_Filter* get_cipher(const std::string& algo_spec,
                         const SymmetricKey& key,
                         const InitializationVector& iv,
                         Cipher_Dir direction)
   {
   Keyed_Filter* cipher = get_cipher(algo_spec, direction);
   cipher->set_key(key);

   if(iv.length())
      cipher->set_iv(iv);

   return cipher;
   }

/*
* Get a cipher object
*/
Keyed_Filter* get_cipher(const std::string& algo_spec,
                         const SymmetricKey& key,
                         Cipher_Dir direction)
   {
   return get_cipher(algo_spec,
                     key, InitializationVector(), direction);
   }

}
