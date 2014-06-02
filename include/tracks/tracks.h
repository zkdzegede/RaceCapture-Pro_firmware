#ifndef TRACKS_H_
#define TRACKS_H_

#include "stddef.h"
#include "default_tracks.h"
#include "geopoint.h"

#define DEFAULT_TRACK_TARGET_RADIUS	0.0001

#define TF_STAGE    (1 << 0);
#define TF_IGNORE   (1 << 1);

typedef struct _Track {
    unsigned char track_type;
    union {
        GeoPoint sectors[];
        GeoPoint startFinish;
        struct {
            GeoPoint finish;
            GeoPoint start;
        };
    };
} Track;

typedef struct _Tracks{
	size_t count;
	Track tracks[];
} Tracks;

int flash_tracks(const Tracks *source, size_t rawSize);
int update_track(const Track *track, size_t index, int mode);
int flash_default_tracks(void);
const Tracks * get_tracks();



#endif /* TRACKS_H_ */
