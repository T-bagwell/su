#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#define TS_PKT_SIZE             188
#define SYNC_BYTE               0x47

short save_char_to_short(char *data)
{
    short out = 0;
    out |= data[0] << 8;
    out |= data[1];

    return out;
}

int adaptation_field(char *data)
{
    char reserved = 0;
    unsigned char *p = (unsigned char *)data;
    int adaptation_field_length = 0;
    int discontinuity_indicator = 0;
    int random_access_indicator = 0;
    int elementary_stream_priority_indicator = 0;
    int PCR_flag = 0;
    int OPCR_flag = 0;
    int splicing_point_flag = 0;
    int transport_private_data_flag = 0;
    int adaptation_field_extension_flag = 0;
    unsigned long program_clock_reference_base = 0;
    unsigned long program_clock_reference_extension = 0;
    unsigned long original_program_clock_reference_base = 0;
    unsigned long original_program_clock_reference_extension = 0;

    adaptation_field_length = *p;

    p++;
    if (adaptation_field_length > 0) {
        discontinuity_indicator = (*p & 0x80) >> 7;
        random_access_indicator = (*p & 0x40) >> 6;
        elementary_stream_priority_indicator = (*p & 0x20) >> 5;
        PCR_flag = (*p & 0x10) >> 4;
        OPCR_flag = (*p & 0x08) >> 3;
        splicing_point_flag = (*p & 0x04) >> 2;
        transport_private_data_flag = (*p & 0x02) >> 1;
        adaptation_field_extension_flag = *p & 0x01;

        fprintf(stdout,
                "        adaptation_field_length = %d\n"
                "        discontinuity_indicator = %d\n"
                "        random_access_indicator = %d\n"
                "        elementary_stream_priority_indicator = %d\n"
                "        PCR_flag = %d\n"
                "        OPCR_flag = %d\n"
                "        splicing_point_flag = %d\n"
                "        transport_private_data_flag = %d\n"
                "        adaptation_field_extension_flag = %d\n",
                adaptation_field_length,
                discontinuity_indicator,
                random_access_indicator,
                elementary_stream_priority_indicator,
                PCR_flag, OPCR_flag,
                splicing_point_flag,
                transport_private_data_flag,
                adaptation_field_extension_flag);
        p++;
        if (PCR_flag == 1) {
            program_clock_reference_base &= 0;
            program_clock_reference_base |= *p << 25;
            p++;
            program_clock_reference_base |= *p << 17;
            p++;
            program_clock_reference_base |= *p << 9;
            p++;
            program_clock_reference_base |= (*p & 0x80) >> 7;
            reserved = (*p & 0x7E) >> 1;

            program_clock_reference_extension &= 0;
            program_clock_reference_extension = (*p & 0x01) << 8;
            p++;
            program_clock_reference_extension |= *p;
            fprintf(stdout,
                    "        program_clock_reference_base = %lu\n"
                    "        program_clock_reference_extension = %lu\n"
                    "        program_clock_reference = %lu\n",
                    program_clock_reference_base,
                    program_clock_reference_extension,
                    program_clock_reference_base + program_clock_reference_extension);
        }

        if (OPCR_flag == 1) {
            original_program_clock_reference_base &= 0;
            original_program_clock_reference_base |= *p << 25;
            p++;
            original_program_clock_reference_base |= *p << 17;
            p++;
            original_program_clock_reference_base |= *p << 9;
            p++;
            original_program_clock_reference_base |= (*p & 0x80) >> 7;
            reserved = (*p & 0x7E) >> 1;

            original_program_clock_reference_extension &= 0;
            original_program_clock_reference_extension = (*p & 0x01) << 8;
            p++;
            original_program_clock_reference_extension |= *p;
            fprintf(stdout,
                    "        program_clock_reference = %lu\n",
                    original_program_clock_reference_base + original_program_clock_reference_extension);
        }


    }

    return 0;
}

void MPEG_transport_stream(int fd)
{
    char buffer[256];
    char *p = NULL;
    short tmp_value = 0;
    int ret = 0;
    int transport_error_indicator = 0;
    int payload_unit_start_indicator = 0;
    int priority = 0;
    int PID = 0;
    int transport_scrambling_control = 0;
    int adaptation_field_control = 0;
    int continuity_counter = 0;

    do {
        memset(buffer, 0, sizeof(buffer));
        ret = read(fd, buffer, TS_PKT_SIZE);
        if (buffer[0] == SYNC_BYTE) {
         /*   fprintf(stdout, "sync_byte\n"); */
        }

        p = buffer;
        p++;
        tmp_value = save_char_to_short(p);
        transport_error_indicator = (*p & 0x80) >> 7;
        payload_unit_start_indicator = (*p & 0x40) >> 6;
        priority = (*p & 0x20) >> 5;
        PID = tmp_value & 0x1FFF;
        p = &buffer[3];
        transport_scrambling_control = (*p & 0xC0) >> 6;
        adaptation_field_control = (*p & 0x30) >> 4;
        continuity_counter = *p & 0x0F;
        fprintf(stdout, "transport_error_indicator = %x "
                "payload_unit_start_indicator = %x "
                "priority = %x "
                "PID = %x "
                "transport_scrambling_control = %x "
                "adaptation_field_control = %x "
                "Counter = %x\n",
                transport_error_indicator,
                payload_unit_start_indicator,
                priority, PID, transport_scrambling_control,
                adaptation_field_control,
                continuity_counter);
        p = &buffer[4];
        if (adaptation_field_control & 0x02 || adaptation_field_control & 0x02) {
            adaptation_field(p);
            continue;
        }

        if (adaptation_field_control & 0x01 || adaptation_field_control & 0x03) {
                continue;
        }
       /*  transport_packet() */
    } while (ret > 0);
}

void usage(char *arg)
{
    fprintf(stderr, "Help:\n"
            "\t%s ts_file\n", arg);
    return;
}

int main(int argc, char *argv[])
{
    char *filename = NULL;
    int fd = 0;

    if (argc < 2 || argc > 2) {
        usage(argv[0]);
        return 0;
    }
    filename = argv[1];

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "cannot open the file %s\n", filename);
        return -ENOENT;
    }

    MPEG_transport_stream(fd);

    close(fd);
    
    return 0;
}
