/* 
 * Proview   Open Source Process Control.
 * Copyright (C) 2005-2017 SSAB EMEA AB.
 *
 * This file is part of Proview.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation, either version 2 of 
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with Proview. If not, see <http://www.gnu.org/licenses/>
 *
 * Linking Proview statically or dynamically with other modules is
 * making a combined work based on Proview. Thus, the terms and 
 * conditions of the GNU General Public License cover the whole 
 * combination.
 *
 * In addition, as a special exception, the copyright holders of
 * Proview give you permission to, from the build function in the
 * Proview Configurator, combine Proview with modules generated by the
 * Proview PLC Editor to a PLC program, regardless of the license
 * terms of these modules. You may copy and distribute the resulting
 * combined work under the terms of your choice, provided that every 
 * copy of the combined work is accompanied by a complete copy of 
 * the source code of Proview (the version used to produce the 
 * combined work), being distributed under the terms of the GNU 
 * General Public License plus this exception.
 **/

#include "glow_std.h"

#include <stdlib.h>
#include <fstream>
#include <math.h>
#include <float.h>
#include "glow_ctx.h"
#include "glow_keyboardctx.h"
#include "glow_node.h"
#include "glow_point.h"
#include "glow_rect.h"
#include "glow_line.h"
#include "glow_arc.h"
#include "glow_text.h"
#include "glow_conpoint.h"
#include "glow_node.h"
#include "glow_nodeclass.h"
#include "glow_con.h"
#include "glow_conclass.h"
#include "glow_growrect.h"
#include "glow_growtext.h"
#include "glow_growline.h"
#include "glow_growpolyline.h"
#include "glow_draw.h"
#include "glow_msg.h"


static char keyboard_keymap_numeric[4][17] = {
  {"789x456-123Ex0.x"},
  {"789x456-123Ex0.x"},
  {"789x456-123Ex0,x"},
  {"789x456-123Ex0,x"}};

static char keyboard_keymap_alphabetic[4][52] = {
  {"�abcdefghijklm@-�"
   "�nopqrstuvwxyz/*�"
   "�1234567890���,.�"},
  {"�ABCDEFGHIJKLM@-�"
   "�NOPQRSTUVWXYZ/*�"
   "�1234567890���,.�"},
  {"�abcdefghijklm�-�"
   "�nopqrstuvwxyz���"
   "�1234567890���,.�"},
  {"�ABCDEFGHIJKLM�-�"
   "�NOPQRSTUVWXYZ�ֽ"
   "�1234567890���,.�"}
};

static char keyboard_keymap[4][50] = {
  // Low_en_us
  {"1234567890-="
   "qwertyuiop[]"
   "asdfghjkl;\'#"
   "\\zxcvbnm,./"},
  // High_en_us
  {"!@#$%^&*()_+"
   "QWERTYUIOP{}"
   "ASDFGHJKL:*~"
   "|ZXCVBNM<>?"},
  // Low_sv_se
  {"1234567890+\'"
   "qwertyuiop�~"
   "asdfghjkl��\'"
   "<zxcvbnm,.-"},
  // High_sv_se
  {"!\"#�%&/()=?`"
   "QWERTYUIOP�^"
   "ASDFGHJKL��*"
   ">ZXCVBNM;:_"}};

keyboard_eKeymap KeyboardCtx::default_keymap = keyboard_eKeymap_Low_en_us;
keyboard_eType KeyboardCtx::default_type = keyboard_eType_Standard;

void KeyboardCtx::set_keymap( keyboard_eKeymap keymap)
{
  current_keymap = keymap;
  configure();
}

void KeyboardCtx::set_type( keyboard_eType t)
{
  type = t;
  configure();
}

void KeyboardCtx::configure()
{
  double x, y, width, height;
  double d = 4.0;
  double sep = 0.1;
  char key_text[40];
  char name[40];
  int i;
  char *keymap;

  if ( nodraw) 
    return;

  delete_all();

  switch ( type) {
  case keyboard_eType_Standard: {
    keymap = keyboard_keymap[current_keymap-1];
  
    y = 0;
    for ( int row = 0; row < 4; row++) {
      x = d + sep;

      for ( i = row * 12; i < (row + 1) * 12; i++) {
	if ( i == 47)
	  break;
	key_text[0] = keymap[i];
	key_text[1] = 0;

	sprintf( name, "Key%d", i);

	keys[i] = new GrowRect( this, name, x, y, d, d, 
				glow_eCtColor_ButtonBordercolor, 1, 0, glow_mDisplayLevel_1, 1, 1, 1, glow_eCtColor_ButtonFillcolor);
	insert( (GlowArrayElem *)keys[i]);
      
	((GrowRect *)keys[i])->shadow_width = 12;
	((GrowRect *)keys[i])->relief = glow_eRelief_Up;

	
	sprintf( name, "KeyText%d", i);
	
	text_keys[i] = new GrowText( this, name, key_text, x + d / 2 - 0.2, y + d / 2 + 0.2, 
				   glow_eDrawType_TextHelvetica, glow_eCtColor_ButtonTextcolor, 8);
	insert( text_keys[i]);

	x += d + sep;
      }
      y += d + sep;
    }

    // special keys
    for ( i = 47; i < 57; i++) {
      switch ( i) {
      case 47:  // Up
	x = (d + sep) * 12;
	y = (d + sep) * 3;
	width = d;
	height = d;
	strcpy( key_text, "Up");
	break;
      case 48:  // Down
	x = (d + sep) * 12;
	y = (d + sep) * 4;
	width = d;
	height = d;
	strcpy( key_text, "Down");
	break;
      case 49:  // Left
	x = (d + sep) * 11;
	y = (d + sep) * 4;
	width = d;
	height = d;
	strcpy( key_text, "Left");
	break;
      case 50:  // Right
	x = (d + sep) * 13;
	y = (d + sep) * 4;
	width = d;
	height = d;
	strcpy( key_text, "Right");
	break;
      case 51:  // Return
	x = (d + sep) * 13;
	y = (d + sep) * 1;
	width = d;
	height = 3 * d + 2 * sep;
	strcpy( key_text, "Enter");
	break;
      case 52:  // Backspace
	x = (d + sep) * 13;
	y = 0;
	width = d;
	height = d;
	strcpy( key_text, "BS");
	break;
      case 53:  // Escape
	x = 0;
	y = 0;
	width = d;
	height = d;
	strcpy( key_text, "Esc");
	break;
      case 54:  // Tab
	x = 0;
	y = d + sep;
	width = d;
	height = d;
	strcpy( key_text, "Tab");
	break;
      case 55:  // Shift
	x = 0;
	y = (d + sep) * 2;
	width = d;
	height = 2 * d + sep;
	strcpy( key_text, "Shift");
	break;
      case 56:  // Space
	x = 0;
	y = (d + sep) * 4;
	width = 11 * d + 10 * sep;
	height = d;
	strcpy( key_text, " ");
	break;
      }

      sprintf( name, "Key%d", i);

      keys[i] = new GrowRect( this, name, x, y, width, height, 
			      glow_eCtColor_ButtonBordercolor, 1, 0, glow_mDisplayLevel_1, 1, 1, 1, glow_eCtColor_ButtonFillcolor);
      insert( (GlowArrayElem *)keys[i]);

      ((GrowRect *)keys[i])->shadow_width = 12;
      ((GrowRect *)keys[i])->relief = glow_eRelief_Up;


      if ( i == 47) {
	// Up, draw arrow
	sprintf( name, "KeyLine%d", i);
      
	glow_sPoint p[4] = {{ x + d / 2, y + d / 2 - 0.6},
			    {x + d / 2 - 0.8, y + d / 2 + 0.6},
			    {x + d / 2 + 0.8, y + d / 2 + 0.6}};

	sprintf( name, "KeyPLine%d", i);
        GlowPolyLine *pline = new GrowPolyLine( this, name, p, 3, glow_eCtColor_ButtonTextcolor, 2, 0,
						1, 0, 0, glow_eCtColor_ButtonTextcolor, 1);
	insert( pline);
      }
      else if ( i == 48) {
	// Down, draw arrow
	sprintf( name, "KeyLine%d", i);
      
	glow_sPoint p[4] = {{ x + d / 2, y + d / 2 + 0.6},
			    {x + d / 2 - 0.8, y + d / 2 - 0.6},
			    {x + d / 2 + 0.8, y + d / 2 - 0.6}};

	sprintf( name, "KeyPLine%d", i);
        GlowPolyLine *pline = new GrowPolyLine( this, name, p, 3, glow_eCtColor_ButtonTextcolor, 2, 0,
						1, 0, 0, glow_eCtColor_ButtonTextcolor, 1);
	insert( pline);
      }
      else if ( i == 49) {
	// Left, draw arrow
	sprintf( name, "KeyLine%d", i);
      
	glow_sPoint p[4] = {{ x + d / 2 - 0.6, y + d / 2},
			    {x + d / 2 + 0.6, y + d / 2 - 0.8},
			    {x + d / 2 + 0.6, y + d / 2 + 0.8}};

	sprintf( name, "KeyPLine%d", i);
        GlowPolyLine *pline = new GrowPolyLine( this, name, p, 3, glow_eCtColor_ButtonTextcolor, 2, 0,
						1, 0, 0, glow_eCtColor_ButtonTextcolor, 1);
	insert( pline);
      }
      else if ( i == 50) {
	// Left, draw arrow
	sprintf( name, "KeyLine%d", i);
      
	glow_sPoint p[4] = {{ x + d / 2 + 0.6, y + d / 2},
			    {x + d / 2 - 0.6, y + d / 2 - 0.8},
			    {x + d / 2 - 0.6, y + d / 2 + 0.8}};

	sprintf( name, "KeyPLine%d", i);
        GlowPolyLine *pline = new GrowPolyLine( this, name, p, 3, glow_eCtColor_ButtonTextcolor, 2, 0,
						1, 0, 0, glow_eCtColor_ButtonTextcolor, 1);
	insert( pline);
      }
      else if ( i == 52) {
	// Backspace, draw arrow
	sprintf( name, "KeyLine%d", i);
      
	GrowLine *line = new GrowLine( this, name, x + d / 2 - 1.0, y + d / 2, x + d / 2 + 1.0, y + d / 2, 
					       glow_eCtColor_ButtonTextcolor, 2, 0);
	insert( line);
	glow_sPoint p[4] = {{ x + d / 2 - 1.0, y + d / 2},
			    {x + d / 2 - 0.3, y + d / 2 + 0.3},
			    {x + d / 2 - 0.3, y + d / 2 - 0.3}};

	sprintf( name, "KeyPLine%d", i);
        GlowPolyLine *pline = new GrowPolyLine( this, name, p, 3, glow_eCtColor_ButtonTextcolor, 2, 0,
						1, 0, 0, glow_eCtColor_ButtonTextcolor, 1);
	insert( pline);
      }
      else if ( i == 51) {
	// Return, draw arrow
	sprintf( name, "KeyLine%d", i);
      
	GrowLine *line = new GrowLine( this, name, x + d / 2 - 1.0, y + 2 * d + d / 2, x + d / 2, y + 2 * d + d / 2, 
					       glow_eCtColor_ButtonTextcolor, 2, 0);
	insert( line);
	line = new GrowLine( this, name, x + d / 2, y + 2 * d + 0.1, x + d / 2, y + 2 * d + d / 2, 
					       glow_eCtColor_ButtonTextcolor, 2, 0);
	insert( line);
	glow_sPoint p[4] = {{ x + d / 2 - 1.0, y + 2 * d + d / 2},
			    {x + d / 2 - 0.3, y + 2 * d + d / 2 + 0.3},
			    {x + d / 2 - 0.3, y + 2 * d + d / 2 - 0.3}};

	sprintf( name, "KeyPLine%d", i);
        GlowPolyLine *pline = new GrowPolyLine( this, name, p, 3, glow_eCtColor_ButtonTextcolor, 2, 0,
						1, 0, 0, glow_eCtColor_ButtonTextcolor, 1);
	insert( pline);

	sprintf( name, "KeyText%d", i);
      
	text_keys[i] = new GrowText( this, name, key_text, x + d / 2 - 1.4, y + d / 2 + 0.2, 
				     glow_eDrawType_TextHelvetica, glow_eCtColor_ButtonTextcolor, 8);
	insert( text_keys[i]);
      }
      else {
	sprintf( name, "KeyText%d", i);
      
	text_keys[i] = new GrowText( this, name, key_text, x + d / 2 - 1.4, y + d / 2 + 0.2, 
				     glow_eDrawType_TextHelvetica, glow_eCtColor_ButtonTextcolor, 8);
	insert( text_keys[i]);
      }
    }
    break;
  }  
  case keyboard_eType_Numeric: {
    keymap = keyboard_keymap_numeric[current_keymap-1];
  
    y = 0;
    for ( int row = 0; row < 4; row++) {
      x = 0;

      for ( i = row * 4; i < (row + 1) * 4; i++) {
	if ( keymap[i] == 'x') {
	  x += d + sep;
	  continue;
	}
	key_text[0] = keymap[i];
	key_text[1] = 0;

	sprintf( name, "Key%d", i);

	keys[i] = new GrowRect( this, name, x, y, d, d, 
				glow_eCtColor_ButtonBordercolor, 1, 0, glow_mDisplayLevel_1, 1, 1, 1, glow_eCtColor_ButtonFillcolor);
	insert( (GlowArrayElem *)keys[i]);
      
	((GrowRect *)keys[i])->shadow_width = 12;
	((GrowRect *)keys[i])->relief = glow_eRelief_Up;

	
	sprintf( name, "KeyText%d", i);
	
	text_keys[i] = new GrowText( this, name, key_text, x + d / 2 - 0.2, y + d / 2 + 0.2, 
				   glow_eDrawType_TextHelvetica, glow_eCtColor_ButtonTextcolor, 8);
	insert( text_keys[i]);

	x += d + sep;
      }
      y += d + sep;
    }
    // special keys
    for ( i = 0; i < 16; i++) {
      if ( keymap[i] != 'x')
	continue;
      switch ( i) {
      case 3:  // Backspace
	x = (d + sep) * 3;
	y = 0;
	width = d;
	height = d;
	strcpy( key_text, "BS");
	break;
      case 15:  // Enter
	x = (d + sep) * 3;
	y = (d + sep) * 3;
	width = d;
	height = d;
	strcpy( key_text, "Enter");
	break;
      case 12:  // Escape
	x = 0;
	y = (d + sep) * 3;
	width = d;
	height = d;
	strcpy( key_text, "Esc");
	break;
      }

      sprintf( name, "Key%d", i);

      keys[i] = new GrowRect( this, name, x, y, width, height, 
			      glow_eCtColor_ButtonBordercolor, 1, 0, glow_mDisplayLevel_1, 1, 1, 1, glow_eCtColor_ButtonFillcolor);
      insert( (GlowArrayElem *)keys[i]);

      ((GrowRect *)keys[i])->shadow_width = 12;
      ((GrowRect *)keys[i])->relief = glow_eRelief_Up;

      if ( i == 3) {
	// Backspace, draw arrow
	sprintf( name, "KeyLine%d", i);
      
	GrowLine *line = new GrowLine( this, name, x + d / 2 - 1.0, y + d / 2, x + d / 2 + 1.0, y + d / 2, 
					       glow_eCtColor_ButtonTextcolor, 2, 0);
	insert( line);
	glow_sPoint p[4] = {{ x + d / 2 - 1.0, y + d / 2},
			    {x + d / 2 - 0.3, y + d / 2 + 0.3},
			    {x + d / 2 - 0.3, y + d / 2 - 0.3}};

	sprintf( name, "KeyPLine%d", i);
        GlowPolyLine *pline = new GrowPolyLine( this, name, p, 3, glow_eCtColor_ButtonTextcolor, 2, 0,
						1, 0, 0, glow_eCtColor_ButtonTextcolor, 1);
	insert( pline);
      }
      else if ( i == 15) {
	// Return, draw arrow
	sprintf( name, "KeyLine%d", i);
      
	GrowLine *line = new GrowLine( this, name, x + d / 2 - 1.0, y + d / 2 + 0.7, x + d / 2 + 0.5, y + d / 2 + 0.7, 
					       glow_eCtColor_ButtonTextcolor, 2, 0);
	insert( line);
	line = new GrowLine( this, name, x + d / 2 + 0.5 , y + d / 2 + 0.1, x + d / 2 + 0.5, y + d / 2  + 0.7, 
					       glow_eCtColor_ButtonTextcolor, 2, 0);
	insert( line);
	glow_sPoint p[4] = {{ x + d / 2 - 1.0, y + d / 2 + 0.7},
			    {x + d / 2 - 0.3, y + d / 2 + 0.7 + 0.3},
			    {x + d / 2 - 0.3, y + d / 2 + 0.7 - 0.3}};

	sprintf( name, "KeyPLine%d", i);
        GlowPolyLine *pline = new GrowPolyLine( this, name, p, 3, glow_eCtColor_ButtonTextcolor, 2, 0,
						1, 0, 0, glow_eCtColor_ButtonTextcolor, 1);
	insert( pline);

	sprintf( name, "KeyText%d", i);
      
	text_keys[i] = new GrowText( this, name, key_text, x + d / 2 - 1.4, y + d / 2 - 0.6, 
				     glow_eDrawType_TextHelvetica, glow_eCtColor_ButtonTextcolor, 8);
	insert( text_keys[i]);
      }
      else {
	sprintf( name, "KeyText%d", i);
      
	text_keys[i] = new GrowText( this, name, key_text, x + d / 2 - 1.4, y + d / 2 + 0.2, 
				     glow_eDrawType_TextHelvetica, glow_eCtColor_ButtonTextcolor, 8);
	insert( text_keys[i]);
      }
    }
    break;
  }
  case keyboard_eType_Alphabetic: {
    keymap = keyboard_keymap_alphabetic[current_keymap-1];
  
    y = 0;
    for ( int row = 0; row < 3; row++) {
      x = 0;

      for ( i = row * 17; i < (row + 1) * 17; i++) {
	if ( keymap[i] == '�') {
	  x += d + sep;
	  continue;
	}
	key_text[0] = keymap[i];
	key_text[1] = 0;

	sprintf( name, "Key%d", i);

	keys[i] = new GrowRect( this, name, x, y, d, d, 
				glow_eCtColor_ButtonBordercolor, 1, 0, glow_mDisplayLevel_1, 1, 1, 1, glow_eCtColor_ButtonFillcolor);
	insert( (GlowArrayElem *)keys[i]);
      
	((GrowRect *)keys[i])->shadow_width = 12;
	((GrowRect *)keys[i])->relief = glow_eRelief_Up;

	
	sprintf( name, "KeyText%d", i);
	
	text_keys[i] = new GrowText( this, name, key_text, x + d / 2 - 0.2, y + d / 2 + 0.2, 
				   glow_eDrawType_TextHelvetica, glow_eCtColor_ButtonTextcolor, 8);
	insert( text_keys[i]);

	x += d + sep;
      }
      y += d + sep;
    }
    // special keys
    for ( i = 0; i < 51; i++) {
      if ( keymap[i] != '�')
	continue;
      switch ( i) {
      case 16:  // Backspace
	x = (d + sep) * 16;
	y = 0;
	width = d;
	height = d;
	strcpy( key_text, "BS");
	break;
      case 33:  // Enter
	x = (d + sep) * 16;
	y = (d + sep);
	width = d;
	height = 2 * d + sep;
	strcpy( key_text, "Enter");
	break;
      case 45:  // Space
	x = (d + sep) * 11;
	y = (d + sep) * 2;
	width = 3 * d + 2 * sep;
	height = d;
	strcpy( key_text, " ");
	break;
      case 0:  // Escape
	x = 0;
	y = 0;
	width = d;
	height = d;
	strcpy( key_text, "Esc");
	break;
      case 17:  // Shift
	x = 0;
	y = d + sep;
	width = d;
	height = 2 * d + sep;
	strcpy( key_text, "Shift");
	break;
      default:
	continue;
      }

      sprintf( name, "Key%d", i);

      keys[i] = new GrowRect( this, name, x, y, width, height, 
			      glow_eCtColor_ButtonBordercolor, 1, 0, glow_mDisplayLevel_1, 1, 1, 1, glow_eCtColor_ButtonFillcolor);
      insert( (GlowArrayElem *)keys[i]);

      ((GrowRect *)keys[i])->shadow_width = 12;
      ((GrowRect *)keys[i])->relief = glow_eRelief_Up;

      if ( i == 16) {
	// Backspace, draw arrow
	sprintf( name, "KeyLine%d", i);
      
	GrowLine *line = new GrowLine( this, name, x + d / 2 - 1.0, y + d / 2, x + d / 2 + 1.0, y + d / 2, 
					       glow_eCtColor_ButtonTextcolor, 2, 0);
	insert( line);
	glow_sPoint p[4] = {{ x + d / 2 - 1.0, y + d / 2},
			    {x + d / 2 - 0.3, y + d / 2 + 0.3},
			    {x + d / 2 - 0.3, y + d / 2 - 0.3}};

	sprintf( name, "KeyPLine%d", i);
        GlowPolyLine *pline = new GrowPolyLine( this, name, p, 3, glow_eCtColor_ButtonTextcolor, 2, 0,
						1, 0, 0, glow_eCtColor_ButtonTextcolor, 1);
	insert( pline);
      }
      else if ( i == 33) {
	// Return, draw arrow
	sprintf( name, "KeyLine%d", i);
      
	GrowLine *line = new GrowLine( this, name, x + d / 2 - 1.0, y + d + d / 2, x + d / 2, y + d + d / 2, 
					       glow_eCtColor_ButtonTextcolor, 2, 0);
	insert( line);
	line = new GrowLine( this, name, x + d / 2, y + d + 0.1, x + d / 2, y + d + d / 2, 
					       glow_eCtColor_ButtonTextcolor, 2, 0);
	insert( line);
	glow_sPoint p[4] = {{ x + d / 2 - 1.0, y + d + d / 2},
			    {x + d / 2 - 0.3, y + d + d / 2 + 0.3},
			    {x + d / 2 - 0.3, y + d + d / 2 - 0.3}};

	sprintf( name, "KeyPLine%d", i);
        GlowPolyLine *pline = new GrowPolyLine( this, name, p, 3, glow_eCtColor_ButtonTextcolor, 2, 0,
						1, 0, 0, glow_eCtColor_ButtonTextcolor, 1);
	insert( pline);

	sprintf( name, "KeyText%d", i);
      
	text_keys[i] = new GrowText( this, name, key_text, x + d / 2 - 1.4, y + d / 2 - 0.2, 
				     glow_eDrawType_TextHelvetica, glow_eCtColor_ButtonTextcolor, 8);
	insert( text_keys[i]);
      }
      else {
	sprintf( name, "KeyText%d", i);
      
	text_keys[i] = new GrowText( this, name, key_text, x + d / 2 - 1.4, y + d / 2 + 0.2, 
				     glow_eDrawType_TextHelvetica, glow_eCtColor_ButtonTextcolor, 8);
	insert( text_keys[i]);
      }

    }
    break;
  }
  }
  get_borders();
  a.zoom();
  redraw();
  hot_mode = glow_eHotMode_Disabled;
}


void KeyboardCtx::redraw()
{
  clear( &mw);
  draw( &mw, 0, 0, mw.window_width, mw.window_height);
  nav_zoom();
}

void KeyboardCtx::zoom( double factor)
{ 
  if ( fabs(factor) < DBL_EPSILON)
    return;

  mw.zoom_factor_x *= factor;
  mw.zoom_factor_y *= factor;
  if ( mw.offset_x != 0)
    mw.offset_x = int( (mw.offset_x - mw.window_width / 2.0 * ( 1.0/factor - 1)) 
		* factor);
  if ( mw.offset_y != 0)
    mw.offset_y = int( (mw.offset_y  - mw.window_height / 2.0 * ( 1.0/factor - 1)) 
		* factor);
  mw.offset_x = max( mw.offset_x, 0);
  mw.offset_y = max( mw.offset_y, 0);
  if ( (x_right - x_left) * mw.zoom_factor_x <= mw.window_width)
    mw.offset_x = 0;
  if ( (y_high - y_low) * mw.zoom_factor_y <= mw.window_height)
    mw.offset_y = 0;
  a.zoom();
  clear( &mw);
  draw( &mw, 0, 0, mw.window_width, mw.window_height);
  nav_zoom();
}

int KeyboardCtx::event_handler( glow_eEvent event, int x, int y, int w, int h)
{
  int 		sts;
  int 		i;
  GlowCtx	*ctx;
  double 	fx, fy;
  int		keyc;
  int 		send_cb = 0;
  int		idx;
  char 		*keymap;
  char 		name[40];

  ctx = this;
//  cout << "Event: " << event << endl;

  fx = double( x + ctx->mw.offset_x) / ctx->mw.zoom_factor_x;
  fy = double( y + ctx->mw.offset_y) / ctx->mw.zoom_factor_y;

  callback_object_type = glow_eObjectType_NoObject;
  callback_object = 0;

  switch ( event) {
  case glow_eEvent_MB1Up: {
    sts = 0;
    for ( i = 0; i < a.a_size; i++) {
      sts = a.a[i]->event_handler( &ctx->mw, event, x, y, fx, fy);
      if ( sts == GLOW__NO_PROPAGATE)
	break;
    }
    
    if ( callback_object_type != glow_eObjectType_NoObject) {
      int idx_found = 0;
      callback_object->get_object_name( name);

      if ( callback_object->type() == glow_eObjectType_GrowText) {
	if ( strncmp( name, "KeyText", 7) == 0) {
	  sscanf( &name[7], "%d", &idx);
	  idx_found = 1;
	}
      }
      else if ( callback_object->type() == glow_eObjectType_GrowPolyLine) {	
	if ( strncmp( name, "KeyPLine", 8) == 0) {
	  sscanf( &name[8], "%d", &idx);
	  idx_found = 1;
	}
      }
      else if ( callback_object->type() == glow_eObjectType_GrowLine) {
	if ( strncmp( name, "KeyLine", 7) == 0) {
	  sscanf( &name[7], "%d", &idx);
	  idx_found = 1;
	}
      }
      else if ( callback_object->type() == glow_eObjectType_GrowRect) {
	if ( strncmp( name, "Key", 3) == 0) {
	  sscanf( &name[3], "%d", &idx);
	  idx_found = 1;
	}
      }
      if ( idx_found) {
	// Invert rect
	((GrowRect *)keys[idx])->relief = glow_eRelief_Up;
	((GrowRect *)keys[idx])->draw();
      }
    }
    break;
  }
  case glow_eEvent_MB1Down: {
    sts = 0;
    for ( i = 0; i < a.a_size; i++) {
      sts = a.a[i]->event_handler( &ctx->mw, event, x, y, fx, fy);
      if ( sts == GLOW__NO_PROPAGATE)
	break;
    }
    
    ctx->gdraw->set_click_sensitivity( &ctx->mw, glow_mSensitivity_MB1Click);

    if ( callback_object_type != glow_eObjectType_NoObject) {
      int idx_found = 0;
      callback_object->get_object_name( name);

      if ( callback_object->type() == glow_eObjectType_GrowText) {
	if ( strncmp( name, "KeyText", 7) == 0) {
	  sscanf( &name[7], "%d", &idx);
	  idx_found = 1;
	}
      }
      else if ( callback_object->type() == glow_eObjectType_GrowPolyLine) {	
	if ( strncmp( name, "KeyPLine", 8) == 0) {
	  sscanf( &name[8], "%d", &idx);
	  idx_found = 1;
	}
      }
      else if ( callback_object->type() == glow_eObjectType_GrowLine) {
	if ( strncmp( name, "KeyLine", 7) == 0) {
	  sscanf( &name[7], "%d", &idx);
	  idx_found = 1;
	}
      }
      else if ( callback_object->type() == glow_eObjectType_GrowRect) {
	if ( strncmp( name, "Key", 3) == 0) {
	  sscanf( &name[3], "%d", &idx);
	  idx_found = 1;
	}
      }
      if ( idx_found) {
	// Invert rect
	((GrowRect *)keys[idx])->relief = glow_eRelief_Down;
	((GrowRect *)keys[idx])->draw();
      }
    }
    break;
  }
  case glow_eEvent_MB1Click:
  case glow_eEvent_MB1ClickShift:
  case glow_eEvent_MB2Click: {
    sts = 0;
    for ( i = 0; i < a.a_size; i++) {
      sts = a.a[i]->event_handler( &ctx->mw, event, x, y, fx, fy);
      if ( sts == GLOW__NO_PROPAGATE)
	break;
    }
    
    if ( callback_object_type != glow_eObjectType_NoObject) {
      int idx_found = 0;
      callback_object->get_object_name( name);
      if ( callback_object->type() == glow_eObjectType_GrowText) {
	if ( strncmp( name, "KeyText", 7) == 0) {	  
	  sscanf( &name[7], "%d", &idx);
	  idx_found = 1;
	}
      }
      else if ( callback_object->type() == glow_eObjectType_GrowPolyLine) {	
	if ( strncmp( name, "KeyPLine", 8) == 0) {
	  sscanf( &name[8], "%d", &idx);
	  idx_found = 1;
	}
      }
      else if ( callback_object->type() == glow_eObjectType_GrowLine) {
	if ( strncmp( name, "KeyLine", 7) == 0) {
	  sscanf( &name[7], "%d", &idx);
	  idx_found = 1;
	}
      }
      else if ( callback_object->type() == glow_eObjectType_GrowRect) {
	if ( strncmp( name, "Key", 3) == 0) {
	  sscanf( &name[3], "%d", &idx);
	  idx_found = 1;
	}
      }
      if ( idx_found) {
	switch ( type) {
	case keyboard_eType_Standard: {
	  keymap = keyboard_keymap[current_keymap-1];

	  if ( idx < 48) {
	    keyc = keymap[idx];
	    send_cb = 1;
	  }
	  else {
	    switch ( idx) {
	    case 47: // Up
	      keyc = 0xff52;
	      send_cb = 1;
	      break;
	    case 48: // Down
	      keyc = 0xff54;
	      send_cb = 1;
	      break;
	    case 49: // Left 
	      keyc = 0xff51;
	      send_cb = 1;
	      break;
	    case 50: // Right
	      keyc = 0xff53;
	      send_cb = 1;
	      break;
	    case 51: // Return
	      keyc = 13;
	      send_cb = 1;
	      break;
	    case 52: // Backspace
	      keyc = 8;
	      send_cb = 1;
	      break;
	    case 53: // Escape
	      keyc = 27;
	      send_cb = 1;
	      break;
	    case 54: // Tab
	      keyc = 9;
	      send_cb = 1;
	      break;
	    case 55: // Shift
	      if ( ODD((int)current_keymap))
		current_keymap = (keyboard_eKeymap)(current_keymap + 1);
	      else
		current_keymap = (keyboard_eKeymap)(current_keymap - 1);
	      configure();
	      break;
	    case 56: // Space
	      keyc = 32;
	      send_cb = 1;
	      break;
	    }
	  }	    
	  break;
	}
	case keyboard_eType_Numeric: {
	  keymap = keyboard_keymap_numeric[current_keymap-1];
	  if ( idx < 16) {
	    if ( keymap[idx] != 'x') {
	      keyc = keymap[idx];
	      send_cb = 1;
	    }
	    else if ( idx == 3) {
	      // Backspace
	      keyc = 8;
	      send_cb = 1;
	    }
	    else if ( idx == 12) {
	      // Escape
	      keyc = 27;
	      send_cb = 1;
	    }
	    else if ( idx == 15) {
	      // Return
	      keyc = 13;
	      send_cb = 1;
	    }
	  }
	  break;
	}
	case keyboard_eType_Alphabetic: {
	  keymap = keyboard_keymap_alphabetic[current_keymap-1];
	  if ( idx < 51) {
	    if ( keymap[idx] != '�') {
	      keyc = keymap[idx];
	      send_cb = 1;
	    }
	    else if ( idx == 16) {
	      // Backspace
	      keyc = 8;
	      send_cb = 1;
	    }
	    else if ( idx == 0) {
	      // Escape
	      keyc = 27;
	      send_cb = 1;
	    }
	    else if ( idx == 33) {
	      // Return
	      keyc = 13;
	      send_cb = 1;
	    }
	    else if ( idx == 45) {
	      // Space
	      keyc = ' ';
	      send_cb = 1;
	    }
	    else if ( idx == 17) {
	      // Shift
	      if ( ODD((int)current_keymap))
		current_keymap = (keyboard_eKeymap)(current_keymap + 1);
	      else
		current_keymap = (keyboard_eKeymap)(current_keymap - 1);
	      configure();
	    }
	  }
	  break;
	}
	}
      }
      if ( send_cb) {
	if ( event_callback[glow_eEvent_Key_Ascii] &&
	     sts != GLOW__NO_PROPAGATE) {
	  static glow_sEvent e;

	  e.event = glow_eEvent_Key_Ascii;
	  e.any.type = glow_eEventType_KeyAscii;
	  e.any.x_pixel = x;
	  e.any.y_pixel = y;
	  e.any.x = 1.0 * (x + mw.offset_x) / mw.zoom_factor_x;
	  e.any.y = 1.0 * (y + mw.offset_y) / mw.zoom_factor_y;
	  e.key.ascii = keyc;
	  event_callback[glow_eEvent_Key_Ascii]( this, &e);
	}
      }
    }
    break;  
  }
  case glow_eEvent_Exposure:
    int width, height;

    gdraw->get_window_size( &mw, &width, &height);
    if ( mw.window_width != width || mw.window_height != height) {
      mw.window_width = width;
      mw.window_height = height;
    }
    
    draw( &mw, x, y, x + w, y + h);
    break;
  default:
    ;
  }

#if 0
  if ( event_callback[event] &&
	sts != GLOW__NO_PROPAGATE && callback)
  {
    static glow_sEvent e;

    e.event = event;
    e.any.type = glow_eEventType_Object;
    e.any.x_pixel = x;
    e.any.y_pixel = y;
    e.any.x = 1.0 * (x + mw.offset_x) / mw.zoom_factor_x;
    e.any.y = 1.0 * (y + mw.offset_y) / mw.zoom_factor_y;
    e.object.object_type = callback_object_type;
    if ( callback_object_type != glow_eObjectType_NoObject)
      e.object.object = callback_object;
    event_callback[event]( this, &e);
  }
#endif
  return 1;
}

void KeyboardCtx::get_size( int *width, int *height)
{
  *width = int((x_right - x_left)*mw.zoom_factor_x);
  *height = int((y_high - y_low)*mw.zoom_factor_y);
}

void KeyboardCtx::set_size( int width, int height)
{
  mw.zoom_factor_x = ((float)width)/(x_right - x_left);
  mw.zoom_factor_y = ((float)height)/(y_high - y_low);
  a.zoom();
  redraw();
}

void KeyboardCtx::set_shift( int shift)
{
  if ( shift) {
    if ( ODD((int)current_keymap))
      current_keymap = (keyboard_eKeymap)(current_keymap + 1);
  }
  else {
    if ( EVEN((int)current_keymap))
      current_keymap = (keyboard_eKeymap)(current_keymap - 1);
  }
  configure();
}