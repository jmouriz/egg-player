/**
 * Copyright (c) 2005 Davyd Madeley <davyd@madeley.id.au>
 * Copyright (c) 2010 Juan Manuel Mouriz <jmouriz@gmail.com>
 */

#ifndef __EGG_PLAYER_H__
#define __EGG_PLAYER_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EGG_TYPE_PLAYER			(egg_player_get_type ())
#define EGG_PLAYER(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), EGG_TYPE_PLAYER, EggPlayer))
#define EGG_PLAYER_CLASS(obj)		(G_TYPE_CHECK_CLASS_CAST ((obj), EGG_PLAYER, EggPlayerClass))
#define EGG_IS_PLAYER(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), EGG_TYPE_PLAYER))
#define EGG_IS_PLAYER_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), EGG_TYPE_PLAYER))
#define EGG_PLAYER_GET_CLASS		(G_TYPE_INSTANCE_GET_CLASS ((obj), EGG_TYPE_PLAYER, EggPlayerClass))

typedef struct _EggPlayer	EggPlayer;
typedef struct _EggPlayerClass	EggPlayerClass;

struct _EggPlayer
{
  GtkDrawingArea parent;

  /* < private > */
};

struct _EggPlayerClass
{
  GtkDrawingAreaClass parent_class;

  void (*end_of_stream)	(EggPlayer *player);
  
   /* < private >*/
};

GtkWidget *egg_player_new (void);

G_END_DECLS

#endif
