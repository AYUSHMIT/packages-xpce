/*  $Id$

    Part of XPCE

    Author:  Jan Wielemaker and Anjo Anjewierden
    E-mail:  jan@swi.psy.uva.nl
    WWW:     http://www.swi.psy.uva.nl/projects/xpce/
    Copying: GPL-2.  See the file COPYING or http://www.gnu.org

    Copyright (C) 1990-2001 SWI, University of Amsterdam. All rights reserved.
*/

#include "include.h"
#include <h/interface.h>
#include <h/unix.h>

#ifdef USE_RLC_FUNCTIONS
static void init_render_hooks(void);
#else
#define init_render_hooks()
#endif

void
ws_flush_display(DisplayObj d)
{ 
}


void
ws_synchronise_display(DisplayObj d)
{ MSG msg;

  if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
  { TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}


void
ws_bell_display(DisplayObj d, int volume)
{ MessageBeep(MB_ICONEXCLAMATION);
}


void
ws_get_size_display(DisplayObj d, int *w, int *h)
{ HDC  hdc = GetDC(NULL);

  *w = GetDeviceCaps(hdc, HORZRES);
  *h = GetDeviceCaps(hdc, VERTRES);

  ReleaseDC(NULL, hdc);
}


Name
ws_get_visual_type_display(DisplayObj d)
{ int depth = ws_depth_display(d);

  if ( depth == 1 )
    return NAME_monochrome;
  else if ( depth <= 8 )		/* test for colourmap? */
    return NAME_pseudoColour;
  else
    return NAME_trueColour;
}


int
ws_depth_display(DisplayObj d)
{ HDC  hdc = GetDC(NULL);
  int depth = GetDeviceCaps(hdc, BITSPIXEL);
  ReleaseDC(NULL, hdc);

  return depth;
}


int
ws_resolution_display(DisplayObj d, int *rx, int *ry)
{ HDC hdc = GetDC(NULL);
  
  *rx = GetDeviceCaps(hdc, LOGPIXELSX);
  *ry = GetDeviceCaps(hdc, LOGPIXELSY);

  ReleaseDC(NULL, hdc);

  succeed;
}


void
ws_activate_screen_saver(DisplayObj d)
{
}


void
ws_deactivate_screen_saver(DisplayObj d)
{
}


void
ws_init_display(DisplayObj d)
{
}


status
ws_legal_display_name(char *s)
{ succeed;
}


status
ws_opened_display(DisplayObj d)
{ if ( d->ws_ref )
    succeed;

  fail;
}


void
ws_open_display(DisplayObj d)
{ d->ws_ref = (WsRef) 1;	/* just flag; nothing to do yet */

  if ( isDefault(d->colour_map) )
  { if ( ws_has_colourmap(d) )
      send(d, NAME_colourMap, newObject(ClassColourMap, NAME_pce, EAV), EAV);
    else
      send(d, NAME_colourMap, NIL, EAV);
  }

  ws_init_loc_still_timer();
}


void
ws_quit_display(DisplayObj d)
{ exitDraw(0);
}

		 /*******************************
		 *	  MOUSE TRACKING	*
		 *******************************/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The %@^%#@& MS-Windows system does  not   tell  the application when the
mouse enters/leaves some window.  News articles   and many mails later I
was hardly any further.  Anyway, the following appears to work:

Use SetWindowsHookEx() to register a WH_MOUSE hook.  When the hwnd field
of the MOUSEHOOKSTRUCT changes, SendMessage()   a  user-defined event to
the window left and entered.  Now, if you do that in the app itself, you
will not see any result if the mouse leaves towards another application.

Therefore we first try to load the pcewh.dll module which does the same,
but in a dll, so it can do the   job system-wide.  If this fails we will
do the job locally, which in any case is better than not at all.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static HWND current_window;

void
PceWhDeleteWindow(HWND win)
{ if ( win == current_window )
    current_window = 0;
}


void
PceEventInWindow(HWND win)
{ if ( win != current_window )
  { if ( current_window )
    { DEBUG(NAME_areaEnter,
	    Cprintf("Posting exit to %s\n",
		    getObjectFromHWND(current_window)));
      SendMessage(current_window, WM_WINEXIT, 0, 0L);
    }
    if ( win )
    { DEBUG(NAME_areaEnter,
	    Cprintf("Posting enter to %s\n",
		    getObjectFromHWND(win)));
      SendMessage(win, WM_WINENTER, 0, 0L);
    }

    current_window = win;
  }
}


static void
init_area_enter_exit_handling(DisplayObj d)
{
}


#ifdef USE_RLC_FUNCTIONS
static RlcUpdateHook system_update_hook;

static void
exit_update_hook(void)
{ rlc_update_hook(system_update_hook);
}
#endif /*USE_RLC_FUNCTIONS*/


status
ws_init_graphics_display(DisplayObj d)
{
#ifdef USE_RLC_FUNCTIONS
  system_update_hook = rlc_update_hook(pceRedraw);
  at_pce_exit(exit_update_hook, ATEXIT_FILO);
#endif
  initDraw();

  init_area_enter_exit_handling(d);

  succeed;
}


void
ws_foreground_display(DisplayObj d, Colour c)
{
}


void
ws_background_display(DisplayObj d, Colour c)
{
}


void
ws_draw_in_display(DisplayObj d, Graphical gr, Bool invert, Bool subtoo)
{ d_screen(d);
  if ( invert == ON ) r_invert_mode(ON);
  if ( subtoo == ON ) r_subwindow_mode(ON);
  RedrawArea(gr, gr->area);
  r_invert_mode(OFF);
  r_subwindow_mode(OFF);
  d_done();
}


void
ws_grab_server(DisplayObj d)
{
}


void
ws_ungrab_server(DisplayObj d)
{
}


Int
ws_display_connection_number(DisplayObj d)
{ fail;
}


status
ws_events_queued_display(DisplayObj d)
{ return GetInputState() ? SUCCEED : FAIL;
}

		 /*******************************
		 *     SELECTION HANDLING	*
		 *******************************/

#ifdef USE_RLC_FUNCTIONS
#define CLIPBOARDWIN	rlc_hwnd()
#else
#define CLIPBOARDWIN	PceHiddenWindow()
#endif

static HGLOBAL
ws_string_to_global_mem(String s)
{ int size  = str_datasize(s);
  int bytes = size + str_count_chr(s, 0, s->size, '\n');
  HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, bytes + 1);
  char far *data;
  int i;

  if ( !mem )
  { Cprintf("Cannot allocate\n");
    return 0;
  }
  data = GlobalLock(mem);

  for(i=0; i<size; i++)
  { if ( s->s_text8[i] == '\n' )
      *data++ = '\r';
    *data++ = s->s_text8[i];
  }
  *data = EOS;

  GlobalUnlock(mem);

  return mem;
}


status
ws_set_cutbuffer(DisplayObj d, int n, String s)
{ if ( n == 0 )
  { HGLOBAL mem = ws_string_to_global_mem(s);

    OpenClipboard(CLIPBOARDWIN);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, mem);
    CloseClipboard();

    succeed;
  }

  Cprintf("Cannot access cut-buffers other than 0\n");
  fail;
}


static Any
get_clipboard_data(DisplayObj d, Name type)
{ HGLOBAL mem;
  HENHMETAFILE hmf;
  Any rval = FAIL;

  OpenClipboard(CLIPBOARDWIN);
  if ( type != NAME_winMetafile && (mem = GetClipboardData(CF_TEXT)) )
  { char far *data = GlobalLock(mem);
    char *copy, *q;
    int i;

    for(i=0; data[i]; i++)
      ;
    q = copy = pceMalloc(i + 1);

    for(; *data; data++)
    { if ( *data == '\r' && data[1] == '\n' )
      { data++;
	*q++ = '\n';
      } else
	*q++ = *data;
    }
    *q = EOS;
    rval = CtoString(copy);
    pceFree(copy);
    GlobalUnlock(mem);
  } else if ( type != NAME_text && (hmf = GetClipboardData(CF_ENHMETAFILE)) )
  { HENHMETAFILE copy = CopyEnhMetaFile(hmf, NULL);
    if ( !copy )
    { errorPce(d, NAME_winMetafile, CtoName("CopyEnhMetaFile"), APIError());
      fail;
    }

    rval = CtoWinMetafile(copy);
    DeleteEnhMetaFile(hmf);

    GlobalUnlock(mem);
  }
  CloseClipboard();

  return rval; 
}


StringObj
ws_get_cutbuffer(DisplayObj d, int n)
{ if ( n == 0 )
    return get_clipboard_data(d, NAME_text); /* DEFAULT? */

  Cprintf("Cannot access cut-buffers other than 0\n");
  fail;
}


unsigned long
ws_get_selection_timeout(void)
{ return 0L;
}


void
ws_set_selection_timeout(unsigned long time)
{
}


Any
ws_get_selection(DisplayObj d, Name which, Name target)
{ return get_clipboard_data(d, target);
}


void
ws_renderall(void)
{ HWND hwnd = CLIPBOARDWIN;
  
  OpenClipboard(hwnd);
  EmptyClipboard();
  CloseClipboard();
}


void
ws_disown_selection(DisplayObj d, Name selection)
{ ws_renderall();
}


int
ws_provide_selection(int format)
{ DisplayObj d = CurrentDisplay(NIL);
  Hyper h;
  Function msg;
  Name which     = NAME_primary;
  Name hypername = getAppendName(which, NAME_selectionOwner);
  Name type;

  if ( d && notNil(d) &&
       (h    = getFindHyperObject(d, hypername, DEFAULT)) &&
       (type = getAttributeObject(h, NAME_type)) &&
       (msg  = getAttributeObject(h, NAME_convertFunction)) &&
       (msg  = checkType(msg, TypeFunction, NIL)) )
  { Any val;

    DEBUG(NAME_selection, Cprintf("Provide %s selection of type %s\n",
				  pp(which), pp(type)));

    if ( !(val = getForwardReceiverFunction(msg, h->to, which, type, EAV)) )
      return FALSE;

    DEBUG(NAME_selection, Cprintf("Got %s\n", pp(val)));

    if ( type == NAME_text )
    { CharArray ca = checkType(val, TypeCharArray, NIL);
      
      if ( ca )
      { String s = &ca->data;
      	HGLOBAL mem = ws_string_to_global_mem(s);

	if ( mem )
	  SetClipboardData(CF_TEXT, mem);

	return TRUE;
      }
    } else if ( type == NAME_emf || type == NAME_wmf )
    { Any mf = checkType(val, nameToType(NAME_winMetafile), NIL);

      if ( mf )
      { DEBUG(NAME_selection, Cprintf("Providing win_metafile\n"));
	return ws_on_clipboard_metafile(mf, type);
      }
    } else
      return errorPce(d, NAME_noSelectionType, type);
  }

  return FALSE;
}


status
ws_own_selection(DisplayObj d, Name selection, Name type)
{ HWND hwnd = CLIPBOARDWIN;
  UINT format;

  if ( type == NAME_emf )
    format = CF_ENHMETAFILE;
  else if ( type == NAME_wmf )
    format = CF_METAFILEPICT;
  else if ( type == NAME_text) 
    format = CF_TEXT;
  else
    return errorPce(d, NAME_noSelectionType, type);

  DEBUG(NAME_selection, Cprintf("%s becomes owner of %selection, type %s\n",
				pp(d), pp(selection), pp(type)));

  OpenClipboard(hwnd);
  EmptyClipboard();
  SetClipboardData(format, NULL);
  CloseClipboard();
  init_render_hooks();

  succeed;
}


#ifdef USE_RLC_FUNCTIONS
static RlcRenderHook	system_render_hook;
static RlcRenderAllHook system_render_all_hook;
static int 		render_hooks_initialised;

static void
exit_render_hooks(void)
{ if ( render_hooks_initialised )
  { rlc_render_hook(system_render_hook);
    rlc_render_all_hook(system_render_all_hook);

    render_hooks_initialised = FALSE;
  }
}


static void
init_render_hooks(void)
{ if ( !render_hooks_initialised )
  { system_render_hook     = rlc_render_hook(ws_provide_selection);
    system_render_all_hook = rlc_render_all_hook(ws_renderall);
    render_hooks_initialised++;

    at_pce_exit(exit_render_hooks, ATEXIT_FIFO);
  }
}
#endif


Name
ws_window_manager(DisplayObj d)
{ answer(CtoName("windows"));
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
These    may    be    useful    to    debug    the    graphics    stuff.
rlc_copy_output_to_debug_output() inplies that all output to the console
is also written to the debugger output,   making the relation with debug
output clear.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

void
ws_synchronous(DisplayObj d)
{ 
#ifdef USE_RLC_FUNCTIONS
  rlc_copy_output_to_debug_output(TRUE);
#endif
}


void
ws_asynchronous(DisplayObj d)
{
#ifdef USE_RLC_FUNCTIONS
  rlc_copy_output_to_debug_output(FALSE);
#endif
}


status
ws_postscript_display(DisplayObj d)
{ int w = valInt(getWidthDisplay(d));
  int h = valInt(getHeightDisplay(d));
  HDC hdc = GetDC(NULL);
  int depth = GetDeviceCaps(hdc, BITSPIXEL);

  if ( depth >= 4 )
    depth = 4;
  else if ( depth == 3 )
    depth = 2;

  ps_output("0 0 ~D ~D ~D greymap\n", w, h, depth);
  postscriptDC(hdc, 0, 0, w, h, depth);
  ps_output("\n");

  succeed;
}


Image
ws_grab_image_display(DisplayObj d, int x, int y, int width, int height)
{ HDC hdc = GetDC(NULL);
  RECT rect;
  Image image;
  int w, h;
  HBITMAP obm, bm;
  HDC hdcimg;
  Size size = getSizeDisplay(d);

  rect.left   = x;
  rect.top    = y;
  rect.right  = x + width;
  rect.bottom = y + height;
  if ( rect.left < 0 ) rect.left = 0;
  if ( rect.top < 0 )  rect.top  = 0;
  if ( rect.bottom > valInt(size->h) ) rect.bottom = valInt(size->h);
  if ( rect.right >  valInt(size->w) ) rect.right  = valInt(size->w);
  
  w = rect.right - rect.left;
  h = rect.bottom - rect.top;

  image = answerObject(ClassImage, NIL,
		       toInt(w), toInt(h), NAME_pixmap, EAV);
  assign(image, display, d);
  bm = ZCreateCompatibleBitmap(hdc, w, h);
  hdcimg = CreateCompatibleDC(hdc);
  obm = SelectObject(hdcimg, bm);

  BitBlt(hdcimg, 0, 0, w, h, hdc, rect.left, rect.top, SRCCOPY);

  SelectObject(hdcimg, obm);
  ZDeleteObject(hdcimg);
  ReleaseDC(NULL, hdc);

  registerXrefObject(image, image->display, (void *) bm);

  return image;
}
