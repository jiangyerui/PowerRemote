#ifndef CONFHOSTINFO_GLOBAL_H
#define CONFHOSTINFO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CONFHOSTINFO_LIBRARY)
#  define CONFHOSTINFOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CONFHOSTINFOSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CONFHOSTINFO_GLOBAL_H