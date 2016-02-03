/****************************************************************************
*   FILE NAME   : libscreenrecord.h
*   CREATE DATE : 2016-2-3
*   MODULE      : libscreenrecord interface file
*   AUTHOR      : chenbd
*---------------------------------------------------------------------------*
*   MEMO        :
*****************************************************************************/

#ifndef __LIBSCREENRECORD_H__
#define __LIBSCREENRECORD_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FORMAT_MP4, FORMAT_H264, FORMAT_FRAMES, FORMAT_RAW_FRAMES, FORMAT_JPG
} OUT_OUT_FORMAT;

typedef struct tag_libscreenrecord_params_t {
    int gVerbose;           // chatty on stdout
    int gRotate;            // rotate 90 degrees
    OUT_OUT_FORMAT gOutputFormat;           // data format for output
    int gSizeSpecified;     // was size explicitly requested?
    int gWantInfoScreen;    // do we want initial info screen?
    int gWantFrameTime;     // do we want times on each frame?
    uint32_t gVideoWidth;        // default width+height
    uint32_t gVideoHeight;
    uint32_t gBitRate;     // 4Mbps
    uint32_t gTimeLimitSec;
    union {
        char fileName[256];
        int fd;
    }u;
} libscreenrecord_params_t;


/**
 * create a context for libscreenrecord library
 * @param argc 
 * @param argv
 * @return pointer for context on success and NULL on error
 */
void *libscreenrecord_init(libscreenrecord_params_t *recparams);

/**
 * destory libscreenrecord context
 * @param ctx native context handle, return value from libscreenrecord_init()
 * @return 0 on success and -1 on error
 */
int libscreenrecord_destory(void *ctx);

/**
 * startup native layer
 * @param ctx native context handle, return value from libscreenrecord_init()
 * @return native context
 */
int libscreenrecord_run(void *ctx);


#define UPDATE_FLAG_DATA 1

/**
 * native evnet listener interface
 */
typedef int (*NATIVE_CALLBACK)(void *para, int update_flag, int arg1, int arg2, void *data);
/**
 * set native event listener
 * @param listener
 */
int libscreenrecord_registListener(void *ctx, NATIVE_CALLBACK callback, void *para);




/**
 * set buffer to native layer
 * @param ctx native context handle
 * @param buf buffer to set
 * @param bufsize buffer max size in byte
 * @return 0 on success and -1 on error
 */
int libscreenrecord_setBuf(void *ctx, void *buf, size_t bufsize);

/**
 * lock buffer to be used
 * this function MUST be called in the callback function registered by registNativeListener()
 * @param ctx native context handle
 * @param buf if buf not null,data is copied to this buffer
 * @param bufsize size of buffer in Byte
 * @return >0 means buf real size in Byte on success, -1 on error
 */
int libscreenrecord_lockBuf(void *ctx, void *buf, size_t bufsize);


#ifdef __cplusplus
}
#endif

#endif /* __LIBSCREENRECORD_H__ */

