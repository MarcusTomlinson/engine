// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/shell/platform/linux/fl_platform_plugin.h"
#include "flutter/shell/platform/linux/public/flutter_linux/fl_json_method_codec.h"
#include "flutter/shell/platform/linux/public/flutter_linux/fl_method_channel.h"

struct _FlPlatformPlugin {
  GObject parent_instance;

  FlMethodChannel* channel;
};

G_DEFINE_TYPE(FlPlatformPlugin, fl_platform_plugin, G_TYPE_OBJECT)

static void method_call_cb(FlMethodChannel* channel,
                           FlMethodCall* method_call,
                           gpointer user_data) {
  // FlPlatformPlugin* self = FL_PLATFORM_PLUGIN(user_data);

  const gchar* method = fl_method_call_get_name(method_call);
  FlValue* args = fl_method_call_get_args(method_call);
  if (strcmp(method, "Clipboard.setData") == 0) {
    g_debug("Got Clipboard.setData\n");
    fl_method_call_respond_not_implemented(method_call, nullptr);
  } else if (strcmp(method, "Clipboard.getData") == 0) {
    g_debug("Got Clipboard.getData\n");
    fl_method_call_respond_not_implemented(method_call, nullptr);
  } else if (strcmp(method, "HapticFeedback.vibrate") == 0) {
    g_debug("Got HapticFeedback.vibrate\n");
    fl_method_call_respond_not_implemented(method_call, nullptr);
  } else if (strcmp(method, "SystemSound.play") == 0) {
    if (fl_value_get_type(args) != FL_VALUE_TYPE_STRING) {
      g_warning("Ignoring unknown SystemSound.play message with unknown type");
      fl_method_call_respond_error(method_call, "FIXME", nullptr, nullptr,
                                   nullptr);
    } else {
      const gchar* type = fl_value_get_string(args);
      g_debug("Got SystemSound.play(\"%s\")", type);
      // FIXME(robert-ancell): Play sound
      fl_method_call_respond(method_call, nullptr, nullptr);
    }
  } else if (strcmp(method, "SystemChrome.setPreferredOrientations") == 0) {
    g_debug("Got SystemChrome.setPreferredOrientations\n");
    fl_method_call_respond_not_implemented(method_call, nullptr);
  } else if (strcmp(method, "SystemChrome.setApplicationSwitcherDescription") ==
             0) {
    if (fl_value_get_type(args) != FL_VALUE_TYPE_MAP) {
      g_warning("Ignoring unknown flutter/platform message type");
      fl_method_call_respond_error(method_call, "FIXME", nullptr, nullptr,
                                   nullptr);
    } else {
      FlValue* label_value = fl_value_lookup_string(args, "label");
      FlValue* primary_color_value =
          fl_value_lookup_string(args, "primaryColor");

      const gchar* label =
          label_value != nullptr &&
                  fl_value_get_type(label_value) == FL_VALUE_TYPE_STRING
              ? fl_value_get_string(label_value)
              : "";
      int64_t primary_color =
          primary_color_value != nullptr &&
                  fl_value_get_type(primary_color_value) == FL_VALUE_TYPE_INT
              ? fl_value_get_int(primary_color_value)
              : -1;

      g_debug(
          "Got SystemChrome.setApplicationSwitcherDescription(\"%s\", "
          "%" G_GINT64_FORMAT ")\n",
          label, primary_color);
      // FIXME(robert-ancell): Use label
      fl_method_call_respond(method_call, nullptr, nullptr);
    }

  } else if (strcmp(method, "SystemChrome.setEnabledSystemUIOverlays") == 0) {
    g_debug("Got SystemChrome.setEnabledSystemUIOverlays\n");
    fl_method_call_respond_not_implemented(method_call, nullptr);
  } else if (strcmp(method, "SystemChrome.setSystemUIOverlayStyle") == 0) {
    g_debug("Got SystemChrome.setSystemUIOverlayStyle\n");
    fl_method_call_respond_not_implemented(method_call, nullptr);
  } else if (strcmp(method, "SystemNavigator.pop") == 0) {
    g_debug("Got SystemNavigator.pop\n");
    // FIXME(robert-ancell): Quit the application
    fl_method_call_respond_not_implemented(method_call, nullptr);
  } else
    fl_method_call_respond_not_implemented(method_call, nullptr);
}

static void fl_platform_plugin_dispose(GObject* object) {
  FlPlatformPlugin* self = FL_PLATFORM_PLUGIN(object);

  g_clear_object(&self->channel);

  G_OBJECT_CLASS(fl_platform_plugin_parent_class)->dispose(object);
}

static void fl_platform_plugin_class_init(FlPlatformPluginClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = fl_platform_plugin_dispose;
}

static void fl_platform_plugin_init(FlPlatformPlugin* self) {}

FlPlatformPlugin* fl_platform_plugin_new(FlBinaryMessenger* messenger) {
  g_return_val_if_fail(FL_IS_BINARY_MESSENGER(messenger), nullptr);

  FlPlatformPlugin* self =
      FL_PLATFORM_PLUGIN(g_object_new(fl_platform_plugin_get_type(), nullptr));

  g_autoptr(FlJsonMethodCodec) codec = fl_json_method_codec_new();
  self->channel = fl_method_channel_new(messenger, "flutter/platform",
                                        FL_METHOD_CODEC(codec));
  fl_method_channel_set_method_call_handler(self->channel, method_call_cb,
                                            self);

  return self;
}
