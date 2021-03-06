    /*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2015 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief QGC Video Streaming Initialization
 *   @author Gus Grubba <mavlink@grubba.com>
 */

#include <QtQml>
#include <QDebug>

#if defined(QGC_GST_STREAMING)
#include <gst/gst.h>
#endif

#include "VideoStreaming.h"
#include "VideoItem.h"
#include "VideoSurface.h"

#if defined(QGC_GST_STREAMING)
    G_BEGIN_DECLS
    // Our own plugin
    GST_PLUGIN_STATIC_DECLARE(QGC_VIDEOSINK_PLUGIN);
    // The static plugins we use
#if defined(__mobile__)
    GST_PLUGIN_STATIC_DECLARE(coreelements);
    GST_PLUGIN_STATIC_DECLARE(libav);
    GST_PLUGIN_STATIC_DECLARE(rtp);
    GST_PLUGIN_STATIC_DECLARE(udp);
    GST_PLUGIN_STATIC_DECLARE(videoparsersbad);
    GST_PLUGIN_STATIC_DECLARE(x264);
#endif
    G_END_DECLS
#endif

#if defined(QGC_GST_STREAMING)
#if defined(__macos__)
#ifdef QGC_INSTALL_RELEASE
static void qgcputenv(const QString& key, const QString& root, const QString& path)
{
    QString value = root + path;
    qputenv(key.toStdString().c_str(), QByteArray(value.toStdString().c_str()));
}
#endif
#endif
#endif

void initializeVideoStreaming(int &argc, char* argv[])
{
#if defined(QGC_GST_STREAMING)
    // Initialize GStreamer
    GError* error = NULL;
    if (!gst_init_check(&argc, &argv, &error)) {
        qCritical() << "gst_init_check() failed: " << error->message;
        g_error_free(error);
    }
    // Our own plugin
    GST_PLUGIN_STATIC_REGISTER(QGC_VIDEOSINK_PLUGIN);
    // The static plugins we use
#if defined(__mobile__)
    GST_PLUGIN_STATIC_REGISTER(coreelements);
    GST_PLUGIN_STATIC_REGISTER(libav);
    GST_PLUGIN_STATIC_REGISTER(rtp);
    GST_PLUGIN_STATIC_REGISTER(udp);
    GST_PLUGIN_STATIC_REGISTER(videoparsersbad);
    GST_PLUGIN_STATIC_REGISTER(x264);
#endif

#ifdef __macos__
#ifdef QGC_INSTALL_RELEASE
    QString currentDir = QCoreApplication::applicationDirPath();
    qgcputenv("GST_PLUGIN_SCANNER",           currentDir, "/gst-plugin-scanner");
    qgcputenv("GTK_PATH",                     currentDir, "/../Frameworks/GStreamer.framework/Versions/Current");
    qgcputenv("GIO_EXTRA_MODULES",            currentDir, "/../Frameworks/GStreamer.framework/Versions/Current/lib/gio/modules");
    qgcputenv("GST_PLUGIN_SYSTEM_PATH_1_0",   currentDir, "/../Frameworks/GStreamer.framework/Versions/Current/lib/gstreamer-1.0");
    qgcputenv("GST_PLUGIN_SYSTEM_PATH",       currentDir, "/../Frameworks/GStreamer.framework/Versions/Current/lib/gstreamer-1.0");
    qgcputenv("GST_PLUGIN_PATH_1_0",          currentDir, "/../Frameworks/GStreamer.framework/Versions/Current/lib/gstreamer-1.0");
    qgcputenv("GST_PLUGIN_PATH",              currentDir, "/../Frameworks/GStreamer.framework/Versions/Current/lib/gstreamer-1.0");
//    QStringList env = QProcessEnvironment::systemEnvironment().keys();
//    foreach(const QString &key, env) {
//        qDebug() << key << QProcessEnvironment::systemEnvironment().value(key);
//    }
#endif
#endif

#else
    Q_UNUSED(argc);
    Q_UNUSED(argv);
#endif
    qmlRegisterType<VideoItem>              ("QGroundControl.QgcQtGStreamer", 1, 0, "VideoItem");
    qmlRegisterUncreatableType<VideoSurface>("QGroundControl.QgcQtGStreamer", 1, 0, "VideoSurface", QLatin1String("VideoSurface from QML is not supported"));
}

void shutdownVideoStreaming()
{
#if defined(QGC_GST_STREAMING)
    gst_deinit();
#endif
}
