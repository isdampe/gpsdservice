#include <gps.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <curl/curl.h>

#define CAR_GUID "MAZDA"
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
	sz = snprintf(NULL,0,"car=%s&lat=%f&lon=%f&spd=%f&t=%ld",CAR_GUID,lat,lon,speed,time);
	buf = (char *)malloc(sz + 1);
	snprintf(buf,sz + 1,"car=%s&lat=%f&lon=%f&spd=%f&t=%ld",CAR_GUID,lat,lon,speed,time);
	
	if ( APP_DEBUG == 1 ) {
		printf("car: %s, latitude: %f, longitude: %f, speed: %f, timestamp: %ld\n",CAR_GUID, lat, lon, speed, time);
	}

	CURL *curl;
	CURLcode res;

	/* In windows, this will init the winsock stuff */ 
	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */ 
	curl = curl_easy_init();
	if(curl) {
		/* First set the URL that is about to receive our POST. This URL can
			 just as well be a https:// URL if that is what should receive the
			 data. */ 
		curl_easy_setopt(curl, CURLOPT_URL, SERVER_POST_URL);
		
		if ( APP_DEBUG == 0 ) {
			//Disable printing out curl output unless debug.
			curl_easy_setopt(curl, CURLOPT_NOBODY,1);
		}

		/* Now specify the POST data */ 
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf);

		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);
			
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();

}
