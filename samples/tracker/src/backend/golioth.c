/*
 * Copyright Circuit Dojo (c) 2021
 *
 * SPDX-License-Identifier: LicenseRef-Circuit-Dojo-5-Clause
 */

#include <stdio.h>
#include <zephyr/kernel.h>

#include <net/golioth/system_client.h>

#include <app_event_manager.h>
#include <app_backend.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(backend_golioth);

static struct golioth_client *client = GOLIOTH_SYSTEM_CLIENT_GET();

static bool is_connected = false;

void golioth_on_connect(struct golioth_client *client)
{
    is_connected = true;

    APP_EVENT_MANAGER_PUSH(APP_EVENT_BACKEND_CONNECTED);
}

/* Public functions*/
int app_backend_connect()
{
    golioth_system_client_start();

    return 0;
}

int app_backend_disconnect()
{
    golioth_system_client_stop();

    return 0;
}

bool app_backend_is_connected()
{
    return is_connected;
}

int app_backend_stream(char *p_topic, uint8_t *p_data, size_t len)
{
    int err;

    char path[128];
    err = snprintf(path, sizeof(path), ".s/%s", p_topic);
    if (err < 0)
    {
        LOG_WRN("Failed to encode path. Err: %i", err);
        return err;
    }

    err = golioth_lightdb_set(client,
                              path,
                              COAP_CONTENT_FORMAT_APP_CBOR,
                              p_data, len);
    if (err)
    {
        LOG_WRN("Failed to stream data: %i", err);
    }

    return err;
}

int app_backend_publish(char *p_topic, uint8_t *p_data, size_t len)
{
    int err;

    char path[128];
    err = snprintf(path, sizeof(path), ".d/%s", p_topic);
    if (err < 0)
    {
        LOG_WRN("Failed to encode path. Err: %i", err);
        return err;
    }

    err = golioth_lightdb_set(client,
                              path,
                              COAP_CONTENT_FORMAT_APP_CBOR,
                              p_data, len);
    if (err)
    {
        LOG_WRN("Failed to publish data: %i", err);
    }

    return err;
}

int app_backend_init(char *client_id, size_t client_id_len)
{
    /*Setup and connect to Golioth*/
    client->on_connect = golioth_on_connect;
    golioth_system_client_start();

    return 0;
}