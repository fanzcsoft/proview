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

#include <math.h>

#include "cow_qt_helpers.h"

#include "flow_printdraw_qt.h"

FlowPrintDrawQt::FlowPrintDrawQt(void* context, const char* t, int p,
    void* flow_ctx, int page_border, int* sts)
    : painter((QPainter*)context), border(page_border), ctx((FlowCtx*)flow_ctx),
      show_red(1), page(p)
{
  strncpy(title, t, sizeof(title));

  if (ctx->type() == flow_eCtxType_Flow) {
    print_margin_x = 10;
    print_margin_y = 10;
  }
  print_margin_x = print_margin_y = 0;

  *sts = 1;
}

FlowPrintDrawQt::~FlowPrintDrawQt()
{
  debug_print("FlowPrintDrawQt::~FlowPrintDrawQt\n");
}

int FlowPrintDrawQt::print_page(
    double ll_x, double ll_y, double ur_x, double ur_y)
{
  if (ctx->type() == flow_eCtxType_Flow) {
    if (ur_x - ll_x > ur_y - ll_y) {
      print_margin_x = 10;
      print_margin_y = 60;
    } else {
      print_margin_x = 60;
      print_margin_y = 10;
    }
  }
  print_margin_x = print_margin_y = 0;

  if (ur_x - ll_x > ur_y - ll_y) {
    ctx->print_zoom_factor = 730 / (ur_x - ll_x);
  } else {
    ctx->print_zoom_factor = 730 / (ur_y - ll_y);
  }

  ctx->print_zoom();
  page_x = ll_x * ctx->print_zoom_factor;
  page_y = ll_y * ctx->print_zoom_factor;

  double width = painter->window().width();
  double height = painter->window().height();

  if (ctx->type() == flow_eCtxType_Brow) {
    QPen pen = QPen(QColor::fromRgbF(0, 0, 0));
    pen.setWidthF(0.5);
    painter->setPen(pen);
    painter->drawLine(
        print_margin_x, print_margin_y, print_margin_x + width, print_margin_y);

    painter->setFont(QFont("Lucida Sans", 9));

    char page_str[40];
    sprintf(page_str, "Page %d", page + 1);
    painter->drawText(
        print_margin_x + width - 90, print_margin_y - height, fl(page_str));
    painter->drawText(print_margin_x, print_margin_y - height, fl(title));
  }

  painter->setClipRect(print_margin_x, print_margin_y,
      (ur_x - ll_x) * ctx->print_zoom_factor,
      (ur_y - ll_y) * ctx->print_zoom_factor);
  painter->setClipping(true);

  ((FlowCtx*)ctx)->current_print = this;
  ((FlowCtx*)ctx)->print(ll_x, ll_y, ur_x, ur_y);

  painter->setClipping(false);

  return 1;
}

int FlowPrintDrawQt::rect(double x, double y, double width, double height,
    flow_eDrawType type, double idx, int highlight)
{
  QPen pen;

  if (highlight || type == flow_eDrawType_LineRed) {
    pen = QPen(QColor::fromRgbF(1, 0, 0));
  } else {
    pen = QPen(QColor::fromRgbF(0, 0, 0));
  }

  pen.setWidth(0.5 * idx);
  painter->setPen(pen);
  painter->setBrush(Qt::NoBrush);
  painter->drawRect(
      print_margin_x + x - page_x, print_margin_y + y - page_y, width, height);

  return 1;
}

int FlowPrintDrawQt::filled_rect(double x, double y, double width,
    double height, flow_eDrawType type, double idx)
{
  switch (type) {
  case flow_eDrawType_LineRed:
    painter->setBrush(QBrush(QColor::fromRgbF(1, 0, 0)));
    break;
  case flow_eDrawType_Green:
    painter->setBrush(QBrush(QColor::fromRgbF(0, 1, 0)));
    break;
  case flow_eDrawType_Yellow:
    painter->setBrush(QBrush(QColor::fromRgbF(1, 1, 0)));
    break;
  case flow_eDrawType_DarkGray:
    painter->setBrush(QBrush(QColor::fromRgbF(0.3, 0.3, 0.3)));
    break;
  default:
    painter->setBrush(QBrush(QColor::fromRgbF(0, 0, 0)));
  }

  QPen pen = QPen(QColor::fromRgbF(0, 0, 0));
  pen.setWidth(0.5 * idx);
  painter->setPen(pen);
  painter->drawRect(
      print_margin_x + x - page_x, print_margin_y + y - page_y, width, height);

  return 1;
}

int FlowPrintDrawQt::arc(double x, double y, double width, double height,
    int angle1, int angle2, flow_eDrawType type, double idx, int highlight)
{
  QPen pen;
  if (highlight || type == flow_eDrawType_LineRed
      || type == flow_eDrawType_LineDashedRed) {
    pen = QPen(QColor::fromRgbF(1, 0, 0));
  } else {
    pen = QPen(QColor::fromRgbF(0, 0, 0));
  }

  if (type == flow_eDrawType_LineDashed
      || type == flow_eDrawType_LineDashedRed) {
    QVector<double> dashes;
    dashes << 4 << 2;
    pen.setDashPattern(dashes);
  }
  pen.setWidth(0.5 * idx);
  painter->setPen(pen);

  painter->drawArc(print_margin_x + x + width / 2 - page_x,
      print_margin_y + y + height / 2 - page_y, width, width,
      -M_PI * (angle1 + angle2) / 180, -M_PI * angle1 / 180);

  if (type == flow_eDrawType_LineDashed
      || type == flow_eDrawType_LineDashedRed) {
    QPen pen = painter->pen();
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
  }

  return 1;
}

int FlowPrintDrawQt::line(double x1, double y1, double x2, double y2,
    flow_eDrawType type, double idx, int highlight)
{
  QPen pen;
  switch (type) {
  case flow_eDrawType_LineGray:
    pen = QPen(QColor::fromRgbF(0.7, 0.7, 0.7));
    break;
  case flow_eDrawType_LineRed:
  case flow_eDrawType_LineDashedRed:
    pen = QPen(QColor::fromRgbF(1, 0, 0));
    break;
  default:
    pen = QPen(QColor::fromRgbF(0, 0, 0));
    break;
  }

  if (type == flow_eDrawType_LineDashed
      || type == flow_eDrawType_LineDashedRed) {
    QVector<double> dashes;
    dashes << 4 << 2;
    pen.setDashPattern(dashes);
  }
  pen.setWidth(0.5 * idx);
  painter->setPen(pen);

  painter->drawLine(print_margin_x + x1 - page_x, print_margin_y + y1 - page_y,
      print_margin_x + x2 - page_x, print_margin_y + y2 - page_y);

  if (type == flow_eDrawType_LineDashed
      || type == flow_eDrawType_LineDashedRed) {
    QPen pen = painter->pen();
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
  }

  return 1;
}

int FlowPrintDrawQt::text(double x, double y, char* text, int len,
    flow_eDrawType type, double size, int line)
{
  QFont font;
  switch (type) {
  case flow_eDrawType_TextHelvetica:
    font = QFont(fl("Lucida Sans"), -1);
    break;
  case flow_eDrawType_TextHelveticaBold:
    font = QFont(fl("Lucida Sans"), -1, QFont::Bold);
    break;
  default:;
  }
  font.setPointSizeF(size);

  char* s;
  for (s = text; *s; s++) {
    if (*s == 10) {
      break;
    }
  }

  if (s - text > 0) {
    QString textutf8 = QString::fromLatin1(text, s - text);

    painter->setFont(font);
    painter->setBrush(QBrush(QColor::fromRgbF(0, 0, 0)));

    painter->drawText(
        print_margin_x + x - page_x, print_margin_y + y - page_y, textutf8);
  }

  return 1;
}

int FlowPrintDrawQt::pixmap(
    double x, double y, flow_sPixmapDataElem* data, flow_eDrawType type)
{
  float scale = 0.7;
  painter->scale(scale, scale);
  painter->drawPixmap((print_margin_x + x - page_x) / scale,
      (print_margin_y + y - page_y) / scale, *((QPixmap*)data));
  painter->scale(1.0 / scale, 1.0 / scale);

  return 1;
}

int FlowPrintDrawQt::image(double x, double y, double width, double height,
    flow_tImImage image, flow_eDrawType type)
{
  float scale = width / ((QImage*)image)->width();
  painter->scale(scale, scale);
  painter->drawImage((print_margin_x + x - page_x) / scale,
      (print_margin_y + y - page_y) / scale, *((QImage*)image));
  painter->scale(1.0 / scale, 1.0 / scale);

  return 1;
}

int FlowPrintDrawQt::arrow(double x1, double y1, double x2, double y2,
    double x3, double y3, flow_eDrawType type, double idx)
{
  switch (type) {
  case flow_eDrawType_LineRed:
    painter->setPen(QColor::fromRgbF(1, 0, 0));
    painter->setBrush(QBrush(QColor::fromRgbF(1, 0, 0)));
    break;
  case flow_eDrawType_LineGray:
    painter->setPen(QColor::fromRgbF(0.7, 0.7, 0.7));
    painter->setBrush(QBrush(QColor::fromRgbF(0.7, 0.7, 0.7)));
    break;
  default:
    painter->setPen(QColor::fromRgbF(0, 0, 0));
    painter->setBrush(QBrush(QColor::fromRgbF(0, 0, 0)));
    break;
  }

  QPainterPath path;
  path.moveTo(print_margin_x + x1 - page_x, print_margin_y + y1 - page_y);
  path.lineTo(print_margin_x + x2 - page_x, print_margin_y + y2 - page_y);
  path.lineTo(print_margin_x + x3 - page_x, print_margin_y + y3 - page_y);
  path.lineTo(print_margin_x + x1 - page_x, print_margin_y + y1 - page_y);
  painter->fillPath(path, painter->brush());

  return 1;
}