/*
 * Copyright (c) 2004, Adam Dunkels.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution. 
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgement:
 *        This product includes software developed by Adam Dunkels. 
 * 4. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.  
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 * This file is part of the Contiki desktop environment 
 *
 * $Id: main.c,v 1.10 2005/02/15 15:09:35 oliverschmidt Exp $
 *
 */

#include <stdlib.h>

#include "ctk.h"
#include "ctk-draw.h"
#include "ek.h"

#include "cfs-win32.h"

#include "program-handler.h"

#include "uip.h"
#include "uip_arp.h"

#include "resolv.h"

#include "packet-service.h"

#include "netconf-dsc.h"
#include "dhcp-dsc.h"
#include "www-dsc.h"
#include "webserver-dsc.h"
#include "ftp-dsc.h"
#include "telnet-dsc.h"
#include "email-dsc.h"
#include "irc-dsc.h"
#include "editor-dsc.h"
#include "calc-dsc.h"
#include "processes-dsc.h"
#include "about-dsc.h"

#include "clock.h"

/*-----------------------------------------------------------------------------------*/
clock_time_t
clock_time(void)
{
  return clock();
}
/*-----------------------------------------------------------------------------------*/
void
main(int argc)
{
  u16_t addr[2];

#ifdef WITH_RAWSOCK
  if(argc != 2) {
    cprintf("\n"
            "usage: Contiki <addr>\n"
            "\n"
            "  <addr> Host IP address of interface to be used by Contiki\n"
	    "\n"
            "notes:\n"
	    "\n"
            "  - Contiki needs Windows 2000 or later to run.\n"
	    "\n"
            "  - Contiki needs administrative rights to run.\n"
	    "\n"
            "  - The IP address to be used by Contiki has to be different from the Host IP\n"
            "    address specified here.\n");
    exit(EXIT_FAILURE);
  }
#endif /* WITH_RAWSOCK */

  ek_init();

  tcpip_init(NULL);
  resolv_init(NULL);

  console_init();
  ctk_init();

  cfs_win32_init(NULL);

  program_handler_init();

#if 0
  uip_ipaddr(addr, 192,168,0,3);
  uip_sethostaddr(addr);
 
  uip_ipaddr(addr, 192,168,0,1);
  resolv_conf(addr);
#endif

#ifdef WITH_RAWSOCK
  packet_service_init(NULL);
#endif /* WITH_RAWSOCK */

#ifdef WITH_PPP
  ppp_arch_init();
  ppp_service_init(NULL);
#endif /* WITH_PPP */

  program_handler_add(&netconf_dsc,   "Network setup", 1);
  program_handler_add(&dhcp_dsc,      "DHCP client",   1);
  program_handler_add(&www_dsc,       "Web browser",   1);
  program_handler_add(&webserver_dsc, "Web server",    1);
  program_handler_add(&ftp_dsc,       "FTP client",    1);
  program_handler_add(&telnet_dsc,    "Telnet",        1);
  program_handler_add(&email_dsc,     "E-mail",        1);
  program_handler_add(&irc_dsc,       "IRC client",    1);
  program_handler_add(&editor_dsc,    "Editor",        1);
  program_handler_add(&calc_dsc,      "Calculator",    1);
  program_handler_add(&processes_dsc, "Processes",     1);
  program_handler_add(&about_dsc,     "About Contiki", 0);

  while(1) {
    ek_run();

    if(console_resize()) {
      ctk_restore();
    }

    _sleep(0);
  }
}
/*-----------------------------------------------------------------------------------*/
