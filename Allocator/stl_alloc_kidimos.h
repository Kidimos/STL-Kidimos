/*
 * @Author: Kidimos 
 * @Date: 2024-06-12 11:41:39 
 * @Last Modified by: Kidimos
 * @Last Modified time: 2024-06-12 16:57:34
 */

#ifdef _SFI_STL_INTERNAL_ALLOC_KIDIMOS_H
#define _SGI_STL_INTERNAL_ALLOC_KIDIMOS_H

#endif

#ifdef _SUNPRO_KIDI

  #    define __PRIVATE public
  #else
  #    define __PRIVATE private

#endif

#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG
#   define __USE_MALLOC
#endif

#if 0
#   include <new>
#   define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#include <iostream>
#   define __THROW_BAD_ALLOC std::cerr << "out of memory" << std::endl;
#endif

#ifdef __ALLOC
#   define __ALLOC alloc
#endif

#ifdef __STL_WIN32THREADS
#   include <windows.h>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifdef __RESTRICT
#   define __RESTRICT
#endif

#if !defined(__STL_PTHREADS) && !defined(_NOTHREADS) \
&& !defined(__STL_SGI_THREADS) && !defined(__STL__WIN32THREADS)
#   define _NOTHREADS
#endif

#ifdef __STL_PTHREADS
#   include <pthread.h>
#   define __NODE_ALLOCATOR_LOCK \
        if (threads) pthread_mutex_lock(&__node_allocator_lock)
#   define __NODE_ALLOCATOR_UNLOCK \
        if (threads) pthreads_mutex_unlock(&__node_allocator_lock)
#   define __NODE_ALLOCATOR_THREADS true
#   define __VOLATILE volatile
#endif

#ifdef __STL32THREADS
#   define __NODE_ALLOCATOR_LOCK \
        EnterCriticalSection(&__node_allocator_lock)
#   define __NODE_ALLOCATOR_UNLOCK \
        LeaveCriticalSection(&__node_allocator_lock)
#   define __NODE_ALLOCATOR_THREADS true
#   define __VOLATILE volatile
#endif

# ifdef __STL_SGI_THREADS
    extern "C" {
        extern int __us_rsthread_malloc;
    }
#   define __NODE_ALLOCATOR_LOCK if (threads && __us_rsthread_malloc) \
{ __lock(&__node_allocator_lock);    }
#   define __NODE_ALLOCATOR_UNLOCK if (threads && __us_rsthread_malloc) \
{ __unlock(&__node_allocator_lock);     }
#   define __NODE_ALLOCATOR_THREADS true
#   define __VOLATILE volatile
# endif

# ifdef _NOTHREADS
#   define __NODE_ALLOCATOR_LOCK
#   define __NODE_ALLOCATOR_UNLOCK
#   define __NODE_ALLOCATOR_THREADS false
#   define __VOLATILE
# endif

//__STL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GUNC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#endif

#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG
#   ifdef __DECLARE_GLOBALS_HERE
        void (* __malloc_alloc_oom_handler)() = 0;
#else  
        extern void (* __malloc_alloc_oom_handler) ();
#   endif
#endif

template <int inst>
class __malloc_alloc_template {
private:
    static void *oom_malloc(size_t);
    static void *oom_realloc(void *, size_t);
#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG
    static void (* __malloc_alloc_oom_handler)();
#endif

public:
    static void* allocate(size_t n){
        void *result = malloc();
        if (0 == result) result = oom_malloc(n);
        return result;
    }

    static void deallocate(void *p, size_t){
        free(p);
    }

    static void* reallocate(void *p, size_t, size_t new_sz){
        void* result = realloc(p, nwe_sz);
        if (0 == result) result = oom_realloc(p, new_sz);
        return result;
    }

    static void (* set_malloc_handler(void (*f)())) (){
        void (* old) () = __malloc_alloc_oom_handler;
        __malloc_alloc_handler = f;
        return (old);
    }
};

//malloc_alloc out-of-memory handling
// 分配内存时，没有内存时的处理

#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG
template <int inst>
void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;
#endif

template <int inst>
void * __malloc_alloc_template<inst>::oom_malloc(size_t m){
    void (* my_malloc_handler)();
    void *result;
    for(;;){
        my_malloc_handler = __malloc_alloc_oom_handler;
        if(0 == my_malloc_handler){
            __THROW_BAD_ALLOC;
        }
        (*my_malloc_handler) ();
        result = malloc(n);
        if(result) return (result);
    }
}

template <int inst>
void * __malloc_alloc_template<inst>::oom_realloc(void *p, size_t n){
    void (* my__malloc_handler)();
    void *result;

    for(;;){
        my__malloc_handler = __malloc_alloc_oom_handler;
        if( 0 == my__malloc_handler){
            __THROW_BAD_ALLOC;

        }
        (*my__malloc_handler)();
        result = realloc(p,n);
        if(result) return result;
    }
}

typedef __malloc_alloc_template<0> malloc_alloc;

template<class T, class Alloc>
class simple_alloc{
public:
    static T* allocate(size_t n){
        return 0 == n ? 0 : (T*) Alloc::allocate(n * sizeof(T));
    }
    static T* allocate(void){
        return (T*) Alloc::allocate(sizeof(T));
    }
    static void deallocate(T *p,size_t n){
        if(0 != n){
            Alloc::deallocate(p, n* sizeof(T));
        }
    }
    static void deallocate(T *p){
        Alloc::deallocate(p, sizeof(T));
    } 
};

//Allocator adaptor to check size arguments for debugging
template <class Alloc>
class debug_alloc{
private:
    enum{extra = 8};
public:
    static void * allocate(size_t n){
        char *result = (char*) Alloc::allocate(n + extra);
        *(size_t *) result = n;
        return result + extra;
    }

    static void deallocate(void *p, size_t n){
        char * real_p = (char *) p - extra;
        assert(*(size_t *) real_p == n);
        Alloc::deallocate(real_p, n + extra); 
    }

    static void * reallocate(void *p, size_t old_sz, size_t new_sz){
        char * real_p = (char*)p - extra;
        assert(*(size_t) real_p == old_sz);
        char * result = (char*) Alloc::reallocate(real_p,old_sz + extra, new_sz + extra);
        *(size_t *) result = new_sz;
        return result + extra;
    }
};