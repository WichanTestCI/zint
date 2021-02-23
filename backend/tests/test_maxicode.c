/*
    libzint - the open source barcode library
    Copyright (C) 2019 - 2021 Robin Stuart <rstuart114@gmail.com>

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the project nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
 */
/* vim: set ts=4 sw=4 et : */

#include "testcommon.h"

static void test_large(int index, int debug) {

    testStart("");

    int ret;
    struct item {
        int option_1;
        int option_2;
        char *pattern;
        int length;
        char* primary;
        int ret;
        int expected_rows;
        int expected_width;
    };
    // s/\/\*[ 0-9]*\*\//\=printf("\/*%3d*\/", line(".") - line("'<"))
    struct item data[] = {
        /*  0*/ { -1, -1, "1", 138, "", 0, 33, 30 }, // Mode 4 (138 agrees with ISO/IEC 16023:2000)
        /*  1*/ { -1, -1, "1", 139, "", ZINT_ERROR_TOO_LONG, -1, -1 },
        /*  2*/ { -1, -1, "A", 93, "", 0, 33, 30 },
        /*  3*/ { -1, -1, "A", 94, "", ZINT_ERROR_TOO_LONG, -1, -1 },
        /*  4*/ { -1, -1, "\001", 91, "", 0, 33, 30 },
        /*  5*/ { -1, -1, "\001", 92, "", ZINT_ERROR_TOO_LONG, -1, -1 },
        /*  6*/ { -1, -1, "\200", 91, "", 0, 33, 30 },
        /*  7*/ { -1, -1, "\200", 92, "", ZINT_ERROR_TOO_LONG, -1, -1 },
        /*  8*/ { 2, -1, "1", 126, "123456789123123", 0, 33, 30 },
        /*  9*/ { 2, -1, "1", 127, "123456789123123", ZINT_ERROR_TOO_LONG, -1, -1 },
        /* 10*/ { 2, -1, "A", 84, "123456789123123", 0, 33, 30 },
        /* 11*/ { 2, -1, "A", 85, "123456789123123", ZINT_ERROR_TOO_LONG, -1, -1 },
        /* 12*/ { 2, 96, "1", 109, "123456789123123", 0, 33, 30 },
        /* 13*/ { 2, 96, "1", 110, "123456789123123", ZINT_ERROR_TOO_LONG, -1, -1 },
        /* 14*/ { 3, -1, "1", 126, "ABCDEF123123", 0, 33, 30 },
        /* 15*/ { 3, -1, "1", 127, "ABCDEF123123", ZINT_ERROR_TOO_LONG, -1, -1 },
        /* 16*/ { 3, -1, "A", 84, "ABCDEF123123", 0, 33, 30 },
        /* 17*/ { 3, -1, "A", 85, "ABCDEF123123", ZINT_ERROR_TOO_LONG, -1, -1 },
        /* 18*/ { 3, 96, "1", 109, "ABCDEF123123", 0, 33, 30 },
        /* 19*/ { 3, 96, "1", 110, "ABCDEF123123", ZINT_ERROR_TOO_LONG, -1, -1 },
        /* 20*/ { 0, -1, "1", 126, "123456789123123", 0, 33, 30 }, // Mode 2
        /* 21*/ { 0, -1, "1", 127, "123456789123123", ZINT_ERROR_TOO_LONG, -1, -1 },
        /* 22*/ { 0, -1, "1", 126, "ABCDEF123123", 0, 33, 30 }, // Mode 3
        /* 23*/ { 0, -1, "1", 127, "ABCDEF123123", ZINT_ERROR_TOO_LONG, -1, -1 },
        /* 24*/ { 5, -1, "1", 113, "", 0, 33, 30 }, // Extra EEC
        /* 25*/ { 5, -1, "1", 114, "", ZINT_ERROR_TOO_LONG, -1, -1 },
        /* 26*/ { 5, -1, "A", 77, "", 0, 33, 30 },
        /* 27*/ { 5, -1, "A", 78, "", ZINT_ERROR_TOO_LONG, -1, -1 },
        /* 28*/ { 6, -1, "1", 138, "", 0, 33, 30 },
        /* 29*/ { 6, -1, "1", 139, "", ZINT_ERROR_TOO_LONG, -1, -1 },
        /* 30*/ { 6, -1, "A", 93, "", 0, 33, 30 },
        /* 31*/ { 6, -1, "A", 94, "", ZINT_ERROR_TOO_LONG, -1, -1 },
    };
    int data_size = ARRAY_SIZE(data);

    char data_buf[256];

    for (int i = 0; i < data_size; i++) {

        if (index != -1 && i != index) continue;

        struct zint_symbol *symbol = ZBarcode_Create();
        assert_nonnull(symbol, "Symbol not created\n");

        testUtilStrCpyRepeat(data_buf, data[i].pattern, data[i].length);
        assert_equal(data[i].length, (int) strlen(data_buf), "i:%d length %d != strlen(data_buf) %d\n", i, data[i].length, (int) strlen(data_buf));

        int length = testUtilSetSymbol(symbol, BARCODE_MAXICODE, -1 /*input_mode*/, -1 /*eci*/, data[i].option_1, data[i].option_2, -1, -1 /*output_options*/, data_buf, data[i].length, debug);
        strcpy(symbol->primary, data[i].primary);

        ret = ZBarcode_Encode(symbol, (unsigned char *) data_buf, length);
        assert_equal(ret, data[i].ret, "i:%d ZBarcode_Encode ret %d != %d (%s)\n", i, ret, data[i].ret, symbol->errtxt);

        if (ret < ZINT_ERROR) {
            assert_equal(symbol->rows, data[i].expected_rows, "i:%d symbol->rows %d != %d\n", i, symbol->rows, data[i].expected_rows);
            assert_equal(symbol->width, data[i].expected_width, "i:%d symbol->width %d != %d\n", i, symbol->width, data[i].expected_width);
        }

        ZBarcode_Delete(symbol);
    }

    testFinish();
}

static void test_input(int index, int generate, int debug) {

    testStart("");

    int ret;
    struct item {
        int input_mode;
        int eci;
        int option_1;
        int option_2;
        char *data;
        int length;
        char *primary;
        int ret;
        int expected_width;
        char *expected;
        char *comment;
    };
    struct item data[] = {
        /*  0*/ { UNICODE_MODE, -1, -1, -1, "A", -1, "", 0, 30, "(144) 04 01 21 21 21 21 21 21 21 21 08 0E 19 2B 20 0C 24 06 32 1C 21 21 21 21 21 21 21 21", "" },
        /*  1*/ { UNICODE_MODE, -1, 2, -1, "A", -1, "", ZINT_ERROR_INVALID_DATA, 0, "Error 551: Invalid length for Primary Message", "" },
        /*  2*/ { UNICODE_MODE, -1, 2, -1, "A", -1, "A123456", ZINT_ERROR_INVALID_DATA, 0, "Error 555: Non-numeric postcode in Primary Message", "" },
        /*  3*/ { UNICODE_MODE, -1, 2, -1, "A", -1, "1123456", 0, 30, "(144) 12 00 00 00 00 10 30 1E 20 1C 1A 3D 1C 0D 1B 15 3C 17 3C 08 01 21 21 21 21 21 21 21", "1-digit postcode" },
        /*  4*/ { UNICODE_MODE, -1, 2, -1, "A", -1, "123456789123456", 0, 30, "(144) 12 05 0D 2F 35 11 32 1E 20 1C 0D 1D 3B 12 22 3F 30 14 23 1A 01 21 21 21 21 21 21 21", "9-digit postcode" },
        /*  5*/ { UNICODE_MODE, -1, 2, -1, "A", -1, "1234567890123456", ZINT_ERROR_INVALID_DATA, 0, "Error 551: Invalid length for Primary Message", "10-digit postcode" },
        /*  6*/ { UNICODE_MODE, -1, -1, -1, "A", -1, "1123456", 0, 30, "(144) 12 00 00 00 00 10 30 1E 20 1C 1A 3D 1C 0D 1B 15 3C 17 3C 08 01 21 21 21 21 21 21 21", "1-digit postcode" },
        /*  7*/ { UNICODE_MODE, -1, -1, -1, "A", -1, "123456789123456", 0, 30, "(144) 12 05 0D 2F 35 11 32 1E 20 1C 0D 1D 3B 12 22 3F 30 14 23 1A 01 21 21 21 21 21 21 21", "9-digit postcode" },
        /*  8*/ { UNICODE_MODE, -1, -1, -1, "A", -1, "1234567890123456", ZINT_ERROR_INVALID_DATA, 0, "Error 551: Invalid length for Primary Message", "10-digit postcode" },
        /*  9*/ { UNICODE_MODE, -1, -1, -1, "A", -1, "123456", ZINT_ERROR_INVALID_DATA, 0, "Error 551: Invalid length for Primary Message", "0-digit postcode" },
        /* 10*/ { UNICODE_MODE, -1, -1, -1, "A", -1, "12345678123456", 0, 30, "(144) 22 13 21 31 0B 00 32 1E 20 1C 04 14 07 30 10 07 08 28 1D 09 01 21 21 21 21 21 21 21", "8-digit postcode" },
        /* 11*/ { UNICODE_MODE, -1, 3, -1, "A", -1, "", ZINT_ERROR_INVALID_DATA, 0, "Error 551: Invalid length for Primary Message", "" },
        /* 12*/ { UNICODE_MODE, -1, 3, -1, "A", -1, "A123456", 0, 30, "(144) 03 08 08 08 08 18 30 1E 20 1C 22 35 1C 0F 02 1A 26 04 10 31 01 21 21 21 21 21 21 21", "1-alphanumeric postcode" },
        /* 13*/ { UNICODE_MODE, -1, 3, -1, "A", -1, "1123456", 0, 30, "(144) 03 08 08 08 08 18 3C 1E 20 1C 13 37 07 2C 26 2D 18 29 3F 2C 01 21 21 21 21 21 21 21", "1-digit postcode" },
        /* 14*/ { UNICODE_MODE, -1, -1, -1, "A", -1, "A123456", 0, 30, "(144) 03 08 08 08 08 18 30 1E 20 1C 22 35 1C 0F 02 1A 26 04 10 31 01 21 21 21 21 21 21 21", "1-alphanumeric postcode" },
        /* 15*/ { UNICODE_MODE, -1, -1, -1, "A", -1, "ABCDEF123456", 0, 30, "(144) 23 11 01 31 20 10 30 1E 20 1C 3C 1D 22 03 19 15 0F 20 0F 2A 01 21 21 21 21 21 21 21", "6-alphanumeric postcode" },
        /* 16*/ { UNICODE_MODE, -1, -1, -1, "A", -1, "ABCDEFG123456", 0, 30, "(144) 23 11 01 31 20 10 30 1E 20 1C 3C 1D 22 03 19 15 0F 20 0F 2A 01 21 21 21 21 21 21 21", "7-alphanumeric postcode truncated" },
        /* 17*/ { UNICODE_MODE, -1, -1, -1, "A", -1, "ABCDE123456", 0, 30, "(144) 03 18 01 31 20 10 30 1E 20 1C 0F 38 38 1A 39 10 2F 37 22 12 01 21 21 21 21 21 21 21", "5-alphanumeric postcode" },
        /* 18*/ { UNICODE_MODE, -1, -1, -1, "A", -1, "AAAAAA   840001", 0, 30, "(144) 13 10 10 10 10 10 00 12 07 00 17 36 2E 38 04 29 16 0D 27 16 01 21 21 21 21 21 21 21", "6-alphanumeric postcode with padding" },
        /* 19*/ { UNICODE_MODE, -1, -1, -1, "A", -1, "AAAAA A840001", 0, 30, "(144) 03 18 10 10 10 10 00 12 07 00 19 07 29 31 26 01 23 2C 2E 07 01 21 21 21 21 21 21 21", "7-alphanumeric with embedded padding truncated" },
        /* 20*/ { UNICODE_MODE, -1, -1, -1, "A", -1, "AA\015AAA840001", ZINT_ERROR_INVALID_DATA, 0, "Error 556: Invalid characters in postcode in Primary Message", "Alphanumeric postcode with CR" },
        /* 21*/ { UNICODE_MODE, -1, -1, -1, "A", -1, "A#%-/A840001", 0, 30, "(144) 13 30 1B 1B 39 18 00 12 07 00 3F 1E 25 07 2A 1E 14 3C 28 2D 01 21 21 21 21 21 21 21", "Alphanumeric postcode with non-control Code A chars" },
        /* 22*/ { UNICODE_MODE, -1, -1, -1, "A", -1, "1A23456", ZINT_ERROR_INVALID_DATA, 0, "Error 552: Non-numeric country code or service class in Primary Message", "Non-numeric country code" },
        /* 23*/ { UNICODE_MODE, -1, -1, -1, "A", -1, "12345678912345A", ZINT_ERROR_INVALID_DATA, 0, "Error 552: Non-numeric country code or service class in Primary Message", "Non-numeric service class" },
        /* 24*/ { UNICODE_MODE, -1, 0, -1, "A", -1, "123456789123456", 0, 30, "(144) 12 05 0D 2F 35 11 32 1E 20 1C 0D 1D 3B 12 22 3F 30 14 23 1A 01 21 21 21 21 21 21 21", "Auto-determine mode 2" },
        /* 25*/ { UNICODE_MODE, -1, 0, -1, "A", -1, "", ZINT_ERROR_INVALID_DATA, 0, "Error 554: Primary Message empty", "Auto-determine mode 2/3 requires primary message" },
        /* 26*/ { UNICODE_MODE, -1, 0, -1, "A", -1, "A23456123456", 0, 30, "(144) 23 1D 0D 3D 2C 1C 30 1E 20 1C 24 35 30 31 2A 0D 17 14 16 3D 01 21 21 21 21 21 21 21", "Auto-determine mode 3" },
        /* 27*/ { UNICODE_MODE, -1, -1, 100, "A", -1, "123456123456", 0, 30, "(144) 02 10 22 07 00 20 31 1E 20 1C 0E 29 13 1B 0D 26 36 25 3B 22 3B 2A 29 3B 28 1E 30 31", "SCM prefix version" },
        /* 28*/ { UNICODE_MODE, -1, -1, 101, "A", -1, "123456123456", ZINT_ERROR_INVALID_OPTION, 0, "Error 557: Invalid SCM prefix version", "SCM prefix version" },
        /* 29*/ { UNICODE_MODE, 3, -1, -1, "A", -1, "", 0, 30, "(144) 04 1B 03 01 21 21 21 21 21 21 2F 14 23 21 05 24 27 00 24 0C 21 21 21 21 21 21 21 21", "" },
        /* 30*/ { UNICODE_MODE, 32, -1, -1, "A", -1, "", 0, 30, "(144) 04 1B 20 20 01 21 21 21 21 21 3D 15 0F 30 0D 22 24 35 22 06 21 21 21 21 21 21 21 21", "" },
        /* 31*/ { UNICODE_MODE, 1024, -1, -1, "A", -1, "", 0, 30, "(144) 04 1B 30 10 00 01 21 21 21 21 11 2F 15 10 1D 29 06 35 14 2B 21 21 21 21 21 21 21 21", "" },
        /* 32*/ { UNICODE_MODE, 32768, -1, -1, "A", -1, "", 0, 30, "(144) 04 1B 38 08 00 00 01 21 21 21 10 30 3A 04 26 23 0E 21 3D 0F 21 21 21 21 21 21 21 21", "" },
        /* 33*/ { UNICODE_MODE, -1, 1, -1, "A", -1, "", ZINT_ERROR_INVALID_OPTION, 0, "Error 550: Invalid MaxiCode Mode", "" },
    };
    int data_size = ARRAY_SIZE(data);

    char escaped[1024];

    for (int i = 0; i < data_size; i++) {

        if (index != -1 && i != index) continue;

        struct zint_symbol *symbol = ZBarcode_Create();
        assert_nonnull(symbol, "Symbol not created\n");

        symbol->debug = ZINT_DEBUG_TEST; // Needed to get codeword dump in errtxt

        int length = testUtilSetSymbol(symbol, BARCODE_MAXICODE, data[i].input_mode, data[i].eci, data[i].option_1, data[i].option_2, -1, -1 /*output_options*/, data[i].data, data[i].length, debug);
        strcpy(symbol->primary, data[i].primary);

        ret = ZBarcode_Encode(symbol, (unsigned char *) data[i].data, length);
        assert_equal(ret, data[i].ret, "i:%d ZBarcode_Encode ret %d != %d (%s)\n", i, ret, data[i].ret, symbol->errtxt);

        if (generate) {
            printf("        /*%3d*/ { %s, %d, %d, %d, \"%s\", %d, \"%s\", %s, %d, \"%s\", \"%s\" },\n",
                    i, testUtilInputModeName(data[i].input_mode), data[i].eci, data[i].option_1, data[i].option_2,
                    testUtilEscape(data[i].data, length, escaped, sizeof(escaped)), data[i].length, data[i].primary,
                    testUtilErrorName(data[i].ret), symbol->width, symbol->errtxt, data[i].comment);
        } else {
            if (ret < ZINT_ERROR) {
                assert_equal(symbol->width, data[i].expected_width, "i:%d symbol->width %d != %d (%s)\n", i, symbol->width, data[i].expected_width, data[i].data);
            }
            assert_zero(strcmp(symbol->errtxt, data[i].expected), "i:%d strcmp(%s, %s) != 0\n", i, symbol->errtxt, data[i].expected);
        }

        ZBarcode_Delete(symbol);
    }

    testFinish();
}

static void test_encode(int index, int generate, int debug) {

    testStart("");

    int ret;
    struct item {
        int input_mode;
        int option_1;
        int option_2;
        char *data;
        int length;
        char* primary;
        int ret;

        int expected_rows;
        int expected_width;
        char *comment;
        char *expected;
    };
    struct item data[] = {
        /*  0*/ { -1, -1, -1, "THIS IS A 93 CHARACTER CODE SET A MESSAGE THAT FILLS A MODE 4, UNAPPENDED, MAXICODE SYMBOL...", -1, "", 0, 33, 30, "ISO/IEC 16023:2000 Figure 2, same",
                    "011111010000001000001000100111"
                    "000100000001000000001010000000"
                    "001011001100100110110010010010"
                    "100000010001100010010000000000"
                    "001011000000101000001010110011"
                    "111010001000001011001000111100"
                    "100000000110000010010000000000"
                    "000010100010010010001001111100"
                    "111011100000001000000110000000"
                    "000000011011000000010100011000"
                    "101111000001010110001100000011"
                    "001110001010000000111010001110"
                    "000111100000000000100001011000"
                    "100010000000000000000111001000"
                    "100000001000000000011000001000"
                    "000010111000000000000010000010"
                    "111000001000000000001000001101"
                    "011000000000000000001000100100"
                    "000000101100000000001001010001"
                    "101010001000000000100111001100"
                    "001000011000000000011100001010"
                    "000000000000000000110000100000"
                    "101011001010100001000101010001"
                    "100011110010101001101010001010"
                    "011010000000000101011010011111"
                    "000001110011111111111100010100"
                    "001110100111000101011000011100"
                    "110111011100100001101001010110"
                    "000001011011101010010111001100"
                    "111000110111100010001111011110"
                    "101111010111111000010110111001"
                    "001001101111101101101010011100"
                    "001011000000111101100100001000"
                },
        /*  1*/ { -1, 4, -1, "MaxiCode (19 chars)", -1, "", 0, 33, 30, "ISO/IEC 16023:2000 Figure H1 **NOT SAME** different encodation (figure uses '3 Shift A' among other differences)",
                    "001101011111011100000010101111"
                    "101100010001001100010000001100"
                    "101100001010001111001001111101"
                    "010101010101010101010101010100"
                    "000000000000000000000000000111"
                    "101010101010101010101010101000"
                    "010101010101010101010101010111"
                    "000000000000000000000000000010"
                    "101010101010101010101010101000"
                    "010101011111111100000001010100"
                    "000000000011110110001000000000"
                    "101010101110000000111010101000"
                    "010101100010000000001101010101"
                    "000000101000000000001000000000"
                    "101010000000000000011010101000"
                    "010101010000000000001101010100"
                    "000000001000000000001000000011"
                    "101010110000000000001010101010"
                    "010101101100000000010101010111"
                    "000000100000000000000000000000"
                    "101010010110000000000110101011"
                    "010101010110000000001001010100"
                    "000000000110001011000000000010"
                    "101010100110111001010010101000"
                    "010101010101010101010000101111"
                    "000000000000000000001100100000"
                    "101010101010101010100101000001"
                    "000011000111010110101100010000"
                    "111001111110111110011000111111"
                    "000001110010000010110001100100"
                    "000111000000001111011000010010"
                    "010110010110001110100000010100"
                    "010011110011000001010111100111"
                },
        /*  2*/ { DATA_MODE | ESCAPE_MODE, 2, 96, "1Z00004951\\GUPSN\\G06X610\\G159\\G1234567\\G1/1\\G\\GY\\G634 ALPHA DR\\GPITTSBURGH\\GPA\\R\\E", -1, "152382802840001", 0, 33, 30, "ISO/IEC 16023:2000 Figure B2 **NOT SAME** uses different encodation (figure uses Latch B/Latch A instead of Shift B for '>\\R', and precedes PAD chars with Latch B)",
                    "110101110110111110111111101111"
                    "010101010111000011011000010010"
                    "110110110001001010101010010011"
                    "111000101010101111111111111100"
                    "001111000010110010011000000011"
                    "001001110010101010100000000000"
                    "111011111110111111101111111110"
                    "100110000011001001110000001010"
                    "010001100010101010101001110001"
                    "110111100011010000011011111100"
                    "001100110011110000001110101001"
                    "101110101000000001011111011000"
                    "101010000000000000010110111100"
                    "111101100000000000011011100010"
                    "101010010000000000000110011101"
                    "001000010000000000011100011110"
                    "010011001000000000001000001010"
                    "000000101000000000001010000010"
                    "000100111100000000001110101010"
                    "000010101100000000001000110010"
                    "100000111010000000011101100011"
                    "101000100000000000110110100000"
                    "001000001110100101100110100101"
                    "011001110010101001100000001000"
                    "000010100010110001010101011010"
                    "100111000011111000001001011000"
                    "110010001001010010101100011101"
                    "001001110101110100011001110010"
                    "011111010011101100111101010011"
                    "111111101111101010101101111000"
                    "101001110101110111010111000011"
                    "010110101101000001111000100110"
                    "110110100000010000001011110011"
                },
        /*  3*/ { -1, 3, -1, "CEN", -1, "B1050056999", 0, 33, 30, "ISO/IEC 16023:2000 B.1 Example (primary only given, data arbitrary); verified manually against BWIP and tec-it",
                    "000000010101010101010101010111"
                    "001011000000000000000000000010"
                    "111001101010101010101010101000"
                    "010101010101010101010101010110"
                    "000000000000000000000000000001"
                    "101010101010101010101010101010"
                    "010101010101010101010101010100"
                    "000000000000000000000000000010"
                    "101010101010101010101010101000"
                    "010101010111100000100101010110"
                    "000000000001110000010100000010"
                    "101010101000000011010010101010"
                    "010101111100000000100001010110"
                    "000000001100000000011000000010"
                    "101010100100000000011110101001"
                    "010101011000000000000001010110"
                    "000000101000000000001000000001"
                    "101010110000000000001010101010"
                    "010101010000000000011101010101"
                    "000000101000000000011100000000"
                    "101010111100000000001010101001"
                    "010101011110000000011101010110"
                    "000000001110000001111000000011"
                    "101010101011011001000110101010"
                    "010101010101010101011100100010"
                    "000000000000000000000011011100"
                    "101010101010101010101001110100"
                    "111011101110000110101011010110"
                    "100001011111111101000011100111"
                    "110100001000001101100010100110"
                    "110110111111011110000011011111"
                    "010010001001110010000101000010"
                    "010001011010000011010010011100"
                },
        /*  4*/ { UNICODE_MODE | ESCAPE_MODE, -1, -1, "Comité Européen de Normalisation\034rue de Stassart 36\034B-1050 BRUXELLES\034TEL +3225196811", -1, "", 0, 33, 30, "ISO/IEC 16023:2000 Example F.5 **NOT SAME** uses different encodation (2 Shift A among other things)",
                    "010010100010110000000100001111"
                    "001010001100110110111110100110"
                    "001010011100101010011100100000"
                    "000000100010000000001000000110"
                    "111101100000011100110011110001"
                    "011110001011100010100111010010"
                    "101010000100001101101000101001"
                    "110010101110100100001000000000"
                    "000110101100100000110010111110"
                    "111101111111111000110110100000"
                    "100000111001111010010010000011"
                    "011100111100000000101100011010"
                    "100001101110000000101111111101"
                    "110011000100000000111100001010"
                    "000110010000000000010110001010"
                    "101010010000000000001000011100"
                    "011000001000000000001000000010"
                    "001001010000000000001101000000"
                    "000000010000000000010100101000"
                    "101111110100000000011110001100"
                    "100000000010000000011010110011"
                    "101001010010000001011100001010"
                    "001101000010001011110111101010"
                    "111111001010000001100100100000"
                    "001000001101010101010010111001"
                    "111001000000000000001010100000"
                    "010001101010101010101010110001"
                    "011011000011100001011001101100"
                    "101100000001111010000001100011"
                    "110001001100011100110111011010"
                    "011110010010101101110100000100"
                    "001011110011100001001001101100"
                    "000010111011111010110011000011"
                },
        /*  5*/ { -1, -1, -1, "999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999", -1, "", 0, 33, 30, "Numeric compaction, verified manually against BWIPP and tec-it",
                    "010111101101010111101101010111"
                    "111011110110111011110110111010"
                    "001111111101001111111101001100"
                    "101101010111101101010111101100"
                    "111110111001111110111001111100"
                    "111111000111111111000111111110"
                    "110101011110110101011110110110"
                    "011011101111011011101111011000"
                    "110100111111110100111111110101"
                    "010111101111111100110111010100"
                    "111001111011011110011111101100"
                    "000111111110000000111011110000"
                    "011110100110000000000111010101"
                    "101111001000000000110101101110"
                    "111111110100000000000011010001"
                    "010111111000000000001110110110"
                    "111001101000000000001011111001"
                    "000111010000000000001011111100"
                    "011110011000000000011011010101"
                    "101111000100000000010001101100"
                    "111111100110000000100111010010"
                    "010101110100000001010110110110"
                    "101110011010101111111011111011"
                    "110001110111110101111011111110"
                    "111011010101111111110000111011"
                    "111101101110110101010001001000"
                    "111111010011111010101111110011"
                    "000101011000111100010001000010"
                    "011110001101100001011010110010"
                    "101110000100010011000001001000"
                    "100000001010110100100110001100"
                    "111010101011001101111001011010"
                    "011110011111000011101011111011"
                },
        /*  6*/ { -1, 5, -1, "\000\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017\020\021\022\023\024\025\026\027\030\031\032\033\037\237\240\242\243\244\245\246\247\251\255\256\266\225\226\227\230\231\232\233\234\235\236", 51, "", 0, 33, 30, "Mode 5 set E; single difference from BWIPP - Zint uses Latch B at end before padding instead of Latch A",
                    "000000000000000000101010101011"
                    "100101010111111111000000001000"
                    "110010011100100111001001110001"
                    "010101010101011010101010101000"
                    "010110101010001101010110101000"
                    "100011011000110101100011011000"
                    "010101010101111111111111111110"
                    "010111111111000000001010101010"
                    "011100100111001001110010011101"
                    "010101011011110000001011111100"
                    "000000001111110010010011010100"
                    "101010100110000010001011100000"
                    "010101110010000000001101010100"
                    "000000111000000000001000000010"
                    "101010110100000000001110101011"
                    "010101010000000000001001010100"
                    "000000001000000000001000000001"
                    "101010100000000000001010101010"
                    "010101100000000000001001010100"
                    "000000000000000000000000000000"
                    "101010100100000000011110101010"
                    "101100110100000001110101100110"
                    "011100010010110101110111010011"
                    "110011100010110001001000101010"
                    "110001101111100011111110101000"
                    "111110010101110100010110100000"
                    "110001110000111101111111000011"
                    "111001000100001011001011011110"
                    "101010110110100001110011010011"
                    "100100100001001100000001100000"
                    "010101001001000001111101011111"
                    "110110111000001000001101100100"
                    "100100010010000000010001010111"
                },
        /*  7*/ { -1, 6, -1, "\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377\241\250\253\257\260\264\267\270\273\277\212\213\214\215\216\217\220\221\222\223\224", -1, "", 0, 33, 30, "Mode 6 set D; single difference from BWIPP as above - Zint uses Latch B at end before padding instead of Latch A",
                    "000000000000000000101010101011"
                    "100101010111111111000000001000"
                    "110010011100100111001001110001"
                    "010101010101010110101010101000"
                    "101010100000000001010110101000"
                    "110110001101100001100011011000"
                    "010101010101010111111111111111"
                    "010101011111111100000000101000"
                    "001001110010011100100111001001"
                    "010111110111110000000011111100"
                    "000011011011110010011101101000"
                    "101011101110000001000100110100"
                    "010101110000000000000001010100"
                    "000000101000000000000000000010"
                    "101010110100000000000110101010"
                    "010101010000000000001001010100"
                    "000000001000000000001000000011"
                    "101010100000000000001010101000"
                    "010101101100000000001001010110"
                    "000000101000000000001000000010"
                    "101010010110000000011010101001"
                    "010101011100000000111101010100"
                    "000000001110110111011000000011"
                    "101010100110110001100110101010"
                    "010101010101010101011101101100"
                    "000000000000000000001000010000"
                    "101010101010101010100110000001"
                    "101000111111101000101010110100"
                    "010100110011011001101100111001"
                    "100101010011111101011110000010"
                    "111101110110111101001001010101"
                    "011110000010110111001011110110"
                    "001110010110111101101011110010"
                },
        /*  8*/ { -1, 6, -1, "\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337\252\254\261\262\263\265\271\272\274\275\276\200\201\202\203\204\205\206\207\210\211", -1, "", 0, 33, 30, "Mode 6 set C; single difference from BWIPP as above - Zint uses Latch B at end before padding instead of Latch A",
                    "000000000000000000101010101011"
                    "100101010111111111000000001000"
                    "110010011100100111001001110001"
                    "010101010101010110101010101000"
                    "101010100000000001010110101000"
                    "110110001101100001100011011000"
                    "010101010101010111111111111111"
                    "010101011111111100000000101000"
                    "001001110010011100100111001001"
                    "010111111111110000000111111100"
                    "000011010111110010010001101000"
                    "101011101010000001001100110100"
                    "010101100000000000000101010100"
                    "000000011000000000000100000010"
                    "101010110100000000000010101010"
                    "010101010000000000001001010100"
                    "000000001000000000001000000011"
                    "101010100000000000001010101000"
                    "010101110000000000001001010110"
                    "000000111100000000001100000010"
                    "101010110000000000010110101001"
                    "010101010100000000101101010100"
                    "000000001010110101011000000011"
                    "101010100010110001010110101010"
                    "010101010101010101011101101100"
                    "000000000000000000001000010000"
                    "101010101010101010100110000001"
                    "101000111111101000101010110100"
                    "010100110011011001101100111001"
                    "100101010011111101011110000010"
                    "111101110110111101001001010101"
                    "011110000010110111001011110110"
                    "001110010110111101101011110010"
                },
    };
    int data_size = ARRAY_SIZE(data);

    char escaped[1024];

    for (int i = 0; i < data_size; i++) {

        if (index != -1 && i != index) continue;

        struct zint_symbol *symbol = ZBarcode_Create();
        assert_nonnull(symbol, "Symbol not created\n");

        int length = testUtilSetSymbol(symbol, BARCODE_MAXICODE, data[i].input_mode, -1 /*eci*/, data[i].option_1, data[i].option_2, -1, -1 /*output_options*/, data[i].data, data[i].length, debug);
        strcpy(symbol->primary, data[i].primary);

        ret = ZBarcode_Encode(symbol, (unsigned char *) data[i].data, length);
        assert_equal(ret, data[i].ret, "i:%d ZBarcode_Encode ret %d != %d (%s)\n", i, ret, data[i].ret, symbol->errtxt);

        if (generate) {
            printf("        /*%3d*/ { %s, %d, %d, \"%s\", %d, \"%s\", %s, %d, %d, \"%s\",\n",
                    i, testUtilInputModeName(data[i].input_mode), data[i].option_1, data[i].option_2, testUtilEscape(data[i].data, length, escaped, sizeof(escaped)), data[i].length,
                    data[i].primary, testUtilErrorName(data[i].ret), symbol->rows, symbol->width, data[i].comment);
            testUtilModulesPrint(symbol, "                    ", "\n");
            printf("                },\n");
        } else {
            if (ret < ZINT_ERROR) {
                assert_equal(symbol->rows, data[i].expected_rows, "i:%d symbol->rows %d != %d (%s)\n", i, symbol->rows, data[i].expected_rows, data[i].data);
                assert_equal(symbol->width, data[i].expected_width, "i:%d symbol->width %d != %d (%s)\n", i, symbol->width, data[i].expected_width, data[i].data);

                int width, row;
                ret = testUtilModulesCmp(symbol, data[i].expected, &width, &row);
                assert_zero(ret, "i:%d testUtilModulesCmp ret %d != 0 width %d row %d (%s)\n", i, ret, width, row, data[i].data);
            }
        }

        ZBarcode_Delete(symbol);
    }

    testFinish();
}

static void test_best_supported_set(int index, int generate, int debug) {

    testStart("");

    int ret;
    struct item {
        char *data;
        int ret;
        float w;
        float h;
        int ret_vector;

        int expected_rows;
        int expected_width;
        char *comment;
        char *expected;
    };
    struct item data[] = {
        /* 0*/ { "am.//ab,\034TA# z\015!", 0, 100, 100, 0, 33, 30, "Different encodation than BWIPP, same number of codewords",
                    "111010000101111000111101010111"
                    "111110000000010100111000000000"
                    "110000101100110100111010101011"
                    "010101010101010101010101010100"
                    "000000000000000000000000000000"
                    "101010101010101010101010101000"
                    "010101010101010101010101010110"
                    "000000000000000000000000000000"
                    "101010101010101010101010101011"
                    "010101010111001100000101010110"
                    "000000001011000010000000000010"
                    "101010101100000000100110101010"
                    "010101001100000000101101010101"
                    "000000100000000000010000000010"
                    "101010110000000000010010101010"
                    "010101011000000000000101010110"
                    "000000001000000000001000000010"
                    "101010001000000000001010101000"
                    "010101010000000000001101010101"
                    "000000001100000000000000000010"
                    "101010110010000000010110101010"
                    "010101010100000001111001010100"
                    "000000001110110111111100000011"
                    "101010100110111101011010101010"
                    "010101010101010101010011101000"
                    "000000000000000000001101100000"
                    "101010101010101010100000100110"
                    "101001001101110001001011010000"
                    "100100110110001010011000011100"
                    "011011000001011011100100100110"
                    "111001100000101101000111001000"
                    "111100000110000011011101001110"
                    "010100101001110111101010110010"
                },
    };
    int data_size = ARRAY_SIZE(data);

    char escaped_data[1024];

    for (int i = 0; i < data_size; i++) {

        if (index != -1 && i != index) continue;

        struct zint_symbol *symbol = ZBarcode_Create();
        assert_nonnull(symbol, "Symbol not created\n");

        int length = testUtilSetSymbol(symbol, BARCODE_MAXICODE, -1 /*input_mode*/, -1 /*eci*/, -1 /*option_1*/, -1, -1, -1 /*output_options*/, data[i].data, -1, debug);

        ret = ZBarcode_Encode(symbol, (unsigned char *) data[i].data, length);
        assert_equal(ret, data[i].ret, "i:%d ZBarcode_Encode ret %d != %d\n", i, ret, data[i].ret);

        if (generate) {
            printf("        /*%2d*/ { \"%s\", %d, %.0f, %.0f, %d, %d, %d, \"%s\",\n",
                    i, testUtilEscape(data[i].data, length, escaped_data, sizeof(escaped_data)), ret,
                    data[i].w, data[i].h, data[i].ret_vector, symbol->rows, symbol->width, data[i].comment);
            testUtilModulesPrint(symbol, "                    ",  "\n");
            printf("                },\n");
        } else {
            assert_equal(symbol->rows, data[i].expected_rows, "i:%d symbol->rows %d != %d\n", i, symbol->rows, data[i].expected_rows);
            assert_equal(symbol->width, data[i].expected_width, "i:%d symbol->width %d != %d\n", i, symbol->width, data[i].expected_width);

            int width, row;
            ret = testUtilModulesCmp(symbol, data[i].expected, &width, &row);
            assert_zero(ret, "i:%d testUtilModulesCmp ret %d != 0 width %d row %d\n", i, ret, width, row);

            ret = ZBarcode_Buffer_Vector(symbol, 0);
            assert_equal(ret, data[i].ret_vector, "i:%d ZBarcode_Buffer_Vector ret %d != %d\n", i, ret, data[i].ret_vector);
        }

        ZBarcode_Delete(symbol);
    }

    testFinish();
}

// #181 Nico Gunkel OSS-Fuzz
static void test_fuzz(int index, int debug) {

    testStart("");

    int ret;
    struct item {
        int eci;
        char *data;
        int length;
        int ret;
    };
    // s/\/\*[ 0-9]*\*\//\=printf("\/*%2d*\/", line(".") - line("'<"))
    struct item data[] = {
        /* 0*/ { -1, "\223\223\223\223\223\200\000\060\060\020\122\104\060\343\000\000\040\104\104\104\104\177\377\040\000\324\336\000\000\000\000\104\060\060\060\060\104\104\104\104\104\104\104\104\104\104\104\104\104\104\104\104\104\104\104\104\104\104\104\104\104\104\104\060\104\104\000\000\000\040\104\104\104\104\177\377\377\377\324\336\000\000\000\000\104\377\104\001\104\104\104\104\104\104\233\233\060\060\060\060\060\060\060\060\060\325\074", 107, ZINT_ERROR_TOO_LONG }, // Original OSS-Fuzz triggering data
        /* 1*/ { -1, "AaAaAaAaAaAaAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA123456789", -1, ZINT_ERROR_TOO_LONG }, // Add 6 lowercase a's so 6 SHIFTS inserted so 6 + 138 (max input len) = 144 and numbers come at end of buffer
        /* 2*/ { -1, "AaAaAaAaAaAaAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA123456789A", -1, ZINT_ERROR_TOO_LONG },
        /* 3*/ { -1, "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789", -1, ZINT_ERROR_TOO_LONG },
        /* 4*/ { 32768, "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678", -1, ZINT_ERROR_TOO_LONG },
    };
    int data_size = ARRAY_SIZE(data);

    for (int i = 0; i < data_size; i++) {

        if (index != -1 && i != index) continue;

        struct zint_symbol *symbol = ZBarcode_Create();
        assert_nonnull(symbol, "Symbol not created\n");

        int length = testUtilSetSymbol(symbol, BARCODE_MAXICODE, -1 /*input_mode*/, data[i].eci, -1 /*option_1*/, -1, -1, -1 /*output_options*/, data[i].data, data[i].length, debug);

        ret = ZBarcode_Encode(symbol, (unsigned char *) data[i].data, length);
        assert_equal(ret, data[i].ret, "i:%d ZBarcode_Encode ret %d != %d (%s)\n", i, ret, data[i].ret, symbol->errtxt);

        ZBarcode_Delete(symbol);
    }

    testFinish();
}

#include <time.h>

#define TEST_PERF_ITERATIONS    1000

// Not a real test, just performance indicator
static void test_perf(int index, int debug) {

    if (!(debug & ZINT_DEBUG_TEST_PERFORMANCE)) { /* -d 256 */
        return;
    }

    int ret;
    struct item {
        int symbology;
        int input_mode;
        int option_1;
        int option_2;
        char *data;
        char *primary;
        int ret;

        int expected_rows;
        int expected_width;
        char *comment;
    };
    struct item data[] = {
        /*  0*/ { BARCODE_MAXICODE, UNICODE_MODE | ESCAPE_MODE, -1, -1,
                    "1Z34567890\\GUPSN\\G102562\\G034\\G\\G1/1\\G\\GY\\G2201 Second St\\GFt Myers\\GFL\\R\\E",
                    "339010000840001", 0, 33, 30, "Mode 2" },
    };
    int data_size = ARRAY_SIZE(data);

    clock_t start, total_encode = 0, total_buffer = 0, diff_encode, diff_buffer;

    for (int i = 0; i < data_size; i++) {

        if (index != -1 && i != index) continue;

        diff_encode = diff_buffer = 0;

        for (int j = 0; j < TEST_PERF_ITERATIONS; j++) {
            struct zint_symbol *symbol = ZBarcode_Create();
            assert_nonnull(symbol, "Symbol not created\n");

            int length = testUtilSetSymbol(symbol, data[i].symbology, data[i].input_mode, -1 /*eci*/, data[i].option_1, data[i].option_2, -1, -1 /*output_options*/, data[i].data, -1, debug);
            strcpy(symbol->primary, data[i].primary);

            start = clock();
            ret = ZBarcode_Encode(symbol, (unsigned char *) data[i].data, length);
            diff_encode += clock() - start;
            assert_equal(ret, data[i].ret, "i:%d ZBarcode_Encode ret %d != %d (%s)\n", i, ret, data[i].ret, symbol->errtxt);

            assert_equal(symbol->rows, data[i].expected_rows, "i:%d symbol->rows %d != %d (%s)\n", i, symbol->rows, data[i].expected_rows, data[i].data);
            assert_equal(symbol->width, data[i].expected_width, "i:%d symbol->width %d != %d (%s)\n", i, symbol->width, data[i].expected_width, data[i].data);

            start = clock();
            ret = ZBarcode_Buffer(symbol, 0 /*rotate_angle*/);
            diff_buffer += clock() - start;
            assert_zero(ret, "i:%d ZBarcode_Buffer ret %d != 0 (%s)\n", i, ret, symbol->errtxt);

            ZBarcode_Delete(symbol);
        }

        printf("%s: diff_encode %gms, diff_buffer %gms\n", data[i].comment, diff_encode * 1000.0 / CLOCKS_PER_SEC, diff_buffer * 1000.0 / CLOCKS_PER_SEC);

        total_encode += diff_encode;
        total_buffer += diff_buffer;
    }
    if (index != -1) {
        printf("totals: encode %gms, buffer %gms\n", total_encode * 1000.0 / CLOCKS_PER_SEC, total_buffer * 1000.0 / CLOCKS_PER_SEC);
    }
}

int main(int argc, char *argv[]) {

    testFunction funcs[] = { /* name, func, has_index, has_generate, has_debug */
        { "test_large", test_large, 1, 0, 1 },
        { "test_input", test_input, 1, 1, 1 },
        { "test_encode", test_encode, 1, 1, 1 },
        { "test_best_supported_set", test_best_supported_set, 1, 1, 1 },
        { "test_fuzz", test_fuzz, 1, 0, 1 },
        { "test_perf", test_perf, 1, 0, 1 },
    };

    testRun(argc, argv, funcs, ARRAY_SIZE(funcs));

    testReport();

    return 0;
}
