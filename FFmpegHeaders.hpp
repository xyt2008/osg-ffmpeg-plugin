/* Improved ffmpeg plugin for OpenSceneGraph -
 * Copyright (C) 1998-2010 Robert Osfield
 * Modifications copyright (C) 2014-2015 Digitalis Education Solutions, Inc. (http://DigitalisEducation.com)
 * Modification author: Oktay Radzhabov (oradzhabov at jazzros dot com)
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/


#ifndef HEADER_GUARD_FFMPEG_HEADERS_H
#define HEADER_GUARD_FFMPEG_HEADERS_H

#include <osg/Notify>

extern "C"
{
#define FF_API_OLD_SAMPLE_FMT 0
#include <errno.h>    // for error codes defined in avformat.h
#include <stdint.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#if defined(__unix__) || defined(__unix) || defined(unix)
    // When VDPAU will be available, uncomment it
    // #define USE_VDPAU
    #include <vdpau/vdpau.h> // do not forget call "sudo apt-get libvdpau-dev" and add library libvdpau.a
    #include <libavcodec/vdpau.h>
#endif

#ifndef AV_CH_LAYOUT_STEREO
	#include <libavutil/channel_layout.h> // Appears to be needed specifically for versions on some Linux distros
#endif

#ifndef ANDROID
#include <libavdevice/avdevice.h>
#endif

#include <libavutil/mathematics.h>
#include <libavutil/parseutils.h>
#include <libavutil/pixdesc.h>

#ifdef USE_SWSCALE
    #include <libswscale/swscale.h>
#endif


#if LIBAVUTIL_VERSION_INT <  AV_VERSION_INT(50,38,0)
#define AV_SAMPLE_FMT_NONE SAMPLE_FMT_NONE
#define AV_SAMPLE_FMT_U8   SAMPLE_FMT_U8
#define AV_SAMPLE_FMT_S16  SAMPLE_FMT_S16
#define AV_SAMPLE_FMT_S32  SAMPLE_FMT_S32
#define AV_SAMPLE_FMT_FLT  SAMPLE_FMT_FLT
#define AV_SAMPLE_FMT_DBL  SAMPLE_FMT_DBL
#define AV_SAMPLE_FMT_NB   SAMPLE_FMT_NB
#endif

// See: https://gitorious.org/libav/libav/commit/8889cc4f5b767b323901115a92318a024336e2a1
// "Add planar sample formats and av_sample_fmt_is_planar() to samplefmt.h."
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(51, 17, 0)
    #define OSG_ABLE_PLANAR_AUDIO
#endif
// See: https://gitorious.org/libav/dondiego-libav/commit/30223b3bf2ab1c55499d3d52a244221d24fcc784
// "deprecate the audio resampling API."
// But: Existence of swresample does not depends on version of avcodec.
// By analogue with USE_SWSCALE (which defined during preparing OSG-solution), and example from:
// https://gitorious.org/neutrino-hd/martiis-libstb-hal/commit/8a68eb3f1522642d5318abb2c75b86285f1ae87e
// using of swresample delegate to definition of USE_SWRESAMPLE
// So, to use modern way (if available) of resampling, we should modify CMake-environment before preparing OSG-solution.
// tips:
// 1. Modify "./src/CMakeModules/FindFFmpeg.cmake"
//
// add follow line:
//
// FFMPEG_FIND(LIBSWRESAMPLE  swresample  swresample.h)
//
// after block of lines:
//
// FFMPEG_FIND(LIBAVFORMAT avformat avformat.h)
// FFMPEG_FIND(LIBAVDEVICE avdevice avdevice.h)
// FFMPEG_FIND(LIBAVCODEC  avcodec  avcodec.h)
// FFMPEG_FIND(LIBAVUTIL   avutil   avutil.h)
// FFMPEG_FIND(LIBSWSCALE  swscale  swscale.h)  # not sure about the header to look for here.
//
// 2. Modify "CMakeLists.txt" of plugin, similar to swscale and targeted to definition of USE_SWRESAMPLE
//    add follow lines:
//
//IF(FFMPEG_LIBSWRESAMPLE_FOUND)
//
//    INCLUDE_DIRECTORIES( ${FFMPEG_LIBSWRESAMPLE_INCLUDE_DIRS} ${FFMPEG_LIBSWRESAMPLE_INCLUDE_DIRS}/libswresample )
//
//    ADD_DEFINITIONS(-DUSE_SWRESAMPLE)
//
//    SET(TARGET_EXTERNAL_LIBRARIES ${FFMPEG_LIBRARIES} ${FFMPEG_LIBSWRESAMPLE_LIBRARIES})
//
//ENDIF()
#ifdef USE_SWRESAMPLE
    #include <libswresample/swresample.h>
#endif

// Changes for FFMpeg version greater than 0.6
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 64, 0)
#define CODEC_TYPE_AUDIO AVMEDIA_TYPE_AUDIO
#define CODEC_TYPE_VIDEO AVMEDIA_TYPE_VIDEO
#endif

#ifdef AVERROR
#define AVERROR_IO AVERROR(EIO)
#define AVERROR_NUMEXPECTED AVERROR(EDOM)
#define AVERROR_NOMEM AVERROR(ENOMEM)
#define AVERROR_NOFMT AVERROR(EILSEQ)
#define AVERROR_NOTSUPP AVERROR(ENOSYS)
#define AVERROR_NOENT AVERROR(ENOENT)
#endif

#if LIBAVCODEC_VERSION_MAJOR < 56
   #define AV_CODEC_ID_NONE CODEC_ID_NONE
#endif

#if LIBAVCODEC_VERSION_MAJOR >= 53 || \
    (LIBAVCODEC_VERSION_MAJOR==52 && LIBAVCODEC_VERSION_MINOR>=30) || \
    (LIBAVCODEC_VERSION_MAJOR==52 && LIBAVCODEC_VERSION_MINOR==20 && LIBAVCODEC_VERSION_MICRO >= 1)
    #define USE_AV_LOCK_MANAGER
#endif

#if LIBAVCODEC_VERSION_MAJOR >= 55
    #define OSG_ALLOC_FRAME     av_frame_alloc
    #define OSG_FREE_FRAME      av_frame_free
#else
    #define OSG_ALLOC_FRAME     avcodec_alloc_frame
    #if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(54, 59, 100)
        #define OSG_FREE_FRAME      avcodec_free_frame
    #else
        #define OSG_FREE_FRAME      av_freep
    #endif
#endif

}


#endif // HEADER_GUARD_FFMPEG_HEADERS_H
