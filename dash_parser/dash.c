#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libxml/parser.h>

struct Representation {
    /* Mandatory
     * specifies an identifier for this Representation.
     * The identifier shall be unique within a Period unless the
     * Representation is functionally identically to another Representation in the same Period.
     * The identifier shall not contain whitespace characters.
     * If used in the template-based URL construction as defined in 5.3.9.4.4,
     * the string shall only contain characters that are
     * permitted within an HTTP-URL according to RFC 1738.
     * */
    int id;

    /* Mandatory
     * Consider a hypothetical constant bitrate channel of
     * bandwidth with the value of this attribute in bits per second (bps).
     * Then, if the Representation is continuously delivered at this bitrate,
     * starting at any SAP that is indicated either by @startWithSAP or
     * by any Segment Index box, a client can be assured of
     * having enough data for continuous playout providing
     * playout begins after @minBufferTime * @bandwidth bits have been
     * received (i.e. at time @minBufferTime after the first bit is received).
     *
     * For dependent Representations this value shall specify the
     * minimum bandwidth as defined above of this Representation and
     * all complementary Representations.
     * */
    int bandwidth;

    /* Optional
     * specifies a quality ranking of the Representation relative to
     * other Representations in the same Adaptation Set.
     * Lower values represent higher quality content.
     * If not present then no ranking is defined.
     * */
    int qualityRanking;

    /* Optional
     * specifies all complementary Representations the Representation depends on
     * in the decoding and/or presentation process as a
     * whitespace-separated list of values of @id attributes.
     *
     * If not present, the Representation can be decoded and
     * presented independently of any other Representation.
     *
     * This attribute shall not be present where there are no dependencies.
     * */
    int dependencyId;

    /* Optional
     * The attribute may be present for Representations containing video and
     * its semantics are unspecified for any other type of Representations.
     *
     * If present, the attribute @mediaStreamStructureId specifies a
     * whitespace-separated list of media stream structure identifier values.
     * If media streams share the same media stream structure identifier value,
     * the media streams shall have the following characteristics:
     *  . The media streams have the same number of Stream Access Points of type 1 to 3.
     *  . The values of TSAP, TDEC, TEPT, and TPTF of
     *    the i-th SAP of type 1 to 3 in one media stream are identical to the values of
     *    TSAP, TDEC, TEPT, and TPTF, respectively, of the i-th SAP of type 1 to 3 in the
     *    other media streams for any value of i from 1 to the number of
     *    SAPs of type 1 to 3 in any of the media streams.
     *  . A media stream formed by concatenating the media stream of
     *    a first Representation until ISAU (exclusive) of the i-th SAP of
     *    type 1 to 3 and the media stream of a second Representation
     *    (having the same media stream structure identifier value as for
     *    the first Representation) starting from the ISAU (inclusive) of
     *    the i-th SAP of type 1 to 3 conforms to the specification in
     *    which the media stream format is specified for any value of
     *    i from 1 to the number of SAPs of type 1 to 3 in either media stream.
     *    Furthermore, the decoded pictures have an acceptable quality regardless of
     *    type of the Stream Access Point access unit used.
     *
     *  All media stream structure identifier values for one
     *  Adaptation Set shall differ from those of another Adaptation Set.
     *
     *  If not present, then for this Representation no similarities to
     *  other Representations are known.
     *
     *  NOTE Indicating multiple media stream structure
     *  identifier values for a Representation can be useful in
     *  cases where switching between Representations A and
     *  B as well as between Representations B and C is
     *  allowed at non-IDR intra pictures, but switching
     *  between Representations A and C would cause too
     *  severe a degradation in the quality of the leading
     *  pictures and is hence not allowed. To indicate these
     *  permissions and restrictions, Representation A would
     *  contain @mediaStreamStructureId equal to “1”,
     *  Representation B would contain
     *  @mediaStreamStructureId equal to “1 2”, and
     *  Representation C would contain
     *  @mediaStreamStructureId equal to “2”
     * */
    int mediaStreamStructureId;



};
struct ContentComponent {
    /* Optional
     * specifies an identifier for this media component.
     * The attribute shall be unique in the scope of the containing Adaptation Set.
     * */
    int id;

    /* Optional
     * same semantics as in Table 5 for @lang attribute
     * */
    char lang[64];

    /* Optional
     * same semantics as in Table 5 for @contentType attribute
     * */
    char contentType[32];

    /* Optional
     * same semantics as in Table 5 for @par attribute
     * */
    char par[16];
};

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

int dash_contentcomponent_attr_get(struct ContentComponent *content_component, xmlAttrPtr attr, xmlNodePtr adaptionset_node)
{
    xmlChar *val = NULL;
    while (attr) {
        val = xmlGetProp(adaptionset_node, attr->name);
        if (!strcasecmp((const char *)attr->name, (const char *)"id")) {
            printf("id = [%s] ", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"lang")) {
            printf("lang = [%s] ", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"contentType")) {
            printf("contentType = [%s] ", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"par")) {
            printf("par = [%s] ", val);
        } else {
        }
        attr = attr->next;
        xmlFree(val);
    }
    return 0;
}

int dash_representation_attr_get(struct Representation *representation, xmlNodePtr adaptionset_node)
{
    xmlAttrPtr attr = NULL;
    xmlChar *val = NULL;
    attr = adaptionset_node->properties;
    while (attr) {
        val = xmlGetProp(adaptionset_node, attr->name);
        if (!strcasecmp((const char *)attr->name, (const char *)"id")) {
            printf("id = [%s] ", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"bandwidth")) {
            printf("bandwidth = [%s] ", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"qualityRanking")) {
            printf("qualityRanking = [%s] ", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"dependencyId")) {
            printf("dependencyId = [%s] ", val);
        } else if (!strcasecmp((const char *)attr->name, (const char *)"mediaStreamStructureId")) {
            printf("mediaStreamStructureId = [%s] ", val);
        } else {
        }
        attr = attr->next;
        xmlFree(val);
    }

    return 0;
}

int dash_adaptationset_attr_get(xmlAttrPtr attr, xmlNodePtr period_node,  struct AdaptationSet *adaptionset)
{
    xmlChar *val = NULL;
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

    return 0;
}

int dash_period_attr_get(struct Period *peroid, xmlAttrPtr attr, xmlNodePtr period_node)
{
    xmlChar *val = NULL;
    while (attr) {
        val = xmlGetProp(period_node, attr->name);
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

    return 0;
}

int dash_mpd_attr_get(struct dash_mpd_context *mpd, xmlAttrPtr attr, xmlNodePtr node)
{
    xmlChar *val = NULL;
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
    return 0;
}

int dash_contentcomponent_context_get(struct ContentComponent *content_component, xmlNodePtr adaptionset_node, xmlNodePtr content_component_node)
{
    xmlAttrPtr attr = NULL;

    printf("ContentComponent\n");
    attr = adaptionset_node->properties;
    dash_contentcomponent_attr_get(content_component, attr, adaptionset_node);
    printf("\n");
    content_component_node = xmlFirstElementChild(adaptionset_node);
    while (content_component_node) {
        if (!strcasecmp((const char *)content_component_node->name, (const char *)"Accessibility")) {
            printf("Accessibility\n");
            attr = content_component_node->properties;
        } else if (!strcasecmp((const char *)content_component_node->name, (const char *)"Role")){
            printf("Role\n");
            attr = content_component_node->properties;
        } else if (!strcasecmp((const char *)content_component_node->name, (const char *)"Rating")) {
            printf("Rating\n");
            attr = content_component_node->properties;
        } else if (!strcasecmp((const char *)content_component_node->name, (const char *)"Viewpoint")){
            printf("Viewpoint\n");
            attr = content_component_node->properties;
        } else {
        }
        content_component_node = xmlNextElementSibling(content_component_node);
    }
    return 0;
}

int dash_representation_context_get(struct Representation *representation, xmlNodePtr adaptionset_node, xmlNodePtr representation_node)
{
    xmlAttrPtr attr = NULL;
    printf("Representation--\n");

    dash_representation_attr_get(representation, adaptionset_node);
    printf("\n");
    representation_node = xmlFirstElementChild(adaptionset_node);
    while (representation_node) {
        if (!strcasecmp((const char *)representation_node->name, (const char *)"CommonAttributesElements")) {
            printf("CommonAttributesElements\n");
            attr = representation_node->properties;
        } else if (!strcasecmp((const char *)representation_node->name, (const char *)"BaseURL")) {
            printf("BaseURL\n");
            attr = representation_node->properties;
        } else if (!strcasecmp((const char *)representation_node->name, (const char *)"SubRepresentation")) {
            printf("SubRepresentation\n");
            attr = representation_node->properties;
        } else if (!strcasecmp((const char *)representation_node->name, (const char *)"SegmentBase")) {
            printf("SegmentBase\n");
            attr = representation_node->properties;
        } else if (!strcasecmp((const char *)representation_node->name, (const char *)"SegmentList")) {
            printf("SegmentList");
            attr = representation_node->properties;
        } else if (!strcasecmp((const char *)representation_node->name, (const char *)"SegmentTemplate")) {
            printf("SegmentTemplate");
            attr = representation_node->properties;
        } else {
        }
        representation_node = xmlNextElementSibling(representation_node);
    }

    return 0;
}

int dash_adaptationset_context_get(xmlNodePtr adaptionset_node, struct AdaptationSet *adaptionset)
{
    xmlNodePtr content_component_node = NULL;
    xmlNodePtr representation_node = NULL;
    xmlAttrPtr attr = NULL;
    struct ContentComponent *content_component = (struct ContentComponent *)malloc(sizeof(struct ContentComponent));
    struct Representation *representation = (struct Representation *)malloc(sizeof(struct Representation));

    while (adaptionset_node) {
        if (!strcasecmp((const char *)adaptionset_node->name, (const char *)"ContentComponent")) {
            dash_contentcomponent_context_get(content_component, adaptionset_node, content_component_node);
        } else if (!strcasecmp((const char *)adaptionset_node->name, (const char *)"BaseURL")) {
            printf("BaseURL\n");
            attr = adaptionset_node->properties;
        } else if (!strcasecmp((const char *)adaptionset_node->name, (const char *)"SegmentBase")) {
            printf("SegmentBase\n");
            attr = adaptionset_node->properties;
        } else if (!strcasecmp((const char *)adaptionset_node->name, (const char *)"SegmentTemplate")) {
            printf("SegmentTemplate\n");
            attr = adaptionset_node->properties;
        } else if (!strcasecmp((const char *)adaptionset_node->name, (const char *)"SegmentList")) {
            printf("SegmentList\n");
            attr = adaptionset_node->properties;
        } else if (!strcasecmp((const char *)adaptionset_node->name, (const char *)"Accessibility")) {
            printf("Accessibility\n");
            attr = adaptionset_node->properties;
        } else if (!strcasecmp((const char *)adaptionset_node->name, (const char *)"Role")) {
            printf("Role\n");
            attr = adaptionset_node->properties;
        } else if (!strcasecmp((const char *)adaptionset_node->name, (const char *)"Rating")) {
            printf("Rating\n");
            attr = adaptionset_node->properties;
        } else if (!strcasecmp((const char *)adaptionset_node->name, (const char *)"Viewpoint")) {
            printf("Viewpoint\n");
            attr = adaptionset_node->properties;
        } else if (!strcasecmp((const char *)adaptionset_node->name, (const char *)"Representation")) {
            dash_representation_context_get(representation, adaptionset_node, representation_node);
        }
        adaptionset_node = xmlNextElementSibling(adaptionset_node);
    }

    return 0;
}

int dash_peroid_context_get(xmlNodePtr node, struct Period *dash_peroid)
{
    xmlNodePtr period_node = NULL;
    xmlNodePtr adaptionset_node = NULL;
    xmlAttrPtr attr = NULL;
    struct AdaptationSet *adaptionset = (struct AdaptationSet *)malloc(sizeof(struct AdaptationSet));

    printf("[in %s] ", node->name);
    if (!strcasecmp((const char *)node->name, (const char *)"Period")) {
        period_node = node;
        attr = node->properties;
        dash_period_attr_get(dash_peroid, attr, period_node);
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
                dash_adaptationset_attr_get(attr, period_node, adaptionset);
                printf("\n");
                adaptionset_node = xmlFirstElementChild(period_node);
                dash_adaptationset_context_get(adaptionset_node, adaptionset);
            }
            period_node = xmlNextElementSibling(period_node);
        }
        printf("\n");
    } else if (!strcasecmp((const char *)node->name, (const char *)"ProgramInformation")) {
    } else if (!strcasecmp((const char *)node->name, (const char *)"BaseURL")) {
    } else if (!strcasecmp((const char *)node->name, (const char *)"Location")) {
    } else if (!strcasecmp((const char *)node->name, (const char *)"Metrics")) {
    }
    return 0;
}

int dash_mpd_context_get(char *buf, struct dash_mpd_context *dmc, int filesize)
{
    xmlDoc *doc = NULL;
    xmlNodePtr root_element = NULL;
    xmlNodePtr node = NULL;
    xmlAttrPtr attr = NULL;
    xmlChar *val = NULL;
    struct Period *peroid = (struct Period *)malloc(sizeof(struct Period));
    struct dash_mpd_context *mpd = (struct dash_mpd_context *)malloc(sizeof(struct dash_mpd_context));

    doc = xmlReadMemory(buf, filesize, NULL, NULL, 0);
    root_element = xmlDocGetRootElement(doc);
    node = root_element;
    val = xmlGetProp(node, (xmlChar *)"type");
    if (val) {
        printf("type = [%s]\n", val);
        xmlFree(val);
    }

    attr = node->properties;
    dash_mpd_attr_get(mpd, attr, node);

    node = xmlFirstElementChild(node);
    while (node) {
        dash_peroid_context_get(node, peroid);
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
