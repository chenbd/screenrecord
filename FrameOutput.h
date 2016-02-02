/*
 * Copyright 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SCREENRECORD_FRAMEOUTPUT_H
#define SCREENRECORD_FRAMEOUTPUT_H

#include "Program.h"
#include "EglWindow.h"

#include <gui/BufferQueue.h>
#if PLATFORM_SDK_VERSION <= 19
#include <gui/IGraphicBufferConsumer.h> // for BufferItem
#endif
#include <gui/GLConsumer.h>

#include "turbojpeg.h"

namespace android {
typedef enum {
    FORMAT_MP4, FORMAT_H264, FORMAT_FRAMES, FORMAT_RAW_FRAMES, FORMAT_JPG
} OUT_OUT_FORMAT;

/*
 * Support for "frames" output format.
 */
class FrameOutput : public GLConsumer::FrameAvailableListener {
public:
    FrameOutput(OUT_OUT_FORMAT format) : mFrameAvailable(false),
        mExtTextureName(0),
        mPixelBuf(NULL),
        mFormat(format),
        destinationJpegBuffer(NULL),
        tjCompressHandle(NULL)
        {}

    // Create an "input surface", similar in purpose to a MediaCodec input
    // surface, that the virtual display can send buffers to.  Also configures
    // EGL with a pbuffer surface on the current thread.
    status_t createInputSurface(int width, int height,
            sp<IGraphicBufferProducer>* pBufferProducer);

    // Copy one from input to output.  If no frame is available, this will wait up to the
    // specified number of microseconds.
    //
    // Returns ETIMEDOUT if the timeout expired before we found a frame.
    status_t copyFrame(FILE* fp, long timeoutUsec, bool rawFrames);

    // Prepare to copy frames.  Makes the EGL context used by this object current.
    void prepareToCopy() {
        mEglWindow.makeCurrent();
    }

private:
    FrameOutput(const FrameOutput&);
    FrameOutput& operator=(const FrameOutput&);

    // Destruction via RefBase.
    virtual ~FrameOutput() {
        delete[] mPixelBuf;
        if(tjCompressHandle) {
            tjDestroy(tjCompressHandle);
            tjCompressHandle = NULL;
        }
        if (destinationJpegBuffer) {
          delete destinationJpegBuffer;
          destinationJpegBuffer = NULL;
        }
#ifdef USE_PBO // Pixel Buffer Object
        glDeleteFramebuffers(1, &mFBO);
#endif
    }

    // (overrides GLConsumer::FrameAvailableListener method)
#if PLATFORM_SDK_VERSION > 19
    virtual void onFrameAvailable(const BufferItem& item);
#else
    virtual void onFrameAvailable(const IGraphicBufferConsumer::BufferItem& item);
    virtual void onFrameAvailable() {
        static const IGraphicBufferConsumer::BufferItem tmp;
        onFrameAvailable(tmp);
    }
#endif
    // Reduces RGBA to RGB, in place.
    static void reduceRgbaToRgb(uint8_t* buf, unsigned int pixelCount);

    // Put a 32-bit value into a buffer, in little-endian byte order.
    static void setValueLE(uint8_t* buf, uint32_t value);

    // Used to wait for the FrameAvailableListener callback.
    Mutex mMutex;
    Condition mEventCond;

    // Set by the FrameAvailableListener callback.
    bool mFrameAvailable;

    // This receives frames from the virtual display and makes them available
    // as an external texture.
    sp<GLConsumer> mGlConsumer;

    // EGL display / context / surface.
    EglWindow mEglWindow;

    // GL rendering support.
    Program mExtTexProgram;

    // External texture, updated by GLConsumer.
    GLuint mExtTextureName;

    // Pixel data buffer.
    uint8_t* mPixelBuf;

    OUT_OUT_FORMAT mFormat;
    unsigned long destinationJpegBufferSize;
    unsigned char *destinationJpegBuffer;
    tjhandle tjCompressHandle;
#ifdef USE_PBO // Pixel Buffer Object
    GLuint mPBO;
#endif
};

}; // namespace android

#endif /*SCREENRECORD_FRAMEOUTPUT_H*/
