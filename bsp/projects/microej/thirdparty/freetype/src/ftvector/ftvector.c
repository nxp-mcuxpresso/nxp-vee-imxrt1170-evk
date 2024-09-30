/*
 * C
 *
 * Copyright 2019-2024 MicroEJ Corp. This file has been modified and/or created by MicroEJ Corp.
 *
 * This file is part of the FreeType project, and may only be used,
 * modified, and distributed under the terms of the FreeType project
 * license, LICENSE.TXT.  By continuing to use, modify, or distribute
 * this file you indicate that you have read the license and
 * understand and accept it fully.
 */

/**
 * @file
 * @brief Freetype microvg renderer
* @author MicroEJ Developer Team
* @version 3.0.0
 */

#include "vg_configuration.h"
#if defined VG_FEATURE_FONT && (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR)

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <freetype/internal/ftobjs.h>
#include FT_OUTLINE_H
#include "ftvector/ftvector.h"

#include <LLVG_PATH_impl.h>

// -----------------------------------------------------------------------------
// Defines
// -----------------------------------------------------------------------------

#ifndef FT_PATH_POOL_CHUNK
#define FT_PATH_POOL_CHUNK      0x100
#endif

// -----------------------------------------------------------------------------
// Private variables
// -----------------------------------------------------------------------------

// cppcheck-suppress [unreadVariable] "memory" is used by "FT_REALLOC"
static FT_Memory memory;

// pointer on current path
static jbyte* jpath;
static jint jpath_pool_size;

// path drawer and its data
static FTVECTOR_draw_glyph_data_t* drawer_data;

// -----------------------------------------------------------------------------
// Private functions
// -----------------------------------------------------------------------------

static FT_Error _alloc_jpath(jint length) {
	FT_Error error = FT_ERR( Ok ); // /!\ FT_REALLOC updates this field !
	jint new_length = length + jpath_pool_size;
	while ( new_length > jpath_pool_size ) {
		if ( FT_ERR( Ok )  != FT_REALLOC( jpath, jpath_pool_size, jpath_pool_size + (uint32_t)FT_PATH_POOL_CHUNK ) ) {
			error = FT_ERR( Out_Of_Memory );
			break;
		}
		jpath_pool_size += (uint32_t)FT_PATH_POOL_CHUNK;
	};

	return (FT_ERR( Ok ) == error) ? FT_ERR( Unlisted_Object ) /* means "allocated but not set" */ : error;
}

static FT_Error _append_command(jint append_ret) {
	return (LLVG_SUCCESS == append_ret) ? FT_ERR( Ok ) : _alloc_jpath(append_ret);
}

// cppcheck-suppress [unusedFunction] function used through a list
static int _move_to(const FT_Vector* to) {

	FT_Error error = _append_command(
			LLVG_PATH_IMPL_appendPathCommand1(
					jpath, jpath_pool_size,
					LLVG_PATH_CMD_MOVE, to->x, -to->y
			)
	);

	if (FT_ERR( Unlisted_Object ) == error) {
		// alloc done, add the command
		error = _move_to(to);
	}

	return error;
}

// cppcheck-suppress [unusedFunction] function used through a list
static int _line_to(const FT_Vector* to) {

	FT_Error error = _append_command(
			LLVG_PATH_IMPL_appendPathCommand1(
					jpath, jpath_pool_size,
					LLVG_PATH_CMD_LINE, to->x, -to->y
			)
	);

	if (FT_ERR( Unlisted_Object ) == error) {
		// alloc done, add the command
		error = _line_to(to);
	}

	return error;
}

// cppcheck-suppress [unusedFunction] function used through a list
static int _conic_to(const FT_Vector* control, const FT_Vector* to) {

	FT_Error error = _append_command(
			LLVG_PATH_IMPL_appendPathCommand2(
					jpath, jpath_pool_size,
					LLVG_PATH_CMD_QUAD,
					control->x, -control->y,
					to->x, -to->y
			)
	);

	if (FT_ERR( Unlisted_Object ) == error) {
		// alloc done, add the command
		error = _conic_to(control, to);
	}

	return error;
}

// cppcheck-suppress [unusedFunction] function used through a list
static int _cubic_to(const FT_Vector* control1, const FT_Vector* control2, const FT_Vector* to) {

	FT_Error error = _append_command(
			LLVG_PATH_IMPL_appendPathCommand3(
					jpath, jpath_pool_size,
					LLVG_PATH_CMD_CUBIC,
					control1->x, -control1->y,
					control2->x, -control2->y,
					to->x, -to->y
			)
	);

	if (FT_ERR( Unlisted_Object ) == error) {
		// alloc done, add the command
		error = _cubic_to(control1, control2, to);
	}

	return error;
}

static int _end(FT_BBox* cbox){

	FT_Error error = _append_command(
			LLVG_PATH_IMPL_appendPathCommand2(
					jpath, jpath_pool_size,
					LLVG_PATH_CMD_CLOSE, (float) cbox->xMin, - (float) cbox->yMax, (float) cbox->xMax, - (float) cbox->yMin
			)
	);

	if (FT_ERR( Unlisted_Object ) == error) {
		// alloc done, add the command
		error = _end(cbox);
	}

	return error;
}

FT_DEFINE_OUTLINE_FUNCS(func_interface,
		(FT_Outline_MoveTo_Func) _move_to,
		(FT_Outline_LineTo_Func) _line_to,
		(FT_Outline_ConicTo_Func)_conic_to,
		(FT_Outline_CubicTo_Func)_cubic_to,
		0,
		0
)

static FT_Error _convert_glyph(FT_Outline* outline, FT_BBox* cbox) {

	// reset path (path array is enough large for sure)
	(void)LLVG_PATH_IMPL_initializePath(jpath, jpath_pool_size);

	FT_Error error = FT_Outline_Decompose( outline, &func_interface, NULL);

	if ( FT_ERR( Ok ) == error ) {

		/* compute the control box, and grid fit it */
		error = _end(cbox);

		if ( FT_ERR( Ok ) == error ) {
			VG_FREETYPE_draw_glyph_t drawer = (VG_FREETYPE_draw_glyph_t)(drawer_data->drawer);
			jint llvg_error = (*drawer)((VG_PATH_HEADER_t*)jpath, drawer_data->matrix, drawer_data->color, (outline->flags & FT_OUTLINE_EVEN_ODD_FILL), drawer_data->user_data);

			if (LLVG_SUCCESS != llvg_error) {
				error = (LLVG_OUT_OF_MEMORY == llvg_error) ? FT_ERR( Out_Of_Memory ) : FT_ERR( Cannot_Render_Glyph ) ;
			}
		}
	}

	return error;
}

static FT_Error _get_cbox(FT_Renderer render, FT_GlyphSlot slot, FT_BBox* cbox, bool* empty) {
	FT_Error error = FT_ERR( Ok );
	*empty = true;

	/* check glyph image format */
	if (slot->format != render->glyph_format) {
		error = FT_ERR( Invalid_Argument );
	}
	else{
		FT_Outline* outline = &slot->outline;

		/* compute the control box, and grid fit it */
		FT_Outline_Get_CBox( outline, cbox );

		FT_Pos xMin = FT_PIX_FLOOR( cbox->xMin );
		FT_Pos yMin = FT_PIX_FLOOR( cbox->yMin );
		FT_Pos xMax = FT_PIX_CEIL( cbox->xMax );
		FT_Pos yMax = FT_PIX_CEIL( cbox->yMax );

		if ( ((xMin < 0) && (xMax > (FT_INT_MAX + xMin))) || ( (yMin < 0) && (yMax > (FT_INT_MAX + yMin)) )) {
			error = FT_ERR( Raster_Overflow );
		}
		else {
			FT_Pos width = ( xMax - xMin ) >> 6;
			FT_Pos height = ( yMax - yMin ) >> 6;

			if ((width > 0) && (height > 0)) {
				/* Required check is (pitch * height < FT_ULONG_MAX),        */
				/* but we care realistic cases only.  Always pitch <= width. */
				if ( (width > 0x7FFF) || (height > 0x7FFF) )  {
					error = FT_ERR( Raster_Overflow );
				}
				else {
					*empty = false;
				}
			}
			// else: nothing to draw (*empty == true and no error)
		}
	}
	return error;
}

// cppcheck-suppress [unusedFunction] function used through a list
static FT_Error _raster_new(FT_Memory memory, FT_Raster* araster){
	return FT_ERR( Ok );
}

FT_DEFINE_RASTER_FUNCS(ft_raster,
		FT_GLYPH_FORMAT_OUTLINE,

		(FT_Raster_New_Func)     _raster_new,
		(FT_Raster_Reset_Func)   0,
		(FT_Raster_Set_Mode_Func)0,
		(FT_Raster_Render_Func)  0,
		(FT_Raster_Done_Func)    0
)

/* initialize renderer -- init its raster */
static FT_Error ft_init( FT_Renderer  render ) {

	// keep a pointer to the freetype memory
	FT_Library  library = FT_MODULE_LIBRARY( render );
	memory = library->memory;

	// allocate a path in this memory
	jpath_pool_size = 0;
	FT_Error error = _alloc_jpath(LLVG_PATH_IMPL_initializePath(jpath, jpath_pool_size));

	return (FT_ERR( Unlisted_Object ) == error ) ? FT_ERR( Ok ) : error;
}

/* sets render-specific mode */
static FT_Error ft_set_mode(FT_Renderer render, FT_ULong  mode_tag, FT_Pointer data ){
	FT_Error ret = FT_ERR(Invalid_Argument);

	switch (mode_tag){
	case FT_PARAM_TAG_DRAWER:{
		drawer_data = (FTVECTOR_draw_glyph_data_t*)data;
		ret = FT_ERR(Ok);
		break;
	}
	default:{
		/* we simply pass it to the raster */
		ret = render->clazz->raster_class->raster_set_mode(render->raster, mode_tag, data);
		break;
	}
	}

	return ret;
}

/* render a slot's glyph with GPU */
static FT_Error ft_render(FT_Renderer render, FT_GlyphSlot slot, FT_Render_Mode mode, const FT_Vector* origin) {

	(void)mode;
	(void)origin;

	FT_BBox  cbox;
	bool empty_path;
	FT_Error error = _get_cbox(render, slot, &cbox, &empty_path);

	if (!empty_path) {
		error = _convert_glyph(&slot->outline, &cbox);
	}

	return error;
}

// -----------------------------------------------------------------------------
// Freetype Low-Level Entry Point
// -----------------------------------------------------------------------------


FT_DEFINE_RENDERER( ft_microvg_renderer_class,

		FT_MODULE_RENDERER,
		sizeof ( FT_RendererRec ),

		"microvg",
		0x10000L,   // Version
		0x20000L,   // Requires

		0,    /* module specific interface */

		(FT_Module_Constructor)     ft_init,
		(FT_Module_Destructor)      0,
		(FT_Module_Requester)       0,

		FT_GLYPH_FORMAT_OUTLINE,

		(FT_Renderer_RenderFunc)    ft_render,
		(FT_Renderer_TransformFunc) 0,
		(FT_Renderer_GetCBoxFunc)   0,
		(FT_Renderer_SetModeFunc)   ft_set_mode,

		(FT_Raster_Funcs*)          &ft_raster
)

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#endif // defined VG_FEATURE_FONT && (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR)
