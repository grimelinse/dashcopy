#ifndef _DASHSRC_H_
#define _DASHSRC_H_


#define DASHSRC_OK       0
#define DASHSRC_ERROR    1

#define DASHSRC_HTTP_TRY_TIMES  3

#define DASHCOPY_MPD_FILE_NAME  "dashcopy.mpd"

int dashsrc_get_mpdfile(const char *mpdurl);






#endif /* _DASHSRC_H_ */