#include "lwip/apps/fs.h"
#include "lwip/def.h"


#define file_NULL (struct fsdata_file *) NULL


#ifndef FS_FILE_FLAGS_HEADER_INCLUDED
#define FS_FILE_FLAGS_HEADER_INCLUDED 1
#endif
#ifndef FS_FILE_FLAGS_HEADER_PERSISTENT
#define FS_FILE_FLAGS_HEADER_PERSISTENT 0
#endif
/* FSDATA_FILE_ALIGNMENT: 0=off, 1=by variable, 2=by include */
#ifndef FSDATA_FILE_ALIGNMENT
#define FSDATA_FILE_ALIGNMENT 0
#endif
#ifndef FSDATA_ALIGN_PRE
#define FSDATA_ALIGN_PRE
#endif
#ifndef FSDATA_ALIGN_POST
#define FSDATA_ALIGN_POST
#endif
#if FSDATA_FILE_ALIGNMENT==2
#include "fsdata_alignment.h"
#endif
#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__404_html = 0;
#endif
static const unsigned char FSDATA_ALIGN_PRE data__404_html[] FSDATA_ALIGN_POST = {
/* /404.html (10 chars) */
0x2f,0x34,0x30,0x34,0x2e,0x68,0x74,0x6d,0x6c,0x00,0x00,0x00,

/* HTTP header */
/* "HTTP/1.0 404 File not found
" (29 bytes) */
0x48,0x54,0x54,0x50,0x2f,0x31,0x2e,0x30,0x20,0x34,0x30,0x34,0x20,0x46,0x69,0x6c,
0x65,0x20,0x6e,0x6f,0x74,0x20,0x66,0x6f,0x75,0x6e,0x64,0x0d,0x0a,
/* "Server: lwIP/2.2.0d (http://savannah.nongnu.org/projects/lwip)
" (64 bytes) */
0x53,0x65,0x72,0x76,0x65,0x72,0x3a,0x20,0x6c,0x77,0x49,0x50,0x2f,0x32,0x2e,0x32,
0x2e,0x30,0x64,0x20,0x28,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x73,0x61,0x76,0x61,
0x6e,0x6e,0x61,0x68,0x2e,0x6e,0x6f,0x6e,0x67,0x6e,0x75,0x2e,0x6f,0x72,0x67,0x2f,
0x70,0x72,0x6f,0x6a,0x65,0x63,0x74,0x73,0x2f,0x6c,0x77,0x69,0x70,0x29,0x0d,0x0a,

/* "Content-Length: 242
" (18+ bytes) */
0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x4c,0x65,0x6e,0x67,0x74,0x68,0x3a,0x20,
0x32,0x34,0x32,0x0d,0x0a,
/* "Content-Encoding: deflate
" (27 bytes) */
0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x45,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,
0x3a,0x20,0x64,0x65,0x66,0x6c,0x61,0x74,0x65,0x0d,0x0a,
/* "Content-Type: text/html

" (27 bytes) */
0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x54,0x79,0x70,0x65,0x3a,0x20,0x74,0x65,
0x78,0x74,0x2f,0x68,0x74,0x6d,0x6c,0x0d,0x0a,0x0d,0x0a,
/* raw file data (242 bytes) */
0x5d,0x51,0xed,0x4e,0xc4,0x20,0x10,0xfc,0x7d,0x4d,0xfa,0x0e,0x1b,0x12,0xfd,0xa5,
0xd2,0x4b,0xce,0xc4,0x44,0xda,0x67,0x30,0xf1,0x09,0xe8,0xb1,0x16,0x22,0x42,0xa5,
0xdb,0xd6,0xbe,0xbd,0x14,0x7a,0x9f,0xfc,0x81,0xec,0xcc,0xee,0x0c,0xb3,0x42,0xd3,
0x8f,0x6d,0xca,0x42,0x68,0x94,0xaa,0x11,0x64,0xc8,0x62,0xf3,0x21,0x3b,0x84,0xd1,
0xc9,0x49,0x1a,0x2b,0x5b,0x8b,0x82,0xe7,0xba,0xe0,0x89,0x15,0xd9,0xad,0x57,0x0b,
0xb4,0xdd,0xd1,0x5b,0x1f,0x6a,0x36,0x6b,0x43,0xc8,0x80,0xf0,0x8f,0x6a,0xd6,0x5a,
0x79,0xfc,0x66,0x91,0x54,0x16,0x10,0x8f,0xa0,0x75,0x02,0xcc,0x46,0x91,0xae,0xd9,
0xbe,0xaa,0x1e,0x56,0x0c,0x20,0x63,0x01,0x26,0x69,0x4d,0xe7,0x6a,0x46,0xbe,0x67,
0x51,0x5f,0x9d,0x98,0x6f,0x15,0x6b,0x76,0x00,0x65,0xb1,0x8b,0xea,0xea,0x1a,0x79,
0xad,0xce,0x50,0x1c,0xa1,0xf7,0xcd,0xa1,0x3a,0xc0,0x33,0x24,0xd3,0xce,0x13,0x7c,
0xf9,0xd1,0x29,0xf0,0xe1,0xf6,0x07,0x91,0x97,0x1b,0xfa,0x7c,0x03,0x7c,0xfa,0x10,
0x96,0x27,0x20,0x8d,0xd0,0xaf,0xbd,0x8b,0x1f,0x41,0x06,0x84,0x80,0xbf,0x23,0x0e,
0x64,0x5c,0x07,0xb3,0x1c,0xae,0x47,0xba,0x48,0x36,0xc3,0xd6,0x3e,0x60,0x98,0x30,
0xdc,0xe9,0xbc,0x6c,0xae,0x78,0x52,0x39,0x39,0xcf,0x8a,0x8f,0xae,0x1d,0xfa,0xf7,
0x73,0x99,0x53,0xb8,0x04,0xc1,0x53,0x4a,0x6b,0xb2,0x7c,0x8d,0x36,0x3d,0xb6,0xcd,
0xfc,0x03,};

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__incorrect_command_html = 1;
#endif
static const unsigned char FSDATA_ALIGN_PRE data__incorrect_command_html[] FSDATA_ALIGN_POST = {
/* /incorrect_command.html (24 chars) */
0x2f,0x69,0x6e,0x63,0x6f,0x72,0x72,0x65,0x63,0x74,0x5f,0x63,0x6f,0x6d,0x6d,0x61,
0x6e,0x64,0x2e,0x68,0x74,0x6d,0x6c,0x00,

/* HTTP header */
/* "HTTP/1.0 200 OK
" (17 bytes) */
0x48,0x54,0x54,0x50,0x2f,0x31,0x2e,0x30,0x20,0x32,0x30,0x30,0x20,0x4f,0x4b,0x0d,
0x0a,
/* "Server: lwIP/2.2.0d (http://savannah.nongnu.org/projects/lwip)
" (64 bytes) */
0x53,0x65,0x72,0x76,0x65,0x72,0x3a,0x20,0x6c,0x77,0x49,0x50,0x2f,0x32,0x2e,0x32,
0x2e,0x30,0x64,0x20,0x28,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x73,0x61,0x76,0x61,
0x6e,0x6e,0x61,0x68,0x2e,0x6e,0x6f,0x6e,0x67,0x6e,0x75,0x2e,0x6f,0x72,0x67,0x2f,
0x70,0x72,0x6f,0x6a,0x65,0x63,0x74,0x73,0x2f,0x6c,0x77,0x69,0x70,0x29,0x0d,0x0a,

/* "Content-Length: 149
" (18+ bytes) */
0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x4c,0x65,0x6e,0x67,0x74,0x68,0x3a,0x20,
0x31,0x34,0x39,0x0d,0x0a,
/* "Content-Encoding: deflate
" (27 bytes) */
0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x45,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,
0x3a,0x20,0x64,0x65,0x66,0x6c,0x61,0x74,0x65,0x0d,0x0a,
/* "Content-Type: text/html

" (27 bytes) */
0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x54,0x79,0x70,0x65,0x3a,0x20,0x74,0x65,
0x78,0x74,0x2f,0x68,0x74,0x6d,0x6c,0x0d,0x0a,0x0d,0x0a,
/* raw file data (149 bytes) */
0x65,0x8e,0x41,0x0e,0xc2,0x20,0x10,0x45,0xd7,0x6d,0xd2,0x3b,0x8c,0xec,0x2b,0xe9,
0xc2,0xc4,0x44,0xca,0xde,0x63,0x20,0x4c,0xa5,0x49,0x19,0x94,0x4e,0x8d,0xde,0x5e,
0xa0,0x0b,0x17,0xee,0x26,0xf9,0xef,0xff,0x79,0xca,0x73,0x58,0x60,0x31,0x74,0x1f,
0x05,0x92,0xd0,0x00,0x5d,0xab,0x3c,0x1a,0xa7,0xbb,0xb6,0x51,0x3c,0xf3,0x82,0xfa,
0x4a,0x36,0xa6,0x84,0x96,0xc1,0xc6,0x10,0x0c,0x39,0x25,0xf7,0xa0,0x20,0x01,0xd9,
0x80,0xf5,0x26,0xad,0xc8,0xa3,0xd8,0x78,0xea,0xcf,0xe2,0x17,0x78,0xe6,0x47,0x8f,
0xcf,0x6d,0x7e,0x8d,0x22,0xe1,0x94,0x70,0xf5,0x22,0xaf,0x10,0x23,0x65,0xfa,0x74,
0x91,0x33,0x39,0x7c,0x1f,0x8b,0x44,0x69,0x29,0x59,0x5f,0x43,0xb1,0x00,0x50,0xb7,
0xe8,0x3e,0x7a,0xbf,0x1b,0xe5,0x87,0x7f,0x91,0x43,0x2e,0x0c,0xba,0xb2,0xb2,0xc2,
0x75,0x22,0x8f,0xe9,0x2f,};

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__index_html = 2;
#endif
static const unsigned char FSDATA_ALIGN_PRE data__index_html[] FSDATA_ALIGN_POST = {
/* /index.html (12 chars) */
0x2f,0x69,0x6e,0x64,0x65,0x78,0x2e,0x68,0x74,0x6d,0x6c,0x00,

/* HTTP header */
/* "HTTP/1.0 200 OK
" (17 bytes) */
0x48,0x54,0x54,0x50,0x2f,0x31,0x2e,0x30,0x20,0x32,0x30,0x30,0x20,0x4f,0x4b,0x0d,
0x0a,
/* "Server: lwIP/2.2.0d (http://savannah.nongnu.org/projects/lwip)
" (64 bytes) */
0x53,0x65,0x72,0x76,0x65,0x72,0x3a,0x20,0x6c,0x77,0x49,0x50,0x2f,0x32,0x2e,0x32,
0x2e,0x30,0x64,0x20,0x28,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x73,0x61,0x76,0x61,
0x6e,0x6e,0x61,0x68,0x2e,0x6e,0x6f,0x6e,0x67,0x6e,0x75,0x2e,0x6f,0x72,0x67,0x2f,
0x70,0x72,0x6f,0x6a,0x65,0x63,0x74,0x73,0x2f,0x6c,0x77,0x69,0x70,0x29,0x0d,0x0a,

/* "Content-Length: 3111
" (18+ bytes) */
0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x4c,0x65,0x6e,0x67,0x74,0x68,0x3a,0x20,
0x33,0x31,0x31,0x31,0x0d,0x0a,
/* "Content-Encoding: deflate
" (27 bytes) */
0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x45,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,
0x3a,0x20,0x64,0x65,0x66,0x6c,0x61,0x74,0x65,0x0d,0x0a,
/* "Content-Type: text/html

" (27 bytes) */
0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x54,0x79,0x70,0x65,0x3a,0x20,0x74,0x65,
0x78,0x74,0x2f,0x68,0x74,0x6d,0x6c,0x0d,0x0a,0x0d,0x0a,
/* raw file data (3111 bytes) */
0xad,0x1a,0xdb,0x6e,0xe3,0xb8,0xf5,0xd9,0x05,0xfa,0x0f,0x1c,0x02,0x1d,0xcb,0x48,
0x7c,0x9d,0xcc,0x05,0xb6,0x13,0x60,0xae,0x3b,0x59,0xcc,0x24,0x8b,0x89,0xa7,0xed,
0x62,0xb0,0x30,0x64,0x8b,0xb6,0xd9,0x91,0x25,0x55,0xa2,0x73,0x99,0x20,0xfd,0xb1,
0x3e,0xf4,0x93,0xfa,0x0b,0x3d,0xbc,0x4a,0x94,0x28,0xd9,0xd9,0x6e,0x5e,0x62,0x91,
0xe7,0xce,0xc3,0xc3,0xc3,0x73,0xf8,0xdf,0x7f,0xff,0x67,0xfa,0xe4,0xdd,0xe5,0xdb,
0xd9,0xaf,0xbf,0xbc,0x47,0x1b,0xb6,0x0d,0xcf,0xfe,0xfc,0xa7,0x29,0xff,0x8f,0x42,
0x3f,0x5a,0x9f,0x62,0x12,0x61,0x18,0x69,0x4d,0x37,0xc4,0x0f,0xf8,0x8f,0xd6,0x94,
0x51,0x16,0x92,0xb3,0x2c,0x5e,0xd1,0x39,0xf3,0x17,0x21,0x99,0xf6,0xe5,0x88,0x98,
0xdc,0x12,0xe6,0xa3,0xe5,0xc6,0x4f,0x33,0xc2,0x4e,0xf1,0x8e,0xad,0xba,0xaf,0xb0,
0x9c,0xc9,0x96,0x29,0x4d,0x18,0xfc,0x46,0x85,0xbf,0x90,0x30,0xb4,0xb8,0x9b,0xd3,
0x00,0x9d,0xa2,0xd5,0x2e,0x5a,0x32,0x1a,0x47,0x1e,0x82,0xcf,0x0e,0xba,0x47,0x29,
0x61,0xbb,0x34,0x42,0x41,0xbc,0xdc,0x6d,0x49,0xc4,0x7a,0x6b,0xc2,0xde,0x87,0x84,
0xff,0x7c,0x73,0x77,0x1e,0x48,0xb0,0x09,0x7a,0x98,0x54,0x49,0x2e,0x53,0xe2,0x33,
0x72,0x08,0x4d,0x09,0xa9,0xc8,0x16,0x49,0xb6,0x5a,0x2d,0x4e,0xc9,0x0f,0x82,0x14,
0xe8,0x60,0x9c,0x0f,0x6d,0xfd,0xdb,0x79,0x4a,0xd6,0xd9,0x3c,0xda,0x6d,0xe5,0xa8,
0xe6,0x82,0x16,0x94,0x79,0x49,0x9c,0x51,0xfe,0xd1,0xb9,0x87,0x39,0x2e,0x50,0xab,
0xa5,0x98,0x0e,0xd1,0x74,0x8a,0xf4,0xb4,0xc4,0x7c,0xb0,0xf1,0x93,0x94,0x82,0x14,
0x5b,0x92,0x65,0xfe,0x9a,0x08,0x02,0xf0,0xb7,0x8c,0xa3,0x2c,0x0e,0x49,0x2f,0x8c,
0xd7,0x66,0xca,0x89,0x0d,0xf6,0x99,0xfb,0x61,0x28,0x84,0xf3,0x34,0x76,0x51,0x5a,
0x50,0x84,0xc3,0xe8,0x4f,0x4f,0x91,0x69,0x49,0xb6,0xf8,0x86,0x86,0x21,0x5a,0x10,
0xb0,0xd1,0x3f,0x77,0x24,0x63,0x08,0x1f,0x15,0x91,0x8f,0x30,0x4c,0xac,0x69,0xc6,
0x48,0x9a,0x61,0x8d,0xc9,0xed,0x41,0x81,0xec,0xa0,0xb4,0x08,0x7a,0x21,0x12,0x7f,
0xf9,0x1d,0xa6,0x95,0x2c,0x2d,0xac,0x48,0xe3,0x31,0xe6,0x64,0xf1,0x71,0x3e,0xbe,
0xe6,0x3c,0x60,0x7c,0x98,0x0f,0x72,0x59,0xd9,0x5d,0x42,0x60,0x74,0xb5,0x0b,0xc3,
0x7c,0x82,0x46,0x01,0xb9,0xc5,0xe3,0x2b,0x06,0x72,0xaf,0x3d,0xda,0x51,0x13,0x0f,
0x6e,0x19,0x52,0x92,0x25,0x60,0x42,0x62,0x2b,0x2b,0x24,0x4b,0x52,0x72,0x4d,0xc9,
0xcd,0x18,0xdb,0x96,0xe0,0x73,0x7a,0x64,0x15,0xa7,0xc8,0x93,0x2a,0x82,0xa6,0x53,
0x7b,0xf5,0x11,0x3d,0x3a,0xd2,0x86,0x6e,0x71,0xac,0x9e,0x10,0x0d,0xa0,0x8d,0x6c,
0x8a,0x4c,0x4b,0x4b,0x01,0x73,0x19,0x89,0x82,0xf9,0x3f,0x32,0xf0,0xca,0x22,0xa3,
0x16,0x5d,0x79,0x1a,0xa8,0x17,0xf9,0x5b,0x80,0x3c,0x05,0xc7,0x4b,0xfc,0x2c,0x9b,
0x7f,0x27,0x77,0xd8,0xf0,0x69,0x05,0x20,0xf4,0x92,0xcc,0xf5,0x0c,0x50,0x34,0x78,
0xd7,0x7e,0xb8,0x23,0x4f,0x07,0xb7,0x1f,0xd4,0xdf,0xa4,0x06,0x67,0x0e,0x72,0x16,
0xf1,0x84,0xd8,0x06,0x58,0x99,0xa8,0x80,0x33,0x46,0x18,0x1d,0xa1,0x12,0x11,0x23,
0xf9,0x83,0xfa,0x0f,0x9b,0x65,0x1e,0xf8,0xcc,0x9f,0xb3,0x58,0x86,0x06,0xa3,0x90,
0x06,0x7d,0x70,0xf9,0x6d,0x15,0x8d,0x7f,0x69,0x7d,0xc1,0x2c,0x22,0x42,0x78,0x72,
0xd9,0xe7,0xf8,0x88,0xcf,0x4a,0x89,0x3b,0x60,0x23,0x14,0x81,0x6f,0x18,0xe3,0xf0,
0x05,0x67,0x8b,0x38,0xe0,0x56,0x51,0x68,0x82,0x24,0xee,0x14,0xc2,0x47,0xf6,0xe6,
0x6e,0xe6,0xaf,0x2f,0xc0,0xc6,0x5e,0x7b,0xf6,0xe6,0xf2,0xdd,0xaf,0xed,0xce,0xb7,
0xc1,0x6f,0x93,0x02,0x89,0x90,0x46,0x7c,0xa5,0x64,0x74,0xf0,0xf0,0xec,0x8b,0xf1,
0x90,0x16,0x9f,0xea,0x89,0x80,0x85,0xf9,0x4f,0x4b,0x1e,0xb5,0x0b,0x0c,0x58,0xc6,
0xee,0x42,0x4e,0x07,0x2f,0x60,0xa1,0xd7,0x69,0xbc,0x8b,0x82,0xee,0x32,0x0e,0xe3,
0x74,0x8c,0x7e,0xda,0xc4,0x19,0xfb,0xdb,0x86,0x32,0x82,0x2d,0xca,0x02,0xa5,0x77,
0x4d,0x33,0xba,0xa0,0x21,0x65,0x5c,0x0f,0x2c,0xbe,0xc2,0x1c,0x50,0x28,0xd8,0xf3,
0x93,0x04,0x1c,0xe9,0xed,0x86,0x86,0x81,0xc7,0x51,0x8d,0x84,0x3c,0x60,0x40,0x0c,
0x04,0x45,0x89,0x88,0x5d,0x8c,0xdc,0xb2,0xae,0x1f,0xd2,0x75,0x34,0x56,0xa3,0x13,
0xd8,0x98,0x41,0x00,0x0e,0x3a,0x46,0xcf,0x93,0x5b,0x6c,0x23,0x86,0x64,0xc5,0xca,
0x68,0x7c,0xac,0x06,0x49,0x58,0x3c,0x98,0x6b,0xc7,0x37,0x26,0x7b,0x97,0x9b,0x4c,
0x4f,0x2b,0xb3,0x55,0xd7,0xb1,0x02,0xa8,0x0d,0xa7,0xc4,0xad,0x6e,0x6c,0xbd,0xb9,
0x01,0x43,0xee,0x95,0x3a,0xc6,0x7c,0x56,0xf1,0xe5,0x3f,0xeb,0xd8,0x0a,0x30,0xcd,
0x55,0x68,0xdb,0xc8,0x93,0x87,0xa5,0x7a,0x9e,0x7c,0x56,0xf1,0xe4,0x3f,0xeb,0x78,
0x0a,0xb0,0xc7,0x68,0xba,0x8d,0x83,0xc5,0x2e,0x9b,0x9b,0x8d,0x53,0x2b,0x40,0x09,
0x50,0xc9,0x52,0x1a,0xad,0x13,0xab,0x8c,0xfc,0x28,0x09,0x83,0xc5,0x5c,0x1d,0x99,
0x75,0xa2,0x29,0x08,0x5b,0x26,0x3e,0x02,0x47,0x5b,0xad,0x48,0x1a,0xe9,0x31,0xb2,
0xac,0x42,0x7f,0x9d,0xd5,0x0b,0x22,0xa6,0x95,0x14,0xe2,0x77,0x1d,0x73,0x09,0x78,
0xb8,0x6f,0x04,0x04,0x52,0x9d,0x7a,0xbe,0x62,0x5a,0xf1,0x15,0xbf,0xeb,0xf8,0x4a,
0xc0,0xc3,0xf9,0x8a,0xd8,0x5f,0xcf,0x57,0x4c,0x2b,0xbe,0xe2,0x77,0x1d,0x5f,0x09,
0x68,0x42,0x57,0x73,0x18,0x98,0xa0,0x9b,0x38,0x0d,0xba,0x37,0xa9,0x9f,0x8c,0xd1,
0x02,0x38,0x7f,0xef,0xf2,0x01,0x5c,0xa1,0xb7,0x0c,0xe1,0xd4,0xc8,0x79,0x43,0x14,
0xc4,0xcd,0xea,0xa4,0xfe,0xcd,0x3e,0x95,0x0c,0x88,0x52,0xcb,0x7c,0xd7,0xa9,0x96,
0x23,0x18,0xf5,0x02,0x9a,0x25,0xa1,0x7f,0x37,0x46,0x51,0x1c,0x91,0x3d,0x22,0x2d,
0x76,0x8c,0x41,0x88,0xac,0x17,0x48,0x01,0x28,0x71,0xd4,0x57,0x9d,0x30,0x1a,0xf8,
0xf0,0x15,0x86,0xbc,0x3a,0x5a,0x17,0xed,0xf1,0xe6,0xeb,0x6c,0x76,0x79,0x61,0x89,
0x20,0x61,0x2c,0x09,0xea,0x04,0x50,0xa0,0x90,0xa9,0xbf,0x66,0x90,0xae,0x00,0x30,
0x90,0x14,0x09,0xd7,0xb1,0x46,0x75,0x91,0x8e,0xa3,0x65,0x48,0x45,0x5a,0x67,0xb2,
0x6b,0xc8,0xac,0xe5,0x24,0x4f,0x8d,0xbc,0x42,0xb8,0x3f,0x46,0xa5,0x65,0xea,0x3c,
0x54,0x09,0x16,0x0f,0xb2,0x52,0x62,0x3e,0x03,0xe7,0xbb,0x88,0x03,0x90,0xeb,0xad,
0xd4,0x5d,0xd0,0xc1,0x1d,0xfb,0x34,0x2e,0x12,0xd0,0xcc,0x1b,0x21,0x78,0xb4,0x6f,
0x04,0xe0,0x56,0x68,0x04,0x28,0x05,0xc9,0x66,0x58,0x15,0xbd,0x1a,0x81,0x44,0x94,
0x69,0x84,0x10,0xf1,0xa0,0x11,0x42,0x18,0xa7,0x11,0xc2,0x2c,0x45,0x23,0x94,0xf2,
0xcc,0x62,0x66,0xfa,0x2f,0x9e,0x8c,0xcb,0x30,0xc9,0x03,0x82,0xc8,0xd0,0x7a,0x52,
0x66,0xe9,0x57,0x97,0x2b,0x0f,0xb2,0x78,0x3f,0x98,0xc7,0x51,0x08,0xc9,0xaa,0x49,
0xc8,0x1e,0x48,0x98,0x11,0x93,0xba,0x16,0xdc,0xbe,0xc4,0x53,0xfa,0x42,0x39,0xa5,
0x7c,0x30,0x39,0x60,0xee,0xc1,0xe8,0x49,0x9e,0xf3,0xb9,0x36,0x4b,0x7d,0xb6,0x08,
0xff,0x22,0x92,0x7e,0x9c,0x7d,0xfe,0x04,0xce,0x5b,0xd9,0x12,0x25,0x5e,0x22,0xad,
0x38,0x8c,0x55,0x25,0xb1,0x70,0x70,0xe2,0x30,0x6e,0x46,0x22,0x97,0x38,0x8c,0x51,
0x25,0x9b,0xb0,0x19,0xe9,0x0b,0x53,0x61,0x89,0x8a,0x8e,0x5c,0xe2,0x5b,0xce,0x26,
0x9e,0x38,0x52,0x69,0x93,0x2b,0x80,0x0f,0xf3,0x80,0xb2,0x89,0x43,0x1e,0xfa,0xe7,
0xfa,0x26,0x88,0x15,0x6c,0x76,0x43,0xd9,0x72,0x83,0x9c,0x84,0x3b,0xe6,0xfa,0xd7,
0x5a,0xfa,0x19,0xf1,0x86,0x9d,0xb1,0xfe,0x6e,0x95,0xe9,0x2f,0x63,0x1a,0x56,0x88,
0xb7,0x5a,0xe2,0x60,0xb1,0x88,0x8c,0x1a,0x88,0x40,0x48,0x87,0xe8,0xc1,0xc8,0x41,
0x84,0x9e,0x35,0x10,0xaa,0xd3,0xd6,0x45,0xe7,0xa4,0x81,0x0e,0x8d,0x92,0x1d,0xdb,
0x47,0x25,0x20,0x2b,0x7f,0x17,0xb2,0xc7,0x4b,0xf3,0xd0,0xe8,0x32,0x4d,0x49,0x9f,
0xed,0x3d,0x45,0x7e,0x8d,0x1e,0xa3,0x92,0xb5,0x8a,0xc3,0xd8,0x1c,0x5d,0x29,0x9d,
0x63,0x5f,0xd8,0xd0,0x6e,0xbe,0x32,0x93,0x3b,0x6c,0x8b,0x54,0x53,0xb9,0xea,0x1e,
0x71,0xc7,0x31,0x37,0x6f,0x11,0x75,0xa1,0x70,0x55,0xda,0x21,0x0d,0x12,0x54,0x93,
0x3a,0x87,0xda,0x05,0xb2,0x6e,0xbe,0x32,0xf5,0x39,0x8c,0x63,0x35,0xe7,0xa9,0xea,
0x2c,0x61,0x12,0x5e,0x98,0x2b,0xc4,0xd2,0x63,0x94,0x33,0xeb,0x4c,0x1a,0x99,0xb8,
0x93,0x2b,0x87,0x6a,0x02,0xa8,0xe9,0xbe,0x5f,0x16,0x87,0x4a,0x29,0xb4,0x1f,0xf1,
0xb8,0xa3,0xee,0x74,0x76,0x7c,0xa5,0x05,0x66,0x13,0xcb,0x5a,0xb2,0x54,0x12,0xf9,
0x51,0x5e,0x25,0x51,0x35,0x37,0x7c,0xf1,0xfa,0x42,0x27,0xa3,0xd6,0x51,0xa4,0x0b,
0x81,0x80,0xbd,0x4f,0xd6,0xbc,0x5e,0x56,0x90,0xb0,0xb9,0xb2,0x25,0x8a,0x59,0xc7,
0xd6,0xc2,0xaa,0xa2,0x8a,0xa9,0xc5,0x69,0x84,0x4e,0x4d,0xe9,0x8c,0x17,0x88,0xe6,
0x7b,0xab,0x46,0x60,0x00,0x1b,0xf0,0x09,0xb7,0xc4,0x17,0x49,0x7b,0x4e,0xd2,0x34,
0x4e,0x2b,0x36,0xb1,0x10,0x7a,0x76,0x29,0xd3,0x36,0x92,0x92,0xf9,0x8a,0xa4,0xd7,
0x50,0x4a,0xa0,0x19,0x24,0xc9,0xba,0x9e,0xc6,0x43,0x51,0x9e,0x1d,0xfa,0x21,0x49,
0x0f,0x02,0x54,0x22,0x0c,0xf6,0xd9,0x3c,0x3f,0xc6,0xc4,0x09,0x76,0xef,0xb6,0x91,
0x81,0x52,0xe4,0xe4,0x31,0x64,0x50,0x74,0x70,0x46,0x43,0x1d,0x55,0x0d,0x02,0x0f,
0xa8,0x5f,0x5f,0x99,0x6b,0x8a,0x88,0xc4,0x93,0x02,0xc6,0xc8,0x8d,0x31,0x7c,0x51,
0x8f,0x72,0xe2,0x46,0x79,0x36,0xaa,0x47,0x79,0xe5,0x46,0x79,0x71,0x52,0x8f,0x32,
0x7c,0xe1,0xc4,0x39,0x6f,0xd0,0xe5,0x99,0x5b,0x99,0xab,0x26,0x65,0x5e,0xb8,0xb5,
0xb9,0x6a,0xd2,0x66,0x38,0x72,0xeb,0x73,0xd5,0xa4,0xcf,0xe8,0xb9,0x5b,0xa1,0x0f,
0x9f,0x2e,0x5f,0xcf,0xea,0xd1,0x9e,0x0f,0xdd,0x4a,0xbd,0xbb,0xfc,0xfa,0xe6,0xd3,
0xfb,0x06,0x19,0x07,0x23,0xb7,0x66,0xe7,0x17,0x0d,0xdc,0x46,0x83,0x13,0xb7,0x6a,
0xb3,0xf3,0xcf,0x6e,0x66,0xa5,0xd3,0xdc,0x42,0x7a,0xff,0xe5,0xcb,0xe5,0x17,0xec,
0x40,0x52,0x91,0x42,0xed,0x91,0x92,0x73,0x3b,0x36,0x49,0xe1,0x1c,0x93,0x47,0x58,
0xcd,0x36,0xc9,0xe1,0x0a,0xad,0x0c,0x1e,0x3a,0xe4,0xc1,0xfa,0x54,0xf4,0x2d,0x86,
0x90,0xb3,0x17,0x00,0x8f,0xf8,0x7d,0x5a,0xe7,0xf3,0x08,0x4f,0x1e,0x9c,0x48,0xa3,
0x2a,0x52,0xe6,0x5f,0x93,0x00,0x61,0x37,0xfc,0xf3,0x2a,0x3c,0x1c,0x83,0x8c,0x2e,
0xfd,0xd0,0xa0,0x28,0xf5,0x73,0x30,0xa7,0xfe,0x85,0xfb,0xa6,0x3a,0xc9,0xe4,0x21,
0x76,0x6f,0x45,0x5b,0x31,0x05,0xc5,0xeb,0x23,0x79,0x92,0x94,0x8f,0xac,0x52,0x03,
0x44,0x50,0xc8,0xa1,0x25,0x41,0x1b,0xda,0xdd,0x60,0x58,0xcf,0x9d,0x07,0x56,0x0d,
0xd3,0xca,0x29,0x96,0x1f,0x0d,0xe2,0xe0,0xe3,0x12,0x68,0x9a,0x2e,0x9e,0xfa,0x40,
0x06,0x10,0xb8,0xbb,0xcb,0xeb,0x2e,0xd4,0xb7,0x75,0xb1,0xc4,0xcd,0x55,0xb1,0xe2,
0xb5,0x76,0x79,0xf6,0xf2,0x8b,0x5f,0xe6,0xd5,0xdb,0xa5,0x86,0xab,0x38,0x86,0xa0,
0x29,0xb2,0x05,0x8e,0xd5,0x22,0x0a,0x8d,0x38,0x59,0xdc,0x2c,0x34,0xd7,0x0b,0xdb,
0x89,0x83,0x1e,0xae,0x47,0x5c,0xf9,0x01,0xd9,0xc3,0xd2,0xdd,0x59,0x28,0x6a,0x2b,
0xf5,0xbc,0xff,0x83,0x7a,0x55,0x43,0x57,0xa7,0x8a,0x6f,0x17,0xc9,0xb3,0xd2,0xaf,
0x92,0x7e,0xda,0xd8,0xab,0x92,0xc9,0x8c,0xe3,0xac,0x77,0x01,0x0b,0x9d,0x00,0x7a,
0x38,0x71,0xa4,0x8e,0xbd,0x90,0x44,0x6b,0xb6,0xc9,0x6f,0x73,0x0a,0xb8,0x02,0x31,
0x71,0x65,0x29,0x7e,0x9a,0xfa,0x77,0x28,0xa3,0x3f,0x84,0x33,0x0a,0xdc,0x3a,0x29,
0x4a,0xdd,0x16,0xed,0x95,0x8d,0x5d,0x17,0x2c,0xba,0x2e,0xb8,0xd0,0x75,0xb9,0x81,
0x3a,0x00,0xf1,0x64,0x63,0x63,0xc9,0x6b,0x02,0x0a,0xe9,0x2d,0xf4,0x4c,0x98,0x51,
0x42,0xce,0xa7,0x64,0x1b,0x5f,0x13,0x55,0x39,0xc8,0x31,0x78,0x9d,0x28,0x03,0x8a,
0x85,0x44,0xde,0x25,0x2f,0xad,0x69,0xec,0x09,0x25,0x4d,0x47,0xaf,0xae,0x0a,0xf7,
0xa8,0xc6,0x10,0xdd,0xdf,0xb0,0x69,0xea,0x31,0xc8,0x78,0x5d,0xd7,0x63,0x80,0x59,
0xdd,0x63,0x80,0x9f,0x16,0x37,0x35,0xeb,0x2c,0xe1,0xa6,0x74,0xbd,0x81,0x1a,0xee,
0x0d,0x0d,0xd8,0x66,0x8c,0x5e,0x0d,0x0a,0x9d,0x1c,0x8d,0x77,0x50,0x3d,0x0e,0x5c,
0x88,0xf3,0x3c,0xc2,0xe0,0x20,0xcd,0xe5,0x38,0x4e,0x73,0x8f,0xaa,0xe2,0x82,0xdc,
0xd4,0x3c,0x82,0x69,0xad,0xac,0xf8,0x2c,0xaa,0xeb,0x2e,0x01,0x02,0x46,0x13,0xd3,
0x32,0xc7,0xf3,0x8b,0x5f,0xbe,0xce,0x72,0xa6,0x92,0xa3,0xea,0xf2,0x60,0xd8,0xed,
0x0b,0xb8,0x6f,0xdb,0x93,0xc6,0xba,0xca,0x94,0xc3,0xe7,0xdc,0x96,0x16,0x88,0x3a,
0x14,0xb4,0xad,0x6c,0x7c,0xdd,0x0b,0xe3,0xa5,0x81,0xe2,0x64,0xd3,0x26,0x56,0xa8,
0x3a,0xd2,0xe7,0x80,0xdf,0xa8,0x69,0x5f,0xda,0x05,0xb7,0x3a,0x84,0x72,0xeb,0xd6,
0x58,0xb9,0x68,0x48,0x63,0xc5,0xc2,0x65,0x20,0xd7,0x4c,0x2c,0xfd,0x51,0x81,0x41,
0x63,0x93,0x57,0x5d,0x3a,0x44,0x70,0xf0,0xf8,0xf9,0x51,0x9b,0xd3,0xff,0xdf,0xf1,
0xe4,0x51,0xc1,0x9d,0x8b,0xd2,0x53,0x5f,0x52,0x8d,0x72,0xd8,0x16,0x10,0xb2,0xa0,
0x6d,0xcf,0xcb,0x28,0x3f,0xfe,0xf6,0x9b,0x7d,0xd1,0x03,0x78,0x2f,0x7f,0x17,0x21,
0x62,0x4b,0x4d,0x5c,0x73,0xbc,0x1f,0x90,0xcb,0x9e,0xec,0xb2,0x4d,0xde,0xf5,0x56,
0x1e,0xd2,0x71,0x59,0xb9,0xe5,0xbe,0x16,0x6e,0x68,0x10,0x10,0x63,0x6f,0x71,0x5e,
0x7b,0xee,0x47,0x23,0x4e,0xc8,0xfb,0xdf,0x79,0xfc,0xd7,0xf5,0x50,0xf6,0x9d,0xe1,
0x1a,0x4f,0x8a,0x67,0x0b,0x58,0x52,0xf0,0xfe,0x80,0xdb,0x72,0xd3,0x8d,0x5a,0x39,
0xc2,0xcf,0x57,0x97,0x17,0x20,0x08,0x7f,0xa4,0x41,0x57,0x77,0x8d,0xc7,0xac,0x7e,
0x0e,0x03,0xa5,0x18,0x72,0x83,0xfe,0xfe,0xf9,0xd3,0x47,0xc6,0x12,0x75,0xc3,0x36,
0x4f,0x68,0xb4,0x07,0xc5,0xb0,0x7b,0xbc,0xf6,0x4f,0xef,0x67,0xed,0x63,0xf1,0x76,
0xe8,0xa8,0xdd,0xe7,0xc9,0x01,0x67,0xdd,0x5b,0xae,0x29,0x8c,0xae,0xfc,0x30,0x7f,
0x0f,0xa1,0xd1,0xc0,0xf4,0x8a,0xe2,0x47,0xc8,0xb9,0x49,0xea,0xb5,0x7f,0x06,0x8c,
0xf6,0xb1,0x11,0x79,0x3f,0xc2,0xeb,0xe5,0x92,0x24,0xac,0x7d,0xdc,0xe6,0xf1,0xb6,
0xcf,0x5f,0x70,0xb5,0xab,0x48,0x51,0xe0,0xe5,0x95,0x03,0xe4,0xe9,0x09,0x9e,0xe8,
0xdf,0x5d,0x31,0xf1,0x66,0x0a,0xea,0x07,0x27,0xe8,0xe9,0x53,0x64,0x90,0x60,0x78,
0x97,0x89,0xf1,0xd1,0x60,0xd0,0x29,0x95,0x06,0x32,0x79,0xe3,0xd7,0x96,0x97,0xa9,
0xab,0x26,0x2a,0xc7,0x66,0x2a,0xfe,0x0b,0xb4,0x72,0x55,0x43,0xac,0x83,0x2c,0x03,
0x35,0xe2,0xd5,0xb0,0xc3,0x25,0x00,0x8b,0x7c,0xa7,0xa1,0xa4,0xa1,0x8c,0x87,0x54,
0x85,0xa4,0x4e,0xbc,0x52,0x25,0xc5,0x75,0x4d,0x2b,0x7b,0x9e,0x06,0x98,0xf6,0xcd,
0x8b,0x37,0xfe,0xfa,0x4d,0xf8,0x3c,0x3f,0x50,0x4e,0xc5,0x71,0xdc,0x5f,0x66,0x99,
0x7c,0x18,0xd7,0xea,0xf1,0xed,0xd4,0x4d,0xe2,0x64,0x97,0x28,0x21,0xad,0x7d,0xa1,
0xef,0x08,0xea,0xdd,0xd8,0x18,0xad,0xe8,0x2d,0x09,0xd4,0x28,0x8b,0xa1,0xed,0xca,
0x0f,0x1e,0xf3,0x1e,0x6b,0xc5,0xf8,0x40,0x3e,0x62,0x9a,0xb5,0xa3,0x7c,0x6c,0x01,
0x1d,0x5a,0x92,0x76,0x85,0x4c,0x63,0x94,0xc5,0x21,0x0d,0xec,0x19,0x75,0xa4,0xb1,
0x0d,0xd5,0xd5,0xc9,0xea,0x03,0x96,0x1b,0xfe,0x76,0x45,0xcd,0xfe,0xe8,0xaa,0x8b,
0xd6,0xa8,0x18,0x63,0x7a,0x8b,0x10,0xb0,0xba,0x90,0xab,0xa5,0xa0,0xcd,0xef,0xd3,
0x6d,0xf0,0x17,0x4b,0x33,0xf3,0xa9,0x0f,0xdd,0x81,0x19,0xd9,0x10,0x9e,0xd9,0x58,
0x43,0x55,0xa1,0x85,0x44,0x65,0xa1,0x75,0xfe,0xdc,0xdd,0xc6,0x3f,0xba,0x31,0xec,
0x36,0xca,0x40,0xbe,0x41,0xef,0xa5,0x1a,0xd7,0x43,0xbd,0x97,0xba,0x6c,0xb5,0xa2,
0x21,0x14,0xe0,0xc7,0xc8,0x0f,0x93,0x8d,0xef,0xa9,0xf9,0xd3,0x97,0x83,0x8e,0xad,
0xbe,0xec,0x74,0x59,0x4b,0x2b,0x17,0xac,0xbc,0x5e,0xd5,0xd5,0xca,0x87,0x1a,0x2d,
0xaf,0x3d,0x8d,0xaf,0xa4,0x78,0xa6,0xd9,0xd7,0xef,0x34,0xa7,0xe2,0x18,0x85,0xdb,
0x48,0xec,0x07,0xa7,0xd8,0x7e,0x1a,0xa8,0x5e,0x64,0x6e,0x86,0x67,0x37,0x74,0x45,
0x11,0x8b,0x51,0x16,0xd2,0x04,0xda,0xf7,0x34,0x58,0x13,0x20,0x31,0x94,0xf3,0x01,
0xbd,0x46,0x14,0x90,0x95,0x1e,0x18,0x89,0x1e,0xbe,0xf9,0x96,0x7a,0x29,0x27,0x9e,
0xca,0x84,0x4a,0x08,0x72,0x5a,0xb9,0x9c,0xa1,0x16,0xa7,0xc3,0x99,0xf1,0x43,0x86,
0x81,0x86,0x40,0x4d,0x6e,0x06,0xd5,0x67,0x96,0xb7,0xf7,0x0a,0x8c,0xea,0x35,0x9f,
0xe2,0x30,0x5e,0xfa,0xe2,0x35,0xca,0x26,0x25,0xab,0xd3,0x76,0xdf,0x82,0xeb,0x89,
0x68,0xa7,0xb4,0xea,0x83,0xd8,0xf2,0xd7,0x22,0x55,0x0f,0x56,0x79,0xea,0x20,0x34,
0x91,0x49,0x84,0x96,0xb2,0xe0,0x43,0x5a,0x0b,0x66,0x9e,0xb9,0xf2,0x8f,0x54,0x43,
0x36,0x3e,0xdf,0x52,0xe0,0x1c,0xb9,0x44,0x59,0x64,0x84,0x67,0xe7,0xdc,0xcb,0xa6,
0x7d,0xb6,0xa9,0x07,0x94,0xb9,0xe3,0x19,0xcf,0x65,0x9a,0x01,0x5f,0x0a,0xb8,0x19,
0x58,0xee,0x10,0xb8,0xcf,0xa2,0xb7,0x62,0x1a,0x3b,0x8f,0x40,0x51,0xed,0x98,0x83,
0xa4,0xfe,0xc0,0x0b,0x35,0xcd,0x90,0x27,0x22,0x20,0xa1,0x2d,0x8d,0x74,0x68,0x19,
0x0d,0x04,0xee,0xbb,0xbc,0x0d,0xb2,0x4f,0xba,0x1a,0x0a,0x7f,0xe5,0x7e,0x53,0x83,
0x6b,0xe7,0x17,0x67,0xa6,0x83,0x51,0x03,0x5e,0x20,0x3e,0x14,0x17,0xa3,0xb3,0x02,
0x20,0xfc,0x4c,0x95,0x93,0xf4,0x0b,0x5e,0x32,0x15,0xa9,0x9d,0x99,0x30,0x1f,0xf0,
0x9b,0xfb,0x5a,0xbe,0x91,0xd4,0xde,0xc9,0x63,0x3d,0x16,0x1e,0x99,0xe7,0x53,0x8a,
0xc6,0x66,0x28,0xc6,0x4d,0x9d,0x44,0x0b,0xe7,0x78,0xc4,0xc7,0xe5,0x53,0x7b,0xb5,
0x35,0xe5,0x34,0x0c,0xa6,0xce,0xe4,0xb0,0xa8,0x27,0x95,0xc7,0xce,0xa6,0x4f,0xba,
0x5d,0x54,0x32,0xf0,0x48,0x2c,0x26,0xea,0x76,0xb5,0xbe,0x72,0x43,0xe7,0x27,0x56,
0x4e,0x4b,0x66,0xce,0x7a,0xc7,0x8a,0xfe,0x01,0x10,0xc7,0x2a,0x95,0x34,0xbb,0xc7,
0xec,0x3a,0x3b,0x85,0x2f,0x18,0x3e,0xdf,0x6b,0x62,0xb7,0x99,0xdf,0xf0,0x11,0x9c,
0x29,0x01,0x8a,0x04,0x64,0x42,0xae,0x64,0x52,0x97,0x32,0x25,0x95,0x9a,0x72,0x38,
0xe8,0x04,0x95,0xdc,0x00,0x96,0xa6,0xc0,0xb6,0x9f,0xf3,0xb5,0x16,0xd6,0x5e,0x5a,
0x7b,0x71,0xed,0xe5,0x2d,0xd9,0xaa,0x14,0xd0,0xde,0xfa,0xd1,0x92,0x84,0x65,0xc9,
0x5e,0x8d,0xb8,0xb1,0x4d,0x74,0x73,0x26,0xe1,0xb8,0x89,0xba,0x73,0x2b,0x6a,0x9e,
0x57,0xbc,0x83,0x06,0xb9,0x2a,0x2f,0x28,0xca,0x4b,0x4a,0xce,0xca,0xba,0x87,0xd5,
0x3d,0xad,0x6f,0x17,0x25,0x69,0x77,0x3a,0x45,0xa3,0x4d,0xfb,0x7c,0xb0,0x1c,0x6b,
0xf5,0x59,0x21,0x52,0x7c,0x73,0x50,0x14,0xcf,0x7f,0x4e,0x43,0x81,0x4f,0xfb,0xca,
0x94,0x53,0x91,0xa7,0x9e,0xfd,0x0f,};

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__wifi_settings_html = 3;
#endif
static const unsigned char FSDATA_ALIGN_PRE data__wifi_settings_html[] FSDATA_ALIGN_POST = {
/* /wifi_settings.html (20 chars) */
0x2f,0x77,0x69,0x66,0x69,0x5f,0x73,0x65,0x74,0x74,0x69,0x6e,0x67,0x73,0x2e,0x68,
0x74,0x6d,0x6c,0x00,

/* HTTP header */
/* "HTTP/1.0 200 OK
" (17 bytes) */
0x48,0x54,0x54,0x50,0x2f,0x31,0x2e,0x30,0x20,0x32,0x30,0x30,0x20,0x4f,0x4b,0x0d,
0x0a,
/* "Server: lwIP/2.2.0d (http://savannah.nongnu.org/projects/lwip)
" (64 bytes) */
0x53,0x65,0x72,0x76,0x65,0x72,0x3a,0x20,0x6c,0x77,0x49,0x50,0x2f,0x32,0x2e,0x32,
0x2e,0x30,0x64,0x20,0x28,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x73,0x61,0x76,0x61,
0x6e,0x6e,0x61,0x68,0x2e,0x6e,0x6f,0x6e,0x67,0x6e,0x75,0x2e,0x6f,0x72,0x67,0x2f,
0x70,0x72,0x6f,0x6a,0x65,0x63,0x74,0x73,0x2f,0x6c,0x77,0x69,0x70,0x29,0x0d,0x0a,

/* "Content-Length: 2523
" (18+ bytes) */
0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x4c,0x65,0x6e,0x67,0x74,0x68,0x3a,0x20,
0x32,0x35,0x32,0x33,0x0d,0x0a,
/* "Content-Encoding: deflate
" (27 bytes) */
0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x45,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,
0x3a,0x20,0x64,0x65,0x66,0x6c,0x61,0x74,0x65,0x0d,0x0a,
/* "Content-Type: text/html

" (27 bytes) */
0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x54,0x79,0x70,0x65,0x3a,0x20,0x74,0x65,
0x78,0x74,0x2f,0x68,0x74,0x6d,0x6c,0x0d,0x0a,0x0d,0x0a,
/* raw file data (2523 bytes) */
0xdd,0x1a,0x5d,0x6f,0xdb,0xc8,0xf1,0x59,0x05,0xfa,0x1f,0x36,0x04,0x1a,0x51,0xb0,
0xf5,0xd9,0x5c,0xec,0xca,0x92,0x81,0x8b,0x13,0x5c,0x72,0xb8,0x4b,0x0e,0x67,0x5d,
0xdb,0xc3,0xa1,0x10,0x28,0x72,0x25,0xb1,0xa1,0x48,0x96,0x5c,0x59,0xd1,0xa5,0xfe,
0xef,0x9d,0xd9,0x9d,0x5d,0x2e,0xbf,0x64,0xe7,0x80,0xfa,0x21,0x7e,0xb0,0xc8,0xdd,
0xf9,0x9e,0xd9,0xd9,0x99,0x5d,0xce,0x9e,0xbd,0xfe,0x70,0xb3,0xf8,0xf5,0xa7,0x37,
0x6c,0x2b,0x76,0xd1,0xf5,0x9f,0xff,0x34,0xc3,0x5f,0x16,0x79,0xf1,0x66,0xee,0xf0,
0xd8,0x91,0x23,0xdc,0x0b,0xe0,0xb7,0x33,0x13,0xa1,0x88,0xf8,0x75,0x9e,0xac,0x43,
0x76,0xe0,0xab,0xd9,0x50,0xbd,0xe3,0xcc,0x8e,0x0b,0x8f,0xf9,0x5b,0x2f,0xcb,0xb9,
0x98,0x3b,0x7b,0xb1,0xee,0x5f,0x3a,0x72,0x22,0xf7,0xb3,0x30,0x15,0xf8,0xd8,0x89,
0xb8,0x60,0xab,0xe3,0x32,0x0c,0xd8,0x9c,0xad,0xf7,0xb1,0x2f,0xc2,0x24,0x76,0x19,
0xbc,0xf6,0xd8,0x67,0x96,0x71,0xb1,0xcf,0x62,0x16,0x24,0xfe,0x7e,0xc7,0x63,0x31,
0xd8,0x70,0xf1,0x26,0xe2,0xf8,0xf8,0xea,0xf8,0x2e,0x50,0x60,0x57,0xec,0xfe,0x4a,
0x13,0xf2,0x33,0xee,0x09,0xfe,0x18,0x4a,0x0a,0x92,0x88,0xd5,0x09,0x79,0x41,0x90,
0x01,0x19,0xc7,0x91,0x23,0x9a,0x1a,0x5b,0x85,0xc2,0x4d,0x93,0x3c,0xc4,0x97,0xde,
0x67,0x9c,0xea,0x10,0xe1,0x31,0x9b,0xcd,0x98,0x9e,0x92,0x48,0xf7,0x25,0xcc,0x34,
0x0b,0x81,0xcf,0x8e,0xe7,0xb9,0xb7,0xe1,0x84,0xea,0x27,0x71,0x9e,0x44,0x7c,0x10,
0x25,0x1b,0x33,0xd3,0x80,0x0a,0x4a,0x2f,0xbd,0x28,0x5a,0x66,0x7c,0x93,0xbb,0x84,
0x8a,0x32,0x66,0x3c,0x4f,0x81,0x02,0xaa,0x8b,0x20,0x87,0x70,0x1d,0x2e,0x63,0x6f,
0xc7,0x5d,0x45,0xa4,0x03,0x3a,0x2c,0x03,0x4f,0x78,0x4b,0x91,0x2c,0x85,0xb7,0x8a,
0xb8,0xab,0x31,0x08,0x20,0xdf,0x26,0x87,0x25,0xb8,0x27,0xde,0xf0,0xe5,0x6a,0x2f,
0x04,0x98,0x4b,0x43,0x0c,0xc2,0x38,0xe0,0x9f,0x08,0xae,0x89,0x51,0xea,0xe5,0xf9,
0x21,0xc9,0x82,0x27,0x61,0x96,0x25,0x7b,0xc1,0xb3,0xa7,0x53,0x8e,0xf8,0xfd,0xbf,
0x74,0x2c,0xbb,0xb7,0x4e,0x15,0xdf,0xc8,0xcf,0xe1,0xda,0x95,0x8b,0xc3,0x75,0x24,
0xfe,0xd2,0x39,0xc3,0x49,0x22,0xc6,0xe6,0x73,0x16,0xef,0xa3,0x88,0x60,0x65,0x50,
0x88,0x55,0x12,0x1c,0x41,0x17,0xc2,0x92,0x04,0x9d,0x9e,0xb5,0x70,0xf2,0x57,0xc7,
0x85,0xb7,0x79,0x8f,0xa6,0xec,0x2e,0x5e,0x7d,0x78,0xfd,0x6b,0xb7,0xf7,0xdb,0xe8,
0x5f,0x57,0x05,0x85,0x28,0x8c,0xd1,0x18,0x6a,0x81,0xb8,0xce,0xe2,0x67,0xa7,0xa7,
0x67,0x61,0x66,0x20,0x17,0xaa,0x83,0x8f,0x25,0x61,0x6c,0x90,0x5c,0x1c,0x23,0x24,
0xe1,0xac,0x3c,0xff,0xe3,0x06,0x8c,0x19,0x07,0x7d,0x3f,0x89,0x92,0x6c,0xca,0xbe,
0xdb,0x26,0xb9,0xf8,0xc7,0x36,0x14,0xdc,0xa9,0x61,0x0c,0xee,0xc2,0x3c,0x5c,0x85,
0x51,0x28,0x50,0x01,0x47,0xbe,0x45,0x06,0x4e,0x2a,0x36,0xf0,0xd2,0x94,0xc7,0xc1,
0xcd,0x36,0x8c,0x02,0x17,0x31,0xb5,0x68,0xb8,0x94,0x60,0xf9,0x83,0x7e,0x5c,0xae,
0x5b,0xc1,0x3f,0x89,0xbe,0x17,0x85,0x9b,0x78,0x4a,0xa3,0x57,0x2c,0x05,0x4b,0x87,
0xf1,0x66,0xca,0xbe,0x49,0x3f,0x39,0x25,0xbc,0x88,0xaf,0x45,0x15,0x0b,0xc7,0x9a,
0x71,0xa4,0x99,0x83,0xa5,0x54,0xdb,0x36,0xd4,0x6b,0x63,0x28,0x3d,0x4b,0xc6,0xaa,
0xbb,0xae,0x0a,0xa7,0x2d,0x46,0xa2,0x96,0xf8,0x60,0xd4,0xb7,0xb2,0xc1,0x49,0xe2,
0x82,0x8f,0x2d,0x4c,0x24,0x94,0xe6,0x21,0x15,0x2b,0x71,0x08,0x38,0xe4,0xe3,0x56,
0x16,0x72,0x96,0x78,0xc8,0xe7,0x16,0x26,0x0a,0xae,0x9d,0xcb,0x9d,0x17,0xed,0xdb,
0x15,0x91,0xb3,0xc4,0x45,0x3e,0xb7,0x70,0x51,0x70,0x26,0xc2,0x4e,0x7b,0xec,0x8a,
0xe1,0xf2,0xed,0x1f,0x32,0x2f,0x9d,0xb2,0x15,0xf0,0xfd,0xd8,0xc7,0x01,0xa7,0x4a,
0xce,0x8f,0x60,0xa5,0x17,0x9c,0x21,0x56,0x2b,0xbe,0xce,0xbc,0xc3,0x03,0xf2,0x1b,
0x08,0xd2,0xc1,0xbc,0xb7,0xe8,0x51,0xc0,0x1b,0x5d,0x82,0x30,0x4f,0x23,0xef,0x38,
0x65,0x71,0x12,0xf3,0x8a,0x00,0x2a,0x9b,0xe4,0xad,0xec,0x69,0x9e,0x98,0xd3,0x5b,
0x0b,0x6b,0x0d,0xdb,0xee,0x2a,0x95,0xc2,0x2c,0x66,0xaf,0x7e,0x59,0x2c,0x3e,0xbc,
0xb7,0x19,0x2a,0x90,0x12,0xbf,0x16,0x76,0x04,0x09,0x25,0xc0,0xb7,0x42,0x64,0x21,
0xc0,0x02,0x41,0x71,0x4c,0xb9,0x73,0xae,0x31,0x1b,0x08,0x27,0xb1,0x1f,0x85,0xfe,
0x47,0x7b,0x2b,0x87,0x6d,0x9c,0x72,0x2b,0xec,0x84,0xae,0xb5,0xc6,0xce,0x59,0xc5,
0x01,0xbd,0xfb,0x1a,0x3d,0x3b,0x71,0x54,0x8a,0x80,0x05,0xc4,0xd0,0xfb,0x24,0x00,
0xa9,0x6e,0x94,0xde,0x92,0x8c,0xd3,0x2b,0xa5,0x3d,0x1b,0x5f,0xb3,0x3e,0x05,0x80,
0x4b,0xee,0xd4,0xbc,0x5c,0x2d,0xa7,0x00,0xa4,0x10,0xa7,0x00,0x8c,0xc2,0xa7,0x80,
0xc8,0xd7,0x0d,0x91,0x52,0x01,0x54,0x66,0x22,0xb8,0x7b,0xbd,0xf7,0x14,0xfe,0x64,
0xcf,0xca,0x7b,0x4d,0xfb,0xae,0x04,0x3f,0x31,0xcf,0xde,0x2e,0x7e,0xfc,0x01,0x9c,
0x57,0x0d,0x88,0x32,0x65,0x99,0xda,0x9a,0x09,0xd7,0xb2,0x59,0x03,0x5d,0x84,0x69,
0x22,0x2b,0x6d,0x0b,0xf5,0x25,0xee,0xae,0xcd,0xd4,0xeb,0x79,0xac,0x81,0xbc,0x45,
0xa6,0x89,0x8b,0xca,0x07,0x16,0x7d,0x06,0x7f,0x05,0x8b,0xfa,0xe2,0x2f,0xb3,0x80,
0x6a,0xf8,0x72,0xc9,0x63,0x1f,0x03,0xaf,0xa0,0xa7,0x1d,0x45,0x44,0x9a,0xb3,0x48,
0x83,0xac,0x12,0xc8,0x92,0xb2,0x5e,0x41,0x5a,0xe5,0x61,0x51,0x42,0xa6,0x9e,0x5c,
0x62,0x64,0x1c,0x27,0xe3,0xff,0xd9,0xf3,0x5c,0x38,0x53,0x07,0x6b,0x4d,0xe7,0xdc,
0x0c,0x6f,0x96,0xf1,0x7e,0x07,0xc3,0x63,0x33,0x86,0x24,0xe5,0x22,0x9e,0x3a,0x6b,
0xd0,0xdf,0x8c,0x23,0x07,0x18,0x33,0xdc,0x68,0xe2,0xde,0xb0,0xfc,0x77,0x0e,0x99,
0xc2,0x2a,0xba,0x72,0x88,0xc2,0x25,0x0e,0xba,0x28,0x0c,0xe9,0x0f,0x36,0x2e,0xc3,
0x3d,0x83,0x52,0xc7,0xf9,0x59,0x89,0xb7,0xe4,0x59,0x96,0x64,0x8e,0x76,0x29,0x15,
0xe0,0x25,0x78,0xb2,0x04,0x8f,0x72,0x4e,0x50,0x5e,0xc4,0x33,0xe1,0x3a,0xb7,0x3c,
0xbb,0x83,0x0a,0x21,0xcc,0x21,0xc9,0xea,0x0a,0x1a,0xb7,0x0b,0x93,0x81,0x88,0xda,
0xe8,0x61,0x5b,0x16,0xd5,0xe1,0x53,0xd9,0x53,0x73,0xfc,0x7a,0x6d,0x5a,0x2a,0xf4,
0x9f,0xca,0xac,0x16,0xd3,0xaf,0xde,0xb2,0x4f,0x1f,0xb4,0x15,0xc6,0x5f,0x87,0x85,
0x4b,0x36,0xb6,0x8a,0x12,0x99,0x9e,0xcf,0x55,0x01,0x51,0xea,0xd4,0x37,0xba,0x8c,
0x2f,0x6f,0x6f,0xea,0xad,0x9a,0xd4,0xad,0x47,0xc5,0x56,0xef,0x06,0x40,0x06,0xca,
0x28,0x55,0x7c,0x40,0x53,0xa7,0x2b,0xd2,0x66,0x2a,0x57,0x72,0x47,0x6a,0x6e,0x0b,
0x35,0xa5,0x93,0xed,0xa1,0x23,0xdb,0x43,0x87,0xda,0x43,0x45,0x6b,0x38,0xcc,0xf8,
0x2e,0xb9,0xe3,0x70,0x14,0x04,0x7d,0x93,0x12,0x20,0x8c,0xd3,0xbd,0xc8,0x35,0xc4,
0x01,0x2a,0x0a,0xee,0xaa,0x6e,0xcd,0xc7,0xea,0x82,0x08,0xdf,0x40,0x1b,0x28,0x68,
0x9b,0x2c,0xfa,0x39,0x45,0x8d,0xaa,0x90,0x02,0x07,0xcb,0xb1,0x1c,0xf8,0xf6,0x0c,
0xe3,0x7b,0x5b,0x9b,0xe6,0x16,0x95,0xe6,0x6b,0x4d,0xea,0xc8,0xcc,0xb5,0xf7,0x90,
0x96,0xa5,0x82,0x25,0x36,0x15,0xf5,0x4a,0x9b,0x48,0xa8,0x69,0xe2,0x80,0x8f,0x65,
0x0e,0x34,0xdd,0xd8,0xa0,0x64,0xe1,0x66,0x0b,0x1d,0xca,0x21,0x0c,0xc4,0x76,0xca,
0x2e,0x47,0xaa,0xa5,0x2c,0x23,0x3e,0xaa,0x4e,0x0d,0xf8,0x5d,0xe8,0x73,0x26,0x97,
0x59,0xaf,0xa2,0x7a,0xa5,0xae,0x43,0xa2,0x35,0x05,0xa5,0xc7,0x4e,0x68,0x28,0xe7,
0xb5,0x8a,0xf2,0xb5,0xa4,0x64,0x73,0x41,0x0c,0x28,0x65,0x46,0x55,0x2e,0xef,0xde,
0xff,0xf4,0xcb,0xc2,0x62,0xa4,0xb8,0x60,0xfe,0x40,0x3e,0x39,0xf4,0x06,0xb0,0xee,
0x2a,0xb3,0xc6,0x8e,0x64,0xb4,0xf1,0x37,0x68,0xb5,0x32,0x0c,0xad,0x2c,0x55,0x71,
0x55,0x09,0xe8,0x0e,0x1c,0x9e,0x4b,0x3a,0xa8,0xd9,0xf2,0x0a,0x32,0x35,0xdb,0x63,
0x96,0x65,0xc5,0x58,0xb6,0x39,0x2a,0xb6,0x50,0x47,0x7f,0x85,0xac,0x67,0xce,0x94,
0x39,0x67,0x96,0x00,0xbd,0xd2,0x2a,0x97,0x09,0x70,0x9d,0x64,0x3b,0x58,0x97,0xf5,
0xde,0x30,0x8c,0xd1,0xfa,0x4e,0x2d,0x2f,0xa8,0x58,0x28,0x15,0x86,0x7a,0xb8,0x04,
0xbb,0xf6,0x02,0xfe,0x20,0xe1,0xf2,0x1e,0x42,0x99,0x54,0x66,0x0c,0x17,0x05,0xab,
0xec,0x1d,0x57,0x0f,0xe7,0x39,0xc4,0x52,0xb5,0x2b,0x69,0xdc,0x60,0xca,0x62,0x1b,
0xa2,0x0c,0x61,0xed,0x44,0x98,0xab,0x28,0x40,0xce,0x8b,0x79,0x95,0x07,0xa7,0x55,
0xe2,0x16,0x84,0x0a,0x89,0xa9,0x69,0x56,0x28,0x0a,0x28,0x75,0x5a,0xb9,0x05,0x0f,
0xef,0x9a,0x36,0x9e,0x6d,0x18,0x04,0xdc,0x68,0x2f,0xdd,0xe2,0x36,0x1d,0xe2,0xc1,
0xce,0x01,0xd3,0x6a,0x5d,0xba,0x8d,0xf6,0x39,0xb9,0xcb,0x22,0xf1,0xea,0xee,0xe9,
0x27,0xbb,0x9d,0x17,0x07,0x66,0x98,0x74,0x99,0x8c,0x2f,0x5e,0x8e,0xac,0x3d,0xb4,
0x49,0xec,0xb2,0x6c,0x8d,0x3a,0x7c,0x7e,0x7c,0xc8,0x59,0xc7,0x11,0x0f,0x05,0x51,
0x01,0x5a,0x16,0xa1,0xe1,0x60,0xd4,0xde,0x23,0x8b,0x53,0x4e,0x7d,0x92,0xc0,0xce,
0x68,0x13,0xad,0x34,0x6e,0xb5,0xf9,0x87,0x96,0x48,0x43,0x4d,0x54,0xb1,0xd8,0xe7,
0xe6,0x52,0x84,0xbc,0x26,0xd7,0xae,0x23,0x9d,0x97,0x66,0xe0,0x5e,0x7e,0x98,0xea,
0xda,0x40,0xcd,0x59,0xc1,0x62,0x68,0x90,0xaf,0xbf,0xbf,0xfd,0xf0,0x7e,0xa0,0xc2,
0x36,0x5c,0x1f,0x6d,0x48,0x22,0x6b,0x80,0x31,0x21,0x98,0x97,0x9e,0xbd,0xa0,0x64,
0x94,0x00,0xad,0x98,0x1f,0xd8,0x3f,0x7f,0xfc,0xe1,0xad,0x10,0x29,0x95,0x3d,0xae,
0x39,0xc0,0x96,0xaf,0x83,0x04,0x12,0x90,0xdb,0xfd,0xee,0xcd,0xa2,0x7b,0x2e,0xaf,
0x2f,0xce,0xba,0x43,0xac,0xcf,0x90,0xec,0xc0,0xdf,0x84,0x30,0xba,0xf6,0x22,0x73,
0x54,0xad,0xb1,0x20,0x22,0x88,0xde,0x5b,0xb8,0xcf,0xe1,0x99,0xdb,0xfd,0x1e,0x10,
0xba,0xe7,0x55,0x69,0xda,0xe1,0xbf,0xf5,0x7d,0x9e,0x8a,0xee,0x79,0x17,0xb7,0x9a,
0x21,0xde,0x13,0x75,0x6b,0x38,0xb1,0x39,0x43,0x0f,0xd7,0xcc,0xd5,0xe3,0xb0,0x2f,
0x04,0xc7,0x5b,0x21,0x6f,0x6c,0xa0,0x9e,0x7b,0xc1,0x9e,0x3f,0x67,0x06,0x07,0x86,
0xf7,0xb9,0x1c,0x9f,0x8c,0x46,0x3a,0x0a,0xc8,0x6e,0xb9,0xaa,0xda,0xb4,0xab,0xd0,
0x7a,0x05,0x4d,0x35,0xb6,0xa0,0x7d,0x0f,0xb1,0xaa,0x25,0xa6,0xf4,0x4b,0x8a,0xb7,
0x52,0xee,0x29,0xb4,0x16,0x66,0x4e,0x79,0xbe,0x44,0xbc,0x57,0x2f,0x34,0x89,0x0a,
0x19,0x8d,0x51,0xad,0xda,0x22,0x59,0xa5,0xa4,0xb5,0x96,0x79,0x6b,0x55,0x5b,0xad,
0x46,0xad,0x73,0x05,0xe6,0xca,0xcb,0xb7,0xa5,0x97,0x65,0xde,0x11,0x6f,0xc1,0x4c,
0x5c,0x49,0x20,0x2a,0x73,0xe8,0x8e,0xab,0x03,0xeb,0x9f,0xb9,0x38,0x19,0xc3,0xe0,
0xe8,0x0a,0x7e,0x66,0x16,0xfa,0x20,0xe2,0xf1,0x46,0x6c,0x61,0xf8,0xec,0x4c,0x53,
0x52,0x57,0x6e,0xb8,0xc3,0x1b,0xb0,0xdf,0x62,0x7d,0x95,0x80,0x8e,0xf6,0x81,0xc4,
0x78,0x72,0x69,0xe0,0x3b,0x05,0xdf,0xb3,0x39,0xbb,0x95,0x2b,0x63,0xb0,0xce,0x92,
0x1d,0x1c,0xbc,0x65,0x37,0x58,0xda,0xf8,0xda,0x32,0xf7,0x0c,0x6d,0xc8,0x20,0x33,
0x00,0x95,0x6b,0xa0,0x72,0xd1,0xc3,0xf0,0x90,0x24,0x27,0xa3,0x17,0x97,0xbd,0x2f,
0x20,0x8a,0x14,0xae,0xd9,0xcb,0x1e,0xfb,0x2f,0x1b,0xff,0x6d,0xa2,0x39,0x3c,0x0a,
0xef,0x39,0x7b,0xf9,0x57,0x89,0x07,0x5a,0x68,0xc9,0xb4,0x60,0x3e,0xde,0xbf,0x98,
0xd0,0xec,0xc8,0xf3,0xe5,0xb2,0xf4,0x5f,0x28,0xe1,0x78,0x82,0xac,0x26,0x93,0x17,
0x5f,0x20,0xa2,0xd1,0xad,0x41,0xd2,0x3f,0xac,0x61,0x3d,0xee,0x0c,0x25,0x93,0xdc,
0x67,0xc3,0xe2,0x12,0x77,0xa6,0x92,0x30,0x96,0x72,0x73,0x59,0xf1,0x0e,0xfd,0x3c,
0x97,0x37,0xbd,0x9d,0x01,0x6e,0x2b,0xfd,0x34,0x49,0xf7,0xa9,0xb2,0x46,0x69,0xaf,
0xa0,0x6c,0x48,0xd7,0xa6,0x53,0xb6,0x0e,0x3f,0xf1,0x40,0x0d,0x8a,0x04,0xce,0xec,
0xb1,0xda,0xd3,0xd9,0x70,0x2d,0xf0,0xdd,0x0c,0x98,0x73,0xfe,0x89,0x19,0x5a,0x41,
0x7f,0xc9,0xb3,0xbe,0x14,0x66,0xca,0xe0,0x6a,0x35,0x0c,0x4a,0x13,0x54,0x43,0x8a,
0x6d,0x48,0x07,0x7a,0xf5,0xfb,0xa9,0x03,0x5e,0x4d,0xa9,0xc9,0xdf,0xfb,0xb2,0xa6,
0x00,0x06,0xc5,0x86,0x36,0x58,0x45,0x80,0xd2,0x87,0x96,0x25,0x03,0x1d,0xfe,0x80,
0x42,0xa3,0xbf,0xd8,0xea,0xe8,0x37,0x5d,0xdc,0x8e,0xf4,0xc0,0x96,0x63,0xab,0x60,
0x8f,0xd4,0x45,0x95,0xa2,0x54,0x44,0x1d,0xab,0xf7,0xfe,0x2e,0xf9,0xbd,0x9f,0x40,
0xf6,0x86,0xab,0x34,0xe0,0x32,0xb8,0x50,0xc3,0x7a,0x64,0x70,0x41,0xed,0xec,0x3a,
0x8c,0xa0,0x37,0x9f,0x32,0x2f,0x4a,0xb7,0x9e,0x4b,0xd3,0xf3,0x8b,0x51,0xcf,0xd6,
0x58,0x9d,0x17,0xdb,0x2e,0x54,0xae,0xa9,0x78,0xa6,0xe6,0x17,0x33,0x72,0xca,0xca,
0x14,0x49,0xe8,0x31,0xfc,0xa6,0x60,0x48,0x1f,0x15,0xcc,0x64,0xbb,0x9a,0xc4,0x51,
0xe2,0x05,0x73,0xa7,0x7c,0x01,0xae,0x3e,0x3e,0x18,0x9b,0x0f,0x0e,0x18,0x1e,0x33,
0xc0,0xa6,0x2e,0x04,0xb0,0xcd,0x81,0xc4,0x18,0x01,0x82,0xf0,0x0e,0x2e,0xf5,0xe7,
0xfa,0x2a,0xc4,0x61,0xf2,0x8e,0xc7,0xbc,0x2b,0x75,0xd4,0xc7,0x08,0xaa,0x4f,0x91,
0x32,0xcc,0x6b,0x55,0x04,0xeb,0x20,0x15,0xbb,0xbc,0x73,0x3a,0x2a,0xce,0xe9,0xea,
0x42,0xd5,0x21,0x04,0xc2,0x08,0x84,0xd1,0xf5,0x45,0x19,0x93,0x64,0x1f,0x82,0x6c,
0x52,0xc7,0x0c,0xff,0xcb,0x16,0x5c,0x8a,0xaa,0x9a,0x71,0x2d,0x88,0x15,0x13,0x4a,
0x4c,0x51,0x7c,0x70,0x91,0x69,0xa0,0x93,0x17,0xac,0x72,0xfd,0x01,0x5a,0x85,0xa2,
0xec,0xa0,0xae,0xdf,0x61,0xc0,0xcc,0x86,0x62,0xdb,0x02,0xa5,0x1a,0xad,0x6b,0x3c,
0x08,0x38,0x01,0xf5,0x42,0xae,0x47,0xb6,0x0b,0x63,0xbd,0xb8,0x26,0x23,0x89,0xf7,
0xba,0x38,0x3b,0x3f,0x81,0x7e,0xd1,0x86,0xfe,0x77,0xb4,0x69,0x13,0x62,0xb9,0xd2,
0xbc,0x36,0xe7,0xe3,0x4d,0xb0,0x16,0xd9,0xb1,0x6c,0xb6,0xaf,0x35,0x14,0xfc,0x66,
0xf4,0x5b,0x18,0x15,0x43,0x8e,0x06,0xe9,0x11,0x9e,0xd0,0x25,0x3a,0x9c,0x28,0x82,
0x8a,0x84,0xe6,0x48,0xb7,0x15,0xc5,0xb3,0xc4,0xde,0x8e,0x99,0x8a,0x18,0xea,0xc9,
0xb4,0x34,0x0d,0x37,0xd2,0x28,0x90,0x8c,0xd6,0xce,0x0c,0xf1,0x0d,0x9e,0xae,0xd2,
0x1d,0xd9,0xe1,0x57,0xc7,0xae,0x67,0xcf,0xfa,0x7d,0x56,0xb1,0xe4,0x44,0xba,0x8b,
0xf5,0xfb,0xca,0x08,0x2a,0xa4,0x8b,0x7c,0x5c,0x50,0x52,0x5d,0x47,0x11,0xb4,0xd4,
0x7b,0x50,0x93,0x40,0x26,0x34,0x21,0x59,0x3e,0x27,0x92,0xb3,0x56,0x20,0xe2,0x73,
0xa6,0x1e,0xe0,0x29,0xb8,0x26,0xb6,0x36,0xa2,0x6a,0xd1,0x48,0x12,0x38,0x3e,0x5c,
0xf1,0x4c,0xcb,0x42,0x53,0x0d,0x61,0x77,0xc5,0x2a,0x6e,0x06,0x47,0x68,0x86,0x43,
0xcd,0xd1,0xf2,0x5d,0xc7,0xf2,0x5e,0xc7,0xf2,0x9f,0x7c,0x26,0x0f,0x56,0x8c,0x52,
0x59,0xbc,0x37,0x5e,0xec,0xf3,0xa8,0x2a,0xcc,0xe5,0x04,0x6d,0x6a,0x96,0x72,0x63,
0x1f,0xe5,0xb4,0xd3,0x6e,0x5c,0x51,0x9a,0xe3,0x2d,0x16,0x5a,0x50,0xd3,0x03,0x35,
0xba,0x6f,0x2c,0x18,0x95,0x1a,0xee,0xb6,0x6f,0x9f,0xba,0xb6,0x1c,0xdd,0x5e,0xaf,
0xb0,0xd2,0x6c,0x88,0x43,0x76,0xa6,0xd1,0xc9,0x50,0xf6,0x6d,0x26,0x13,0xda,0x5b,
0x19,0x62,0x13,0xf0,0xd0,0x44,0xbf,0xfc,0xf6,0xeb,0x7f,};



const struct fsdata_file file__404_html[] = { {
file_NULL,
data__404_html,
data__404_html + 12,
sizeof(data__404_html) - 12,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT,
}};

const struct fsdata_file file__incorrect_command_html[] = { {
file__404_html,
data__incorrect_command_html,
data__incorrect_command_html + 24,
sizeof(data__incorrect_command_html) - 24,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT,
}};

const struct fsdata_file file__index_html[] = { {
file__incorrect_command_html,
data__index_html,
data__index_html + 12,
sizeof(data__index_html) - 12,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT,
}};

const struct fsdata_file file__wifi_settings_html[] = { {
file__index_html,
data__wifi_settings_html,
data__wifi_settings_html + 20,
sizeof(data__wifi_settings_html) - 20,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT,
}};

#define FS_ROOT file__wifi_settings_html
#define FS_NUMFILES 4

