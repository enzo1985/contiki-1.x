/*
 * Copyright (c) 2002, Adam Dunkels.
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
 * This file is part of the "ctk" console GUI toolkit for cc65
 *
 * $Id: ctk-conio.c,v 1.2 2003/03/28 12:16:22 adamdunkels Exp $
 *
 */

#include "ctk.h"
#include "ctk-draw.h"

#include "ctk-conio-conf.h"

#include <conio.h>
#include <string.h>

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

static unsigned char sizex, sizey;

/*-----------------------------------------------------------------------------------*/
static char tmp[40];
static void
cputsn(char *str, unsigned char len)
{
  strncpy(tmp, str, len);
  tmp[len] = 0;
  cputs(tmp);
}
/*-----------------------------------------------------------------------------------*/
void
ctk_draw_init(void)
{
  bgcolor(SCREENCOLOR);
  bordercolor(BORDERCOLOR);
  screensize(&sizex, &sizey);
  ctk_draw_clear(0, sizey);
}
/*-----------------------------------------------------------------------------------*/
static void
draw_widget(struct ctk_widget *w,
	    unsigned char x, unsigned char y,
	    unsigned char clipx,
	    unsigned char clipy,
	    unsigned char clipy1, unsigned char clipy2,
	    unsigned char focus)
{
  unsigned char xpos, ypos, xscroll;
  unsigned char i, j;
  char c, *text;
  unsigned char len;

  if(focus & CTK_FOCUS_WINDOW) {    
    textcolor(WIDGETCOLOR_FWIN);
    if(focus & CTK_FOCUS_WIDGET) {
      textcolor(WIDGETCOLOR_FOCUS);
    }
  } else if(focus & CTK_FOCUS_DIALOG) {
    textcolor(WIDGETCOLOR_DIALOG);
    if(focus & CTK_FOCUS_WIDGET) {
      textcolor(WIDGETCOLOR_FOCUS);
    }
  } else {
    textcolor(WIDGETCOLOR);
  }
  
  xpos = x + w->x;
  ypos = y + w->y;
    
  switch(w->type) {
  case CTK_WIDGET_SEPARATOR:
    if(ypos >= clipy1 && ypos < clipy2) {
      chlinexy(xpos, ypos, w->w);
    }
    break;
  case CTK_WIDGET_LABEL:
    text = w->widget.label.text;
    for(i = 0; i < w->widget.label.h; ++i) {
      if(ypos >= clipy1 && ypos < clipy2) {
	gotoxy(xpos, ypos);
	cputsn(text, w->w);
	if(w->w - (wherex() - xpos) > 0) {
	  cclear(w->w - (wherex() - xpos));
	}
      }
      ++ypos;
      text += w->w;
    }
    break;
  case CTK_WIDGET_BUTTON:
    if(ypos >= clipy1 && ypos < clipy2) {
      if(focus & CTK_FOCUS_WIDGET) {
	revers(1);
      } else {
	revers(0);
      }
      cputcxy(xpos, ypos, '[');
      cputsn(w->widget.button.text, w->w);
      cputc(']');
      revers(0);
    }
    break;
  case CTK_WIDGET_HYPERLINK:
    if(ypos >= clipy1 && ypos < clipy2) {
      if(focus & CTK_FOCUS_WIDGET) {
	revers(0);
      } else {
	revers(1);
      }
      gotoxy(xpos, ypos);
      textcolor(WIDGETCOLOR_HLINK);
      cputsn(w->widget.button.text, w->w);
      revers(0);
    }
    break;
  case CTK_WIDGET_TEXTENTRY:
    text = w->widget.textentry.text;
    if(focus & CTK_FOCUS_WIDGET) {
      revers(1);
    } else {
      revers(0);
    }
    xscroll = 0;
    if(w->widget.textentry.xpos >= w->w - 1) {
      xscroll = w->widget.textentry.xpos - w->w + 1;
    }
    for(j = 0; j < w->widget.textentry.h; ++j) {
      if(ypos >= clipy1 && ypos < clipy2) {
	if(w->widget.textentry.state == CTK_TEXTENTRY_EDIT &&
	   w->widget.textentry.ypos == j) {
	  revers(0);
	  cputcxy(xpos, ypos, '>');
	  for(i = 0; i < w->w; ++i) {
	    c = text[i + xscroll];
	    if(i == w->widget.textentry.xpos - xscroll) {
	      revers(1);
	    } else {
	      revers(0);
	    }
	    if(c == 0) {
	      cputc(' ');
	    } else {
	      cputc(c);
	    }
	    revers(0);
	  }
	  cputc('<');
	} else {
	  cvlinexy(xpos, ypos, 1);
	  gotoxy(xpos + 1, ypos);          
	  cputsn(text, w->w);
	  i = wherex();
	  if(i - xpos - 1 < w->w) {
	    cclear(w->w - (i - xpos) + 1);
	  }
	  cvline(1);
	}
      }
      ++ypos;
      text += w->w;
    }
    revers(0);
    break;
  case CTK_WIDGET_ICON:
    if(ypos >= clipy1 && ypos < clipy2) {
      if(focus) {
	revers(1);
      } else {
	revers(0);
      }
      gotoxy(xpos, ypos);
      if(w->widget.icon.textmap != NULL) {
	for(i = 0; i < 3; ++i) {
	  gotoxy(xpos, ypos);
	  if(ypos >= clipy1 && ypos < clipy2) {
	    cputc(w->widget.icon.textmap[0 + 3 * i]);
	    cputc(w->widget.icon.textmap[1 + 3 * i]);
	    cputc(w->widget.icon.textmap[2 + 3 * i]);
	  }
	  ++ypos;
	}
      }
      x = xpos;
  
      len = strlen(w->widget.icon.title);
      if(x + len >= sizex) {
	x = sizex - len;
      }

      gotoxy(x, ypos);
      if(ypos >= clipy1 && ypos < clipy2) {
	cputs(w->widget.icon.title);
      }
      revers(0);
    }
    break;

  default:
    break;
  }
}
/*-----------------------------------------------------------------------------------*/
void
ctk_draw_widget(struct ctk_widget *w,
		unsigned char focus,
		unsigned char clipy1,
		unsigned char clipy2)
{
  struct ctk_window *win = w->window;
  unsigned char posx, posy;

  posx = win->x + 1;
  posy = win->y + 2;

  if(w == win->focused) {
    focus |= CTK_FOCUS_WIDGET;
  }
  
  draw_widget(w, posx, posy,
	      posx + win->w,
	      posy + win->h,
	      clipy1, clipy2,
	      focus);
}
/*-----------------------------------------------------------------------------------*/
void
ctk_draw_window(struct ctk_window *window, unsigned char focus,
		unsigned char clipy1, unsigned char clipy2)
{
  unsigned char x, y;
  unsigned char i, h;
  struct ctk_widget *w;
  unsigned char wfocus;
  unsigned char x1, y1, x2, y2;

  if(window->y + 1 >= clipy2) {
    return;
  }
  x = window->x;
  y = window->y + 1;
  x1 = x + 1;
  y1 = y + 1;
  x2 = x1 + window->w;
  y2 = y1 + window->h;
  
  if(focus & CTK_FOCUS_WINDOW) {
    textcolor(WINDOWCOLOR_FOCUS);
  } else {
    textcolor(WINDOWCOLOR);
  }



  h = y1 + window->h;
  /* Clear window contents. */
  for(i = y1; i < h; ++i) {
    if(i >= clipy1 && i < clipy2) {
      cclearxy(x1, i, window->w);
    }
  }
    
  
  /* Draw window frame. */  
  if(y >= clipy1) {
    cputcxy(x, y, CH_ULCORNER);
    /*    cputc(' ');
    cputs(window->title);
    cputc(' ');*/
    gotoxy(wherex() + window->titlelen + 2, wherey());
    chline(window->w - (wherex() - x) - 2);
    cputcxy(x2, y, CH_URCORNER);
  }

  h = window->h;
  
  if(clipy1 > y1) {
    if(clipy1 - y1 < h) {
      h = clipy1 - y1;
      y1 = clipy1;
    } else {
      h = 0;
    }
  }

  if(y1 + h >= clipy2) {
    if(y1 >= clipy2) {
      h = 0;
    } else {
      h = clipy2 - y1;
    }
  }
  
  cvlinexy(x, y1, h);
  cvlinexy(x2, y1, h);

  if(y + window->h >= clipy1 &&
     y + window->h < clipy2) {
    cputcxy(x, y2, CH_LLCORNER);
    chlinexy(x1, y2, window->w);
    cputcxy(x2, y2, CH_LRCORNER);
  }

  focus = focus & CTK_FOCUS_WINDOW;
  
  /* Draw inactive widgets. */
  for(w = window->inactive; w != NULL; w = w->next) {
    draw_widget(w, x1, y1, x2, y2,
		clipy1, clipy2,
		focus);
  }
  
  /* Draw active widgets. */
  for(w = window->active; w != NULL; w = w->next) {  
    wfocus = focus;
    if(w == window->focused) {
      wfocus |= CTK_FOCUS_WIDGET;
    }

   draw_widget(w, x1, y1, x2, y2, 
	       clipy1, clipy2,
	       wfocus);
  }
}
/*-----------------------------------------------------------------------------------*/
void
ctk_draw_dialog(struct ctk_window *dialog)
{
  unsigned char x, y;
  unsigned char i;
  struct ctk_widget *w;
  unsigned char wfocus;
  unsigned char x1, y1, x2, y2;
  
  textcolor(DIALOGCOLOR);

  x = dialog->x;
  y = dialog->y + 1;


  x1 = x + 1;
  y1 = y + 1;
  x2 = x1 + dialog->w;
  y2 = y1 + dialog->h;


  /* Draw dialog frame. */
  
  cvlinexy(x, y1,
	   dialog->h);
  cvlinexy(x2, y1,
	   dialog->h);

  chlinexy(x1, y,
	   dialog->w);
  chlinexy(x1, y2,
	   dialog->w);

  cputcxy(x, y, CH_ULCORNER);
  cputcxy(x, y2, CH_LLCORNER);
  cputcxy(x2, y, CH_URCORNER);
  cputcxy(x2, y2, CH_LRCORNER);
  
  
  /* Clear window contents. */
  for(i = y1; i < y2;  ++i) {
    cclearxy(x1, i, dialog->w);
  }


  
  /* Clear dialog contents. */
  for(i = y1; i < y2; ++i) {
    cclearxy(x1, i, dialog->w);
  }
  
  /* Draw inactive widgets. */
  for(w = dialog->inactive; w != NULL; w = w->next) {
    draw_widget(w, x1, y1, x2, y2,
		0, sizey, CTK_FOCUS_DIALOG);
  }


  /* Draw active widgets. */
  for(w = dialog->active; w != NULL; w = w->next) {
    wfocus = CTK_FOCUS_DIALOG;
    if(w == dialog->focused) {
      wfocus |= CTK_FOCUS_WIDGET;
    }
    draw_widget(w, x1, y1, x2, y2,
		0, sizey, wfocus);
  }

}
/*-----------------------------------------------------------------------------------*/
void
ctk_draw_clear(unsigned char y1, unsigned char y2)
{
  unsigned char i;
 
  for(i = y1; i < y2; ++i) {
    cclearxy(0, i, sizex);
  }
}
/*-----------------------------------------------------------------------------------*/
static void
draw_menu(struct ctk_menu *m)
{
  unsigned char x, x2, y;
  textcolor(OPENMENUCOLOR);
  x = wherex();
  cputs(m->title);
  cputc(' ');
  x2 = wherex();
  if(x + CTK_CONF_MENUWIDTH > sizex) {
    x = sizex - CTK_CONF_MENUWIDTH;
  }
  
  
  for(y = 0; y < m->nitems; ++y) {
    if(y == m->active) {
      textcolor(ACTIVEMENUITEMCOLOR);
      revers(0);
    } else {
      textcolor(MENUCOLOR);	  
    }
    gotoxy(x, y + 1);
    if(m->items[y].title[0] == '-') {
      chline(CTK_CONF_MENUWIDTH);
    } else {
      cputs(m->items[y].title);
    }
    if(x + CTK_CONF_MENUWIDTH > wherex()) {
      cclear(x + CTK_CONF_MENUWIDTH - wherex());
    }
    revers(1);
  }
  gotoxy(x2, 0);
  textcolor(MENUCOLOR);  
}
/*-----------------------------------------------------------------------------------*/
void
ctk_draw_menus(struct ctk_menus *menus)
{
  struct ctk_menu *m;  
  
  
  /* Draw menus */
  textcolor(MENUCOLOR);
  gotoxy(0, 0);
  revers(1);
  for(m = menus->menus->next; m != NULL; m = m->next) {
    if(m != menus->open) {
      cputs(m->title);
      cputc(' ');
    } else {
      draw_menu(m);
    }
  }


  if(wherex() + strlen(menus->desktopmenu->title) + 1>= sizex) {
    gotoxy(sizex - strlen(menus->desktopmenu->title) - 1, 0);
  } else {
    cclear(sizex - wherex() -
	   strlen(menus->desktopmenu->title) - 1);
  }
  
  /* Draw desktopmenu */
  if(menus->desktopmenu != menus->open) {
    cputs(menus->desktopmenu->title);
    cputc(' ');
  } else {
    draw_menu(menus->desktopmenu);
  }

  revers(0);
}
/*-----------------------------------------------------------------------------------*/
unsigned char
ctk_draw_height(void)
{
  return sizey;
}
/*-----------------------------------------------------------------------------------*/
unsigned char
ctk_draw_width(void)
{
  return sizex;
}
/*-----------------------------------------------------------------------------------*/
