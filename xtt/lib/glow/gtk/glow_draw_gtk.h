/*
 * ProviewR   Open Source Process Control.
 * Copyright (C) 2005-2018 SSAB EMEA AB.
 *
 * This file is part of ProviewR.
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
 * along with ProviewR. If not, see <http://www.gnu.org/licenses/>
 *
 * Linking ProviewR statically or dynamically with other modules is
 * making a combined work based on ProviewR. Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the copyright holders of
 * ProviewR give you permission to, from the build function in the
 * ProviewR Configurator, combine ProviewR with modules generated by the
 * ProviewR PLC Editor to a PLC program, regardless of the license
 * terms of these modules. You may copy and distribute the resulting
 * combined work under the terms of your choice, provided that every
 * copy of the combined work is accompanied by a complete copy of
 * the source code of ProviewR (the version used to produce the
 * combined work), being distributed under the terms of the GNU
 * General Public License plus this exception.
 */

#ifndef glow_draw_gtk_h
#define glow_draw_gtk_h

#include <stdlib.h>

#include <fstream>

#include <gtk/gtk.h>

#include "glow_draw.h"

#define DRAW_CLIP_SIZE 10
#define CUSTOMCOLORS_STACK_SIZE 10

class GlowCustomColorsGtk;

class DrawWindGtk : public DrawWind {
public:
  DrawWindGtk();
  GtkWidget* toplevel;
  GtkWidget* shell;
  GdkWindow* window;
  GdkColormap* colormap;
  GdkPixmap* buffer;
  int buffer_width;
  int buffer_height;
  int clip_on;
  int clip_cnt;
  GdkRectangle clip_rectangle[DRAW_CLIP_SIZE];
  GdkPixmap* background_pixmap;
  int background_pixmap_width;
  int background_pixmap_height;
};

class GlowDrawGtk : public GlowDraw {
public:
  GlowDrawGtk(GtkWidget* toplevel, void** glow_ctx,
      int (*init_proc)(GtkWidget* w, GlowCtx* ctx, void* client_data),
      void* client_data, glow_eCtxType type);
  ~GlowDrawGtk();

  DrawWindGtk m_wind;
  DrawWindGtk nav_wind;
  GdkDisplay* display;
  GdkScreen* screen;
  GdkGC* gc;
  GdkGC* gc_erase;
  GdkGC* gc_inverse;
  GdkGC* gcs[glow_eDrawType_CustomColor__][DRAW_TYPE_SIZE];
  GdkFont* font[glow_eFont__][glow_eDrawFont__][DRAW_FONT_SIZE];
  GdkCursor* cursors[glow_eDrawCursor__];
  int ef;
  GdkColormap* colormap;
  GdkColor background;
  GdkColor original_background;
  guint timer_id;
  int click_sensitivity;
  GdkColor custom_color_vect[120];
  GdkColor color_vect[400];
  int color_vect_cnt;
  int closing_down;
  GlowCustomColorsGtk* customcolors[CUSTOMCOLORS_STACK_SIZE];
  int customcolors_cnt;

  int event_handler(GdkEvent event);
  virtual void enable_event(glow_eEvent event, glow_eEventType event_type,
      int (*event_cb)(GlowCtx* ctx, glow_tEvent event));
  virtual void clear(GlowWind* w);
  virtual void copy_buffer(GlowWind* w, int ll_x, int ll_y, int ur_x, int ur_y);

  virtual void get_window_size(GlowWind* w, int* width, int* height);
  virtual void set_window_size(GlowWind* w, int width, int height);

  virtual int rect(GlowWind* w, int x, int y, int width, int height,
      glow_eDrawType gc_type, int idx, int highlight);
  virtual int rect_erase(
      GlowWind* w, int x, int y, int width, int height, int idx);
  virtual int arrow(GlowWind* w, int x1, int y1, int x2, int y2, int x3, int y3,
      glow_eDrawType gc_type, int idx, int highlight);
  virtual int arrow_erase(
      GlowWind* w, int x1, int y1, int x2, int y2, int x3, int y3, int idx);
  virtual int arc(GlowWind* w, int x, int y, int width, int height, int angle1,
      int angle2, glow_eDrawType gc_type, int idx, int highlight);
  virtual int fill_arc(GlowWind* w, int x, int y, int width, int height,
      int angle1, int angle2, glow_eDrawType gc_type, int highlight);
  virtual int arc_erase(GlowWind* w, int x, int y, int width, int height,
      int angle1, int angle2, int idx);
  virtual int line(GlowWind* w, int x1, int y1, int x2, int y2,
      glow_eDrawType gc_type, int idx, int highlight);
  virtual int line_dashed(GlowWind* w, int x1, int y1, int x2, int y2,
      glow_eDrawType gc_type, int idx, int highlight, glow_eLineType line_type);
  virtual int line_erase(GlowWind* w, int x1, int y1, int x2, int y2, int idx);
  virtual int polyline(GlowWind* w, glow_sPointX* points, int point_cnt,
      glow_eDrawType gc_type, int idx, int highlight);
  virtual int fill_polyline(GlowWind* w, glow_sPointX* points, int point_cnt,
      glow_eDrawType gc_type, int highlight);
  virtual int polyline_erase(
      GlowWind* w, glow_sPointX* points, int point_cnt, int idx);
  virtual int text(GlowWind* w, int x, int y, char* text, int len,
      glow_eDrawType gc_type, glow_eDrawType color, int idx, int highlight,
      int line, glow_eFont font_idx, double size, int rot);
  virtual int text_cursor(GlowWind* w, int x, int y, char* text, int len,
      glow_eDrawType gc_type, glow_eDrawType color, int idx, int highlight,
      int pos, glow_eFont font, double size);
  virtual int text_erase(GlowWind* w, int x, int y, char* text, int len,
      glow_eDrawType gc_type, int idx, int line, glow_eFont font_idx,
      double size, int rot);
  virtual int fill_rect(
      GlowWind* w, int x, int y, int width, int height, glow_eDrawType gc_type);
  virtual int pixmaps_create(
      GlowWind* w, glow_sPixmapData* pixmap_data, void** pixmaps);
  virtual void pixmaps_delete(GlowWind* w, void* pixmaps);
  virtual int pixmap(GlowWind* w, int x, int y, glow_sPixmapData* pixmap_data,
      void* pixmaps, glow_eDrawType gc_type, int idx, int highlight, int line);
  virtual int pixmap_inverse(GlowWind* w, int x, int y,
      glow_sPixmapData* pixmap_data, void* pixmaps, glow_eDrawType gc_type,
      int idx, int line);
  virtual int pixmap_erase(GlowWind* w, int x, int y,
      glow_sPixmapData* pixmap_data, void* pixmaps, glow_eDrawType gc_type,
      int idx, int line);
  virtual int image(GlowWind* w, int x, int y, int width, int height,
      glow_tImImage image, glow_tPixmap pixmap, glow_tPixmap clip_mask);
  virtual int image_d(GlowWind* wind, double x, double y, int width, int height,
      glow_tImImage image, glow_tPixmap pixmap, glow_tPixmap clip_mask);

  virtual void set_cursor(GlowWind* w, glow_eDrawCursor cursor);
  virtual int get_text_extent(const char* text, int len, glow_eDrawType gc_type,
      int idx, glow_eFont font_idx, int* width, int* height, int* descent,
      double size, int rot);
  virtual void copy_area(GlowWind* w, int x, int y);
  virtual void clear_area(GlowWind* w, int ll_x, int ur_x, int ll_y, int ur_y);
  virtual void set_inputfocus(GlowWind* w);
  virtual void set_background(GlowWind* w, glow_eDrawType drawtype,
      glow_tPixmap pixmap, glow_tImImage image, int pixmap_width,
      int pixmap_height);
  virtual void reset_background(GlowWind* w);
  virtual void set_image_clip_mask(glow_tPixmap pixmap, int x, int y);
  virtual void reset_image_clip_mask();
  virtual int set_clip_rectangle(
      GlowWind* w, int ll_x, int ll_y, int ur_x, int ur_y);
  virtual void reset_clip_rectangle(GlowWind* w);
  virtual int clip_level(GlowWind* w);
  virtual int draw_point(GlowWind* w, int x1, int y1, glow_eDrawType gc_type);
  virtual int draw_points(GlowWind* w, glow_sPointX* points, int point_num,
      glow_eDrawType gc_type, int idx = 0);
  virtual void set_click_sensitivity(GlowWind* w, int value);
  virtual void draw_background(GlowWind* wind, int x, int y, int w, int h);
  virtual int create_buffer(GlowWind* w);
  virtual void delete_buffer(GlowWind* w);
  virtual void buffer_background(DrawWind* w, GlowCtx* cctx);
  virtual int print(char* filename, double x0, double x1, int end);
  virtual int export_image(char* filename);
  void set_clip(DrawWind* w, GdkGC* gc);
  void reset_clip(DrawWind* w, GdkGC* gc);

  virtual void set_timer(GlowCtx* gctx, int time_ms,
      void (*callback_func)(GlowCtx* ctx), void** id);
  virtual void remove_timer(void* id);
  int init_nav(GtkWidget* nav_widget);
  GdkPoint* points_to_gdk_points(glow_sPointX* points, int point_cnt);
  GdkPoint* points_to_gdk_points_curve(
      GlowWind* w, glow_sPointX* points, int point_cnt, int* cnt);
  int get_font_type(int gc_type);
  void load_font(glow_eFont font_idx, int font_type, int idx);

  int image_get_width(glow_tImImage image);
  int image_get_height(glow_tImImage image);
  int image_get_rowstride(glow_tImImage image);
  unsigned char* image_get_data(glow_tImImage image);
  void image_copy(glow_tImImage orig_image, glow_tImImage* image);
  void image_rotate(glow_tImImage* image, int to_rotation, int from_rotation);
  void image_flip_vertical(glow_tImImage* image);
  void image_flip_horizontal(glow_tImImage* image);
  int image_scale(int width, int height, glow_tImImage orig_im,
      glow_tImImage* im, glow_tImData* im_data, glow_tPixmap* im_pixmap,
      glow_tPixmap* im_mask);
  int image_load(char* imagefile, glow_tImImage* orig_im, glow_tImImage* im,
      glow_tImData* im_data);
  int image_render(int width, int height, glow_tImImage orig_im,
      glow_tImImage* im, glow_tPixmap* im_pixmap, glow_tPixmap* im_mask);
  void image_free(glow_tImImage image);
  void pixmap_free(glow_tPixmap pixmap);
  void image_pixel_iter(glow_tImImage orig_image, glow_tImImage* image,
      void (*pixel_cb)(void*, unsigned char*), void* userdata);

  void set_cairo_clip(DrawWind* wind, cairo_t* cr);
  void reset_cairo_clip(DrawWind* wind, cairo_t* cr);
  int gradient_create_pattern(int x, int y, int w, int h, glow_eDrawType d0,
      glow_eDrawType d1, glow_eDrawType d2, glow_eGradient gradient,
      cairo_pattern_t** pat);
  virtual glow_eGradient gradient_rotate(double rot, glow_eGradient gradient);
  virtual int gradient_fill_rect(GlowWind* wind, int x, int y, int w, int h,
      glow_eDrawType d0, glow_eDrawType d1, glow_eDrawType d2,
      glow_eGradient gradient);
  virtual int gradient_fill_rectrounded(GlowWind* wind, int x, int y, int w,
      int h, int roundamount, glow_eDrawType d0, glow_eDrawType d1,
      glow_eDrawType d2, glow_eGradient gradient);
  virtual int gradient_fill_arc(GlowWind* wind, int x, int y, int w, int h,
      int angle1, int angle2, glow_eDrawType d0, glow_eDrawType d1,
      glow_eDrawType d2, glow_eGradient gradient);
  virtual int gradient_fill_polyline(GlowWind* wind, glow_sPointX* points,
      int point_cnt, glow_eDrawType d0, glow_eDrawType d1, glow_eDrawType d2,
      glow_eGradient gradient);
  int text_pango(GlowWind* wind, int x, int y, char* text, int len,
      glow_eDrawType gc_type, glow_eDrawType color, int idx, int highlight,
      int line, glow_eFont font_idx, double size, int rot);
  int text_erase_pango(GlowWind* wind, int x, int y, char* text, int len,
      glow_eDrawType gc_type, int idx, int line, glow_eFont font_idx,
      double size, int rot);
  int get_text_extent_pango(const char* text, int len, glow_eDrawType gc_type,
      int idx, glow_eFont font_idx, int* width, int* height, int* descent,
      double size, int rot);
  void log_event(GdkEvent* event);
  virtual void event_exec(void* event, unsigned int size);
  virtual int open_color_selection(double* r, double* g, double* b);
  virtual void update_color(glow_eDrawType color);
  void push_customcolors(GlowCustomColors* cc);
  void set_customcolors(GlowCustomColors* cc);
  void pop_customcolors();
  GlowCustomColors* create_customcolors();
  GlowCustomColorsGtk* get_customcolors();
  void reset_customcolors(GlowCustomColors* cc);
};

class DrawPs {
public:
  DrawPs(char* filename) : fp(filename), x(0), y(0)
  {
  }
  ~DrawPs()
  {
    fp.close();
  }
  std::ofstream fp;
  double x;
  double y;
};

#endif
