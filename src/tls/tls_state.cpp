/*
* TLS Handshaking
* (C) 2004-2006,2011 Jack Lloyd
*
* Released under the terms of the Botan license
*/

#include <botan/internal/tls_state.h>

#include <stdio.h>

namespace Botan {

namespace {

u32bit bitmask_for_handshake_type(Handshake_Type type)
   {
   switch(type)
      {
      case HELLO_REQUEST:
         return (1 << 0);

      /*
      * Same code point for both client hello styles
      */
      case CLIENT_HELLO:
      case CLIENT_HELLO_SSLV2:
         return (1 << 1);

      case SERVER_HELLO:
         return (1 << 2);

      case CERTIFICATE:
         return (1 << 3);

      case SERVER_KEX:
         return (1 << 4);

      case CERTIFICATE_REQUEST:
         return (1 << 5);

      case SERVER_HELLO_DONE:
         return (1 << 6);

      case CERTIFICATE_VERIFY:
         return (1 << 7);

      case CLIENT_KEX:
         return (1 << 8);

      case FINISHED:
         return (1 << 9);

      case HANDSHAKE_CCS:
         return (1 << 10);

      // allow explicitly disabling new handshakes
      case HANDSHAKE_NONE:
         return 0;

      default:
         throw Internal_Error("Unknown handshake type " + to_string(type));
      }

   return 0;
   }

}

/*
* Initialize the SSL/TLS Handshake State
*/
Handshake_State::Handshake_State()
   {
   client_hello = 0;
   server_hello = 0;
   server_certs = 0;
   server_kex = 0;
   cert_req = 0;
   server_hello_done = 0;

   client_certs = 0;
   client_kex = 0;
   client_verify = 0;
   client_finished = 0;
   server_finished = 0;

   kex_pub = 0;
   kex_priv = 0;

   //do_client_auth = got_client_ccs = got_server_ccs = false;
   version = SSL_V3;

   hand_expecting_mask = 0;
   hand_received_mask = 0;
   }

void Handshake_State::confirm_transition_to(Handshake_Type handshake_msg)
   {
   const u32bit mask = bitmask_for_handshake_type(handshake_msg);

   hand_received_mask |= mask;

   const bool ok = (hand_expecting_mask & mask); // overlap?

   if(!ok)
      printf("Bad handshake transition, got %d expected %08X\n",
             handshake_msg, hand_expecting_mask);

   /* We don't know what to expect next, so force a call to
      set_expected_next; if it doesn't happen, the next transition
      check will always fail which is what we want.
   */
   hand_expecting_mask = 0;

   if(!ok)
      throw Unexpected_Message("Unexpected state transition in handshake");
   }

void Handshake_State::set_expected_next(Handshake_Type handshake_msg)
   {
   hand_expecting_mask |= bitmask_for_handshake_type(handshake_msg);
   }

bool Handshake_State::received_handshake_msg(Handshake_Type handshake_msg) const
   {
   const u32bit mask = bitmask_for_handshake_type(handshake_msg);

   return (hand_received_mask & mask);
   }

/*
* Destroy the SSL/TLS Handshake State
*/
Handshake_State::~Handshake_State()
   {
   delete client_hello;
   delete server_hello;
   delete server_certs;
   delete server_kex;
   delete cert_req;
   delete server_hello_done;

   delete client_certs;
   delete client_kex;
   delete client_verify;
   delete client_finished;
   delete server_finished;

   delete kex_pub;
   delete kex_priv;
   }

}