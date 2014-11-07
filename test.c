/**
 * Copyright (c) 2010 Juan Manuel Mouriz <jmouriz@gmail.com>
 */

#include <gtk/gtk.h>

int
main (int argc, char *argv[])
{
  gtk_init (&argc, &argv);

  GtkBuilder *builder = gtk_builder_new ();

  gtk_builder_add_from_file (builder, "test.ui", NULL);

  GtkWidget *window = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));

  g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (gtk_main_quit), NULL);

  gtk_widget_show_all (window);

  gtk_main ();

  return 0;
}
