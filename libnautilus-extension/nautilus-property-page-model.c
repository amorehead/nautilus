/*
 *  nautilus-property-page.h - Property pages exported by
 *                             NautilusPropertyProvider objects.
 *
 *  Copyright (C) 2003 Novell, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 *  Author:  Dave Camp <dave@ximian.com>
 *
 */

#include <config.h>
#include "nautilus-property-page-model.h"

#include <glib-object.h>

enum
{
    PROP_0,
    PROP_TITLE,
    PROP_SECTIONS,
    PROP_ITEMS,
    LAST_PROP
};

typedef struct
{
    char *title;
    GList *sections;
    GList *items;
} NautilusPropertyPageModelPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (NautilusPropertyPageModel, nautilus_property_page_model, G_TYPE_OBJECT)

/**
 * SECTION:nautilus-property-page
 * @title: NautilusPropertyPageModel
 * @short_description: Property page descriptor object
 *
 * #NautilusPropertyPageModel is an object that describes a page in the file
 * properties dialog. Extensions can provide #NautilusPropertyPageModel objects
 * by registering a #NautilusPropertyPageModelProvider and returning them from
 * nautilus_property_page_model_provider_get_pages(), which will be called by the
 * main application when creating file properties dialogs.
 */

/**
 * nautilus_property_page_model_new:
 * @name: the identifier for the property page
 * @label: the user-visible label of the property page
 * @page: the property page to display
 *
 * Creates a new #NautilusPropertyPageModel from page_widget.
 *
 * Returns: a newly created #NautilusPropertyPageModel
 */
NautilusPropertyPageModel *
nautilus_property_page_model_new (const char *title,
                                  GList      *sections,
                                  GList      *items)
{
    NautilusPropertyPageModel *page;

    g_return_val_if_fail (title != NULL, NULL);
    g_return_val_if_fail (sections != NULL, NULL);
    g_return_val_if_fail (items != NULL, NULL);

    page = g_object_new (NAUTILUS_TYPE_PROPERTY_PAGE_MODEL,
                         "title", title,
                         "sections", sections,
                         "items", items,
                         NULL);

    return page;
}

char *
nautilus_property_page_model_get_title (NautilusPropertyPageModel *self)
{
    NautilusPropertyPageModelPrivate *priv;

    g_return_val_if_fail (NAUTILUS_IS_PROPERTY_PAGE_MODEL (self), NULL);

    priv = nautilus_property_page_model_get_instance_private (self);

    return priv->title;
}

static void
free_section (gpointer data)
{
    NautilusPropertyPageModelSection *section = (NautilusPropertyPageModelSection *) data;

    g_free(section->title);
}

static NautilusPropertyPageModelSection*
copy_section (gconstpointer src,
              gpointer      data)
{
    NautilusPropertyPageModelSection *section = (NautilusPropertyPageModelSection *) src;
    NautilusPropertyPageModelSection *copy = g_new (NautilusPropertyPageModelSection, 1);

    copy->title = g_strdup (section->title);
    copy->id = section->id;

    return copy;
}

static void
free_item (gpointer data)
{
    NautilusPropertyPageModelItem *section = (NautilusPropertyPageModelItem *) data;

    g_free(section->field);
    g_free(section->value);
}

static NautilusPropertyPageModelItem*
copy_item (gconstpointer src,
           gpointer      data)
{
    NautilusPropertyPageModelItem *item = (NautilusPropertyPageModelItem *) src;
    NautilusPropertyPageModelItem *copy = g_new (NautilusPropertyPageModelItem, 1);

    copy->field = g_strdup (item->field);
    copy->value = g_strdup (item->value);
    copy->section_id = item->section_id;

    return copy;
}


static void
nautilus_property_page_model_get_property (GObject    *object,
                                           guint       param_id,
                                           GValue     *value,
                                           GParamSpec *pspec)
{
    NautilusPropertyPageModel *self;
    NautilusPropertyPageModelPrivate *priv;

    g_return_if_fail (NAUTILUS_IS_PROPERTY_PAGE_MODEL (object));

    self = NAUTILUS_PROPERTY_PAGE_MODEL (object);
    priv = nautilus_property_page_model_get_instance_private (self);

    switch (param_id)
    {
        case PROP_TITLE:
        {
            g_value_set_string (value, priv->title);
        }
        break;

        case PROP_SECTIONS:
        {
            g_value_set_pointer (value, priv->sections);
        }
        break;

        case PROP_ITEMS:
        {
            g_value_set_pointer (value, priv->items);
        }
        break;

        default:
        {
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
        }
        break;
    }
}

static void
nautilus_property_page_model_set_property (GObject      *object,
                                           guint         param_id,
                                           const GValue *value,
                                           GParamSpec   *pspec)
{
    NautilusPropertyPageModel *self;
    NautilusPropertyPageModelPrivate *priv;

    g_return_if_fail (NAUTILUS_IS_PROPERTY_PAGE_MODEL (object));

    self = NAUTILUS_PROPERTY_PAGE_MODEL (object);
    priv = nautilus_property_page_model_get_instance_private (self);

    switch (param_id)
    {
        case PROP_TITLE:
        {
            g_free (priv->title);
            priv->title = g_strdup (g_value_get_string (value));
        }
        break;

        case PROP_SECTIONS:
        {
            if (priv->sections)
            {
                g_list_free_full (priv->sections, free_section);
            }

            priv->sections = g_list_copy_deep (g_value_get_pointer (value),
                                               (GCopyFunc) copy_section, NULL);

    g_object_notify (G_OBJECT (self), "sections");
        }
        break;

        case PROP_ITEMS:
        {
            if (priv->items)
            {
                g_list_free_full (priv->items, free_item);
            }

            priv->items = g_list_copy_deep (g_value_get_pointer (value),
                                            (GCopyFunc) copy_item, NULL);
        }
        break;

        default:
        {
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
        }
        break;
    }
}

static void
nautilus_property_page_model_finalize (GObject *object)
{
    NautilusPropertyPageModel *self;
    NautilusPropertyPageModelPrivate *priv;

    self = NAUTILUS_PROPERTY_PAGE_MODEL (object);
    priv = nautilus_property_page_model_get_instance_private (self);

    if (priv->title != NULL)
    {
        g_free (priv->title);
        priv->title = NULL;
    }
    if (priv->sections != NULL)
    {
        g_list_free_full (priv->sections, free_section);
        priv->sections = NULL;
    }
    if (priv->items != NULL)
    {
        g_list_free_full (priv->items, free_item);
        priv->items = NULL;
    }

    G_OBJECT_CLASS (nautilus_property_page_model_parent_class)->finalize (object);
}

static void
nautilus_property_page_model_init (NautilusPropertyPageModel *page)
{
}

static void
nautilus_property_page_model_class_init (NautilusPropertyPageModelClass *class)
{
    G_OBJECT_CLASS (class)->finalize = nautilus_property_page_model_finalize;
    G_OBJECT_CLASS (class)->get_property = nautilus_property_page_model_get_property;
    G_OBJECT_CLASS (class)->set_property = nautilus_property_page_model_set_property;

    g_object_class_install_property (G_OBJECT_CLASS (class),
                                     PROP_TITLE,
                                     g_param_spec_string ("title",
                                                          "Title",
                                                          "Title of the page",
                                                          NULL,
                                                          G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE | G_PARAM_READABLE));
    g_object_class_install_property (G_OBJECT_CLASS (class),
                                     PROP_SECTIONS,
                                     g_param_spec_pointer ("sections",
                                                           "Sections",
                                                           "Sections of the page",
                                                           G_PARAM_READWRITE));
    g_object_class_install_property (G_OBJECT_CLASS (class),
                                     PROP_ITEMS,
                                     g_param_spec_pointer ("items",
                                                           "Items",
                                                           "Items for the property page",
                                                           G_PARAM_READWRITE));
}

GList*
nautilus_property_page_model_get_sections (NautilusPropertyPageModel *self)
{
    NautilusPropertyPageModelPrivate *priv;

    priv = nautilus_property_page_model_get_instance_private (self);

    return priv->sections;
}

GList*
nautilus_property_page_model_get_items (NautilusPropertyPageModel *self)
{
    NautilusPropertyPageModelPrivate *priv;

    priv = nautilus_property_page_model_get_instance_private (self);

    return priv->items;
}

void
nautilus_property_page_model_set_sections (NautilusPropertyPageModel *self,
                                           GList                     *sections)
{
    g_object_set (self, "sections", sections, NULL);
}

void
nautilus_property_page_model_set_items (NautilusPropertyPageModel *self,
                                        GList                     *items)
{
    g_object_set (self, "items", items, NULL);
}

void
nautilus_property_page_model_set_title (NautilusPropertyPageModel *self,
                                        const char                *title)

{
    g_object_set (self, "title", title, NULL);
}