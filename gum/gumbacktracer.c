/*
 * Copyright (C) 2008-2015 Ole André Vadla Ravnås <ole.andre.ravnas@tillitech.com>
 *
 * Licence: wxWindows Library Licence, Version 3.1
 */

#include "gumbacktracer.h"

#ifdef G_OS_WIN32
# include "backend-dbghelp/gumdbghelpbacktracer.h"
# include "arch-x86/gumx86backtracer.h"
#elif defined (HAVE_DARWIN)
# include "backend-darwin/gumdarwinbacktracer.h"
#elif defined (HAVE_GLIBC)
# include "backend-glibc/gumgnubacktracer.h"
#elif defined (HAVE_I386)
# include "arch-x86/gumx86backtracer.h"
#elif defined (HAVE_ARM)
# include "arch-arm/gumarmbacktracer.h"
#endif

GType
gum_backtracer_get_type (void)
{
  static volatile gsize gonce_value;

  if (g_once_init_enter (&gonce_value))
  {
    GType gtype;

    gtype = g_type_register_static_simple (G_TYPE_INTERFACE, "GumBacktracer",
        sizeof (GumBacktracerIface), NULL, 0, NULL, 0);
    g_type_interface_add_prerequisite (gtype, G_TYPE_OBJECT);

    g_once_init_leave (&gonce_value, (GType) gtype);
  }

  return (GType) gonce_value;
}

GumBacktracer *
gum_backtracer_make_default (void)
{
#if defined (G_OS_WIN32)
  GumDbgHelpImpl * dbghelp;

  dbghelp = gum_dbghelp_impl_obtain ();
  if (dbghelp != NULL)
    return gum_dbghelp_backtracer_new (dbghelp);
  else
    return gum_x86_backtracer_new ();
#elif defined (HAVE_DARWIN)
  return gum_darwin_backtracer_new ();
#elif defined (HAVE_GLIBC)
  return gum_gnu_backtracer_new ();
#elif defined (HAVE_I386)
  return gum_x86_backtracer_new ();
#elif defined (HAVE_ARM)
  return gum_arm_backtracer_new ();
#else
  return NULL;
#endif
}

void
gum_backtracer_generate (GumBacktracer * self,
                         const GumCpuContext * cpu_context,
                         GumReturnAddressArray * return_addresses)
{
  GumBacktracerIface * iface = GUM_BACKTRACER_GET_INTERFACE (self);

  g_assert (iface->generate != NULL);

  iface->generate (self, cpu_context, return_addresses);
}
