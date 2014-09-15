#include <stdio.h>
#include <stdlib.h>

#include "vrt.h"
#include "cache/cache.h"

#include "vcc_if.h"

int getVisitorId(char * ad_ip, char* visitorId);
int getTimestamp(char* timestamp);
int getip( char* ip, char* res);


int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf)
{
	return (0);
}

VCL_STRING
vmod_gettimestamp(const struct vrt_ctx *ctx)
{
        char *p;
        unsigned u, v;

        char timestamp[14];
        memset( (char*) timestamp, 0, 14);

        u = WS_Reserve(ctx->ws, 0); /* Reserve some work space */
        p = ctx->ws->f;         /* Front of workspace area */

        getTimestamp((char*) timestamp);

        v = snprintf(p, u, "%s", timestamp);

        v++;
        if (v > u) {
                /* No space, reset and leave */
                WS_Release(ctx->ws, 0);
                return (NULL);
        }
        /* Update work space with what we've used */
        WS_Release(ctx->ws, v);
        return (p);
}


VCL_STRING
vmod_parseip(const struct vrt_ctx *ctx, VCL_STRING ad_ip)
{

	char *p;
        unsigned u, v;

        char res[4];
        char ip[16];
        memset( (char*) res, 0, 4);
        memset( (char*) ip, 0, 16);

	strncpy(ip, ad_ip, 16 );

        u = WS_Reserve(ctx->ws, 0); /* Reserve some work space */
        p = ctx->ws->f;         /* Front of workspace area */

        getip((char*) ip, (char*) res);

        v = snprintf(p, u, "%s", res);

        v++;
        if (v > u) {
                /* No space, reset and leave */
                WS_Release(ctx->ws, 0);
                return (NULL);
        }
        /* Update work space with what we've used */
        WS_Release(ctx->ws, v);
        return (p);

}


VCL_STRING
vmod_getvisitorid(const struct vrt_ctx *ctx, VCL_STRING ad_ip)
{
	char *p;
	unsigned u, v;
	
        char visitorId[18];
        memset( (char*) visitorId, 0, 18);
	
	u = WS_Reserve(ctx->ws, 0); /* Reserve some work space */
	p = ctx->ws->f;		/* Front of workspace area */

	getVisitorId((char*)ad_ip, (char*)visitorId);

	v = snprintf(p, u, "%s", visitorId);

	v++;
	if (v > u) {
		/* No space, reset and leave */
		WS_Release(ctx->ws, 0);
		return (NULL);
	}
	/* Update work space with what we've used */
	WS_Release(ctx->ws, v);
	return (p);
}

int getip( char* ad_ip, char* res){

  	char * pch;
        pch = strtok (ad_ip,".");
        int i=1;
        while (pch != NULL)
        {
                i++;
                pch= strtok (NULL, ".");
                if (i==4) break;
        }
        if (strlen(pch) == 1 ) sprintf(res, "00%s", pch);
        if (strlen(pch) == 2 ) sprintf(res, "0%s", pch);
        if (strlen(pch) == 3 ) sprintf(res, "%s", pch);

	return 0;
}


int getVisitorId(char * ad_ip, char* visitorId) {

        int random;
        char timestamp[13];
        memset( (char*) timestamp, 0, 13);


        getTimestamp( (char*) timestamp );

        //printf( "Retour methode : %s \n", timestamp);

        srand(time(NULL));
        random = rand() % 99 + 1;

        //printf( "Retour random : %02i \n", random);

        sprintf(visitorId, "%s%s%02i", (char*)ad_ip,timestamp, random);

       // printf("visitorId = %s \n", visitorId);
        return 0;
}


int getTimestamp(char* timestamp) {

        struct timespec tms;

    /* The C11 way */
    /* if (! timespec_get(&tms, TIME_UTC)) { */

    /* POSIX.1-2008 way */
    if (clock_gettime(CLOCK_REALTIME,&tms)) {
        return -1;
    }
    /* seconds, multiplied with 1000  */
    int64_t millis = tms.tv_sec * 1000;
    millis += tms.tv_nsec/1000;
    /* round up if necessary */
    if (tms.tv_nsec % 1000 >= 500) {
        ++millis;
    }
    //printf("Milliseconds: %"PRId64"\n",millis);

    sprintf((char*) timestamp, "%lld", millis);

    return 1;

}


