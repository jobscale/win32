const char filters_rcs[] = "";
/*********************************************************************
 *
 * File        :  $Source: /cvsroot/ijbswa/current/filters.c,v $
 *
 * Purpose     :  Declares functions to parse/crunch headers and pages.
 *                Functions declared include:
 *                   `acl_addr', `add_stats', `block_acl', `block_imageurl',
 *                   `block_url', `url_actions', `domain_split',
 *                   `filter_popups', `forward_url', 'redirect_url',
 *                   `ij_untrusted_url', `intercept_url', `pcrs_filter_respose',
 *                   `ijb_send_banner', `trust_url', `gif_deanimate_response',
 *                   `execute_single_pcrs_command', `rewrite_url',
 *                   `get_last_url'
 *
 * Copyright   :  Written by and Copyright (C) 2001-2010 the
 *                pAds team. /
 *
 *                Based on the Internet Junkbuster originally written
 *                by and Copyright (C) 1997 Anonymous Coders and
 *                Junkbusters Corporation.  http://www.junkbusters.com
 *
 *                This program is free software; you can redistribute it
 *                and/or modify it under the terms of the GNU General
 *                Public License as published by the Free Software
 *                Foundation; either version 2 of the License, or (at
 *                your option) any later version.
 *
 *                This program is distributed in the hope that it will
 *                be useful, but WITHOUT ANY WARRANTY; without even the
 *                implied warranty of MERCHANTABILITY or FITNESS FOR A
 *                PARTICULAR PURPOSE.  See the GNU General Public
 *                License for more details.
 *
 *                The GNU General Public License should be included with
 *                this file.  If not, you can view it at
 *                http://www.gnu.org/copyleft/gpl.html
 *                or write to the Free Software Foundation, Inc., 59
 *                Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *********************************************************************/


#include "config.h"

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#if !(defined(_WIN32) || defined(WINVER))
#ifndef __OS2__
#include <unistd.h>
#endif /* ndef __OS2__ */
#include <netinet/in.h>
#else
#include <winsock2.h>
#endif /* ndef _WIN32 */

#ifdef __OS2__
#include <utils.h>
#endif /* def __OS2__ */

#include "project.h"
#include "filters.h"
#include "encode.h"
#include "parsers.h"
#include "ssplit.h"
#include "errlog.h"
#include "jbsockets.h"
#include "miscutil.h"
#include "actions.h"
#include "cgi.h"
#include "list.h"
#include "deanimate.h"
#include "urlmatch.h"
#include "loaders.h"

#if defined(_WIN32) || defined(WINVER)
#include "win32.h"
#endif

const char filters_h_rcs[] = FILTERS_H_VERSION;

/* Fix a problem with Solaris.  There should be no effect on other
 * platforms.
 * Solaris's isspace() is a macro which uses it's argument directly
 * as an array index.  Therefore we need to make sure that high-bit
 * characters generate +ve values, and ideally we also want to make
 * the argument match the declared parameter type of "int".
 */
#define ijb_isdigit(__X) isdigit((int)(unsigned char)(__X))

typedef char *(*filter_function_ptr)(struct client_state *csp, char *hdr);
static filter_function_ptr get_filter_function(const struct client_state *csp);
static jb_err remove_chunked_transfer_coding(char *buffer, size_t *size);
static jb_err prepare_for_filtering(struct client_state *csp);

#ifdef FEATURE_ACL
#ifdef HAVE_RFC2553
/*********************************************************************
 *
 * Function    :  sockaddr_storage_to_ip
 *
 * Description :  Access internal structure of sockaddr_storage
 *
 * Parameters  :
 *          1  :  addr = socket address
 *          2  :  ip   = IP address as array of octets in network order
 *                       (it points into addr)
 *          3  :  len  = length of IP address in octets
 *          4  :  port = port number in network order;
 *
 * Returns     :  0 = no errror; -1 otherwise.
 *
 *********************************************************************/
static int sockaddr_storage_to_ip(const struct sockaddr_storage *addr,
                                  uint8_t **ip, unsigned int *len,
                                  in_port_t **port)
{
   if (NULL == addr)
   {
      return(-1);
   }

   switch (addr->ss_family)
   {
      case AF_INET:
         if (NULL != len)
         {
            *len = 4;
         }
         if (NULL != ip)
         {
            *ip = (uint8_t *)
               &(((struct sockaddr_in *)addr)->sin_addr.s_addr);
         }
         if (NULL != port)
         {
            *port = &((struct sockaddr_in *)addr)->sin_port;
         }
         break;

      case AF_INET6:
         if (NULL != len)
         {
            *len = 16;
         }
         if (NULL != ip)
         {
            *ip = ((struct sockaddr_in6 *)addr)->sin6_addr.s6_addr;
         }
         if (NULL != port)
         {
            *port = &((struct sockaddr_in6 *)addr)->sin6_port;
         }
         break;

      default:
         /* Unsupported address family */
         return(-1);
   }

   return(0);
}


/*********************************************************************
 *
 * Function    :  match_sockaddr
 *
 * Description :  Check whether address matches network (IP address and port)
 *
 * Parameters  :
 *          1  :  network = socket address of subnework
 *          2  :  netmask = network mask as socket address
 *          3  :  address = checked socket address against given network
 *
 * Returns     :  0 = doesn't match; 1 = does match
 *
 *********************************************************************/
static int match_sockaddr(const struct sockaddr_storage *network,
                          const struct sockaddr_storage *netmask,
                          const struct sockaddr_storage *address)
{
   uint8_t *network_addr, *netmask_addr, *address_addr;
   unsigned int addr_len;
   in_port_t *network_port, *netmask_port, *address_port;
   int i;

   if (network->ss_family != netmask->ss_family)
   {
      /* This should never happen */
      log_error(LOG_LEVEL_ERROR,
         "Internal error at %s:%llu: network and netmask differ in family",
         __FILE__, __LINE__);
      return 0;
   }

   sockaddr_storage_to_ip(network, &network_addr, &addr_len, &network_port);
   sockaddr_storage_to_ip(netmask, &netmask_addr, NULL, &netmask_port);
   sockaddr_storage_to_ip(address, &address_addr, NULL, &address_port);

   /* Check for family */
   if ((network->ss_family == AF_INET) && (address->ss_family == AF_INET6)
      && IN6_IS_ADDR_V4MAPPED((struct in6_addr *)address_addr))
   {
      /* Map AF_INET6 V4MAPPED address into AF_INET */
      address_addr += 12;
      addr_len = 4;
   }
   else if ((network->ss_family == AF_INET6) && (address->ss_family == AF_INET)
      && IN6_IS_ADDR_V4MAPPED((struct in6_addr *)network_addr))
   {
      /* Map AF_INET6 V4MAPPED network into AF_INET */
      network_addr += 12;
      netmask_addr += 12;
      addr_len = 4;
   }
   else if (network->ss_family != address->ss_family)
   {
      return 0;
   }

   /* XXX: Port check is signaled in netmask */
   if (*netmask_port && *network_port != *address_port)
   {
      return 0;
   }

   /* TODO: Optimize by checking by words insted of octets */
   for (i = 0; (i < addr_len) && netmask_addr[i]; i++)
   {
      if ((network_addr[i] & netmask_addr[i]) !=
          (address_addr[i] & netmask_addr[i]))
      {
         return 0;
      }
   }

   return 1;
}
#endif /* def HAVE_RFC2553 */


/*********************************************************************
 *
 * Function    :  block_acl
 *
 * Description :  Block this request?
 *                Decide yes or no based on ACL file.
 *
 * Parameters  :
 *          1  :  dst = The proxy or gateway address this is going to.
 *                      Or NULL to check all possible targets.
 *          2  :  csp = Current client state (buffers, headers, etc...)
 *                      Also includes the client IP address.
 *
 * Returns     : 0 = FALSE (don't block) and 1 = TRUE (do block)
 *
 *********************************************************************/
int block_acl(const struct access_control_addr *dst, const struct client_state *csp)
{
   struct access_control_list *acl = csp->config->acl;

   /* if not using an access control list, then permit the connection */
   if (acl == NULL)
   {
      return(0);
   }

   /* search the list */
   while (acl != NULL)
   {
      if (
#ifdef HAVE_RFC2553
            match_sockaddr(&acl->src->addr, &acl->src->mask, &csp->tcp_addr)
#else
            (csp->ip_addr_long & acl->src->mask) == acl->src->addr
#endif
            )
      {
         if (dst == NULL)
         {
            /* Just want to check if they have any access */
            if (acl->action == ACL_PERMIT)
            {
               return(0);
            }
            else
            {
               return(1);
            }
         }
         else if (
#ifdef HAVE_RFC2553
               /*
                * XXX: An undefined acl->dst is full of zeros and should be
                * considered a wildcard address. sockaddr_storage_to_ip()
                * fails on such destinations because of unknown sa_familly
                * (glibc only?). However this test is not portable.
                *
                * So, we signal the acl->dst is wildcard in wildcard_dst.
                */
               acl->wildcard_dst ||
                  match_sockaddr(&acl->dst->addr, &acl->dst->mask, &dst->addr)
#else
               ((dst->addr & acl->dst->mask) == acl->dst->addr)
           && ((dst->port == acl->dst->port) || (acl->dst->port == 0))
#endif
           )
         {
            if (acl->action == ACL_PERMIT)
            {
               return(0);
            }
            else
            {
               return(1);
            }
         }
      }
      acl = acl->next;
   }

   return(1);

}


/*********************************************************************
 *
 * Function    :  acl_addr
 *
 * Description :  Called from `load_config' to parse an ACL address.
 *
 * Parameters  :
 *          1  :  aspec = String specifying ACL address.
 *          2  :  aca = struct access_control_addr to fill in.
 *
 * Returns     :  0 => Ok, everything else is an error.
 *
 *********************************************************************/
int acl_addr(const char *aspec, struct access_control_addr *aca)
{
   int i, masklength;
#ifdef HAVE_RFC2553
   struct addrinfo hints, *result;
   uint8_t *mask_data;
   in_port_t *mask_port;
   unsigned int addr_len;
#else
   long port;
#endif /* def HAVE_RFC2553 */
   char *p;
   char *acl_spec = NULL;

#ifdef HAVE_RFC2553
   /* XXX: Depend on ai_family */
   masklength = 128;
#else
   masklength = 32;
   port       =  0;
#endif

   /*
    * Use a temporary acl spec copy so we can log
    * the unmodified original in case of parse errors.
    */
   acl_spec = strdup(aspec);
   if (acl_spec == NULL)
   {
      /* XXX: This will be logged as parse error. */
      return(-1);
   }

   if ((p = strchr(acl_spec, '/')) != NULL)
   {
      *p++ = '\0';
      if (ijb_isdigit(*p) == 0)
      {
         freez(acl_spec);
         return(-1);
      }
      masklength = atoi(p);
   }

   if ((masklength < 0) ||
#ifdef HAVE_RFC2553
         (masklength > 128)
#else
         (masklength > 32)
#endif
         )
   {
      freez(acl_spec);
      return(-1);
   }

   if ((*acl_spec == '[') && (NULL != (p = strchr(acl_spec, ']'))))
   {
      *p = '\0';
      memmove(acl_spec, acl_spec + 1, (size_t)(p - acl_spec));

      if (*++p != ':')
      {
         p = NULL;
      }
   }
   else
   {
      p = strchr(acl_spec, ':');
   }

#ifdef HAVE_RFC2553
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   i = getaddrinfo(acl_spec, ((p) ? ++p : NULL), &hints, &result);

   if (i != 0)
   {
      log_error(LOG_LEVEL_ERROR, "Can not resolve [%s]:%s: %s",
         acl_spec, p, gai_strerror(i));
      freez(acl_spec);
      return(-1);
   }
   freez(acl_spec);

   /* TODO: Allow multihomed hostnames */
   memcpy(&(aca->addr), result->ai_addr, result->ai_addrlen);
   freeaddrinfo(result);
#else
   if (p != NULL)
   {
      char *endptr;

      *p++ = '\0';
      port = strtol(p, &endptr, 10);

      if (port <= 0 || port > 65535 || *endptr != '\0')
      {
         freez(acl_spec);
         return(-1);
      }
   }

   aca->port = (unsigned long)port;

   aca->addr = ntohl(resolve_hostname_to_ip(acl_spec));
   freez(acl_spec);

   if (aca->addr == INADDR_NONE)
   {
      /* XXX: This will be logged as parse error. */
      return(-1);
   }
#endif /* def HAVE_RFC2553 */

   /* build the netmask */
#ifdef HAVE_RFC2553
   /* Clip masklength according to current family. */
   if ((aca->addr.ss_family == AF_INET) && (masklength > 32))
   {
      masklength = 32;
   }

   aca->mask.ss_family = aca->addr.ss_family;
   if (sockaddr_storage_to_ip(&aca->mask, &mask_data, &addr_len, &mask_port))
   {
      return(-1);
   }

   if (p)
   {
      /* ACL contains a port number, check ports in the future. */
      *mask_port = 1;
   }

   /*
    * XXX: This could be optimized to operate on whole words instead
    * of octets (128-bit CPU could do it in one iteration).
    */
   /*
    * Octets after prefix can be ommitted because of
    * previous initialization to zeros.
    */
   for (i = 0; (i < addr_len) && masklength; i++)
   {
      if (masklength >= 8)
      {
         mask_data[i] = 0xFF;
         masklength -= 8;
      }
      else
      {
         /*
          * XXX: This assumes MSB of octet is on the left side.
          * This should be true for all architectures or solved
          * by the link layer.
          */
         mask_data[i] = (uint8_t)~((1 << (8 - masklength)) - 1);
         masklength = 0;
      }
   }

#else
   aca->mask = 0;
   for (i=1; i <= masklength ; i++)
   {
      aca->mask |= (1U << (32 - i));
   }

   /* now mask off the host portion of the ip address
    * (i.e. save on the network portion of the address).
    */
   aca->addr = aca->addr & aca->mask;
#endif /* def HAVE_RFC2553 */

   return(0);

}
#endif /* def FEATURE_ACL */


/*********************************************************************
 *
 * Function    :  connect_port_is_forbidden
 *
 * Description :  Check to see if CONNECT requests to the destination
 *                port of this request are forbidden. The check is
 *                independend of the actual request method.
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *
 * Returns     :  True if yes, false otherwise.
 *
 *********************************************************************/
int connect_port_is_forbidden(const struct client_state *csp)
{
   return ((csp->action->flags & ACTION_LIMIT_CONNECT) &&
     !match_portlist(csp->action->string[ACTION_STRING_LIMIT_CONNECT],
        csp->http->port));
}


/*********************************************************************
 *
 * Function    :  block_url
 *
 * Description :  Called from `chat'.  Check to see if we need to block this.
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *
 * Returns     :  NULL => unblocked, else HTTP block response
 *
 *********************************************************************/
struct http_response *block_url(struct client_state *csp)
{
   struct http_response *rsp;
   const char *new_content_type = NULL;

   /*
    * If it's not blocked, don't block it ;-)
    */
   if ((csp->action->flags & ACTION_BLOCK) == 0)
   {
      return NULL;
   }
   if (csp->action->flags & ACTION_REDIRECT)
   {
      log_error(LOG_LEVEL_ERROR, "redirect{} overruled by block.");     
   }
   /*
    * Else, prepare a response
    */
   if (NULL == (rsp = alloc_http_response()))
   {
      return cgi_error_memory();
   }

   /*
    * If it's an image-url, send back an image or redirect
    * as specified by the relevant +image action
    */
#ifdef FEATURE_IMAGE_BLOCKING
   if (((csp->action->flags & ACTION_IMAGE_BLOCKER) != 0)
        && is_imageurl(csp))
   {
      char *p;
      /* determine HOW images should be blocked */
      p = csp->action->string[ACTION_STRING_IMAGE_BLOCKER];

      if(csp->action->flags & ACTION_HANDLE_AS_EMPTY_DOCUMENT)
      {
         log_error(LOG_LEVEL_ERROR, "handle-as-empty-document overruled by handle-as-image.");
      }

      /* and handle accordingly: */
      if ((p == NULL) || (0 == strcmpic(p, "pattern")))
      {
         rsp->status = strdup("403 Request blocked by pAds");
         if (rsp->status == NULL)
         {
            free_http_response(rsp);
            return cgi_error_memory();
         }
         rsp->body = bindup(image_pattern_data, image_pattern_length);
         if (rsp->body == NULL)
         {
            free_http_response(rsp);
            return cgi_error_memory();
         }
         rsp->content_length = image_pattern_length;

         if (enlist_unique_header(rsp->headers, "Content-Type", BUILTIN_IMAGE_MIMETYPE))
         {
            free_http_response(rsp);
            return cgi_error_memory();
         }
      }
      else if (0 == strcmpic(p, "blank"))
      {
         rsp->status = strdup("403 Request blocked by pAds");
         if (rsp->status == NULL)
         {
            free_http_response(rsp);
            return cgi_error_memory();
         }
         rsp->body = bindup(image_blank_data, image_blank_length);
         if (rsp->body == NULL)
         {
            free_http_response(rsp);
            return cgi_error_memory();
         }
         rsp->content_length = image_blank_length;

         if (enlist_unique_header(rsp->headers, "Content-Type", BUILTIN_IMAGE_MIMETYPE))
         {
            free_http_response(rsp);
            return cgi_error_memory();
         }
      }
      else
      {
         rsp->status = strdup("302 Local Redirect from pAds");
         if (rsp->status == NULL)
         {
            free_http_response(rsp);
            return cgi_error_memory();
         }

         if (enlist_unique_header(rsp->headers, "Location", p))
         {
            free_http_response(rsp);
            return cgi_error_memory();
         }
      }

   }
   else
#endif /* def FEATURE_IMAGE_BLOCKING */
   if(csp->action->flags & ACTION_HANDLE_AS_EMPTY_DOCUMENT)
   {
     /*
      *  Send empty document.               
      */
      new_content_type = csp->action->string[ACTION_STRING_CONTENT_TYPE];

      freez(rsp->body);
      rsp->body = strdup(" ");
      rsp->content_length = 1;

      if (csp->config->feature_flags & RUNTIME_FEATURE_EMPTY_DOC_RETURNS_OK)
      {
         /*
          * Workaround for firefox bug 492459
          *   https://bugzilla.mozilla.org/show_bug.cgi?id=492459
          * Return a 200 OK status for pages blocked with +handle-as-empty-document
          * if the "handle-as-empty-doc-returns-ok" runtime config option is set.
          */
         rsp->status = strdup("200 Request blocked by pAds");
      }
      else
      {
         rsp->status = strdup("403 Request blocked by pAds");
      }

      if (rsp->status == NULL)
      {
         free_http_response(rsp);
         return cgi_error_memory();
      }
      if (new_content_type != 0)
      {
         log_error(LOG_LEVEL_HEADER, "Overwriting Content-Type with %s", new_content_type);
         if (enlist_unique_header(rsp->headers, "Content-Type", new_content_type))
         {
            free_http_response(rsp);
            return cgi_error_memory();
         }
      }
   }
   else

   /*
    * Else, generate an HTML "blocked" message:
    */
   {
      jb_err err;
      struct map * exports;
      char *p;

      /*
       * Workaround for stupid Netscape bug which prevents
       * pages from being displayed if loading a referenced
       * JavaScript or style sheet fails. So make it appear
       * as if it succeeded.
       */
      if ( NULL != (p = get_header_value(csp->headers, "User-Agent:"))
           && !strncmpic(p, "mozilla", 7) /* Catch Netscape but */
           && !strstr(p, "Gecko")         /* save Mozilla, */
           && !strstr(p, "compatible")    /* MSIE */
           && !strstr(p, "Opera"))        /* and Opera. */
      {
         rsp->status = strdup("200 Request for blocked URL");
      }
      else
      {
         rsp->status = strdup("403 Request for blocked URL");
      }

      if (rsp->status == NULL)
      {
         free_http_response(rsp);
         return cgi_error_memory();
      }

      exports = default_exports(csp, NULL);
      if (exports == NULL)
      {
         free_http_response(rsp);
         return cgi_error_memory();
      }

#ifdef FEATURE_FORCE_LOAD
      err = map(exports, "force-prefix", 1, FORCE_PREFIX, 1);
      /*
       * Export the force conditional block killer if
       *
       * - pAds was compiled without FEATURE_FORCE_LOAD, or
       * - pAds is configured to enforce blocks, or
       * - it's a CONNECT request and enforcing wouldn't work anyway.
       */
      if ((csp->config->feature_flags & RUNTIME_FEATURE_ENFORCE_BLOCKS)
       || (0 == strcmpic(csp->http->gpc, "connect")))
#endif /* ndef FEATURE_FORCE_LOAD */
      {
         err = map_block_killer(exports, "force-support");
      }

      if (!err) err = map(exports, "protocol", 1, csp->http->ssl ? "https://" : "http://", 1);
      if (!err) err = map(exports, "hostport", 1, html_encode(csp->http->hostport), 0);
      if (!err) err = map(exports, "path", 1, html_encode(csp->http->path), 0);
      if (!err) err = map(exports, "path-ue", 1, url_encode(csp->http->path), 0);
      if (!err)
      {
         const char *block_reason;
         if (csp->action->string[ACTION_STRING_BLOCK] != NULL)
         {
            block_reason = csp->action->string[ACTION_STRING_BLOCK];
         }
         else
         {
            assert(connect_port_is_forbidden(csp));
            block_reason = "Forbidden CONNECT port.";
         }
         err = map(exports, "block-reason", 1, html_encode(block_reason), 0);
      }
      if (err)
      {
         free_map(exports);
         free_http_response(rsp);
         return cgi_error_memory();
      }

      err = template_fill_for_cgi(csp, "blocked", exports, rsp);
      if (err)
      {
         free_http_response(rsp);
         return cgi_error_memory();
      }
   }
   rsp->crunch_reason = BLOCKED;

   return finish_http_response(csp, rsp);
}


#ifdef FEATURE_TRUST
/*********************************************************************
 *
 * Function    :  trust_url FIXME: I should be called distrust_url
 *
 * Description :  Calls is_untrusted_url to determine if the URL is trusted
 *                and if not, returns a HTTP 403 response with a reject message.
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *
 * Returns     :  NULL => trusted, else http_response.
 *
 *********************************************************************/
struct http_response *trust_url(struct client_state *csp)
{
   struct http_response *rsp;
   struct map * exports;
   char buf[BUFFER_SIZE];
   char *p;
   struct url_spec **tl;
   struct url_spec *t;
   jb_err err;

   /*
    * Don't bother to work on trusted URLs
    */
   if (!is_untrusted_url(csp))
   {
      return NULL;
   }

   /*
    * Else, prepare a response:
    */
   if (NULL == (rsp = alloc_http_response()))
   {
      return cgi_error_memory();
   }

   rsp->status = strdup("403 Request blocked by pAds");
   exports = default_exports(csp, NULL);
   if (exports == NULL || rsp->status == NULL)
   {
      free_http_response(rsp);
      return cgi_error_memory();
   }

   /*
    * Export the protocol, host, port, and referrer information
    */
   err = map(exports, "hostport", 1, csp->http->hostport, 1);
   if (!err) err = map(exports, "protocol", 1, csp->http->ssl ? "https://" : "http://", 1); 
   if (!err) err = map(exports, "path", 1, csp->http->path, 1);

   if (NULL != (p = get_header_value(csp->headers, "Referer:")))
   {
      if (!err) err = map(exports, "referrer", 1, html_encode(p), 0);
   }
   else
   {
      if (!err) err = map(exports, "referrer", 1, "none set", 1);
   }

   if (err)
   {
      free_map(exports);
      free_http_response(rsp);
      return cgi_error_memory();
   }

   /*
    * Export the trust list
    */
   p = strdup("");
   for (tl = csp->config->trust_list; (t = *tl) != NULL ; tl++)
   {
      snprintf(buf, sizeof(buf), "<li>%s</li>\n", t->spec);
      string_append(&p, buf);
   }
   err = map(exports, "trusted-referrers", 1, p, 0);

   if (err)
   {
      free_map(exports);
      free_http_response(rsp);
      return cgi_error_memory();
   }

   /*
    * Export the trust info, if available
    */
   if (csp->config->trust_info->first)
   {
      struct list_entry *l;

      p = strdup("");
      for (l = csp->config->trust_info->first; l ; l = l->next)
      {
         snprintf(buf, sizeof(buf), "<li> <a href=\"%s\">%s</a><br>\n", l->str, l->str);
         string_append(&p, buf);
      }
      err = map(exports, "trust-info", 1, p, 0);
   }
   else
   {
      err = map_block_killer(exports, "have-trust-info");
   }

   if (err)
   {
      free_map(exports);
      free_http_response(rsp);
      return cgi_error_memory();
   }

   /*
    * Export the force conditional block killer if
    *
    * - pAds was compiled without FEATURE_FORCE_LOAD, or
    * - pAds is configured to enforce blocks, or
    * - it's a CONNECT request and enforcing wouldn't work anyway.
    */
#ifdef FEATURE_FORCE_LOAD
   if ((csp->config->feature_flags & RUNTIME_FEATURE_ENFORCE_BLOCKS)
    || (0 == strcmpic(csp->http->gpc, "connect")))
   {
      err = map_block_killer(exports, "force-support");
   }
   else
   {
      err = map(exports, "force-prefix", 1, FORCE_PREFIX, 1);
   }
#else /* ifndef FEATURE_FORCE_LOAD */
   err = map_block_killer(exports, "force-support");
#endif /* ndef FEATURE_FORCE_LOAD */

   if (err)
   {
      free_map(exports);
      free_http_response(rsp);
      return cgi_error_memory();
   }

   /*
    * Build the response
    */
   err = template_fill_for_cgi(csp, "untrusted", exports, rsp);
   if (err)
   {
      free_http_response(rsp);
      return cgi_error_memory();
   }
   rsp->crunch_reason = UNTRUSTED;

   return finish_http_response(csp, rsp);
}
#endif /* def FEATURE_TRUST */


/*********************************************************************
 *
 * Function    :  compile_dynamic_pcrs_job_list
 *
 * Description :  Compiles a dynamic pcrs job list (one with variables
 *                resolved at request time)
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *          2  :  b = The filter list to compile
 *
 * Returns     :  NULL in case of errors, otherwise the
 *                pcrs job list.  
 *
 *********************************************************************/
pcrs_job *compile_dynamic_pcrs_job_list(const struct client_state *csp, const struct re_filterfile_spec *b)
{
   struct list_entry *pattern;
   pcrs_job *job_list = NULL;
   pcrs_job *dummy = NULL;
   pcrs_job *lastjob = NULL;
   int error = 0;

   const struct pcrs_variable variables[] =
   {
      {"url",    csp->http->url,   1},
      {"path",   csp->http->path,  1},
      {"host",   csp->http->host,  1},
      {"origin", csp->ip_addr_str, 1},
      {NULL,     NULL,             1}
   };

   for (pattern = b->patterns->first; pattern != NULL; pattern = pattern->next)
   {
      assert(pattern->str != NULL);

      dummy = pcrs_compile_dynamic_command(pattern->str, variables, &error);
      if (NULL == dummy)
      {
         assert(error < 0);
         log_error(LOG_LEVEL_ERROR,
            "Adding filter job \'%s\' to dynamic filter %s failed: %s",
            pattern->str, b->name, pcrs_strerror(error));
         continue;
      }
      else
      {
         if (error == PCRS_WARN_TRUNCATION)
         {
            log_error(LOG_LEVEL_ERROR,
               "At least one of the variables in \'%s\' had to "
               "be truncated before compilation", pattern->str);
         }
         if (job_list == NULL)
         {
            job_list = dummy;
         }
         else
         {
            lastjob->next = dummy;
         }
         lastjob = dummy;
      }
   }

   return job_list;
}


/*********************************************************************
 *
 * Function    :  rewrite_url
 *
 * Description :  Rewrites a URL with a single pcrs command
 *                and returns the result if it differs from the
 *                original and isn't obviously invalid.
 *
 * Parameters  :
 *          1  :  old_url = URL to rewrite.
 *          2  :  pcrs_command = pcrs command formatted as string (s@foo@bar@)
 *
 *
 * Returns     :  NULL if the pcrs_command didn't change the url, or 
 *                the result of the modification.
 *
 *********************************************************************/
char *rewrite_url(char *old_url, const char *pcrs_command)
{
   char *new_url = NULL;
   int hits;

   assert(old_url);
   assert(pcrs_command);

   new_url = pcrs_execute_single_command(old_url, pcrs_command, &hits);

   if (hits == 0)
   {
      log_error(LOG_LEVEL_REDIRECTS,
         "pcrs command \"%s\" didn't change \"%s\".",
         pcrs_command, old_url);
      freez(new_url);
   }
   else if (hits < 0)
   {
      log_error(LOG_LEVEL_REDIRECTS,
         "executing pcrs command \"%s\" to rewrite %s failed: %s",
         pcrs_command, old_url, pcrs_strerror(hits));
      freez(new_url);
   }
   else if (strncmpic(new_url, "http://", 7) && strncmpic(new_url, "https://", 8))
   {
      log_error(LOG_LEVEL_ERROR,
         "pcrs command \"%s\" changed \"%s\" to \"%s\" (%u hi%s), "
         "but the result doesn't look like a valid URL and will be ignored.",
         pcrs_command, old_url, new_url, hits, (hits == 1) ? "t" : "ts");
      freez(new_url);
   }
   else
   {
      log_error(LOG_LEVEL_REDIRECTS,
         "pcrs command \"%s\" changed \"%s\" to \"%s\" (%u hi%s).",
         pcrs_command, old_url, new_url, hits, (hits == 1) ? "t" : "ts");
   }

   return new_url;

}


#ifdef FEATURE_FAST_REDIRECTS
/*********************************************************************
 *
 * Function    :  get_last_url
 *
 * Description :  Search for the last URL inside a string.
 *                If the string already is a URL, it will
 *                be the first URL found.
 *
 * Parameters  :
 *          1  :  subject = the string to check
 *          2  :  redirect_mode = +fast-redirect{} mode 
 *
 * Returns     :  NULL if no URL was found, or
 *                the last URL found.
 *
 *********************************************************************/
char *get_last_url(char *subject, const char *redirect_mode)
{
   char *new_url = NULL;
   char *tmp;

   assert(subject);
   assert(redirect_mode);

   subject = strdup(subject);
   if (subject == NULL)
   {
      log_error(LOG_LEVEL_ERROR, "Out of memory while searching for redirects.");
      return NULL;
   }

   if (0 == strcmpic(redirect_mode, "check-decoded-url"))
   {  
      log_error(LOG_LEVEL_REDIRECTS, "Decoding \"%s\" if necessary.", subject);
      new_url = url_decode(subject);
      if (new_url != NULL)
      {
         freez(subject);
         subject = new_url;
      }
      else
      {
         log_error(LOG_LEVEL_ERROR, "Unable to decode \"%s\".", subject);
      }
   }

   log_error(LOG_LEVEL_REDIRECTS, "Checking \"%s\" for redirects.", subject);

   /*
    * Find the last URL encoded in the request
    */
   tmp = subject;
   while ((tmp = strstr(tmp, "http://")) != NULL)
   {
      new_url = tmp++;
   }
   tmp = (new_url != NULL) ? new_url : subject;
   while ((tmp = strstr(tmp, "https://")) != NULL)
   {
      new_url = tmp++;
   }

   if ((new_url != NULL)
      && (  (new_url != subject)
         || (0 == strncmpic(subject, "http://", 7))
         || (0 == strncmpic(subject, "https://", 8))
         ))
   {
      /*
       * Return new URL if we found a redirect 
       * or if the subject already was a URL.
       *
       * The second case makes sure that we can
       * chain get_last_url after another redirection check
       * (like rewrite_url) without losing earlier redirects.
       */
      new_url = strdup(new_url);
      freez(subject);
      return new_url;
   }

   freez(subject);
   return NULL;

}
#endif /* def FEATURE_FAST_REDIRECTS */


/*********************************************************************
 *
 * Function    :  redirect_url
 *
 * Description :  Checks if pAds should answer the request with
 *                a HTTP redirect and generates the redirect if
 *                necessary.
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *
 * Returns     :  NULL if the request can pass, HTTP redirect otherwise.
 *
 *********************************************************************/
struct http_response *redirect_url(struct client_state *csp)
{
   struct http_response *rsp;
#ifdef FEATURE_FAST_REDIRECTS
   /*
    * XXX: Do we still need FEATURE_FAST_REDIRECTS
    * as compile-time option? The user can easily disable
    * it in his action file.
    */
   char * redirect_mode;
#endif /* def FEATURE_FAST_REDIRECTS */
   char *old_url = NULL;
   char *new_url = NULL;
   char *redirection_string;

   if ((csp->action->flags & ACTION_REDIRECT))
   {
      redirection_string = csp->action->string[ACTION_STRING_REDIRECT];

      /*
       * If the redirection string begins with 's',
       * assume it's a pcrs command, otherwise treat it as
       * properly formatted URL and use it for the redirection
       * directly.
       *
       * According to RFC 2616 section 14.30 the URL
       * has to be absolute and if the user tries:
       * +redirect{shit/this/will/be/parsed/as/pcrs_command.html}
       * she would get undefined results anyway.
       *
       */

      if (*redirection_string == 's')
      {
         old_url = csp->http->url;
         new_url = rewrite_url(old_url, redirection_string);
      }
      else
      {
         log_error(LOG_LEVEL_REDIRECTS,
            "No pcrs command recognized, assuming that \"%s\" is already properly formatted.",
            redirection_string);
         new_url = strdup(redirection_string);
      }
   }

#ifdef FEATURE_FAST_REDIRECTS
   if ((csp->action->flags & ACTION_FAST_REDIRECTS))
   {
      redirect_mode = csp->action->string[ACTION_STRING_FAST_REDIRECTS];

      /*
       * If it exists, use the previously rewritten URL as input
       * otherwise just use the old path.
       */
      old_url = (new_url != NULL) ? new_url : strdup(csp->http->path);
      new_url = get_last_url(old_url, redirect_mode);
      freez(old_url);
   }

   /*
    * Disable redirect checkers, so that they
    * will be only run more than once if the user
    * also enables them through tags.
    *
    * From a performance point of view
    * it doesn't matter, but the duplicated
    * log messages are annoying.
    */
   csp->action->flags &= ~ACTION_FAST_REDIRECTS;
#endif /* def FEATURE_FAST_REDIRECTS */
   csp->action->flags &= ~ACTION_REDIRECT;

   /* Did any redirect action trigger? */   
   if (new_url)
   {
      if (0 == strcmpic(new_url, csp->http->url))
      {
         log_error(LOG_LEVEL_ERROR,
            "New URL \"%s\" and old URL \"%s\" are the same. Redirection loop prevented.",
            csp->http->url, new_url);
            freez(new_url);
      }
      else
      {
         log_error(LOG_LEVEL_REDIRECTS, "New URL is: %s", new_url);

         if (NULL == (rsp = alloc_http_response()))
         {
            freez(new_url);
            return cgi_error_memory();
         }

         if ( enlist_unique_header(rsp->headers, "Location", new_url)
           || (NULL == (rsp->status = strdup("302 Local Redirect from pAds"))) )
         {
            freez(new_url);
            free_http_response(rsp);
            return cgi_error_memory();
         }
         rsp->crunch_reason = REDIRECTED;
         freez(new_url);

         return finish_http_response(csp, rsp);
      }
   }

   /* Only reached if no redirect is required */
   return NULL;
}


#ifdef FEATURE_IMAGE_BLOCKING
/*********************************************************************
 *
 * Function    :  is_imageurl
 *
 * Description :  Given a URL, decide whether it is an image or not,
 *                using either the info from a previous +image action
 *                or, #ifdef FEATURE_IMAGE_DETECT_MSIE, and the browser
 *                is MSIE and not on a Mac, tell from the browser's accept
 *                header.
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *
 * Returns     :  True (nonzero) if URL is an image, false (0)
 *                otherwise
 *
 *********************************************************************/
int is_imageurl(const struct client_state *csp)
{
#ifdef FEATURE_IMAGE_DETECT_MSIE
   char *tmp;

   tmp = get_header_value(csp->headers, "User-Agent:");
   if (tmp && strstr(tmp, "MSIE") && !strstr(tmp, "Mac_"))
   {
      tmp = get_header_value(csp->headers, "Accept:");
      if (tmp && strstr(tmp, "image/gif"))
      {
         /* Client will accept HTML.  If this seems counterintuitive,
          * blame Microsoft.
          */
         return(0);
      }
      else
      {
         return(1);
      }
   }
#endif /* def FEATURE_IMAGE_DETECT_MSIE */

   return ((csp->action->flags & ACTION_IMAGE) != 0);

}
#endif /* def FEATURE_IMAGE_BLOCKING */


#ifdef FEATURE_TRUST
/*********************************************************************
 *
 * Function    :  is_untrusted_url
 *
 * Description :  Should we "distrust" this URL (and block it)?
 *
 *                Yes if it matches a line in the trustfile, or if the
 *                    referrer matches a line starting with "+" in the
 *                    trustfile.
 *                No  otherwise.
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *
 * Returns     :  0 => trusted, 1 => untrusted
 *
 *********************************************************************/
int is_untrusted_url(const struct client_state *csp)
{
   struct file_list *fl;
   struct block_spec *b;
   struct url_spec **trusted_url;
   struct http_request rhttp[1];
   const char * referer;
   jb_err err;

   /*
    * If we don't have a trustlist, we trust everybody
    */
   if (((fl = csp->tlist) == NULL) || ((b  = fl->f) == NULL))
   {
      return 0;
   }

   memset(rhttp, '\0', sizeof(*rhttp));

   /*
    * Do we trust the request URL itself?
    */
   for (b = b->next; b ; b = b->next)
   {
      if (url_match(b->url, csp->http))
      {
         return b->reject;
      }
   }

   if (NULL == (referer = get_header_value(csp->headers, "Referer:")))
   {
      /* no referrer was supplied */
      return 1;
   }


   /*
    * If not, do we maybe trust its referrer?
    */
   err = parse_http_url(referer, rhttp, REQUIRE_PROTOCOL);
   if (err)
   {
      return 1;
   }

   for (trusted_url = csp->config->trust_list; *trusted_url != NULL; trusted_url++)
   {
      if (url_match(*trusted_url, rhttp))
      {
         /* if the URL's referrer is from a trusted referrer, then
          * add the target spec to the trustfile as an unblocked
          * domain and return 0 (which means it's OK).
          */

         FILE *fp;

         if (NULL != (fp = fopen(csp->config->trustfile, "a")))
         {
            char * path;
            char * path_end;
            char * new_entry = strdup("~");

            string_append(&new_entry, csp->http->hostport);

            path = csp->http->path;
            if ( (path[0] == '/')
              && (path[1] == '~')
              && ((path_end = strchr(path + 2, '/')) != NULL))
            {
               /* since this path points into a user's home space
                * be sure to include this spec in the trustfile.
                */
               long path_len = path_end - path; /* save offset */
               path = strdup(path); /* Copy string */
               if (path != NULL)
               {
                  path_end = path + path_len; /* regenerate ptr to new buffer */
                  *(path_end + 1) = '\0'; /* Truncate path after '/' */
               }
               string_join(&new_entry, path);
            }

            /*
             * Give a reason for generating this entry.
             */
            string_append(&new_entry, " # Trusted referrer was: ");
            string_append(&new_entry, referer);

            if (new_entry != NULL)
            {
               if (-1 == fprintf(fp, "%s\n", new_entry))
               {
                  log_error(LOG_LEVEL_ERROR, "Failed to append \'%s\' to trustfile \'%s\': %E",
                     new_entry, csp->config->trustfile);
               }
               freez(new_entry);
            }
            else
            {
               /* FIXME: No way to handle out-of memory, so mostly ignoring it */
               log_error(LOG_LEVEL_ERROR, "Out of memory adding pattern to trust file");
            }

            if (fp) fclose(fp);
         }
         else
         {
            log_error(LOG_LEVEL_ERROR, "Failed to append new entry for \'%s\' to trustfile \'%s\': %E",
               csp->http->hostport, csp->config->trustfile);
         }
         return 0;
      }
   }

   return 1;
}
#endif /* def FEATURE_TRUST */


/*********************************************************************
 *
 * Function    :  pcrs_filter_response
 *
 * Description :  Execute all text substitutions from all applying
 *                +filter actions on the text buffer that's been
 *                accumulated in csp->iob->buf.
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *
 * Returns     :  a pointer to the (newly allocated) modified buffer.
 *                or NULL if there were no hits or something went wrong
 *
 *********************************************************************/
static char *pcrs_filter_response(struct client_state *csp, char *hdr)
{
   int hits = 0;
   int i;
   size_t size, prev_size;

   char *old = NULL;
   char *new = NULL;
   pcrs_job *job;

   struct file_list *fl;
   struct re_filterfile_spec *b;
   struct list_entry *filtername;

   /* 
    * Sanity first
    */
   if (csp->iob->cur >= csp->iob->eod)
   {
      return(NULL);
   }

   if (filters_available(csp) == FALSE)
   {
      log_error(LOG_LEVEL_ERROR, "Inconsistent configuration: "
         "content filtering enabled, but no content filters available.");
      return(NULL);
   }

   size = (size_t)(csp->iob->eod - csp->iob->cur);
   old = csp->iob->cur;

   int info[1];
   info[0] = 0;

   struct utl_trance utl;
   utl.dat = malloc(1024);
   if (utl.dat == NULL)
   {
      return(NULL);
   }
   utl.len = 1023;
   utl.reserve = info;

   int filter_url(const struct client_state *csp, const char *hdr, const char *buf, void* dat);
   filter_url(csp, hdr, NULL, &utl);

   for (i = 0; i < MAX_AF_FILES; i++)
   {
     fl = csp->rlist[i];
     if ((NULL == fl) || (NULL == fl->f))
     {
        /*
         * Either there are no filter files
         * left, or this filter file just
         * contains no valid filters.
         *
         * Continue to be sure we don't miss
         * valid filter files that are chained
         * after empty or invalid ones.
         */
        continue;
     }
   /*
    * For all applying +filter actions, look if a filter by that
    * name exists and if yes, execute it's pcrs_joblist on the
    * buffer.
    */
   for (b = fl->f; b; b = b->next)
   {
      if (b->type != FT_CONTENT_FILTER)
      {
         /* Skip header filters */
         continue;
      }

      for (filtername = csp->action->multi[ACTION_MULTI_FILTER]->first;
           filtername ; filtername = filtername->next)
      {
         if (strcmp(b->name, filtername->str) == 0)
         {
            int current_hits = 0; /* Number of hits caused by this filter */
            int job_number   = 0; /* Which job we're currently executing  */
            int job_hits     = 0; /* How many hits the current job caused */

            pcrs_job *joblist = b->joblist;

            if (b->dynamic) joblist = compile_dynamic_pcrs_job_list(csp, b);

            if (NULL == joblist)
            {
               log_error(LOG_LEVEL_RE_FILTER, "Filter %s has empty joblist. Nothing to do.", b->name);
               continue;
            }

			if (info[0] == -1)
			{
				if (strncmp(b->name, "pad", 3) == 0)
				{
					log_error(LOG_LEVEL_RE_FILTER, "Filter %s pad skip do.", b->name);
					continue;
				}
				else if (strncmp(b->name, "adl", 3) == 0)
				{
					log_error(LOG_LEVEL_RE_FILTER, "Filter %s ero skip do.", b->name);
					continue;
				}
			}
			else if (info[0])
			{
				if (strncmp(b->name, "pad", 3) == 0)
				{
					log_error(LOG_LEVEL_RE_FILTER, "Filter %s pad skip do.", b->name);
					continue;
				}
			}
			else
			{
				if (strncmp(b->name, "adl", 3) == 0)
				{
					log_error(LOG_LEVEL_RE_FILTER, "Filter %s ero skip do.", b->name);
					continue;
				}
			}

            prev_size = size;
            /* Apply all jobs from the joblist */
            for (job = joblist; NULL != job; job = job->next)
            {
               job_number++;
               job_hits = pcrs_execute(job, old, size, &new, &size);

               if (job_hits >= 0)
               {
                  /*
                   * That went well. Continue filtering
                   * and use the result of this job as
                   * input for the next one.
                   */
                  current_hits += job_hits;
                  if (old != csp->iob->cur)
                  {
                     freez(old);
                  }
                  old = new;
               }
               else
               {
                  /*
                   * This job caused an unexpected error. Inform the user
                   * and skip the rest of the jobs in this filter. We could
                   * continue with the next job, but usually the jobs
                   * depend on each other or are similar enough to
                   * fail for the same reason.
                   *
                   * At the moment our pcrs expects the error codes of pcre 3.4,
                   * but newer pcre versions can return additional error codes.
                   * As a result pcrs_strerror()'s error message might be
                   * "Unknown error ...", therefore we print the numerical value
                   * as well.
                   *
                   * XXX: Is this important enough for LOG_LEVEL_ERROR or
                   * should we use LOG_LEVEL_RE_FILTER instead?
                   */
                  log_error(LOG_LEVEL_ERROR, "Skipped filter \'%s\' after job number %u: %s (%d)",
                     b->name, job_number, pcrs_strerror(job_hits), job_hits);
                  break;
               }
            }

            if (b->dynamic) pcrs_free_joblist(joblist);

            log_error(LOG_LEVEL_RE_FILTER,
               "filtering %s%s (size %d) with \'%s\' produced %d hits (new size %d).",
               csp->http->hostport, csp->http->path, prev_size, b->name, current_hits, size);

            hits += current_hits;
         }
      }
   }
   }

   /*
    * If there were no hits, destroy our copy and let
    * chat() use the original in csp->iob
    */
   if (!hits)
   {
      freez(new);
		free(utl.dat);
      return(NULL);
   }

   csp->flags |= CSP_FLAG_MODIFIED;
   csp->content_length = size;
   IOB_RESET(csp);

	char* pqa = strstr(new, "http://pad.plustar.jp/plustar-anal?0123456789");
	if (pqa)
	{
		pqa += 35;
		int length = csp->content_length;
		int left = pqa - new;
		int right = length - left;
		int len = strlen(utl.dat);
		memcpy(pqa, utl.dat, len);
		memcpy(pqa + len, pqa + 200, right - 200);

		csp->content_length -= (200 - len);
	}

	free(utl.dat);

   return(new);

}


/*********************************************************************
 *
 * Function    :  gif_deanimate_response
 *
 * Description :  Deanimate the GIF image that has been accumulated in
 *                csp->iob->buf, set csp->content_length to the modified
 *                size and raise the CSP_FLAG_MODIFIED flag.
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *
 * Returns     :  a pointer to the (newly allocated) modified buffer.
 *                or NULL in case something went wrong.
 *
 *********************************************************************/
static char *gif_deanimate_response(struct client_state *csp, char *hdr)
{
   struct binbuffer *in, *out;
   char *p;
   size_t size;

   size = (size_t)(csp->iob->eod - csp->iob->cur);

   if (  (NULL == (in =  (struct binbuffer *)zalloc(sizeof *in )))
      || (NULL == (out = (struct binbuffer *)zalloc(sizeof *out))) )
   {
      log_error(LOG_LEVEL_DEANIMATE, "failed! (no mem)");
      return NULL;
   }

   in->buffer = csp->iob->cur;
   in->size = size;

   if (gif_deanimate(in, out, strncmp("last", csp->action->string[ACTION_STRING_DEANIMATE], 4)))
   {
      log_error(LOG_LEVEL_DEANIMATE, "failed! (gif parsing)");
      freez(in);
      buf_free(out);
      return(NULL);
   }
   else
   {
      if ((int)size == out->offset)
      {
         log_error(LOG_LEVEL_DEANIMATE, "GIF not changed.");
      }
      else
      {
         log_error(LOG_LEVEL_DEANIMATE, "Success! GIF shrunk from %d bytes to %d.", size, out->offset);
      }
      csp->content_length = out->offset;
      csp->flags |= CSP_FLAG_MODIFIED;
      p = out->buffer;
      freez(in);
      freez(out);
      return(p);
   }

}


/*********************************************************************
 *
 * Function    :  get_filter_function
 *
 * Description :  Decides which content filter function has
 *                to be applied (if any).
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *
 * Returns     :  The content filter function to run, or
 *                NULL if no content filter is active
 *
 *********************************************************************/
static filter_function_ptr get_filter_function(const struct client_state *csp)
{
   filter_function_ptr filter_function = NULL;

   /*
    * Choose the applying filter function based on
    * the content type and action settings.
    */
   if ((csp->content_type & CT_TEXT) &&
       (csp->rlist != NULL) &&
       (!list_is_empty(csp->action->multi[ACTION_MULTI_FILTER])))
   {
      filter_function = pcrs_filter_response;
   }
   else if ((csp->content_type & CT_GIF)  &&
            (csp->action->flags & ACTION_DEANIMATE))
   {
      filter_function = gif_deanimate_response;
   }

   return filter_function;
}


/*********************************************************************
 *
 * Function    :  remove_chunked_transfer_coding
 *
 * Description :  In-situ remove the "chunked" transfer coding as defined
 *                in rfc2616 from a buffer.
 *
 * Parameters  :
 *          1  :  buffer = Pointer to the text buffer
 *          2  :  size =  In: Number of bytes to be processed,
 *                       Out: Number of bytes after de-chunking.
 *                       (undefined in case of errors)
 *
 * Returns     :  JB_ERR_OK for success,
 *                JB_ERR_PARSE otherwise
 *
 *********************************************************************/
static jb_err remove_chunked_transfer_coding(char *buffer, size_t *size)
{
   size_t newsize = 0;
   unsigned int chunksize = 0;
   char *from_p, *to_p;

   assert(buffer);
   from_p = to_p = buffer;

   if (sscanf(buffer, "%x", &chunksize) != 1)
   {
      log_error(LOG_LEVEL_ERROR, "Invalid first chunksize while stripping \"chunked\" transfer coding");
      return JB_ERR_PARSE;
   }

   while (chunksize > 0U)
   {
      if (NULL == (from_p = strstr(from_p, "\r\n")))
      {
         log_error(LOG_LEVEL_ERROR, "Parse error while stripping \"chunked\" transfer coding");
         return JB_ERR_PARSE;
      }

      if ((newsize += chunksize) >= *size)
      {
         /*
          * XXX: The message is a bit confusing. Isn't the real problem that
          *      the specified chunk size is greater than the number of bytes
          *      left in the buffer? This probably means the connection got
          *      closed prematurely. To be investigated after VERSION is out.
          */
         log_error(LOG_LEVEL_ERROR,
            "Chunk size %d exceeds buffer size %d in \"chunked\" transfer coding",
            chunksize, *size);
         return JB_ERR_PARSE;
      }
      from_p += 2;

      memmove(to_p, from_p, (size_t) chunksize);
      to_p = buffer + newsize;
      from_p += chunksize + 2;

      if (sscanf(from_p, "%x", &chunksize) != 1)
      {
         log_error(LOG_LEVEL_INFO, "Invalid \"chunked\" transfer encoding detected and ignored.");
         break;
      }
   }
   
   /* XXX: Should get its own loglevel. */
   log_error(LOG_LEVEL_RE_FILTER, "De-chunking successful. Shrunk from %d to %d", *size, newsize);

   *size = newsize;

   return JB_ERR_OK;

}


/*********************************************************************
 *
 * Function    :  prepare_for_filtering
 *
 * Description :  If necessary, de-chunks and decompresses
 *                the content so it can get filterd.
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *
 * Returns     :  JB_ERR_OK for success,
 *                JB_ERR_PARSE otherwise
 *
 *********************************************************************/
static jb_err prepare_for_filtering(struct client_state *csp)
{
   jb_err err = JB_ERR_OK;

   /*
    * If the body has a "chunked" transfer-encoding,
    * get rid of it, adjusting size and iob->eod
    */
   if (csp->flags & CSP_FLAG_CHUNKED)
   {
      size_t size = (size_t)(csp->iob->eod - csp->iob->cur);

      log_error(LOG_LEVEL_RE_FILTER, "Need to de-chunk first");
      err = remove_chunked_transfer_coding(csp->iob->cur, &size);
      if (JB_ERR_OK == err)
      {
         csp->iob->eod = csp->iob->cur + size;
         csp->flags |= CSP_FLAG_MODIFIED;
      }
      else
      {
         return JB_ERR_PARSE;
      }
   }

#ifdef FEATURE_ZLIB

   /*
    * If the body has a supported transfer-encoding,
    * decompress it, adjusting size and iob->eod.
    */
   if (csp->content_type & (CT_GZIP|CT_DEFLATE))
   {
      if (0 == csp->iob->eod - csp->iob->cur)
      {
         /* Nothing left after de-chunking. */
         return JB_ERR_OK;
      }

      err = decompress_iob(csp);

      if (JB_ERR_OK == err)
      {
         csp->flags |= CSP_FLAG_MODIFIED;
         csp->content_type &= ~CT_TABOO;
      }
      else
      {
         /*
          * Unset CT_GZIP and CT_DEFLATE to remember not
          * to modify the Content-Encoding header later.
          */
         csp->content_type &= ~CT_GZIP;
         csp->content_type &= ~CT_DEFLATE;
      }
   }
#endif

   return err;
}


/*********************************************************************
 *
 * Function    :  execute_content_filters
 *
 * Description :  Executes a given content filter.
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *
 * Returns     :  Pointer to the modified buffer, or
 *                NULL if filtering failed or wasn't necessary.
 *
 *********************************************************************/
char *execute_content_filters(struct client_state *csp, char *hdr)
{
   filter_function_ptr content_filter;

   assert(content_filters_enabled(csp->action));

   if (0 == csp->iob->eod - csp->iob->cur)
   {
      /*
       * No content (probably status code 301, 302 ...),
       * no filtering necessary.
       */
      return NULL;
   }

   if (JB_ERR_OK != prepare_for_filtering(csp))
   {
      /*
       * failed to de-chunk or decompress.
       */
      return NULL;
   }

   if (0 == csp->iob->eod - csp->iob->cur)
   {
      /*
       * Clown alarm: chunked and/or compressed nothing delivered.
       */
      return NULL;
   }

   content_filter = get_filter_function(csp);

   return ((*content_filter)(csp, hdr));
}


/*********************************************************************
 *
 * Function    :  get_url_actions
 *
 * Description :  Gets the actions for this URL.
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *          2  :  http = http_request request for blocked URLs
 *
 * Returns     :  N/A
 *
 *********************************************************************/
void get_url_actions(struct client_state *csp, struct http_request *http)
{
   struct file_list *fl;
   struct url_actions *b;
   int i;

   init_current_action(csp->action);

   for (i = 0; i < MAX_AF_FILES; i++)
   {
      if (((fl = csp->actions_list[i]) == NULL) || ((b = fl->f) == NULL))
      {
         return;
      }

      apply_url_actions(csp->action, http, b);
   }

   return;
}


/*********************************************************************
 *
 * Function    :  apply_url_actions
 *
 * Description :  Applies a list of URL actions.
 *
 * Parameters  :
 *          1  :  action = Destination.
 *          2  :  http = Current URL
 *          3  :  b = list of URL actions to apply
 *
 * Returns     :  N/A
 *
 *********************************************************************/
void apply_url_actions(struct current_action_spec *action,
                       struct http_request *http,
                       struct url_actions *b)
{
   if (b == NULL)
   {
      /* Should never happen */
      return;
   }

   for (b = b->next; NULL != b; b = b->next)
   {
      if (url_match(b->url, http))
      {
         merge_current_action(action, b->action);
      }
   }
}


/*********************************************************************
 *
 * Function    :  get_forward_override_settings
 *
 * Description :  Returns forward settings as specified with the
 *                forward-override{} action. forward-override accepts
 *                forward lines similar to the one used in the
 *                configuration file, but without the URL pattern.
 *
 *                For example:
 *
 *                   forward / .
 *
 *                in the configuration file can be replaced with
 *                the action section:
 *
 *                 {+forward-override{forward .}}
 *                 /
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *
 * Returns     :  Pointer to forwarding structure in case of success.
 *                Invalid syntax is fatal.
 *
 *********************************************************************/
const static struct forward_spec *get_forward_override_settings(struct client_state *csp)
{
   const char *forward_override_line = csp->action->string[ACTION_STRING_FORWARD_OVERRIDE];
   char forward_settings[BUFFER_SIZE];
   char *http_parent = NULL;
   /* variable names were chosen for consistency reasons. */
   struct forward_spec *fwd = NULL;
   int vec_count;
   char *vec[3];

   assert(csp->action->flags & ACTION_FORWARD_OVERRIDE);
   /* Should be enforced by load_one_actions_file() */
   assert(strlen(forward_override_line) < sizeof(forward_settings) - 1);

   /* Create a copy ssplit can modify */
   strlcpy(forward_settings, forward_override_line, sizeof(forward_settings));

   if (NULL != csp->fwd)
   {
      /*
       * XXX: Currently necessary to prevent memory
       * leaks when the show-url-info cgi page is visited.
       */
      unload_forward_spec(csp->fwd);
   }

   /*
    * allocate a new forward node, valid only for
    * the lifetime of this request. Save its location
    * in csp as well, so sweep() can free it later on.
    */
   fwd = csp->fwd = zalloc(sizeof(*fwd));
   if (NULL == fwd)
   {
      log_error(LOG_LEVEL_FATAL,
         "can't allocate memory for forward-override{%s}", forward_override_line);
      /* Never get here - LOG_LEVEL_FATAL causes program exit */
      return NULL;
   }

   vec_count = ssplit(forward_settings, " \t", vec, SZ(vec), 1, 1);
   if ((vec_count == 2) && !strcasecmp(vec[0], "forward"))
   {
      fwd->type = SOCKS_NONE;

      /* Parse the parent HTTP proxy host:port */
      http_parent = vec[1];

   }
   else if (vec_count == 3)
   {
      char *socks_proxy = NULL;

      if  (!strcasecmp(vec[0], "forward-socks4"))
      {
         fwd->type = SOCKS_4;
         socks_proxy = vec[1];
      }
      else if (!strcasecmp(vec[0], "forward-socks4a"))
      {
         fwd->type = SOCKS_4A;
         socks_proxy = vec[1];
      }
      else if (!strcasecmp(vec[0], "forward-socks5"))
      {
         fwd->type = SOCKS_5;
         socks_proxy = vec[1];
      }

      if (NULL != socks_proxy)
      {
         /* Parse the SOCKS proxy host[:port] */
         fwd->gateway_port = 1080;
         parse_forwarder_address(socks_proxy,
            &fwd->gateway_host, &fwd->gateway_port);

         http_parent = vec[2];
      }
   }

   if (NULL == http_parent)
   {
      log_error(LOG_LEVEL_FATAL,
         "Invalid forward-override syntax in: %s", forward_override_line);
      /* Never get here - LOG_LEVEL_FATAL causes program exit */
   }

   /* Parse http forwarding settings */
   if (strcmp(http_parent, ".") != 0)
   {
      fwd->forward_port = 8000;
      parse_forwarder_address(http_parent,
         &fwd->forward_host, &fwd->forward_port);
   }

   assert (NULL != fwd);

   log_error(LOG_LEVEL_CONNECT,
      "Overriding forwarding settings based on \'%s\'", forward_override_line);

   return fwd;
}


/*********************************************************************
 *
 * Function    :  forward_url
 *
 * Description :  Should we forward this to another proxy?
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *          2  :  http = http_request request for current URL
 *
 * Returns     :  Pointer to forwarding information.
 *
 *********************************************************************/
const struct forward_spec *forward_url(struct client_state *csp,
                                       const struct http_request *http)
{
   static const struct forward_spec fwd_default[1] = { FORWARD_SPEC_INITIALIZER };
   struct forward_spec *fwd = csp->config->forward;

   if (csp->action->flags & ACTION_FORWARD_OVERRIDE)
   {
      return get_forward_override_settings(csp);
   }

   if (fwd == NULL)
   {
      return fwd_default;
   }

   while (fwd != NULL)
   {
      if (url_match(fwd->url, http))
      {
         return fwd;
      }
      fwd = fwd->next;
   }

   return fwd_default;
}


/*********************************************************************
 *
 * Function    :  direct_response 
 *
 * Description :  Check if Max-Forwards == 0 for an OPTIONS or TRACE
 *                request and if so, return a HTTP 501 to the client.
 *
 *                FIXME: I have a stupid name and I should handle the
 *                requests properly. Still, what we do here is rfc-
 *                compliant, whereas ignoring or forwarding are not.
 *
 * Parameters  :  
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *
 * Returns     :  http_response if , NULL if nonmatch or handler fail
 *
 *********************************************************************/
struct http_response *direct_response(struct client_state *csp)
{
   struct http_response *rsp;
   struct list_entry *p;

   if ((0 == strcmpic(csp->http->gpc, "trace"))
      || (0 == strcmpic(csp->http->gpc, "options")))
   {
      for (p = csp->headers->first; (p != NULL) ; p = p->next)
      {
         if (!strncmpic("Max-Forwards:", p->str, 13))
         {
            unsigned int max_forwards;

            /*
             * If it's a Max-Forwards value of zero,
             * we have to intercept the request.
             */
            if (1 == sscanf(p->str+12, ": %u", &max_forwards) && max_forwards == 0)
            {
               /*
                * FIXME: We could handle at least TRACE here,
                * but that would require a verbatim copy of
                * the request which we don't have anymore
                */
                log_error(LOG_LEVEL_HEADER,
                  "Detected header \'%s\' in OPTIONS or TRACE request. Returning 501.",
                  p->str);

               /* Get mem for response or fail*/
               if (NULL == (rsp = alloc_http_response()))
               {
                  return cgi_error_memory();
               }
            
               if (NULL == (rsp->status = strdup("501 Not Implemented")))
               {
                  free_http_response(rsp);
                  return cgi_error_memory();
               }

               rsp->is_static = 1;
               rsp->crunch_reason = UNSUPPORTED;

               return(finish_http_response(csp, rsp));
            }
         }
      }
   }
   return NULL;
}


/*********************************************************************
 *
 * Function    :  content_requires_filtering
 *
 * Description :  Checks whether there are any content filters
 *                enabled for the current request and if they
 *                can actually be applied..
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *
 * Returns     :  TRUE for yes, FALSE otherwise
 *
 *********************************************************************/
int content_requires_filtering(struct client_state *csp)
{
   if ((csp->content_type & CT_TABOO)
      && !(csp->action->flags & ACTION_FORCE_TEXT_MODE))
   {
      return FALSE;
   }

   /*
    * Are we enabling text mode by force?
    */
   if (csp->action->flags & ACTION_FORCE_TEXT_MODE)
   {
      /*
       * Do we really have to?
       */
      if (csp->content_type & CT_TEXT)
      {
         log_error(LOG_LEVEL_HEADER, "Text mode is already enabled.");
      }
      else
      {
         csp->content_type |= CT_TEXT;
         log_error(LOG_LEVEL_HEADER, "Text mode enabled by force. Take cover!");
      }
   }

   if (!(csp->content_type & CT_DECLARED))
   {
      /*
       * The server didn't bother to declare a MIME-Type.
       * Assume it's text that can be filtered.
       *
       * This also regulary happens with 304 responses,
       * therefore logging anything here would cause
       * too much noise.
       */
      csp->content_type |= CT_TEXT;
   }

   /*
    * Choose the applying filter function based on
    * the content type and action settings.
    */
   if ((csp->content_type & CT_TEXT) &&
       (csp->rlist != NULL) &&
       (!list_is_empty(csp->action->multi[ACTION_MULTI_FILTER])))
   {
      return TRUE;
   }
   else if ((csp->content_type & CT_GIF)  &&
            (csp->action->flags & ACTION_DEANIMATE))
   {
      return TRUE;
   }

   return FALSE;

}


/*********************************************************************
 *
 * Function    :  content_filters_enabled
 *
 * Description :  Checks whether there are any content filters
 *                enabled for the current request.
 *
 * Parameters  :  
 *          1  :  action = Action spec to check.
 *
 * Returns     :  TRUE for yes, FALSE otherwise
 *
 *********************************************************************/
int content_filters_enabled(const struct current_action_spec *action)
{
   return ((action->flags & ACTION_DEANIMATE) ||
      !list_is_empty(action->multi[ACTION_MULTI_FILTER]));
}


/*********************************************************************
 *
 * Function    :  filters_available
 *
 * Description :  Checks whether there are any filters available.
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *
 * Returns     :  TRUE for yes, FALSE otherwise.
 *
 *********************************************************************/
int filters_available(const struct client_state *csp)
{
   int i;
   for (i = 0; i < MAX_AF_FILES; i++)
   {
      const struct file_list *fl = csp->rlist[i];
      if ((NULL != fl) && (NULL != fl->f))
      {
         return TRUE;
      }
   }
   return FALSE;
}


/*
  Local Variables:
  tab-width: 3
  end:
*/
