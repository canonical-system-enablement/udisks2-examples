/* Copyright (C) 2017 Canonical Ltd.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <udisks/udisks.h>

static UDisksClient *client = NULL;
static GMainLoop *loop = NULL;

static void on_object_added(GDBusObjectManager *manager,
                            GDBusObject *obj,
                            gpointer *user_data)
{
    /* If the object is a block device show some info about it */
    UDisksObject *object = UDISKS_OBJECT(obj);
    UDisksBlock *block = udisks_object_peek_block(object);
    if (block != NULL)
    {
        const gchar *id, *label;
        const gchar * const *symlinks;

        g_print("New block device added:\n");
        /* Print the device name */
        g_print("%s \n", udisks_block_get_device(block));

        /* Print the device ID */
        id = udisks_block_get_id(block);
        if (id[0] != '\0')
            g_print("Block Device ID: %s\n", id);

        /* Print the device label */
        label = udisks_block_get_id_label(block);
        if (label[0] != '\0')
            g_print("Block Device Label: %s\n", label);

        /* Print any mount points associated withthe block device */
        UDisksFilesystem *fs = udisks_object_peek_filesystem(object);
        if (fs != NULL) {
            const gchar * const *mount_points;
            mount_points = udisks_filesystem_get_mount_points(fs);
            if (mount_points[0] != NULL)
                g_print("Mount Points: \n");
            for (guint n = 0; mount_points != NULL & mount_points[n] != NULL; n++)
                g_print("%s \n", mount_points[n]);
        }
        g_print("\n\n");
    }
}

static void on_object_removed(GDBusObjectManager *manager,
                              GDBusObject *obj,
                              gpointer *user_data)
{
    /* If the object is a block device name it */
    UDisksObject *object = UDISKS_OBJECT(obj);
    UDisksBlock *block = udisks_object_peek_block(object);
    if (block != NULL)
    {
        g_print("%s Removed.\n", udisks_block_get_device(block));
    }
}

int main(int argc, char **argv)
{
    GError *error;
    GList *objects;
    GList *l;

    /* Get a new client */
    error = NULL;
    client = udisks_client_new_sync(NULL, &error);
    if (client == NULL)
    {
        g_printerr("Error connecting to the udisks daemon: %s\n",
                   error->message);
        g_error_free(error);
        return 1;
    }

    /* Watch for added and removed object events */
    GDBusObjectManager *manager = udisks_client_get_object_manager(client);
    g_signal_connect(manager,
                     "object-added",
                     G_CALLBACK(on_object_added),
                     NULL);
    g_signal_connect(manager,
                     "object-removed",
                     G_CALLBACK(on_object_removed),
                     NULL);

    /* Wait for events to come in */
    loop = g_main_loop_new(NULL, FALSE);
    g_print("Press Ctrl+C to exit\n");
    g_main_loop_run(loop);
    g_main_loop_unref(loop);

    return 0;
}

