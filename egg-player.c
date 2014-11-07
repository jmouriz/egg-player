/**
 * Copyright (C) 2008 Felipe Contreras.
 * Copyright (C) 2005-2006 Davyd Madeley <davyd@madeley.id.au>
 * Copyright (C) 2010 Juan Manuel Mouriz <jmouriz@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include <gtk/gtk.h>
#include <gst/gst.h>

#include <math.h>
#include <time.h>

#ifndef WIN32
#include <gdk/gdkx.h>
#else
#include <gdk/gdkwin32.h>
#endif

#include <gst/interfaces/xoverlay.h>

#include "egg-player.h"
#include "egg-player-marshallers.h"

enum
{
  PROP_0,

  PROP_URI,
  PROP_FORCE_ASPECT_RATIO,
};

#define EGG_PLAYER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EGG_TYPE_PLAYER, EggPlayerPrivate))

G_DEFINE_TYPE (EggPlayer, egg_player, GTK_TYPE_DRAWING_AREA);

static void egg_player_set_property (GObject      *object,
                                     guint         property_id,
                                     const GValue *value,
                                     GParamSpec   *pspec);


static void egg_player_get_property (GObject      *object,
                                     guint         property_id,
                                     GValue       *value,
                                     GParamSpec   *pspec);

typedef struct _EggPlayerPrivate EggPlayerPrivate;

struct _EggPlayerPrivate
{
  gchar *uri;

  gboolean force_aspect_ratio;
};

enum
{
  END_OF_STREAM,
  LAST_SIGNAL
};

static guint egg_player_signals[LAST_SIGNAL] = { 0 };

static void
egg_player_class_init (EggPlayerClass *class)
{
  GObjectClass *obj_class;
  GParamSpec *param_spec;

  obj_class = G_OBJECT_CLASS (class);

  obj_class->set_property = egg_player_set_property;
  obj_class->get_property = egg_player_get_property;

  /* EggPlayer signals */
  egg_player_signals[END_OF_STREAM] = g_signal_new ("end-of-stream",
                                                    G_OBJECT_CLASS_TYPE (obj_class),
                                                    G_SIGNAL_RUN_FIRST,
                                                    G_STRUCT_OFFSET (EggPlayerClass, end_of_stream),
                                                    NULL, NULL,
                                                    _egg_player_marshal_NONE__NONE,
                                                    G_TYPE_NONE, 0);

  param_spec = g_param_spec_boolean ("force-aspect-ratio",
                                     "Force aspect ratio",
                                     "Force aspect ratio",
                                     FALSE, /* default no force aspect ratio */
                                     G_PARAM_READWRITE);

  g_object_class_install_property (obj_class,
                                   PROP_FORCE_ASPECT_RATIO,
                                   param_spec);


  param_spec = g_param_spec_string ("uri",
                                    "URI",
                                    "Media URI",
                                    NULL, /* no default uri */
                                    G_PARAM_READWRITE);

  g_object_class_install_property (obj_class,
                                   PROP_URI,
                                   param_spec);

  g_type_class_add_private (obj_class, sizeof (EggPlayerPrivate));

  gst_init (NULL, NULL);
}

static void
egg_player_init (EggPlayer *player)
{
}

/* properties setter and getter */
static void
egg_player_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
  EggPlayerPrivate *priv;

  priv = EGG_PLAYER_GET_PRIVATE (object);

  switch (property_id)
  {
    case PROP_URI:
      priv->uri = g_value_get_string (value);
      //egg_player_play_uri (EGG_PLAYER (object), priv->uri);
      egg_player_play_uri (GTK_WIDGET (object), priv->uri);
      break;

    case PROP_FORCE_ASPECT_RATIO:
      priv->force_aspect_ratio = g_value_get_boolean (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
egg_player_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
  EggPlayerPrivate *priv;

  priv = EGG_PLAYER_GET_PRIVATE (object);

  switch (property_id)
  {
    case PROP_URI:
      g_value_set_string (value, priv->uri);
      break;

    case PROP_FORCE_ASPECT_RATIO:
      g_value_set_boolean (value, priv->force_aspect_ratio);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static gboolean
bus_callback (GstBus *bus, GstMessage *message, gpointer data)
{
  switch (GST_MESSAGE_TYPE (message))
  {
    case GST_MESSAGE_EOS:
      g_debug ("end-of-stream");
      break;

    case GST_MESSAGE_ERROR:
    {
      gchar *debug;
      GError *error;

      gst_message_parse_error (message, &error, &debug);
      g_free (debug);

      g_warning ("Error: %s", error->message);
      g_error_free (error);
      break;
    }

    default:
      /* error */
      break;
  }

  return TRUE;
}

void
egg_player_play_uri (GtkWidget *player, gchar *uri)
{
  GstElement *pipeline;
  GstElement *bin;
  GstElement *videosink;

  gpointer window;

  #ifndef WIN32
  window = GINT_TO_POINTER (GDK_WINDOW_XWINDOW (GTK_WIDGET (player)->window));
  #else
  window = GINT_TO_POINTER (GDK_WINDOW_HWND (GTK_WIDGET (player)->window));
  #endif

  /*
  backend_stop ();
  */

  pipeline = gst_pipeline_new ("gst-player");

  bin = gst_element_factory_make ("playbin", "bin");
  videosink = gst_element_factory_make ("xvimagesink", "videosink");

  g_object_set (G_OBJECT (bin), "video-sink", videosink, NULL);

  gst_bin_add (GST_BIN (pipeline), bin);

  {
    GstBus *bus;
    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    gst_bus_add_watch (bus, bus_callback, NULL);
    gst_object_unref (bus);
  }

  g_object_set (G_OBJECT (bin), "uri", uri, NULL);

  g_object_set (G_OBJECT (videosink), "force-aspect-ratio", FALSE, NULL);

  if (GST_IS_X_OVERLAY (videosink))
  {
    gst_x_overlay_set_xwindow_id (GST_X_OVERLAY (videosink), GPOINTER_TO_INT (window));
    gst_x_overlay_handle_events (GST_X_OVERLAY (videosink), FALSE);
  }

  gst_element_set_state (pipeline, GST_STATE_PLAYING);
}

/*
void
backend_stop (void)
{
    if (pipeline)
    {
        gst_element_set_state (pipeline, GST_STATE_NULL);
        gst_object_unref (GST_OBJECT (pipeline));
        pipeline = NULL;
    }
}

void
backend_pause (void)
{
    gst_element_set_state (pipeline, GST_STATE_PAUSED);
}

void
backend_resume (void)
{
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
}

void
backend_reset (void)
{
    gst_element_seek (pipeline, 1.0,
                      GST_FORMAT_TIME,
                      seek_flags,
                      GST_SEEK_TYPE_SET, 0,
                      GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

void
backend_seek (gint value)
{
    gst_element_seek (pipeline, 1.0,
                      GST_FORMAT_TIME,
                      seek_flags,
                      GST_SEEK_TYPE_CUR, value * GST_SECOND,
                      GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

void
backend_seek_absolute (guint64 value)
{
    gst_element_seek (pipeline, 1.0,
                      GST_FORMAT_TIME,
                      seek_flags,
                      GST_SEEK_TYPE_SET, value,
                      GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

guint64
backend_query_position (void)
{
    GstFormat format = GST_FORMAT_TIME;
    gint64 cur;
    gboolean result;

    result = gst_element_query_position (pipeline, &format, &cur);
    if (!result || format != GST_FORMAT_TIME)
        return GST_CLOCK_TIME_NONE;

    return cur;
}

guint64
backend_query_duration (void)
{
    GstFormat format = GST_FORMAT_TIME;
    gint64 cur;
    gboolean result;

    result = gst_element_query_duration (pipeline, &format, &cur);
    if (!result || format != GST_FORMAT_TIME)
        return GST_CLOCK_TIME_NONE;

    return cur;
}
*/

GtkWidget *
egg_player_new (void)
{
  return g_object_new (EGG_TYPE_PLAYER, NULL);
}
