
#ifndef MODERN_GL_UTILS_EXPORT_H
#define MODERN_GL_UTILS_EXPORT_H

#ifdef MODERN_GL_UTILS_STATIC_DEFINE
#  define MODERN_GL_UTILS_EXPORT
#  define MODERN_GL_UTILS_NO_EXPORT
#else
#  ifndef MODERN_GL_UTILS_EXPORT
#    ifdef modern_gl_utils_EXPORTS
        /* We are building this library */
#      define MODERN_GL_UTILS_EXPORT 
#    else
        /* We are using this library */
#      define MODERN_GL_UTILS_EXPORT 
#    endif
#  endif

#  ifndef MODERN_GL_UTILS_NO_EXPORT
#    define MODERN_GL_UTILS_NO_EXPORT 
#  endif
#endif

#ifndef MODERN_GL_UTILS_DEPRECATED
#  define MODERN_GL_UTILS_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef MODERN_GL_UTILS_DEPRECATED_EXPORT
#  define MODERN_GL_UTILS_DEPRECATED_EXPORT MODERN_GL_UTILS_EXPORT MODERN_GL_UTILS_DEPRECATED
#endif

#ifndef MODERN_GL_UTILS_DEPRECATED_NO_EXPORT
#  define MODERN_GL_UTILS_DEPRECATED_NO_EXPORT MODERN_GL_UTILS_NO_EXPORT MODERN_GL_UTILS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef MODERN_GL_UTILS_NO_DEPRECATED
#    define MODERN_GL_UTILS_NO_DEPRECATED
#  endif
#endif

#endif /* MODERN_GL_UTILS_EXPORT_H */
