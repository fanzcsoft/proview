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

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include "co_cdh.h"
#include "co_dcli.h"
#include "co_time.h"
#include "rt_load.h"
#include "wb_pkg_msg.h"

#include "flow.h"
#include "flow_browctx.h"
#include "flow_browapi.h"
#include "flow_browwidget_qt.h"

#include "wb_wrev_qt.h"
#include "wb_wrevnav_qt.h"
#include "wb_wnav.h"
#include "wb_wnav_brow.h"
#include "wb_wnav_item.h"
#include "wb_error.h"

WRevNavQt::WRevNavQt(void *wa_parent_ctx, QWidget *wa_parent_wid,
                     ldh_tSession wa_ldhses, const char *wa_name,
                     wb_eUtility wa_utility, QWidget **w, pwr_tStatus *status)
    : WRevNav(wa_parent_ctx, wa_ldhses, wa_name, wa_utility, status)
{
  debug_print("creating a scrolledbrowwidgetqt\n");
  form_widget = scrolledbrowwidgetqt_new(init_brow_cb, this, &brow_widget);

  showNow(brow_widget);

  *w = form_widget;
  *status = 1;
}

WRevNavQt::~WRevNavQt()
{
  delete brow;
  form_widget->close();
}

void WRevNavQt::set_inputfocus()
{
  if (!displayed) {
    return;
  }
  brow_widget->setFocus();
}