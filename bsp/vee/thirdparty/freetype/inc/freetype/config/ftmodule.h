/*
 * This file registers the FreeType modules compiled into the library.
 *
 * If you use GNU make, this file IS NOT USED!  Instead, it is created in
 * the objects directory (normally `<topdir>/objs/`) based on information
 * from `<topdir>/modules.cfg`.
 *
 * Please read `docs/INSTALL.ANY` and `docs/CUSTOMIZE` how to compile
 * FreeType without GNU make.
 *
 * Copyright 2021-2024 MicroEJ Corp. This file has been modified and/or created by MicroEJ Corp.
 *
 * This file is part of the FreeType project, and may only be used,
 * modified, and distributed under the terms of the FreeType project
 * license, LICENSE.TXT.  By continuing to use, modify, or distribute
 * this file you indicate that you have read the license and
 * understand and accept it fully.
 */

#include "vg_configuration.h"

#if defined (VG_FEATURE_FONT)

FT_USE_MODULE( FT_Module_Class, autofit_module_class )
FT_USE_MODULE( FT_Module_Class, pshinter_module_class )
FT_USE_MODULE( FT_Module_Class, sfnt_module_class )


#if defined(VG_FEATURE_FONT) && defined (VG_FEATURE_FONT_FREETYPE_VECTOR) && (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR)
FT_USE_MODULE( FT_Renderer_Class, ft_microvg_renderer_class )
#endif // defined (VG_FEATURE_FONT_FREETYPE_VECTOR) && (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR)

#if defined(VG_FEATURE_FONT) && defined (VG_FEATURE_FONT_FREETYPE_BITMAP) && (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_BITMAP)
FT_USE_MODULE( FT_Renderer_Class, ft_smooth_renderer_class )
#endif // defined (VG_FEATURE_FONT_FREETYPE_BITMAP) && (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_BITMAP)

#ifdef VG_FEATURE_FREETYPE_TTF
FT_USE_MODULE( FT_Driver_ClassRec, tt_driver_class )
#endif // VG_FEATURE_FREETYPE_TTF

#ifdef VG_FEATURE_FREETYPE_OTF
FT_USE_MODULE( FT_Driver_ClassRec, cff_driver_class )
FT_USE_MODULE( FT_Module_Class, psaux_module_class )
FT_USE_MODULE( FT_Module_Class, psnames_module_class )
#endif // VG_FEATURE_FREETYPE_OTF

#endif // defined (VG_FEATURE_FONT)
/* EOF */
