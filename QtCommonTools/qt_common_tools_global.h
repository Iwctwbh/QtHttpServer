#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QTCOMMONTOOLS_LIB)
#  define QTCOMMONTOOLS_EXPORT Q_DECL_EXPORT
# else
#  define QTCOMMONTOOLS_EXPORT Q_DECL_IMPORT
# endif
#else
# define QTCOMMONTOOLS_EXPORT
#endif
