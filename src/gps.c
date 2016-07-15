#include <gps.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <curl/curl.h>

#define CAR_GUID "MAZDA"
#define PUBLIC_KEY "1930261821342254"
#define APP_DEBUG 1
#define LOOP_WAIT_TIME 1
#define SERVER_POST_URL "http://remote/write"

void post_data(float lat, float lon, float speed, long time);

int main() {
	int rc;
	struct timeval tv;

	struct gps_data_t gps_data;
	if ((rc = gps_open("localhost", "2947", &gps_data)) == -1) {
		printf("code: %d, reason: %s\n", rc, gps_errstr(rc));
		return EXIT_FAILURE;
	}
	gps_stream(&gps_data, WATCH_ENABLE | WATCH_JSON, NULL);

	while (1) {
		/* wait for 2 seconds to receive data */
		if (gps_waiting (&gps_data, 2000000)) {
			/* read data */
			if ((rc = gps_read(&gps_data)) == -1) {
				printf("error occured reading gps data. code: %d, reason: %s\n", rc, gps_errstr(rc));
			} else {
				/* Display data from the GPS receiver. */
				if ((gps_data.status == STATUS_FIX) && 
						(gps_data.fix.mode == MODE_2D || gps_data.fix.mode == MODE_3D) &&
						!isnan(gps_data.fix.latitude) && 
						!isnan(gps_data.fix.longitude)) {
					gettimeofday(&tv, NULL);

					post_data(gps_data.fix.latitude,gps_data.fix.longitude,gps_data.fix.speed,tv.tv_sec);


				} else {
					printf("Nothing\n");
				}
			}
		}

		sleep(LOOP_WAIT_TIME);
	}

	/* When you are done... */
	gps_stream(&gps_data, WATCH_DISABLE, NULL);
	gps_close (&gps_data);

	return EXIT_SUCCESS;
}

void post_data(float lat, float lon, float speed, long time) {

	char *buf;
	size_t sz;
	sz = snprintf(NULL,0,"k=%s&cr=%s&lt=%f&ln=%f&sp=%f&t=%ld",PUBLIC_KEY,CAR_GUID,lat,lon,speed,time);
	buf = (char *)malloc(sz + 1);
	snprintf(buf,sz + 1,"k=%s&cr=%s&lt=%f&ln=%f&sp=%f&t=%ld",PUBLIC_KEY,CAR_GUID,lat,lon,speed,time);
	
	if ( APP_DEBUG == 1 ) {
		printf("key: %s, car: %s, latitude: %f, longitude: %f, speed: %f, timestamp: %ld\n",PUBLIC_KEY,CAR_GUID, lat, lon, speed, time);
	}

	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if(curl) {
	
		curl_easy_setopt(curl, CURLOPT_URL, SERVER_POST_URL);
		
		if ( APP_DEBUG == 0 ) {
			//Disable printing out curl output unless debug.
			curl_easy_setopt(curl, CURLOPT_NOBODY,1);
		}

		//Set buf as post data.
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf);

		//Set timeout
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

		//Trigger CURL request.
		res = curl_easy_perform(curl);
		if ( APP_DEBUG == 1 ) {
			if ( res != CURLE_OK ) {
				printf("Error performing requested, likely timeout or cant resolve DNS");
			}
		}

		//Free memory
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();

}
