#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>


#include <jansson.h>
#include <libwebsockets.h>


struct libwebsocket_context* context;
struct libwebsocket* wsi;

static int was_closed;
static int init_step = 0;
static int new_drive = 0;

json_t* advertise_drive_j;
json_t* publish_drive_j;
json_t* subscribe_sense_j;
json_t* subscribe_compass_j;

json_t* drive_left_j;
json_t* drive_right_j;
double compass = 0.0;
double voltage = 0.0;


static int handle_writes() {
    unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 4096 +
                      LWS_SEND_BUFFER_POST_PADDING];
    FILE* stream = fmemopen(&buf[LWS_SEND_BUFFER_PRE_PADDING], 4096, "wb");

    if (init_step < 4) {
        // Send a sequence of messages on initial connection.
        json_t* msg_j;
        switch(init_step) {
            case 1: msg_j = advertise_drive_j; break;
            case 2: msg_j = subscribe_compass_j; break;
            case 3: msg_j = subscribe_sense_j; break;
        }
        json_dumpf(msg_j, stream, JSON_COMPACT);
        init_step++;
        //libwebsocket_callback_on_writable(context, wsi);
    } else {
        // Here because a command message is ready to be sent.
        // Initial call will be triggered by final init, but it
        // just sends a harmless 0,0 setpoint.
        if (new_drive) {
            json_dumpf(publish_drive_j, stream, JSON_COMPACT);
            new_drive = 0;
        }
    }
    if (ftell(stream) > 0) {
        libwebsocket_write(wsi, &buf[LWS_SEND_BUFFER_PRE_PADDING],
                           ftell(stream), LWS_WRITE_TEXT);
    }
    fclose(stream);
}

static int
callback_rosbridge(struct libwebsocket_context *context,
			struct libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason,
					       void *user, void *in, size_t len)
{
	json_t* msg_j;
	const char* topic;
	switch (reason) {
	case LWS_CALLBACK_CLIENT_ESTABLISHED:
		fprintf(stderr, "Websocket client established.\n");
        libwebsocket_callback_on_writable(context, wsi);
        init_step = 1;
        break;

    case LWS_CALLBACK_CLIENT_WRITEABLE:
        handle_writes();
        libwebsocket_callback_on_writable(context, wsi);
        break;

	case LWS_CALLBACK_CLOSED:
		fprintf(stderr, "LWS_CALLBACK_CLOSED\n");
		was_closed = 1;
		break;

	case LWS_CALLBACK_CLIENT_RECEIVE:
		;
		json_error_t json_error;
		msg_j = json_loadb((char *)in, (size_t)len, 0, &json_error);
		topic = json_string_value(json_object_get(msg_j, "topic"));
		if (strcmp(topic, "sense") == 0) {
			voltage = json_real_value(json_object_get(json_object_get(msg_j, "msg"), "battery"));
		}
		if (strcmp(topic, "imu/compass") == 0) {
			compass = json_real_value(json_object_get(json_object_get(msg_j, "msg"), "data"));	
		}
		json_decref(msg_j);
		break;

	default:
		break;
	}

	return 0;
}

/* list of supported protocols and callbacks */
static struct libwebsocket_protocols protocols[] = {
	{
		"rosbridge-protocol",
		callback_rosbridge,
		0,
	},
	{  /* end of list */
		NULL,
		NULL,
		0
	}
};

int clearpath_init(const char* address, int port)
{
	int ietf_version = -1; /* latest */

    // Load JSON messages from files.
    subscribe_sense_j = json_load_file("/home/administrator/compat/json/subscribe-sense.json", 0, NULL);
    subscribe_compass_j = json_load_file("/home/administrator/compat/json/subscribe-compass.json", 0, NULL);
    advertise_drive_j = json_load_file("/home/administrator/compat/json/advertise-drive.json", 0, NULL);
    publish_drive_j = json_load_file("/home/administrator/compat/json/publish-drive.json", 0, NULL);

    json_t* msg_j = json_object_get(publish_drive_j, "msg");
    drive_left_j = json_object_get(msg_j, "left");
    drive_right_j = json_object_get(msg_j, "right");

    if (!advertise_drive_j || !publish_drive_j ||
            !subscribe_sense_j || !subscribe_compass_j) {
        fprintf(stderr, "Problem loading JSON messages.");
        return 1;
    }

	fprintf(stderr, "C rosbridge client.\n");

    // Create websocket context
	context = libwebsocket_create_context(CONTEXT_PORT_NO_LISTEN, NULL,
				protocols, libwebsocket_internal_extensions,
							 NULL, NULL, -1, -1, 0, NULL);
	if (context == NULL) {
		fprintf(stderr, "Creating libwebsocket context failed\n");
		return 1;
	}

	// create client websocket
	wsi = libwebsocket_client_connect(context, address, port, 0,
			"/", address, address, NULL, ietf_version);

	if (wsi == NULL) {
		fprintf(stderr, "libwebsocket dumb connect failed\n");
		return -1;
	}

	fprintf(stderr, "Websocket connections opened\n");

	return 0;
}

int clearpath_spin()
{
    if (was_closed)
        return 1;
    if (libwebsocket_service(context, 0) < 0)
        return 1;
    return 0;
}

int clearpath_set_output(const double left, const double right)
{
    json_real_set(drive_left_j, left / 100.0);
    json_real_set(drive_right_j, right / 100.0);
    new_drive = 1;
}

double clearpath_get_voltage()
{
    return voltage;
}

double clearpath_get_compass()
{
    return compass;
}

void clearpath_destroy()
{
	fprintf(stderr, "Exiting\n");
	libwebsocket_context_destroy(context);
}
