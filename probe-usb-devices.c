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

#include <string.h>
#include <udisks/udisks.h>

static UDisksClient *client = NULL;
static GMainLoop *loop = NULL;

int main(int argc, char **argv)
{
    GError *error;
    GList *objects;
    GList *l, *m;
    UDisksBlock *block;
    UDisksDrive *drive;
    UDisksFilesystem *fs;

    /* Get a new client */
    error = NULL;
    client = udisks_client_new_sync(NULL, &error);
    if (client == NULL)
    {
        g_printerr("Error connecting to the udisks daemon: %s\n", error->message);
        g_error_free(error);
    }

    /* Get the list of all objects managed by UDisks */
    objects = g_dbus_object_manager_get_objects(udisks_client_get_object_manager(client));
    for (l = objects; l != NULL; l = l->next)
    {
        UDisksObject *object = UDISKS_OBJECT (l->data);
        block = udisks_object_peek_block(object);
        if (block != NULL)
        {
            const gchar *device, *drive_str;
            const gchar *id, *label;
            const gchar * const *symlinks;

            device = udisks_block_get_device(block);
            if (device == NULL)
                continue;

            /* Only print block devices backed by a drive */
            drive_str = udisks_block_get_drive(block);
            if (drive_str == NULL || g_strcmp0(drive_str, "/") == 0)
                continue;

            /* If udisks says to ignore a block device then do so */
            if (udisks_block_get_hint_ignore(block))
                continue;

            /* Exclude system disks */
            if (udisks_block_get_hint_system(block))
                continue;

            /* Check if there's a USB-backed drive associated with this
             * object
             */
            for (m = objects; m != NULL; m = m->next)
            {
                UDisksObject *drive_obj = UDISKS_OBJECT(m->data);
                drive = udisks_object_peek_drive(drive_obj);
                if (drive != NULL)
                {
                    /* We're only interested in removable USB drives */
                    const gchar *bus = udisks_drive_get_connection_bus(drive);
                    gboolean removable = udisks_drive_get_removable(drive);
                    if (!removable || g_strcmp0(bus, "usb") != 0)
                        continue;

                 

                    /* Print the device details */
                    g_print("device path : %s\n", device);
                    g_print("device drive: %s\n", drive_str);

                    /* Print the device ID */
                    id = udisks_block_get_id(block);
                    if (id && strlen(id))
                        g_print("device id   : %s\n", id);

                    /* Print the device label */
                    label = udisks_block_get_id_label(block);
                    if (label && strlen(label))
                        g_print("device label: %s\n", label);

                    /* Print any mount points associated withthe block device */
                    fs = udisks_object_peek_filesystem(object);
                    if (fs != NULL) {
                        const gchar * const *mount_points;
                        mount_points = udisks_filesystem_get_mount_points(fs);
                        if (mount_points[0] != NULL)
                            g_print("device mount points: \n");
                        for (guint n = 0; mount_points != NULL & mount_points[n] != NULL; n++)
                            g_print("%s \n", mount_points[n]);
                    }
                    g_print("---\n");

                }
            }
        }
    }
    g_list_foreach(objects, (GFunc)g_object_unref, NULL);
    g_list_free(objects);

    return 0;
}

