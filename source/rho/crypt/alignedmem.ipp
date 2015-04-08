
static
u8* s_aligned_malloc(size_t size, size_t alignment)
{
    #if __linux__ || __APPLE__ || __CYGWIN__

    void* buf = NULL;
    int ret = posix_memalign(&buf, alignment, size);
    if (ret != 0)
        throw eRuntimeError("Cannot allocated aligned memory!");
    return (u8*)buf;

    #elif __MINGW32__
        #error Not implemented yet -- should use _aligned_malloc() I think.
    #else
        #error Unsupported platform
    #endif
}


static
void s_aligned_free(u8* buf)
{
    #if __linux__ || __APPLE__ || __CYGWIN__

    free(buf);

    #elif __MINGW32__
        #error Not implemented yet
    #else
        #error Unsupported platform
    #endif
}

