/*
 * Copyright (c) 2001, Adam Dunkels.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Adam Dunkels.
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
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: uip_arch.c,v 1.2 2004/07/04 17:55:55 adamdunkels Exp $
 *
 */


#include "uip.h"
#include "uip_arch.h"

#define BUF ((uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])
#define IP_PROTO_TCP    6

/*-----------------------------------------------------------------------------------*/
#if UIP_BUFSIZE > 255
/*-----------------------------------------------------------------------------------*/
void
uip_add32(u8_t *op32, u16_t op16)
{
  asm("ldy #3");
  asm("jsr ldaxysp");
  asm("sta ptr1");
  asm("stx ptr1+1");
  asm("ldy #0");
  asm("lda (sp),y");
  asm("ldy #3");
  asm("clc");
  asm("adc (ptr1),y");
  asm("sta _uip_acc32+3");
  asm("dey");
  asm("lda (ptr1),y");
  asm("ldy #1");
  asm("adc (sp),y");
  asm("sta _uip_acc32+2");
  asm("ldy #1");
  asm("lda (ptr1),y");
  asm("adc #0");
  asm("sta _uip_acc32+1");
  asm("dey");
  asm("lda (ptr1),y");
  asm("adc #0");
  asm("sta _uip_acc32+0");  
}
/*-----------------------------------------------------------------------------------*/
#else /* UIP_BUFSIZE > 255 */
/*-----------------------------------------------------------------------------------*/
void
uip_add32(u8_t *op32, u8_t op8)
{
  asm("ldy #2");
  asm("jsr ldaxysp");
  asm("sta ptr1");
  asm("stx ptr1+1");
  asm("ldy #0");
  asm("lda (sp),y");
  asm("ldy #3");
  asm("clc");
  asm("adc (ptr1),y");
  asm("sta _uip_acc32+3");
  asm("dey");
  asm("lda (ptr1),y");
  asm("adc #0");
  asm("sta _uip_acc32+2");
  asm("dey");
  asm("lda (ptr1),y");
  asm("adc #0");
  asm("sta _uip_acc32+1");
  asm("dey");
  asm("lda (ptr1),y");
  asm("adc #0");
  asm("sta _uip_acc32+0");  
}
/*-----------------------------------------------------------------------------------*/
#endif /* UIP_BUFSIZE > 255 */

static u16_t chksum_ptr, chksum_len, chksum_tmp;
static u16_t chksum(void);
/*-----------------------------------------------------------------------------------*/
u16_t
chksum(void) {

  asm("lda #0");
  asm("sta tmp1");
  asm("sta tmp1+1");
  asm("lda _chksum_ptr");
  asm("sta ptr1");
  asm("lda _chksum_ptr+1");
  asm("sta ptr1+1");


  asm("lda _chksum_len+1");
  asm("beq chksumlast");


  /* If checksum is > 256, do the first runs. */
  asm("ldy #0");
  asm("clc");
  asm("chksumloop_256:");
  asm("lda (ptr1),y");
  asm("adc tmp1");
  asm("sta tmp1");
  asm("iny");
  asm("lda (ptr1),y");
  asm("adc tmp1+1");
  asm("sta tmp1+1");
  asm("iny");
  asm("bne chksumloop_256");
  asm("inc ptr1+1");
  asm("dec _chksum_len+1");
  asm("bne chksumloop_256");

  asm("chksum_endloop_256:");
  asm("lda tmp1");
  asm("adc #0");
  asm("sta tmp1");
  asm("lda tmp1+1");
  asm("adc #0");
  asm("sta tmp1+1");
  asm("bcs chksum_endloop_256");
  
  asm("chksumlast:");
  asm("lda _chksum_len");
  asm("lsr");
  asm("bcc chksum_noodd");  
  asm("ldy _chksum_len");
  asm("dey");
  asm("lda (ptr1),y");
  asm("clc");
  asm("adc tmp1");
  asm("sta tmp1");
  asm("bcc noinc1");
  asm("inc tmp1+1");
  asm("bne noinc1");
  asm("inc tmp1");
  asm("noinc1:");
  asm("dec _chksum_len");

  asm("chksum_noodd:");
  asm("clc");
  asm("php");
  asm("ldy _chksum_len");
  asm("chksum_loop1:");
  asm("cpy #0");
  asm("beq chksum_loop1_end");
  asm("plp");
  asm("dey");
  asm("dey");
  asm("lda (ptr1),y");
  asm("adc tmp1");
  asm("sta tmp1");
  asm("iny");
  asm("lda (ptr1),y");
  asm("adc tmp1+1");
  asm("sta tmp1+1");
  asm("dey");
  asm("php");
  asm("jmp chksum_loop1");
  asm("chksum_loop1_end:");
  asm("plp");
  
  asm("chksum_endloop:");
  asm("lda tmp1");
  asm("adc #0");
  asm("sta tmp1");
  asm("lda tmp1+1");
  asm("adc #0");
  asm("sta tmp1+1");
  asm("bcs chksum_endloop");
  
  asm("lda tmp1");
  asm("ldx tmp1+1");
}
/*-----------------------------------------------------------------------------------*/
u16_t
uip_chksum(u16_t *buf, u16_t len)
{
  /*  unsigned long sum;

  sum = 0;

  chksum_ptr = (u16_t)buf;
  while(len >= 256) {  
    chksum_len = 256;
    sum += chksum();
    len -= 256;
    chksum_ptr += 256;
  }

  if(len < 256) {
    chksum_len = len;
    sum += chksum();
  }

  while((sum >> 16) != 0) {
    sum = (sum >> 16) + (sum & 0xffff);
  }

  return sum;*/

  chksum_len = len;
  chksum_ptr = (u16_t)buf;
  return chksum();
}
/*-----------------------------------------------------------------------------------*/
u16_t
uip_ipchksum(void)
{  
  chksum_ptr = (u16_t)uip_buf + UIP_LLH_LEN;
  chksum_len = 20;  
  return chksum();
}
/*-----------------------------------------------------------------------------------*/
u16_t
uip_tcpchksum(void)
{  
  chksum_ptr = (u16_t)&uip_buf[20 + UIP_LLH_LEN];
  chksum_len = 20;  
  chksum_tmp = chksum();

  chksum_ptr = (u16_t)uip_appdata;
#ifdef WITH_TFE
  asm("lda _uip_buf+3+14");
#else /* WITH_TFE */
  asm("lda _uip_buf+3");
#endif /* WITH_TFE */
  asm("sec");
  asm("sbc #40");
  asm("sta _chksum_len");
#ifdef WITH_TFE
  asm("lda _uip_buf+2+14");
#else /* WITH_TFE */
  asm("lda _uip_buf+2");
#endif /* WITH_TFE */
  asm("sbc #0");
  asm("sta _chksum_len+1");

  asm("jsr %v", chksum);

  asm("clc");
  asm("adc _chksum_tmp");
  asm("sta _chksum_tmp");
  asm("txa");
  asm("adc _chksum_tmp+1");
  asm("sta _chksum_tmp+1");

  /* Fold carry */
  /*  asm("bcc noinc");
  asm("inc _chksum_tmp");
  asm("noinc:");*/
  
  asm("tcpchksum_loop1:");
  asm("lda _chksum_tmp");
  asm("adc #0");
  asm("sta _chksum_tmp");
  asm("lda _chksum_tmp+1");
  asm("adc #0");
  asm("sta _chksum_tmp+1");
  asm("bcs tcpchksum_loop1");


#ifdef WITH_TFE
  asm("lda _uip_buf+3+14");
#else /* WITH_TFE */
  asm("lda _uip_buf+3");
#endif /* WITH_TFE */
  asm("sec");
  asm("sbc #20");
  asm("sta _chksum_len");
#ifdef WITH_TFE
  asm("lda _uip_buf+2+14");
#else /* WITH_TFE */
  asm("lda _uip_buf+2");
#endif /* WITH_TFE */
  asm("sbc #0");
  asm("sta _chksum_len+1");
  
  
  asm("ldy #$0c");
  asm("clc");
  asm("php");
  asm("tcpchksum_loop2:");
  asm("plp");
#ifdef WITH_TFE
  asm("lda _uip_buf+14,y");
#else /* WITH_TFE */
  asm("lda _uip_buf,y");
#endif /* WITH_TFE */  
  asm("adc _chksum_tmp");
  asm("sta _chksum_tmp");
  asm("iny");
#ifdef WITH_TFE 
  asm("lda _uip_buf+14,y");
#else /* WITH_TFE */
  asm("lda _uip_buf,y");
#endif /* WITH_TFE */  
  asm("adc _chksum_tmp+1");
  asm("sta _chksum_tmp+1");
  asm("iny");
  asm("php");
  asm("cpy #$14");
  asm("bne tcpchksum_loop2");

  asm("plp");
  
  asm("lda _chksum_tmp");
  asm("adc #0");
  asm("sta _chksum_tmp");
  asm("lda _chksum_tmp+1");
  asm("adc #6");  /* IP_PROTO_TCP */
  asm("sta _chksum_tmp+1");

  
  asm("lda _chksum_tmp");
  asm("adc _chksum_len+1");
  asm("sta _chksum_tmp");
  asm("lda _chksum_tmp+1");
  asm("adc _chksum_len");
  asm("sta _chksum_tmp+1");

  

  asm("tcpchksum_loop3:");
  asm("lda _chksum_tmp");
  asm("adc #0");
  asm("sta _chksum_tmp");
  asm("lda _chksum_tmp+1");
  asm("adc #0");
  asm("sta _chksum_tmp+1");
  asm("bcs tcpchksum_loop3");


  return chksum_tmp;
}
/*-----------------------------------------------------------------------------------*/
