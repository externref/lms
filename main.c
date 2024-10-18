#include "gtk/gtk.h"
#include "lib/gui.c"
#include <stdio.h>

int main(int argc, char **argv) {

  GtkApplication *application =
      gtk_application_new("org.lms.project", G_APPLICATION_FLAGS_NONE);

  g_signal_connect(application, "activate", G_CALLBACK(gui_init_gui), NULL);
  int status = g_application_run(G_APPLICATION(application), argc, argv);
  g_object_unref(application);

  sqlite3_close(db);
  free(db);
}
