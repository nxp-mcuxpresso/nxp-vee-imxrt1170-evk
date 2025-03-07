/*
 * C
 *
 * Copyright 2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * @file
 * @brief Implementation of all drawing functions of ui_font_drawing.h.
 * @author MicroEJ Developer Team
 * @version 14.2.0
 * @see ui_font_drawing.h
 */

// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------

#include <LLUI_DISPLAY.h>

#include "ui_font_drawing.h"
#include "ui_drawing_soft.h"
#include "dw_drawing_soft.h"
#include "ui_drawing_stub.h"
#include "ui_configuration.h"
#include "bsp_util.h"

#if defined(UI_FEATURE_FONT_CUSTOM_FORMATS)

/*
 * VEE Port supports several fonts formats: internal (Font Generator RAW format) and one or
 * several custom formats. The next functions and tables redirect the font drawing
 * to the right font manager (stub, soft or custom).
 *
 * VEE Port may support several destination formats: display format, one or several
 * standard formats and one or several custom formats. See _get_table_index().
 *
 * Note: The functions are called by ui_drawing.c. The use of GPU should be checked
 * in ui_drawing.c and not here.
 */

// --------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------

/*
 * @brief Indices in the tables UI_FONT_DRAWING_xxx_custom[]
 */
#define TABLE_INDEX_STUB 0          // index to call functions stub
#define TABLE_INDEX_SOFT 1          // index to call functions in software
#define TABLE_INDEX_CUSTOM_OFFSET 2 // index to call VEE Port functions

#define GET_CUSTOM_FONT_INDEX(font) (((uint8_t)TABLE_INDEX_CUSTOM_OFFSET) + ((uint8_t)MICROUI_FONT_FORMAT_CUSTOM_0) - \
									 ((uint8_t)((font)->format)))

// --------------------------------------------------------------------------------
// Extern functions
// --------------------------------------------------------------------------------

/*
 * @brief Set of drawing functions according to the source font format.
 *
 * These functions must be declared in other H files.
 */

extern jint UI_FONT_DRAWING_stringWidth_custom0(jchar *chars, jint length, MICROUI_Font *font);
extern jint UI_FONT_DRAWING_stringWidth_custom1(jchar *chars, jint length, MICROUI_Font *font);
extern jint UI_FONT_DRAWING_stringWidth_custom2(jchar *chars, jint length, MICROUI_Font *font);
extern jint UI_FONT_DRAWING_stringWidth_custom3(jchar *chars, jint length, MICROUI_Font *font);
extern jint UI_FONT_DRAWING_stringWidth_custom4(jchar *chars, jint length, MICROUI_Font *font);
extern jint UI_FONT_DRAWING_stringWidth_custom5(jchar *chars, jint length, MICROUI_Font *font);
extern jint UI_FONT_DRAWING_stringWidth_custom6(jchar *chars, jint length, MICROUI_Font *font);
extern jint UI_FONT_DRAWING_stringWidth_custom7(jchar *chars, jint length, MICROUI_Font *font);

extern jint UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom0(jchar *chars, jint length, MICROUI_Font *font,
                                                                         MICROUI_RenderableString *renderableString);
extern jint UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom1(jchar *chars, jint length, MICROUI_Font *font,
                                                                         MICROUI_RenderableString *renderableString);
extern jint UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom2(jchar *chars, jint length, MICROUI_Font *font,
                                                                         MICROUI_RenderableString *renderableString);
extern jint UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom3(jchar *chars, jint length, MICROUI_Font *font,
                                                                         MICROUI_RenderableString *renderableString);
extern jint UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom4(jchar *chars, jint length, MICROUI_Font *font,
                                                                         MICROUI_RenderableString *renderableString);
extern jint UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom5(jchar *chars, jint length, MICROUI_Font *font,
                                                                         MICROUI_RenderableString *renderableString);
extern jint UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom6(jchar *chars, jint length, MICROUI_Font *font,
                                                                         MICROUI_RenderableString *renderableString);
extern jint UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom7(jchar *chars, jint length, MICROUI_Font *font,
                                                                         MICROUI_RenderableString *renderableString);

extern DRAWING_Status UI_FONT_DRAWING_drawString_custom0(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                         MICROUI_Font *font, jint x, jint y);
extern DRAWING_Status UI_FONT_DRAWING_drawString_custom1(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                         MICROUI_Font *font, jint x, jint y);
extern DRAWING_Status UI_FONT_DRAWING_drawString_custom2(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                         MICROUI_Font *font, jint x, jint y);
extern DRAWING_Status UI_FONT_DRAWING_drawString_custom3(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                         MICROUI_Font *font, jint x, jint y);
extern DRAWING_Status UI_FONT_DRAWING_drawString_custom4(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                         MICROUI_Font *font, jint x, jint y);
extern DRAWING_Status UI_FONT_DRAWING_drawString_custom5(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                         MICROUI_Font *font, jint x, jint y);
extern DRAWING_Status UI_FONT_DRAWING_drawString_custom6(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                         MICROUI_Font *font, jint x, jint y);
extern DRAWING_Status UI_FONT_DRAWING_drawString_custom7(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                         MICROUI_Font *font, jint x, jint y);

extern DRAWING_Status UI_FONT_DRAWING_drawRenderableString_custom0(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                   jint length, MICROUI_Font *font, jint width,
                                                                   MICROUI_RenderableString *renderableString, jint x,
                                                                   jint y);
extern DRAWING_Status UI_FONT_DRAWING_drawRenderableString_custom1(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                   jint length, MICROUI_Font *font, jint width,
                                                                   MICROUI_RenderableString *renderableString, jint x,
                                                                   jint y);
extern DRAWING_Status UI_FONT_DRAWING_drawRenderableString_custom2(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                   jint length, MICROUI_Font *font, jint width,
                                                                   MICROUI_RenderableString *renderableString, jint x,
                                                                   jint y);
extern DRAWING_Status UI_FONT_DRAWING_drawRenderableString_custom3(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                   jint length, MICROUI_Font *font, jint width,
                                                                   MICROUI_RenderableString *renderableString, jint x,
                                                                   jint y);
extern DRAWING_Status UI_FONT_DRAWING_drawRenderableString_custom4(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                   jint length, MICROUI_Font *font, jint width,
                                                                   MICROUI_RenderableString *renderableString, jint x,
                                                                   jint y);
extern DRAWING_Status UI_FONT_DRAWING_drawRenderableString_custom5(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                   jint length, MICROUI_Font *font, jint width,
                                                                   MICROUI_RenderableString *renderableString, jint x,
                                                                   jint y);
extern DRAWING_Status UI_FONT_DRAWING_drawRenderableString_custom6(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                   jint length, MICROUI_Font *font, jint width,
                                                                   MICROUI_RenderableString *renderableString, jint x,
                                                                   jint y);
extern DRAWING_Status UI_FONT_DRAWING_drawRenderableString_custom7(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                   jint length, MICROUI_Font *font, jint width,
                                                                   MICROUI_RenderableString *renderableString, jint x,
                                                                   jint y);

extern DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear_custom0(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                       jint length, MICROUI_Font *font, jint x, jint y,
                                                                       jfloat xRatio, jfloat yRatio);
extern DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear_custom1(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                       jint length, MICROUI_Font *font, jint x, jint y,
                                                                       jfloat xRatio, jfloat yRatio);
extern DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear_custom2(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                       jint length, MICROUI_Font *font, jint x, jint y,
                                                                       jfloat xRatio, jfloat yRatio);
extern DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear_custom3(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                       jint length, MICROUI_Font *font, jint x, jint y,
                                                                       jfloat xRatio, jfloat yRatio);
extern DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear_custom4(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                       jint length, MICROUI_Font *font, jint x, jint y,
                                                                       jfloat xRatio, jfloat yRatio);
extern DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear_custom5(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                       jint length, MICROUI_Font *font, jint x, jint y,
                                                                       jfloat xRatio, jfloat yRatio);
extern DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear_custom6(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                       jint length, MICROUI_Font *font, jint x, jint y,
                                                                       jfloat xRatio, jfloat yRatio);
extern DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear_custom7(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                       jint length, MICROUI_Font *font, jint x, jint y,
                                                                       jfloat xRatio, jfloat yRatio);

extern DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom0(MICROUI_GraphicsContext *gc,
                                                                                 jchar *chars, jint length,
                                                                                 MICROUI_Font *font, jint width,
                                                                                 MICROUI_RenderableString *
                                                                                 renderableString, jint x, jint y,
                                                                                 jfloat xRatio, jfloat yRatio);
extern DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom1(MICROUI_GraphicsContext *gc,
                                                                                 jchar *chars, jint length,
                                                                                 MICROUI_Font *font, jint width,
                                                                                 MICROUI_RenderableString *
                                                                                 renderableString, jint x, jint y,
                                                                                 jfloat xRatio, jfloat yRatio);
extern DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom2(MICROUI_GraphicsContext *gc,
                                                                                 jchar *chars, jint length,
                                                                                 MICROUI_Font *font, jint width,
                                                                                 MICROUI_RenderableString *
                                                                                 renderableString, jint x, jint y,
                                                                                 jfloat xRatio, jfloat yRatio);
extern DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom3(MICROUI_GraphicsContext *gc,
                                                                                 jchar *chars, jint length,
                                                                                 MICROUI_Font *font, jint width,
                                                                                 MICROUI_RenderableString *
                                                                                 renderableString, jint x, jint y,
                                                                                 jfloat xRatio, jfloat yRatio);
extern DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom4(MICROUI_GraphicsContext *gc,
                                                                                 jchar *chars, jint length,
                                                                                 MICROUI_Font *font, jint width,
                                                                                 MICROUI_RenderableString *
                                                                                 renderableString, jint x, jint y,
                                                                                 jfloat xRatio, jfloat yRatio);
extern DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom5(MICROUI_GraphicsContext *gc,
                                                                                 jchar *chars, jint length,
                                                                                 MICROUI_Font *font, jint width,
                                                                                 MICROUI_RenderableString *
                                                                                 renderableString, jint x, jint y,
                                                                                 jfloat xRatio, jfloat yRatio);
extern DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom6(MICROUI_GraphicsContext *gc,
                                                                                 jchar *chars, jint length,
                                                                                 MICROUI_Font *font, jint width,
                                                                                 MICROUI_RenderableString *
                                                                                 renderableString, jint x, jint y,
                                                                                 jfloat xRatio, jfloat yRatio);
extern DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom7(MICROUI_GraphicsContext *gc,
                                                                                 jchar *chars, jint length,
                                                                                 MICROUI_Font *font, jint width,
                                                                                 MICROUI_RenderableString *
                                                                                 renderableString, jint x, jint y,
                                                                                 jfloat xRatio, jfloat yRatio);

extern DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear_custom0(MICROUI_GraphicsContext *gc, jchar c,
                                                                           MICROUI_Font *font, jint x, jint y,
                                                                           jint xRotation, jint yRotation, jfloat angle,
                                                                           jint alpha);
extern DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear_custom1(MICROUI_GraphicsContext *gc, jchar c,
                                                                           MICROUI_Font *font, jint x, jint y,
                                                                           jint xRotation, jint yRotation, jfloat angle,
                                                                           jint alpha);
extern DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear_custom2(MICROUI_GraphicsContext *gc, jchar c,
                                                                           MICROUI_Font *font, jint x, jint y,
                                                                           jint xRotation, jint yRotation, jfloat angle,
                                                                           jint alpha);
extern DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear_custom3(MICROUI_GraphicsContext *gc, jchar c,
                                                                           MICROUI_Font *font, jint x, jint y,
                                                                           jint xRotation, jint yRotation, jfloat angle,
                                                                           jint alpha);
extern DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear_custom4(MICROUI_GraphicsContext *gc, jchar c,
                                                                           MICROUI_Font *font, jint x, jint y,
                                                                           jint xRotation, jint yRotation, jfloat angle,
                                                                           jint alpha);
extern DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear_custom5(MICROUI_GraphicsContext *gc, jchar c,
                                                                           MICROUI_Font *font, jint x, jint y,
                                                                           jint xRotation, jint yRotation, jfloat angle,
                                                                           jint alpha);
extern DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear_custom6(MICROUI_GraphicsContext *gc, jchar c,
                                                                           MICROUI_Font *font, jint x, jint y,
                                                                           jint xRotation, jint yRotation, jfloat angle,
                                                                           jint alpha);
extern DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear_custom7(MICROUI_GraphicsContext *gc, jchar c,
                                                                           MICROUI_Font *font, jint x, jint y,
                                                                           jint xRotation, jint yRotation, jfloat angle,
                                                                           jint alpha);

extern DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom0(MICROUI_GraphicsContext *gc, jchar c,
                                                                                  MICROUI_Font *font, jint x, jint y,
                                                                                  jint xRotation, jint yRotation,
                                                                                  jfloat angle, jint alpha);
extern DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom1(MICROUI_GraphicsContext *gc, jchar c,
                                                                                  MICROUI_Font *font, jint x, jint y,
                                                                                  jint xRotation, jint yRotation,
                                                                                  jfloat angle, jint alpha);
extern DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom2(MICROUI_GraphicsContext *gc, jchar c,
                                                                                  MICROUI_Font *font, jint x, jint y,
                                                                                  jint xRotation, jint yRotation,
                                                                                  jfloat angle, jint alpha);
extern DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom3(MICROUI_GraphicsContext *gc, jchar c,
                                                                                  MICROUI_Font *font, jint x, jint y,
                                                                                  jint xRotation, jint yRotation,
                                                                                  jfloat angle, jint alpha);
extern DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom4(MICROUI_GraphicsContext *gc, jchar c,
                                                                                  MICROUI_Font *font, jint x, jint y,
                                                                                  jint xRotation, jint yRotation,
                                                                                  jfloat angle, jint alpha);
extern DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom5(MICROUI_GraphicsContext *gc, jchar c,
                                                                                  MICROUI_Font *font, jint x, jint y,
                                                                                  jint xRotation, jint yRotation,
                                                                                  jfloat angle, jint alpha);
extern DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom6(MICROUI_GraphicsContext *gc, jchar c,
                                                                                  MICROUI_Font *font, jint x, jint y,
                                                                                  jint xRotation, jint yRotation,
                                                                                  jfloat angle, jint alpha);
extern DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom7(MICROUI_GraphicsContext *gc, jchar c,
                                                                                  MICROUI_Font *font, jint x, jint y,
                                                                                  jint xRotation, jint yRotation,
                                                                                  jfloat angle, jint alpha);

// --------------------------------------------------------------------------------
// Typedef of drawing functions
// --------------------------------------------------------------------------------

typedef jint (*UI_FONT_DRAWING_stringWidth_t)(jchar *chars, jint length, MICROUI_Font *font);
typedef jint (*UI_FONT_DRAWING_initializeRenderableStringSNIContext_t)(jchar *chars, jint length, MICROUI_Font *font,
                                                                       MICROUI_RenderableString *renderableString);
typedef DRAWING_Status (*UI_FONT_DRAWING_drawString_t)(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                       MICROUI_Font *font, jint x, jint y);
typedef DRAWING_Status (*UI_FONT_DRAWING_drawRenderableString_t)(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                                 MICROUI_Font *font, jint width,
                                                                 MICROUI_RenderableString *renderableString, jint x,
                                                                 jint y);
typedef DRAWING_Status (*UI_FONT_DRAWING_drawScaledStringBilinear_t)(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                     jint length, MICROUI_Font *font, jint x, jint y,
                                                                     jfloat xRatio, jfloat yRatio);
typedef DRAWING_Status (*UI_FONT_DRAWING_drawScaledRenderableStringBilinear_t)(MICROUI_GraphicsContext *gc,
                                                                               jchar *chars, jint length,
                                                                               MICROUI_Font *font, jint width,
                                                                               MICROUI_RenderableString *
                                                                               renderableString, jint x, jint y,
                                                                               jfloat xRatio, jfloat yRatio);
typedef DRAWING_Status (*UI_FONT_DRAWING_drawCharWithRotationBilinear_t)(MICROUI_GraphicsContext *gc, jchar c,
                                                                         MICROUI_Font *font, jint x, jint y,
                                                                         jint xRotation, jint yRotation, jfloat angle,
                                                                         jint alpha);
typedef DRAWING_Status (*UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_t)(MICROUI_GraphicsContext *gc, jchar c,
                                                                                MICROUI_Font *font, jint x, jint y,
                                                                                jint xRotation, jint yRotation,
                                                                                jfloat angle, jint alph);

// --------------------------------------------------------------------------------
// Tables according to the source font format.
// --------------------------------------------------------------------------------

static const UI_FONT_DRAWING_stringWidth_t UI_FONT_DRAWING_stringWidth_custom[] = {
	&UI_DRAWING_STUB_stringWidth,
	&UI_DRAWING_SOFT_stringWidth,
	&UI_FONT_DRAWING_stringWidth_custom0,
	&UI_FONT_DRAWING_stringWidth_custom1,
	&UI_FONT_DRAWING_stringWidth_custom2,
	&UI_FONT_DRAWING_stringWidth_custom3,
	&UI_FONT_DRAWING_stringWidth_custom4,
	&UI_FONT_DRAWING_stringWidth_custom5,
	&UI_FONT_DRAWING_stringWidth_custom6,
	&UI_FONT_DRAWING_stringWidth_custom7,
};

static const UI_FONT_DRAWING_initializeRenderableStringSNIContext_t
    UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom[] = {
	&UI_DRAWING_STUB_initializeRenderableStringSNIContext,
	&UI_DRAWING_SOFT_initializeRenderableStringSNIContext,
	&UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom0,
	&UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom1,
	&UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom2,
	&UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom3,
	&UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom4,
	&UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom5,
	&UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom6,
	&UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom7,
};

static const UI_FONT_DRAWING_drawString_t UI_FONT_DRAWING_drawString_custom[] = {
	&UI_DRAWING_STUB_drawString,
	&UI_DRAWING_SOFT_drawString,
	&UI_FONT_DRAWING_drawString_custom0,
	&UI_FONT_DRAWING_drawString_custom1,
	&UI_FONT_DRAWING_drawString_custom2,
	&UI_FONT_DRAWING_drawString_custom3,
	&UI_FONT_DRAWING_drawString_custom4,
	&UI_FONT_DRAWING_drawString_custom5,
	&UI_FONT_DRAWING_drawString_custom6,
	&UI_FONT_DRAWING_drawString_custom7,
};

static const UI_FONT_DRAWING_drawRenderableString_t UI_FONT_DRAWING_drawRenderableString_custom[] = {
	&UI_DRAWING_STUB_drawRenderableString,
	&UI_DRAWING_SOFT_drawRenderableString,
	&UI_FONT_DRAWING_drawRenderableString_custom0,
	&UI_FONT_DRAWING_drawRenderableString_custom1,
	&UI_FONT_DRAWING_drawRenderableString_custom2,
	&UI_FONT_DRAWING_drawRenderableString_custom3,
	&UI_FONT_DRAWING_drawRenderableString_custom4,
	&UI_FONT_DRAWING_drawRenderableString_custom5,
	&UI_FONT_DRAWING_drawRenderableString_custom6,
	&UI_FONT_DRAWING_drawRenderableString_custom7,
};

static const UI_FONT_DRAWING_drawScaledStringBilinear_t UI_FONT_DRAWING_drawScaledStringBilinear_custom[] = {
	&UI_DRAWING_STUB_drawScaledStringBilinear,
	&DW_DRAWING_SOFT_drawScaledStringBilinear,
	&UI_FONT_DRAWING_drawScaledStringBilinear_custom0,
	&UI_FONT_DRAWING_drawScaledStringBilinear_custom1,
	&UI_FONT_DRAWING_drawScaledStringBilinear_custom2,
	&UI_FONT_DRAWING_drawScaledStringBilinear_custom3,
	&UI_FONT_DRAWING_drawScaledStringBilinear_custom4,
	&UI_FONT_DRAWING_drawScaledStringBilinear_custom5,
	&UI_FONT_DRAWING_drawScaledStringBilinear_custom6,
	&UI_FONT_DRAWING_drawScaledStringBilinear_custom7,
};

static const UI_FONT_DRAWING_drawScaledRenderableStringBilinear_t
    UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom[] = {
	&UI_DRAWING_STUB_drawScaledRenderableStringBilinear,
	&DW_DRAWING_SOFT_drawScaledRenderableStringBilinear,
	&UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom0,
	&UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom1,
	&UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom2,
	&UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom3,
	&UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom4,
	&UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom5,
	&UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom6,
	&UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom7,
};

static const UI_FONT_DRAWING_drawCharWithRotationBilinear_t UI_FONT_DRAWING_drawCharWithRotationBilinear_custom[] = {
	&UI_DRAWING_STUB_drawCharWithRotationBilinear,
	&DW_DRAWING_SOFT_drawCharWithRotationBilinear,
	&UI_FONT_DRAWING_drawCharWithRotationBilinear_custom0,
	&UI_FONT_DRAWING_drawCharWithRotationBilinear_custom1,
	&UI_FONT_DRAWING_drawCharWithRotationBilinear_custom2,
	&UI_FONT_DRAWING_drawCharWithRotationBilinear_custom3,
	&UI_FONT_DRAWING_drawCharWithRotationBilinear_custom4,
	&UI_FONT_DRAWING_drawCharWithRotationBilinear_custom5,
	&UI_FONT_DRAWING_drawCharWithRotationBilinear_custom6,
	&UI_FONT_DRAWING_drawCharWithRotationBilinear_custom7,
};

static const UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_t
    UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom[] = {
	&UI_DRAWING_STUB_drawCharWithRotationNearestNeighbor,
	&DW_DRAWING_SOFT_drawCharWithRotationNearestNeighbor,
	&UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom0,
	&UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom1,
	&UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom2,
	&UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom3,
	&UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom4,
	&UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom5,
	&UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom6,
	&UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom7,
};

// --------------------------------------------------------------------------------
// Private functions
// --------------------------------------------------------------------------------

static inline uint32_t _get_table_index(MICROUI_GraphicsContext *gc, MICROUI_Font *font) {
	uint32_t index;
	if (!LLUI_DISPLAY_isCustomFormat(font->format)) {
#if !defined(UI_GC_SUPPORTED_FORMATS) || (UI_GC_SUPPORTED_FORMATS <= 1)
		(void)gc;
		// standard font in display GC -> can use soft algo
		index = TABLE_INDEX_SOFT;
#else
		index = LLUI_DISPLAY_isDisplayFormat(gc->image.format) ? TABLE_INDEX_SOFT : TABLE_INDEX_STUB;
#endif
	} else {
		// use the specific font manager to draw the custom font
		// (this manager must check the destination format)
		// cppcheck-suppress [misra-c2012-10.6] convert font format to an index
		index = GET_CUSTOM_FONT_INDEX(font);
	}
	return index;
}

// --------------------------------------------------------------------------------
// ui_font_drawing.h functions
// --------------------------------------------------------------------------------

// See the header file for the function documentation
jint UI_FONT_DRAWING_stringWidth(jchar *chars, jint length, MICROUI_Font *font) {
	uint32_t index;
	if (!LLUI_DISPLAY_isCustomFormat(font->format)) {
		index = TABLE_INDEX_SOFT;
	} else {
		// use the specific font manager to manage the custom font
		// cppcheck-suppress [misra-c2012-10.6] convert font format to an index
		index = GET_CUSTOM_FONT_INDEX(font);
	}
	return (*UI_FONT_DRAWING_stringWidth_custom[index])(chars, length, font);
}

// See the header file for the function documentation
jint UI_FONT_DRAWING_initializeRenderableStringSNIContext(jchar *chars, jint length, MICROUI_Font *font,
                                                          MICROUI_RenderableString *renderableString) {
	uint32_t index;
	if (!LLUI_DISPLAY_isCustomFormat(font->format)) {
		index = TABLE_INDEX_SOFT;
	} else {
		// use the specific font manager to manage the custom font
		// cppcheck-suppress [misra-c2012-10.6] convert font format to an index
		index = GET_CUSTOM_FONT_INDEX(font);
	}
	return (*UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom[index])(chars, length, font, renderableString);
}

// See the header file for the function documentation
DRAWING_Status UI_FONT_DRAWING_drawString(MICROUI_GraphicsContext *gc, jchar *chars, jint length, MICROUI_Font *font,
                                          jint x, jint y) {
	return (*UI_FONT_DRAWING_drawString_custom[_get_table_index(gc, font)])(gc, chars, length, font, x, y);
}

// See the header file for the function documentation
DRAWING_Status UI_FONT_DRAWING_drawRenderableString(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                    MICROUI_Font *font, jint width,
                                                    MICROUI_RenderableString *renderableString, jint x, jint y) {
	return (*UI_FONT_DRAWING_drawRenderableString_custom[_get_table_index(gc, font)])(gc, chars, length, font, width,
	                                                                                  renderableString, x, y);
}

// See the header file for the function documentation
DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                        MICROUI_Font *font, jint x, jint y, jfloat xRatio,
                                                        jfloat yRatio) {
	return (*UI_FONT_DRAWING_drawScaledStringBilinear_custom[_get_table_index(gc, font)])(gc, chars, length, font, x, y,
	                                                                                      xRatio, yRatio);
}

// See the header file for the function documentation
DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                  jint length, MICROUI_Font *font, jint width,
                                                                  MICROUI_RenderableString *renderableString, jint x,
                                                                  jint y, jfloat xRatio, jfloat yRatio) {
	return (*UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom[_get_table_index(gc, font)])(gc, chars, length,
	                                                                                                font, width,
	                                                                                                renderableString, x,
	                                                                                                y, xRatio, yRatio);
}

// See the header file for the function documentation
DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear(MICROUI_GraphicsContext *gc, jchar c, MICROUI_Font *font,
                                                            jint x, jint y, jint xRotation, jint yRotation,
                                                            jfloat angle, jint alpha) {
	return (*UI_FONT_DRAWING_drawCharWithRotationBilinear_custom[_get_table_index(gc, font)])(gc, c, font, x, y,
	                                                                                          xRotation, yRotation,
	                                                                                          angle, alpha);
}

// See the header file for the function documentation
DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor(MICROUI_GraphicsContext *gc, jchar c,
                                                                   MICROUI_Font *font, jint x, jint y, jint xRotation,
                                                                   jint yRotation, jfloat angle, jint alpha) {
	return (*UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom[_get_table_index(gc, font)])(gc, c, font, x, y,
	                                                                                                 xRotation,
	                                                                                                 yRotation, angle,
	                                                                                                 alpha);
}

// --------------------------------------------------------------------------------
// Table weak functions
// --------------------------------------------------------------------------------

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint UI_FONT_DRAWING_stringWidth_custom0(jchar *chars, jint length, MICROUI_Font *font) {
	return UI_DRAWING_STUB_stringWidth(chars, length, font);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint UI_FONT_DRAWING_stringWidth_custom1(jchar *chars, jint length, MICROUI_Font *font) {
	return UI_DRAWING_STUB_stringWidth(chars, length, font);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint UI_FONT_DRAWING_stringWidth_custom2(jchar *chars, jint length, MICROUI_Font *font) {
	return UI_DRAWING_STUB_stringWidth(chars, length, font);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint UI_FONT_DRAWING_stringWidth_custom3(jchar *chars, jint length, MICROUI_Font *font) {
	return UI_DRAWING_STUB_stringWidth(chars, length, font);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint UI_FONT_DRAWING_stringWidth_custom4(jchar *chars, jint length, MICROUI_Font *font) {
	return UI_DRAWING_STUB_stringWidth(chars, length, font);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint UI_FONT_DRAWING_stringWidth_custom5(jchar *chars, jint length, MICROUI_Font *font) {
	return UI_DRAWING_STUB_stringWidth(chars, length, font);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint UI_FONT_DRAWING_stringWidth_custom6(jchar *chars, jint length, MICROUI_Font *font) {
	return UI_DRAWING_STUB_stringWidth(chars, length, font);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint UI_FONT_DRAWING_stringWidth_custom7(jchar *chars, jint length, MICROUI_Font *font) {
	return UI_DRAWING_STUB_stringWidth(chars, length, font);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom0(jchar *chars, jint length,
                                                                                        MICROUI_Font *font,
                                                                                        MICROUI_RenderableString *
                                                                                        renderableString) {
	return UI_DRAWING_STUB_initializeRenderableStringSNIContext(chars, length, font, renderableString);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom1(jchar *chars, jint length,
                                                                                        MICROUI_Font *font,
                                                                                        MICROUI_RenderableString *
                                                                                        renderableString) {
	return UI_DRAWING_STUB_initializeRenderableStringSNIContext(chars, length, font, renderableString);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom2(jchar *chars, jint length,
                                                                                        MICROUI_Font *font,
                                                                                        MICROUI_RenderableString *
                                                                                        renderableString) {
	return UI_DRAWING_STUB_initializeRenderableStringSNIContext(chars, length, font, renderableString);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom3(jchar *chars, jint length,
                                                                                        MICROUI_Font *font,
                                                                                        MICROUI_RenderableString *
                                                                                        renderableString) {
	return UI_DRAWING_STUB_initializeRenderableStringSNIContext(chars, length, font, renderableString);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom4(jchar *chars, jint length,
                                                                                        MICROUI_Font *font,
                                                                                        MICROUI_RenderableString *
                                                                                        renderableString) {
	return UI_DRAWING_STUB_initializeRenderableStringSNIContext(chars, length, font, renderableString);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom5(jchar *chars, jint length,
                                                                                        MICROUI_Font *font,
                                                                                        MICROUI_RenderableString *
                                                                                        renderableString) {
	return UI_DRAWING_STUB_initializeRenderableStringSNIContext(chars, length, font, renderableString);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom6(jchar *chars, jint length,
                                                                                        MICROUI_Font *font,
                                                                                        MICROUI_RenderableString *
                                                                                        renderableString) {
	return UI_DRAWING_STUB_initializeRenderableStringSNIContext(chars, length, font, renderableString);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint UI_FONT_DRAWING_initializeRenderableStringSNIContext_custom7(jchar *chars, jint length,
                                                                                        MICROUI_Font *font,
                                                                                        MICROUI_RenderableString *
                                                                                        renderableString) {
	return UI_DRAWING_STUB_initializeRenderableStringSNIContext(chars, length, font, renderableString);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawString_custom0(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                        jint length, MICROUI_Font *font, jint x,
                                                                        jint y) {
	return UI_DRAWING_STUB_drawString(gc, chars, length, font, x, y);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawString_custom1(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                        jint length, MICROUI_Font *font, jint x,
                                                                        jint y) {
	return UI_DRAWING_STUB_drawString(gc, chars, length, font, x, y);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawString_custom2(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                        jint length, MICROUI_Font *font, jint x,
                                                                        jint y) {
	return UI_DRAWING_STUB_drawString(gc, chars, length, font, x, y);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawString_custom3(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                        jint length, MICROUI_Font *font, jint x,
                                                                        jint y) {
	return UI_DRAWING_STUB_drawString(gc, chars, length, font, x, y);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawString_custom4(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                        jint length, MICROUI_Font *font, jint x,
                                                                        jint y) {
	return UI_DRAWING_STUB_drawString(gc, chars, length, font, x, y);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawString_custom5(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                        jint length, MICROUI_Font *font, jint x,
                                                                        jint y) {
	return UI_DRAWING_STUB_drawString(gc, chars, length, font, x, y);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawString_custom6(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                        jint length, MICROUI_Font *font, jint x,
                                                                        jint y) {
	return UI_DRAWING_STUB_drawString(gc, chars, length, font, x, y);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawString_custom7(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                        jint length, MICROUI_Font *font, jint x,
                                                                        jint y) {
	return UI_DRAWING_STUB_drawString(gc, chars, length, font, x, y);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawRenderableString_custom0(MICROUI_GraphicsContext *gc,
                                                                                  jchar *chars, jint length,
                                                                                  MICROUI_Font *font, jint width,
                                                                                  MICROUI_RenderableString *
                                                                                  renderableString, jint x, jint y) {
	return UI_DRAWING_STUB_drawRenderableString(gc, chars, length, font, width, renderableString, x, y);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawRenderableString_custom1(MICROUI_GraphicsContext *gc,
                                                                                  jchar *chars, jint length,
                                                                                  MICROUI_Font *font, jint width,
                                                                                  MICROUI_RenderableString *
                                                                                  renderableString, jint x, jint y) {
	return UI_DRAWING_STUB_drawRenderableString(gc, chars, length, font, width, renderableString, x, y);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawRenderableString_custom2(MICROUI_GraphicsContext *gc,
                                                                                  jchar *chars, jint length,
                                                                                  MICROUI_Font *font, jint width,
                                                                                  MICROUI_RenderableString *
                                                                                  renderableString, jint x, jint y) {
	return UI_DRAWING_STUB_drawRenderableString(gc, chars, length, font, width, renderableString, x, y);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawRenderableString_custom3(MICROUI_GraphicsContext *gc,
                                                                                  jchar *chars, jint length,
                                                                                  MICROUI_Font *font, jint width,
                                                                                  MICROUI_RenderableString *
                                                                                  renderableString, jint x, jint y) {
	return UI_DRAWING_STUB_drawRenderableString(gc, chars, length, font, width, renderableString, x, y);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawRenderableString_custom4(MICROUI_GraphicsContext *gc,
                                                                                  jchar *chars, jint length,
                                                                                  MICROUI_Font *font, jint width,
                                                                                  MICROUI_RenderableString *
                                                                                  renderableString, jint x, jint y) {
	return UI_DRAWING_STUB_drawRenderableString(gc, chars, length, font, width, renderableString, x, y);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawRenderableString_custom5(MICROUI_GraphicsContext *gc,
                                                                                  jchar *chars, jint length,
                                                                                  MICROUI_Font *font, jint width,
                                                                                  MICROUI_RenderableString *
                                                                                  renderableString, jint x, jint y) {
	return UI_DRAWING_STUB_drawRenderableString(gc, chars, length, font, width, renderableString, x, y);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawRenderableString_custom6(MICROUI_GraphicsContext *gc,
                                                                                  jchar *chars, jint length,
                                                                                  MICROUI_Font *font, jint width,
                                                                                  MICROUI_RenderableString *
                                                                                  renderableString, jint x, jint y) {
	return UI_DRAWING_STUB_drawRenderableString(gc, chars, length, font, width, renderableString, x, y);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawRenderableString_custom7(MICROUI_GraphicsContext *gc,
                                                                                  jchar *chars, jint length,
                                                                                  MICROUI_Font *font, jint width,
                                                                                  MICROUI_RenderableString *
                                                                                  renderableString, jint x, jint y) {
	return UI_DRAWING_STUB_drawRenderableString(gc, chars, length, font, width, renderableString, x, y);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear_custom0(MICROUI_GraphicsContext *gc,
                                                                                      jchar *chars, jint length,
                                                                                      MICROUI_Font *font, jint x,
                                                                                      jint y, jfloat xRatio,
                                                                                      jfloat yRatio) {
	return UI_DRAWING_STUB_drawScaledStringBilinear(gc, chars, length, font, x, y, xRatio, yRatio);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear_custom1(MICROUI_GraphicsContext *gc,
                                                                                      jchar *chars, jint length,
                                                                                      MICROUI_Font *font, jint x,
                                                                                      jint y, jfloat xRatio,
                                                                                      jfloat yRatio) {
	return UI_DRAWING_STUB_drawScaledStringBilinear(gc, chars, length, font, x, y, xRatio, yRatio);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear_custom2(MICROUI_GraphicsContext *gc,
                                                                                      jchar *chars, jint length,
                                                                                      MICROUI_Font *font, jint x,
                                                                                      jint y, jfloat xRatio,
                                                                                      jfloat yRatio) {
	return UI_DRAWING_STUB_drawScaledStringBilinear(gc, chars, length, font, x, y, xRatio, yRatio);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear_custom3(MICROUI_GraphicsContext *gc,
                                                                                      jchar *chars, jint length,
                                                                                      MICROUI_Font *font, jint x,
                                                                                      jint y, jfloat xRatio,
                                                                                      jfloat yRatio) {
	return UI_DRAWING_STUB_drawScaledStringBilinear(gc, chars, length, font, x, y, xRatio, yRatio);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear_custom4(MICROUI_GraphicsContext *gc,
                                                                                      jchar *chars, jint length,
                                                                                      MICROUI_Font *font, jint x,
                                                                                      jint y, jfloat xRatio,
                                                                                      jfloat yRatio) {
	return UI_DRAWING_STUB_drawScaledStringBilinear(gc, chars, length, font, x, y, xRatio, yRatio);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear_custom5(MICROUI_GraphicsContext *gc,
                                                                                      jchar *chars, jint length,
                                                                                      MICROUI_Font *font, jint x,
                                                                                      jint y, jfloat xRatio,
                                                                                      jfloat yRatio) {
	return UI_DRAWING_STUB_drawScaledStringBilinear(gc, chars, length, font, x, y, xRatio, yRatio);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear_custom6(MICROUI_GraphicsContext *gc,
                                                                                      jchar *chars, jint length,
                                                                                      MICROUI_Font *font, jint x,
                                                                                      jint y, jfloat xRatio,
                                                                                      jfloat yRatio) {
	return UI_DRAWING_STUB_drawScaledStringBilinear(gc, chars, length, font, x, y, xRatio, yRatio);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear_custom7(MICROUI_GraphicsContext *gc,
                                                                                      jchar *chars, jint length,
                                                                                      MICROUI_Font *font, jint x,
                                                                                      jint y, jfloat xRatio,
                                                                                      jfloat yRatio) {
	return UI_DRAWING_STUB_drawScaledStringBilinear(gc, chars, length, font, x, y, xRatio, yRatio);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom0(
	MICROUI_GraphicsContext *gc, jchar *chars,
	jint length, MICROUI_Font *font, jint width,
	MICROUI_RenderableString *renderableString, jint x, jint y,
	jfloat xRatio, jfloat yRatio) {
	return UI_DRAWING_STUB_drawScaledRenderableStringBilinear(gc, chars, length, font, width, renderableString, x, y,
	                                                          xRatio, yRatio);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom1(
	MICROUI_GraphicsContext *gc, jchar *chars,
	jint length, MICROUI_Font *font, jint width,
	MICROUI_RenderableString *renderableString, jint x, jint y,
	jfloat xRatio, jfloat yRatio) {
	return UI_DRAWING_STUB_drawScaledRenderableStringBilinear(gc, chars, length, font, width, renderableString, x, y,
	                                                          xRatio, yRatio);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom2(
	MICROUI_GraphicsContext *gc, jchar *chars,
	jint length, MICROUI_Font *font, jint width,
	MICROUI_RenderableString *renderableString, jint x, jint y,
	jfloat xRatio, jfloat yRatio) {
	return UI_DRAWING_STUB_drawScaledRenderableStringBilinear(gc, chars, length, font, width, renderableString, x, y,
	                                                          xRatio, yRatio);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom3(
	MICROUI_GraphicsContext *gc, jchar *chars,
	jint length, MICROUI_Font *font, jint width,
	MICROUI_RenderableString *renderableString, jint x, jint y,
	jfloat xRatio, jfloat yRatio) {
	return UI_DRAWING_STUB_drawScaledRenderableStringBilinear(gc, chars, length, font, width, renderableString, x, y,
	                                                          xRatio, yRatio);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom4(
	MICROUI_GraphicsContext *gc, jchar *chars,
	jint length, MICROUI_Font *font, jint width,
	MICROUI_RenderableString *renderableString, jint x, jint y,
	jfloat xRatio, jfloat yRatio) {
	return UI_DRAWING_STUB_drawScaledRenderableStringBilinear(gc, chars, length, font, width, renderableString, x, y,
	                                                          xRatio, yRatio);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom5(
	MICROUI_GraphicsContext *gc, jchar *chars,
	jint length, MICROUI_Font *font, jint width,
	MICROUI_RenderableString *renderableString, jint x, jint y,
	jfloat xRatio, jfloat yRatio) {
	return UI_DRAWING_STUB_drawScaledRenderableStringBilinear(gc, chars, length, font, width, renderableString, x, y,
	                                                          xRatio, yRatio);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom6(
	MICROUI_GraphicsContext *gc, jchar *chars,
	jint length, MICROUI_Font *font, jint width,
	MICROUI_RenderableString *renderableString, jint x, jint y,
	jfloat xRatio, jfloat yRatio) {
	return UI_DRAWING_STUB_drawScaledRenderableStringBilinear(gc, chars, length, font, width, renderableString, x, y,
	                                                          xRatio, yRatio);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear_custom7(
	MICROUI_GraphicsContext *gc, jchar *chars,
	jint length, MICROUI_Font *font, jint width,
	MICROUI_RenderableString *renderableString, jint x, jint y,
	jfloat xRatio, jfloat yRatio) {
	return UI_DRAWING_STUB_drawScaledRenderableStringBilinear(gc, chars, length, font, width, renderableString, x, y,
	                                                          xRatio, yRatio);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear_custom0(MICROUI_GraphicsContext *gc,
                                                                                          jchar c, MICROUI_Font *font,
                                                                                          jint x, jint y,
                                                                                          jint xRotation,
                                                                                          jint yRotation, jfloat angle,
                                                                                          jint alpha) {
	return UI_DRAWING_STUB_drawCharWithRotationBilinear(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear_custom1(MICROUI_GraphicsContext *gc,
                                                                                          jchar c, MICROUI_Font *font,
                                                                                          jint x, jint y,
                                                                                          jint xRotation,
                                                                                          jint yRotation, jfloat angle,
                                                                                          jint alpha) {
	return UI_DRAWING_STUB_drawCharWithRotationBilinear(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear_custom2(MICROUI_GraphicsContext *gc,
                                                                                          jchar c, MICROUI_Font *font,
                                                                                          jint x, jint y,
                                                                                          jint xRotation,
                                                                                          jint yRotation, jfloat angle,
                                                                                          jint alpha) {
	return UI_DRAWING_STUB_drawCharWithRotationBilinear(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear_custom3(MICROUI_GraphicsContext *gc,
                                                                                          jchar c, MICROUI_Font *font,
                                                                                          jint x, jint y,
                                                                                          jint xRotation,
                                                                                          jint yRotation, jfloat angle,
                                                                                          jint alpha) {
	return UI_DRAWING_STUB_drawCharWithRotationBilinear(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear_custom4(MICROUI_GraphicsContext *gc,
                                                                                          jchar c, MICROUI_Font *font,
                                                                                          jint x, jint y,
                                                                                          jint xRotation,
                                                                                          jint yRotation, jfloat angle,
                                                                                          jint alpha) {
	return UI_DRAWING_STUB_drawCharWithRotationBilinear(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear_custom5(MICROUI_GraphicsContext *gc,
                                                                                          jchar c, MICROUI_Font *font,
                                                                                          jint x, jint y,
                                                                                          jint xRotation,
                                                                                          jint yRotation, jfloat angle,
                                                                                          jint alpha) {
	return UI_DRAWING_STUB_drawCharWithRotationBilinear(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear_custom6(MICROUI_GraphicsContext *gc,
                                                                                          jchar c, MICROUI_Font *font,
                                                                                          jint x, jint y,
                                                                                          jint xRotation,
                                                                                          jint yRotation, jfloat angle,
                                                                                          jint alpha) {
	return UI_DRAWING_STUB_drawCharWithRotationBilinear(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear_custom7(MICROUI_GraphicsContext *gc,
                                                                                          jchar c, MICROUI_Font *font,
                                                                                          jint x, jint y,
                                                                                          jint xRotation,
                                                                                          jint yRotation, jfloat angle,
                                                                                          jint alpha) {
	return UI_DRAWING_STUB_drawCharWithRotationBilinear(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom0(
	MICROUI_GraphicsContext *gc, jchar c, MICROUI_Font *font,
	jint x, jint y,
	jint xRotation, jint yRotation,
	jfloat angle,
	jint alpha) {
	return UI_DRAWING_STUB_drawCharWithRotationNearestNeighbor(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom1(
	MICROUI_GraphicsContext *gc, jchar c, MICROUI_Font *font,
	jint x, jint y,
	jint xRotation, jint yRotation,
	jfloat angle,
	jint alpha) {
	return UI_DRAWING_STUB_drawCharWithRotationNearestNeighbor(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom2(
	MICROUI_GraphicsContext *gc, jchar c, MICROUI_Font *font,
	jint x, jint y,
	jint xRotation, jint yRotation,
	jfloat angle,
	jint alpha) {
	return UI_DRAWING_STUB_drawCharWithRotationNearestNeighbor(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom3(
	MICROUI_GraphicsContext *gc, jchar c, MICROUI_Font *font,
	jint x, jint y,
	jint xRotation, jint yRotation,
	jfloat angle,
	jint alpha) {
	return UI_DRAWING_STUB_drawCharWithRotationNearestNeighbor(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom4(
	MICROUI_GraphicsContext *gc, jchar c, MICROUI_Font *font,
	jint x, jint y,
	jint xRotation, jint yRotation,
	jfloat angle,
	jint alpha) {
	return UI_DRAWING_STUB_drawCharWithRotationNearestNeighbor(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom5(
	MICROUI_GraphicsContext *gc, jchar c, MICROUI_Font *font,
	jint x, jint y,
	jint xRotation, jint yRotation,
	jfloat angle,
	jint alpha) {
	return UI_DRAWING_STUB_drawCharWithRotationNearestNeighbor(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom6(
	MICROUI_GraphicsContext *gc, jchar c, MICROUI_Font *font,
	jint x, jint y,
	jint xRotation, jint yRotation,
	jfloat angle,
	jint alpha) {
	return UI_DRAWING_STUB_drawCharWithRotationNearestNeighbor(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor_custom7(
	MICROUI_GraphicsContext *gc, jchar c, MICROUI_Font *font,
	jint x, jint y,
	jint xRotation, jint yRotation,
	jfloat angle,
	jint alpha) {
	return UI_DRAWING_STUB_drawCharWithRotationNearestNeighbor(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

#else // #if defined(UI_FEATURE_FONT_CUSTOM_FORMATS)

/*
 * VEE Port supports only standard fonts format (Font Generator RAW format). The
 * next functions redirect the font drawing to the software algorithms.
 *
 * VEE Port may support several destination formats: display format and one or several
 * standard formats.
 *
 * Note: The functions are called by ui_drawing.c. The use of GPU should be checked
 * in ui_drawing.c and not here.
 */

// --------------------------------------------------------------------------------
// Private functions
// --------------------------------------------------------------------------------

static inline bool _can_call_soft_algo(MICROUI_GraphicsContext *gc) {
#if !defined(UI_GC_SUPPORTED_FORMATS) || (UI_GC_SUPPORTED_FORMATS <= 1)
	(void)gc;
	// standard font in display GC -> can use soft algo
	return true;
#else
	return LLUI_DISPLAY_isDisplayFormat(gc->image.format);
#endif
}

// --------------------------------------------------------------------------------
// ui_font_drawing.h functions
// --------------------------------------------------------------------------------

// See the header file for the function documentation
jint UI_FONT_DRAWING_stringWidth(jchar *chars, jint length, MICROUI_Font *font) {
	return UI_DRAWING_SOFT_stringWidth(chars, length, font);
}

// See the header file for the function documentation
jint UI_FONT_DRAWING_initializeRenderableStringSNIContext(jchar *chars, jint length, MICROUI_Font *font,
                                                          MICROUI_RenderableString *renderableString) {
	return UI_DRAWING_SOFT_initializeRenderableStringSNIContext(chars, length, font, renderableString);
}

// See the header file for the function documentation
DRAWING_Status UI_FONT_DRAWING_drawString(MICROUI_GraphicsContext *gc, jchar *chars, jint length, MICROUI_Font *font,
                                          jint x, jint y) {
	return _can_call_soft_algo(gc) ? UI_DRAWING_SOFT_drawString(gc, chars, length, font, x, y)
	       : UI_DRAWING_STUB_drawString(gc, chars, length, font, x, y);
}

// See the header file for the function documentation
DRAWING_Status UI_FONT_DRAWING_drawRenderableString(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                    MICROUI_Font *font, jint width,
                                                    MICROUI_RenderableString *renderableString, jint x, jint y) {
	return _can_call_soft_algo(gc) ? UI_DRAWING_SOFT_drawRenderableString(gc, chars, length, font, width,
	                                                                      renderableString, x, y)
	       : UI_DRAWING_STUB_drawRenderableString(gc, chars, length, font, width, renderableString, x, y);
}

// See the header file for the function documentation
DRAWING_Status UI_FONT_DRAWING_drawScaledStringBilinear(MICROUI_GraphicsContext *gc, jchar *chars, jint length,
                                                        MICROUI_Font *font, jint x, jint y, jfloat xRatio,
                                                        jfloat yRatio) {
	return _can_call_soft_algo(gc) ? DW_DRAWING_SOFT_drawScaledStringBilinear(gc, chars, length, font, x, y, xRatio,
	                                                                          yRatio)
	       : UI_DRAWING_STUB_drawScaledStringBilinear(gc, chars, length, font, x, y, xRatio, yRatio);
}

// See the header file for the function documentation
DRAWING_Status UI_FONT_DRAWING_drawScaledRenderableStringBilinear(MICROUI_GraphicsContext *gc, jchar *chars,
                                                                  jint length, MICROUI_Font *font, jint width,
                                                                  MICROUI_RenderableString *renderableString, jint x,
                                                                  jint y, jfloat xRatio, jfloat yRatio) {
	return _can_call_soft_algo(gc) ? DW_DRAWING_SOFT_drawScaledRenderableStringBilinear(gc, chars, length, font, width,
	                                                                                    renderableString, x, y, xRatio,
	                                                                                    yRatio)
	       : UI_DRAWING_STUB_drawScaledRenderableStringBilinear(gc, chars, length, font, width, renderableString, x, y,
	                                                            xRatio, yRatio);
}

// See the header file for the function documentation
DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationBilinear(MICROUI_GraphicsContext *gc, jchar c, MICROUI_Font *font,
                                                            jint x, jint y, jint xRotation, jint yRotation,
                                                            jfloat angle, jint alpha) {
	return _can_call_soft_algo(gc) ? DW_DRAWING_SOFT_drawCharWithRotationBilinear(gc, c, font, x, y, xRotation,
	                                                                              yRotation, angle, alpha)
	       : UI_DRAWING_STUB_drawCharWithRotationBilinear(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

// See the header file for the function documentation
DRAWING_Status UI_FONT_DRAWING_drawCharWithRotationNearestNeighbor(MICROUI_GraphicsContext *gc, jchar c,
                                                                   MICROUI_Font *font, jint x, jint y, jint xRotation,
                                                                   jint yRotation, jfloat angle, jint alpha) {
	return _can_call_soft_algo(gc) ? DW_DRAWING_SOFT_drawCharWithRotationNearestNeighbor(gc, c, font, x, y, xRotation,
	                                                                                     yRotation, angle, alpha)
	       : UI_DRAWING_STUB_drawCharWithRotationNearestNeighbor(gc, c, font, x, y, xRotation, yRotation, angle, alpha);
}

#endif // #if defined(UI_FEATURE_FONT_CUSTOM_FORMATS)

// --------------------------------------------------------------------------------
// EOF
// --------------------------------------------------------------------------------
