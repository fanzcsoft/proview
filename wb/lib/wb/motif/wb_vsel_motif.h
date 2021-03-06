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

#ifndef wb_vsel_h_motif
#define wb_vsel_h_motif

/* wb_vsel_motif.h -- select volume */

#ifndef wb_vsel_h
#include "wb_vsel.h"
#endif

struct vsel_widgets {
  Widget toplevel;
  Widget vsel_window;
  Widget label;
  Widget adb;
  Widget volumelist;
  Widget creavolume_dia;
  Widget volumeid;
  Widget volumename;
  Widget volumeclass;
  Widget creavolume;
};

class WVselMotif : public WVsel {
public:
  WVselMotif(pwr_tStatus* status, void* wv_parent_ctx, Widget wv_parent_wid,
      const char* wv_name, ldh_tWBContext wv_wbctx, char* volumename,
      int (*bc_success)(void*, pwr_tVolumeId*, int), void (*bc_cancel)(),
      int (*bc_time_to_exit)(void*), int show_volumes, wb_eType wv_wb_type);
  ~WVselMotif();

  Widget parent_wid;
  struct vsel_widgets widgets;

  void message(char* new_label);
  void list_add_item(char* str);
  void list_clear();

  // Backcall functions from uil
  static void activate_showall(
      Widget w, WVselMotif* vsel, XmToggleButtonCallbackStruct* data);
  static void activate_ok(
      Widget w, WVselMotif* vsel, XmAnyCallbackStruct* data);
  static void activate_cancel(
      Widget w, WVselMotif* vsel, XmAnyCallbackStruct* data);
  static void activate_close(
      Widget w, WVselMotif* vsel, XmAnyCallbackStruct* data);
  static void create_label(
      Widget w, WVselMotif* vsel, XmAnyCallbackStruct* data);
  static void create_adb(Widget w, WVselMotif* vsel, XmAnyCallbackStruct* data);
  static void destroy_adb(
      Widget w, WVselMotif* vsel, XmAnyCallbackStruct* data);
  static void create_volumelist(
      Widget w, WVselMotif* vsel, XmAnyCallbackStruct* data);
  static void action_volumelist(
      Widget w, WVselMotif* vsel, XmListCallbackStruct* data);
};

#endif
