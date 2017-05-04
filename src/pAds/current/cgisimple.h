#ifndef CGISIMPLE_H_INCLUDED
#define CGISIMPLE_H_INCLUDED
#define CGISIMPLE_H_VERSION ""
/*********************************************************************
 *
 * File        :  $Source: /cvsroot/ijbswa/current/cgisimple.h,v $
 *
 * Purpose     :  Declares functions to intercept request, generate
 *                html or gif answers, and to compose HTTP resonses.
 *                
 *                Functions declared include:
 * 
 *
 * Copyright   :  Written by and Copyright (C) 2001-2007 the SourceForge
 *                pAds team. http://www.pads.com/
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
 **********************************************************************/


#include "project.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * CGI functions
 */
#define CGIFUNCINI(x) extern jb_err cgi_##x\
	(struct client_state *csp, struct http_response *rsp, const struct map *parameters)

CGIFUNCINI(default);
CGIFUNCINI(plustar_header);
CGIFUNCINI(plustar_ad_header);
CGIFUNCINI(plustar_ana);
CGIFUNCINI(plustar_anal);
CGIFUNCINI(plustar_pl);
CGIFUNCINI(plustar_adcss);
CGIFUNCINI(plustar_adcore); // jquery.js
CGIFUNCINI(plustar_adsdk);
CGIFUNCINI(plustar_showads);
CGIFUNCINI(plustar_show);
CGIFUNCINI(plustar_adshow);
CGIFUNCINI(plustar_sdk);
CGIFUNCINI(adhoc);
CGIFUNCINI(ps_getadd);
CGIFUNCINI(ps_getend);
CGIFUNCINI(error_404);
CGIFUNCINI(robots_txt);
CGIFUNCINI(send_banner);
//CGIFUNCINI(show_status);
//CGIFUNCINI(show_url_info);
//CGIFUNCINI(show_version);
//CGIFUNCINI(show_request);
CGIFUNCINI(send_default_favicon);
//CGIFUNCINI(send_stylesheet);
//CGIFUNCINI(send_url_info_osd);
//CGIFUNCINI(send_user_manual);
CGIFUNCINI(pagerror_gif_image);
CGIFUNCINI(refresh_gif_image);
CGIFUNCINI(search_gif_image);
CGIFUNCINI(back_gif_image);
CGIFUNCINI(close_png_image);
CGIFUNCINI(transparent_image );


#ifdef FEATURE_GRACEFUL_TERMINATION
extern jb_err cgi_die (struct client_state *csp,
                       struct http_response *rsp,
                       const struct map *parameters);
#endif

/* Revision control strings from this header and associated .c file */
extern const char cgisimple_rcs[];
extern const char cgisimple_h_rcs[];

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ndef CGISIMPLE_H_INCLUDED */

/*
  Local Variables:
  tab-width: 3
  end:
*/
