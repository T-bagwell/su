#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libxml/parser.h>

struct AdaptationSet {
    /* Optional
     * specifies an unique identifier for this Adaptation Set in
     * the scope of the Period. The attribute shall be unique in the
     * scope of the containing Period. The attribute shall not be present in a remote element.
     * If not present, no identifier for the Adaptation Set is specified.
     * */
    int id;

    /* Optional
     * specifies an identifier for the group that is unique in
     * the scope of the containing Period.
     * For details refer to 5.3.3.1.
     * */
    int group;

    /* Optional
     * Declares the language code(s) for this Adaptation Set.
     * The syntax and semantics according to IETF RFC 5646 shall be used.
     * If not present,
     * the language code may be defined for each media component or it may be unknown.
     * */
    char lang[64];

    /* Optional
     * specifies the media content component type for this Adaptation Set.
     * A value of the top-level Content-type 'type' value as defined in RFC1521,
     * Clause 4 shall be taken.
     * If not present, the media content component type may be defined for
     * each media component or it may be unknown.
     * */
    char contentType[32];

    /* Optional
     * specifies the picture aspect ratio of the video media component type,
     * in the form of a string consisting of two integers separated by ':',
     * e.g.
     * "16:9". When this attribute is present,
     * and the attributes @width and @height for the set of
     * Representations are also present, the picture aspect ratio as specified by
     * this attribute shall be the same as indicated by the values of @width, @height, and @sar,
     * i.e.
     * it shall express the same ratio as (@width * sarx): (@height * sary),
     * with sarx the first number in @sar and sary the second number.
     * If not present, the picture aspect ratio may be defined for each media component or
     * it may be unknown.
     * */
    char par[16];

    /* Optional
     * pecifies the minimum @bandwidth value in all Representations in this Adaptation Set.
     * This value has the same units as the @bandwidth attribute.
     * If not present, the value is unknown.
     * */
    int minBandwidth;

    /* Optional
     * specifies the maximum @bandwidth value in all Representations in this Adaptation Set.
     * This value has the same units as the @bandwidth attribute.
     * If not present, the value is unknown.
     * */
    int maxBandwidth;

    /* Optional
     * specifies the minimum @width value in all Representations in this Adaptation Set.
     * This value has the same units as the @width attribute.
     * If not present, the value is unknown.
     * */
    int minWidth;

    /* Optional
     * specifies the maximum @width value in all Representations in this Adaptation Set.
     * This value has the same units as the @width attribute.
     * If not present, the value is unknown.
     * */
    int maxWidth;

    /* Optional
     * specifies the minimum @height value in all Representations in this Adaptation Set.
     * This value has the same units as the @height attribute.
     * If not present, the value is unknown.
     * */
    int minHeight;

    /* Optional
     * specifies the maximum @height value in all Representations in this Adaptation Set.
     * This value has the same units as the @height attribute.
     * If not present, the value is unknown.
     * */
    int maxHeight;

    /* Optional
     * specifies the minimum @framerate value in all Representations in this Adaptation Set.
     * This value is encoded in the same format as the @frameRate attribute.
     * If not present, the value is unknown.
     * */
    int minFrameRate;

    /* Optional
     * specifies the maximum @framerate value in all Representations in this Adaptation Set.
     * This value is encoded in the same format as the @frameRate attribute.
     * If not present, the value is unknown.
     * */
    int maxFrameRate;

    /* Optional with Default Value false
     * when not set to ‘false’, this specifies that for any two Representations,
     * X and Y, within the same Adaptation Set, the m-th Segment of X and the
     * n-th Segment of Y are non- overlapping (as defined in 4.5.2) whenever m is not equal to n.
     * For Adaptation Sets containing Representations with multiple media content components,
     * this attribute value shall be either 'true' or 'false'.
     * For Adaptation Sets containing Representations with a single media content component,
     * when two AdaptationSet elements within a Period share the same integer value for
     * this attribute, then for any two Representations, X and Y,
     * within the union of the two Adaptation Sets, the m-th Segment of X and
     * the n-th Segment of Y are non-overlapping (as defined in 4.5.2) whenever m is not equal to n.
     * */
    int segmentAlignment;

    /* Optional
     * When this flag is set to 'true', the following applies:
     * • All Representations in the Adaptation Set shall
     *   have the same number M of Media Segments;
     * • Let R1, R2, ..., RN be all the Representations within
     *   the Adaptation Set.
     * • Let
     *   o Si,j, for j > 0, be the jth Media Segment in the ith Representation (i.e., Ri)
     *   o if present, let Si,0 be the Initialization Segment in the ith Representation, and
     *   o if present, let Bi be the Bitstream Switching Segment in the ith Representation.
     * • The sequence of
     *   o any Initialization Segment, if present, in the Adaptation Set, with,
     *   o if Bitstream Switching Segments are present,
     *      Bi(1), Si(1),1, Bi(2), Si(2),2, ..., Bi(k), Si(k),k, ..., Bi(M), Si(M),M
     *   o else
     *      Si(1),1, Si(2),2, ..., Si(k),k, ..., Si(M),M,
     *   wherein any i(k) for all k values in the range of 1 to M, respectively,
     *   is an integer value in the range of 1 to N,
     *   results in a "conforming Segment sequence" as defined in 4.5.3 with the media format as
     *   specified in the @mimeType attribute.
     * More detailed rules may be defined for specific media formats.
     * */
    int bitstreamSwitching;

    /* Optional with Default Value false
     * If the @subsegmentAlignment for an Adaptation Set is set to other than 'false',
     * all following conditions shall be satisfied:
     * ⎯ Each Media Segment shall be indexed (i.e. either it contains a Segment index or
     *   there is an Index Segment providing an index for the Media Segment)
     * ⎯ For any two Representations, X and Y, within the same Adaptation Set,
     *   the m-th Subsegment of X and the n-th Subsegment of Y are non-
     *   overlapping (as defined in 4.5.2) whenever m is not equal to n.
     * ⎯ For Adaptation Sets containing Representations with a single media content component,
     *   when two AdaptationSet elements within a Period share the same integer value for
     *   this attribute, then for any two Representations, X and Y,
     *   within the union of the two Adaptation Sets, the m-th Subsegment of X and the
     *   n-th Subsegment of Y are non-overlapping (as defined in 4.5.2) whenever m is not equal to n.
     * */
    int subsegmentAlignment;

    /* Optional with Default Value 0
     * when greater than 0, specifies that each Subsegment with SAP_type greater than
     * 0 starts with a SAP of type less than or equal to the value of @subsegmentStartsWithSAP.
     * A Subsegment starts with SAP when the Subsegment contains a SAP, and for the first SAP,
     * ISAU is the index of the first access unit that follows ISAP,
     * and ISAP is contained in the Subsegment.
     * The semantics of @subsegmentStartsWithSAP equal to 0 are unspecified.
     * */
    int subsegmentStartsWithSAP;
};

struct Period {

    /* Optional
     * specifies an identifier for this Period.
     * The idenfifier shall be unique within the scope of the Media Presentation.
     * If not present, no identifier for the Period is provided.
     * */
    int id;

    /* Optional
     * if present, specifies the PeriodStart time of the Period.
     * The PeriodStart time is used as an anchor to determine the
     * MPD start time of each Media Segment as well as to
     * determine the presentation time of each
     * each access unit in the Media Presentation timeline.
     * If not present, refer to the details in 5.3.2.1.
     * */
    int start;

    /* Optional
     * if present specifies the duration of the Period to
     * determine the PeriodStart time of the next Period.
     * If not present, refer to the details in 5.3.2.1.
     * */
    int duration;

    /* Optional with Default Value
     * When set to ‘true’, this is equivalent as if
     * the AdaptationSet@bitstreamSwitching for each Adaptation Set contained
     * in this Period is set to 'true'. In this case,
     * the AdaptationSet@bitstreamSwitching attribute
     * shall not be set to 'false' for any Adaptation Set in this Period.
     * */
    int bitstreamSwitching;

};

struct Period dash_period_context;

struct dash_mpd_context {
    /*
     * Optional
     * specifies an identifier for the Media Presentation.
     * It is recommended to use an identifier that is unique within
     * the scope in which the Media Presentation is published.
     * If not specified, no MPD-internal identifier is provided.
     * However, for example the URL to the MPD may be used
     * as an identifier for the Media Presentation.
     * */
    int id;

    /*
     * Mandatory
     * specifies a list of Media Presentation profiles as described in 8.
     * The contents of this attribute shall conform to either the
     * pro-simple or pro-fancy productions of RFC6381,
     * Section 4.5, without the enclosing DQUOTE characters, i.e.
     * including only the unencodedv or encodedv elements respectively.
     * As profile identifier the URI defined for the conforming
     * Media Presentation profiles as described in 8 shall be used.
     * */
    int profiles;

    /* Optional with Default Value,
     * specifies whether the Media Presentation Description may be
     * updated (@type="dynamic") or not (@type="static").
     *
     *     NOTE
     *          Static MPDs are typically used for On-Demand services,
     *          whereas dynamic MPDs are used for live services.
     * */
    int type;

    /*
     * Conditionally Mandatory
     * For @type='dynamic' this attribute shall be present.
     * In this case it specifies the anchor for the computation of
     * the earliest availability time (in UTC) for any Segment in the Media Presentation.
     *
     * For @type=“static” if present, it specifies the Segment availability start time
     * for all Segments referred to in this MPD. If not present,
     * all Segments described in the MPD shall become available at the time the MPD becomes available.
     * */
    int64_t availabilityStartTime;

    /*
     * Optional
     * specifies the latest Segment availability end time
     * for any Segment in the Media Presentation.
     * When not present, the value is unknown.
     * */
    int64_t availabilityEndTime;

    /*
     * Conditionally Mandatory
     * specifies the duration of the entire Media Presentation.
     * If the attribute is not present,
     * the duration of the Media Presentation is unknown.
     * In this case the attribute MPD@minimumUpdatePeriod shall be present.
     * This attribute shall be present when the attribute MPD@minimumUpdatePeriod is not present.
     * */
    int64_t mediaPresentationDuration;

    /*
     * Optional
     * If this attribute is present,
     * it specifies the smallest period between potential changes to the MPD.
     * This can be useful to control the frequency at which a client checks for updates.
     * If this attribute is not present it indicates that the MPD does not change.
     * If MPD@type is ‘static’, @minimumUpdatePeriod shall not be present.
     * Details on the use of the value of this attribute are specified in 5.4.
     * */
    int64_t minimumUpdatePeriod;

    /*
     * Mandatory
     * specifies a common duration used in the definition
     * of the Representation data rate (see @bandwidth attribute in 5.3.5.2).
     * */
    int64_t minBufferTime;

    /*
     * Optional
     * specifies the duration of the time shifting buffer
     * that is guaranteed to be available for a Media Presentation with type 'dynamic'.
     * When not present, the value is infinite.
     * This value of the attribute is undefined if the type attribute is equal to ‘static’.
     * */
    int64_t timeShiftBufferDepth;

    /*
     * Optional
     * when @type is 'dynamic',
     * it specifies a fixed delay offset in time from the presentation time
     * of each access unit that is suggested to be used for presentation of each access unit.
     * For more details refer to 7.2.1. When not specified,
     * the no value is provided and the client is expected to choose a suitable value.
     *
     * when @type is 'static'
     * the value of the attribute is undefined and may be ignored.
     * */
    int64_t suggestedPresentationDelay;

    /*
     * Optional
     * specifies the maximum duration of any Segment in any Representation
     * in the Media Presentation, i.e. documented in this MPD and any future update of the MPD.
     * If not present, then the maximum Segment duration shall be the maximum duration
     * of any Segment documented in this MPD.
     * */
    int64_t maxSegmentDuration;

    /*
     * Optional
     * specifies the maximum duration of any Media Subsegment
     * in any Representation in the Media Presentation.
     * If not present, the same value as for the maximum Segment duration is implied.
     * */
    int64_t maxSubsegmentDuration;

    /*
    struct ProgramInformation pi;
    struct BaseURL baseurl;
    struct Location location;
    struct Period *period;
    struct Metrics metrics;
    */
};

int dash_playlist_context_get(char *url, char *buf, int buf_size)
{
    int ret = 0;
    int fd = 0;
    struct stat mpd_stat;

    ret = stat(url, &mpd_stat);
    if (ret < 0) {
        fprintf(stderr, "Cannot get status info of  dash MPD file %s\n", url);
        return -ENOENT;
    }

    fd = open(url, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Cannot open dash MPD file %s\n", url);
        return -ENOENT;
    }

    ret = read(fd, buf, mpd_stat.st_size);
    if (ret < mpd_stat.st_size) {
        fprintf(stderr, "Read dash MPD Context error.\n");
        return -ENOENT;
    }

    close(fd);
    return mpd_stat.st_size;
}

int dash_mpd_context_get(char *buf, struct dash_mpd_context *dmc, int filesize)
{
    xmlDoc *doc = NULL;
    xmlNodePtr root_element = NULL;
    xmlNodePtr node = NULL;
    xmlNodePtr period_node = NULL;
    xmlNodePtr adaptionset_node = NULL;
    xmlAttrPtr attr = NULL;
    xmlChar *val = NULL;

    doc = xmlReadMemory(buf, filesize, NULL, NULL, 0);
    root_element = xmlDocGetRootElement(doc);
    node = root_element;
    val = xmlGetProp(node, (xmlChar *)"type");
    if (val) {
        printf("type = [%s]\n", val);
        xmlFree(val);
    }

    attr = node->properties;
    while (attr) {
        val = xmlGetProp(node, attr->name);
        if (!strcasecmp((const char *)attr->name, (const char *)"id")) {
            printf( "id = [%s]\n", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"profiles")) {
            printf( "profiles = [%s]\n", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"availabilityStartTime")) {
            printf( "availabilityStartTime = [%s]\n", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"availabilityEndTime")) {
            printf( "availabilityEndTime = [%s]\n", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"mediaPresentationDuration")) {
            printf( "mediaPresentationDuration = [%s]\n", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"minimumUpdatePeriod")) {
            printf( "minimumUpdatePeriod = [%s]\n", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"minBufferTime")) {
            printf( "minBufferTime = [%s]\n", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"timeShiftBufferDepth")) {
            printf( "timeShiftBufferDepth = [%s]\n", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"suggestedPresentationDelay")) {
            printf( "suggestedPresentationDelay = [%s]\n", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"maxSegmentDuration")) {
            printf( "maxSegmentDuration = [%s]\n", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"maxSubsegmentDuration")) {
            printf( "maxSubsegmentDuration = [%s]\n", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"publishTime")) {
            printf( "publishTime = [%s]\n", val);
        } else {
        }
        attr = attr->next;
        xmlFree(val);
    }

    node = xmlFirstElementChild(node);
    while (node) {
        printf("[in %s] ", node->name);
        if (!strcasecmp((const char *)node->name, (const char *)"Period")) {
            period_node = node;
            attr = node->properties;
            while (attr) {
                val = xmlGetProp(node, attr->name);
                if (!strcasecmp((const char *)attr->name, (const char *)"id")) {
                    printf("id = [%s] ", val);
                } else if (!strcasecmp((const char *)attr->name, (const char *)"start")) {
                    printf("start = [%s] ", val);
                } else if (!strcasecmp((const char *)attr->name, (const char *)"duration")) {
                    printf("duration = [%s] ", val);
                } else if (!strcasecmp((const char *)attr->name, (const char *)"bitstreamSwitching")) {
                    printf("bitstreamSwitching = [%s] ", val);
                } else {

                }
                attr = attr->next;
                xmlFree(val);
            }
            printf("\n");
            period_node = xmlFirstElementChild(node);
            while (period_node) {
                if (!strcasecmp((const char *)period_node->name, (const char *)"BaseURL")) {
                    printf("BaseURL\n");
                    attr = period_node->properties;
                } else if (!strcasecmp((const char *)period_node->name, (const char *)"SegmentBase")) {
                    printf("SegmentBase\n");
                    attr = period_node->properties;
                } else if (!strcasecmp((const char *)period_node->name, (const char *)"Subset")) {
                    printf("Subset\n");
                    attr = period_node->properties;
                } else if (!strcasecmp((const char *)period_node->name, (const char *)"SegmentTemplate")) {
                    printf("SegmentTemplate\n");
                    attr = period_node->properties;
                } else if (!strcasecmp((const char *)period_node->name, (const char *)"SegmentList")) {
                    printf("SegmentList\n");
                    attr = period_node->properties;
                } else if (!strcasecmp((const char *)period_node->name, (const char *)"AdaptationSet")) {
                    printf("AdaptationSet\n");
                    attr = period_node->properties;
                    while (attr) {
                        val = xmlGetProp(period_node, attr->name);
                        if (!strcasecmp((const char *)attr->name, (const char *)"id")) {
                            printf("id = [%s] ", val);
                        } else if (!strcasecmp((const char *)attr->name, (const char *)"group")) {
                            printf("group = [%s] ", val);
                        } else if (!strcasecmp((const char *)attr->name, (const char *)"lang")) {
                            printf("lang = [%s] ", val);
                        } else if (!strcasecmp((const char *)attr->name, (const char *)"contentType")) {
                            printf("contentType = [%s] ", val);
                        } else if (!strcasecmp((const char *)attr->name, (const char *)"par")) {
                            printf("par = [%s] ", val);
                        } else if (!strcasecmp((const char *)attr->name, (const char *)"minBandwidth")) {
                            printf("minBandwidth = [%s] ", val);
                        } else if (!strcasecmp((const char *)attr->name, (const char *)"maxBandwidth")) {
                            printf("maxBandwidth = [%s] ", val);
                        } else if (!strcasecmp((const char *)attr->name, (const char *)"minWidth")) {
                            printf("minWidth = [%s] ", val);
                        } else if (!strcasecmp((const char *)attr->name, (const char *)"maxWidth")) {
                            printf("maxWidth = [%s] ", val);
                        } else if (!strcasecmp((const char *)attr->name, (const char *)"minHeight")) {
                            printf("minHeight = [%s] ", val);
                        } else if (!strcasecmp((const char *)attr->name, (const char *)"maxHeight")) {
                            printf("maxHeight = [%s] ", val);
                        } else if (!strcasecmp((const char *)attr->name, (const char *)"minFrameRate")) {
                            printf("minFrameRate = [%s] ", val);
                        } else if (!strcasecmp((const char *)attr->name, (const char *)"maxFrameRate")) {
                            printf("maxFrameRate = [%s] ", val);
                        } else if (!strcasecmp((const char *)attr->name, (const char *)"segmentAlignment")) {
                            printf("segmentAlignment = [%s] ", val);
                        } else if (!strcasecmp((const char *)attr->name, (const char *)"bitstreamSwitching")) {
                            printf("bitstreamSwitching = [%s] ", val);
                        } else if (!strcasecmp((const char *)attr->name, (const char *)"subsegmentAlignment")) {
                            printf("subsegmentAlignment = [%s] ", val);
                        } else if (!strcasecmp((const char *)attr->name, (const char *)"subsegmentStartsWithSAP")) {
                            printf("subsegmentStartsWithSAP = [%s] ", val);
                        } else {
                        }

                        attr = attr->next;
                        xmlFree(val);
                    }
                    printf("\n");
                }
                period_node = xmlNextElementSibling(period_node);
            }
            printf("\n");
        } else if (!strcasecmp((const char *)node->name, (const char *)"ProgramInformation")) {
        } else if (!strcasecmp((const char *)node->name, (const char *)"BaseURL")) {
        } else if (!strcasecmp((const char *)node->name, (const char *)"Location")) {
        } else if (!strcasecmp((const char *)node->name, (const char *)"Metrics")) {
        }
        node = xmlNextElementSibling(node);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int ret = 0;
    char buf[1024 * 1024] = {0, };
    char *filename = NULL;
    char *p = NULL;
    struct dash_mpd_context dmc;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <MPD file URL or path>\n", argv[0]);
    }
    filename = argv[1];

    ret = dash_playlist_context_get(filename, buf, sizeof(buf));
    if (ret < 0) {
        return ret;
    }
    p = buf;

    memset(&dmc, 0, sizeof(dmc));
    dash_mpd_context_get(buf, &dmc,ret);

    return ret;
}
