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

/* "Content-Length: 3269
" (18+ bytes) */
0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x4c,0x65,0x6e,0x67,0x74,0x68,0x3a,0x20,
0x33,0x32,0x36,0x39,0x0d,0x0a,
/* "Content-Encoding: deflate
" (27 bytes) */
0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x45,0x6e,0x63,0x6f,0x64,0x69,0x6e,0x67,
0x3a,0x20,0x64,0x65,0x66,0x6c,0x61,0x74,0x65,0x0d,0x0a,
/* "Content-Type: text/html

" (27 bytes) */
0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x54,0x79,0x70,0x65,0x3a,0x20,0x74,0x65,
0x78,0x74,0x2f,0x68,0x74,0x6d,0x6c,0x0d,0x0a,0x0d,0x0a,
/* raw file data (3269 bytes) */
0xed,0x1b,0xd9,0x6e,0x1b,0x39,0xf2,0x59,0x03,0xe4,0x1f,0x98,0x06,0x36,0x92,0x60,
0xeb,0xcc,0x09,0x49,0x36,0x10,0xe7,0x98,0x78,0x90,0xd8,0x83,0x58,0xd9,0xdd,0x41,
0x30,0x10,0x5a,0x6a,0x4a,0xe2,0xa6,0xdd,0xad,0xed,0xa6,0x7c,0xc4,0xf0,0xfe,0xd8,
0x3e,0xec,0x27,0xed,0x2f,0x6c,0xf1,0x6c,0xb2,0x9b,0x6c,0xc9,0x33,0x03,0xec,0xcb,
0xe4,0xc5,0x6a,0xb2,0x6e,0x16,0x8b,0xc5,0x2a,0xe6,0xbf,0xff,0xfe,0xcf,0xe4,0xf1,
0xdb,0xf3,0x37,0xd3,0x5f,0x7e,0x7e,0x87,0xd6,0xf4,0x32,0x3e,0x7e,0xf4,0xc3,0x84,
0xfd,0x45,0x71,0x98,0xac,0x8e,0x02,0x9c,0x04,0x30,0xd2,0x98,0xac,0x71,0x18,0xb1,
0x1f,0x8d,0x09,0x25,0x34,0xc6,0xc7,0x79,0xba,0x24,0x33,0x1a,0xce,0x63,0x3c,0xe9,
0x89,0x11,0x3e,0x79,0x89,0x69,0x88,0x16,0xeb,0x30,0xcb,0x31,0x3d,0x0a,0xb6,0x74,
0xd9,0x79,0x15,0x88,0x99,0x7c,0x91,0x91,0x0d,0xe5,0xbf,0x1b,0xcb,0x6d,0xb2,0xa0,
0x24,0x4d,0x50,0x86,0x57,0x24,0xa7,0x38,0xcb,0x67,0x9b,0x70,0x85,0x67,0x69,0x32,
0x8b,0xd3,0x30,0x6a,0xb5,0xef,0x38,0xd8,0xfd,0xa3,0x1f,0x1e,0xfd,0x80,0x8c,0x7f,
0x31,0xa6,0x68,0x7e,0x3b,0x23,0x11,0x3a,0x42,0x8a,0x46,0x0b,0xc1,0x67,0x1b,0xdd,
0x01,0x2d,0xba,0xcd,0x12,0x14,0xa5,0x8b,0xed,0x25,0x4e,0x68,0x77,0x85,0xe9,0xbb,
0x18,0xb3,0x9f,0x27,0xb7,0xa7,0x91,0x00,0x1b,0xa3,0xfb,0x71,0x95,0xe4,0x22,0xc3,
0x21,0xc5,0xfb,0xd0,0x14,0x90,0x92,0xac,0x49,0x12,0xa4,0x65,0x94,0xc2,0x28,0xca,
0x80,0x4e,0x10,0x14,0x43,0x97,0xe1,0xcd,0x0c,0xd4,0xcc,0x67,0xc9,0xf6,0x72,0x6c,
0x6b,0x3f,0x27,0xb4,0xb5,0x49,0x73,0xc2,0x3e,0xb8,0xca,0x4c,0xa0,0x46,0x43,0x32,
0x1d,0xa0,0xc9,0x04,0xa9,0xe9,0xb1,0x32,0x88,0x89,0xbf,0xc9,0x08,0x48,0x71,0x89,
0xf3,0x1c,0x8c,0x27,0x6d,0xd6,0x58,0xa4,0x49,0x9e,0xc6,0xb8,0x1b,0xa7,0x2b,0x3d,
0xa5,0xb1,0x4d,0xcd,0x35,0x19,0x30,0xd4,0x2c,0x8c,0xe3,0xd9,0x22,0x26,0xa0,0x15,
0x17,0x96,0xaf,0x00,0x2a,0xfd,0xe3,0x96,0x6f,0x05,0x0c,0x5c,0x82,0x5e,0xc1,0x3a,
0x07,0xed,0x6e,0x4e,0x6f,0x63,0x66,0xbe,0x20,0x22,0xf9,0x26,0x0e,0x6f,0x47,0x28,
0x49,0x13,0x1c,0x8c,0xbd,0x24,0xb8,0xdf,0x48,0x22,0x4e,0x7c,0x92,0xc4,0xa4,0x4a,
0xa1,0xa4,0xbe,0x92,0x7b,0x1f,0x81,0x7f,0x97,0xa4,0x3b,0x44,0x64,0x56,0x37,0x57,
0x19,0xc0,0x18,0x4b,0xf5,0xd9,0x92,0xe6,0x6f,0x88,0xe5,0x0a,0xae,0x49,0x1c,0xa3,
0x39,0x06,0xdf,0xfa,0xe7,0x16,0xe7,0x14,0x05,0x07,0x26,0xf2,0x41,0x50,0x6c,0x8a,
0x40,0x61,0x32,0x3f,0x22,0x40,0xb6,0xef,0x10,0x94,0xcd,0x6d,0xc2,0xc5,0x37,0x98,
0x96,0x1e,0xd0,0x08,0x24,0xe9,0x60,0x14,0x30,0xb2,0xc1,0x61,0x31,0xbe,0x62,0x3c,
0x60,0x7c,0x50,0x0c,0x32,0x59,0xe9,0xed,0x06,0xc3,0xe8,0x72,0x1b,0xc7,0xc5,0x04,
0x49,0x22,0x7c,0x13,0x8c,0x2e,0x28,0xc8,0xbd,0x6a,0x91,0xb6,0x9c,0xb8,0x77,0xcb,
0x90,0xe1,0x7c,0x03,0xae,0x87,0x6d,0x65,0xb9,0x64,0x9b,0x0c,0x5f,0x11,0x7c,0x3d,
0x0a,0x6c,0x4b,0xb0,0x39,0x35,0xb2,0x4c,0x33,0xd4,0x12,0x2a,0x82,0xa6,0x13,0x7b,
0xd7,0x20,0x72,0x70,0xa0,0xdc,0xbb,0xc1,0xb0,0xba,0x5c,0x34,0x80,0xd6,0xb2,0x49,
0x32,0x0d,0x25,0x05,0xcc,0xe5,0x38,0x89,0x66,0xff,0xc8,0x61,0x37,0x9b,0x8c,0x1a,
0x64,0xd9,0x52,0x40,0xdd,0x24,0xbc,0x04,0xc8,0x23,0x58,0xd6,0x4d,0x98,0xe7,0xb3,
0x6f,0xf8,0x36,0xd0,0x7c,0x1a,0x11,0x08,0xbd,0xc0,0x33,0x35,0x03,0x14,0x35,0xde,
0x55,0x18,0x6f,0xf1,0x93,0xfe,0xcd,0x7b,0xf9,0x6f,0xec,0xc1,0x99,0x81,0x9c,0x26,
0x1e,0x17,0x5b,0x03,0x4b,0x13,0x19,0x38,0x23,0x14,0xa0,0x03,0x54,0x22,0xa2,0x25,
0xbf,0x97,0x7f,0x21,0xc8,0xcc,0xa2,0x90,0x86,0x33,0x9a,0x8a,0x08,0xac,0x15,0x52,
0xa0,0xf7,0xae,0x68,0x51,0x45,0x63,0x5f,0x4a,0x5f,0x30,0x8b,0x74,0x79,0x2e,0xe4,
0x2c,0x38,0x60,0xb3,0x42,0xe2,0x36,0xd8,0x08,0x25,0xe0,0x1b,0xda,0x38,0x6c,0xc1,
0xe9,0x3c,0x8d,0x98,0x55,0x4a,0x3b,0xa5,0x08,0xbb,0xf9,0xc9,0xed,0x34,0x5c,0x9d,
0x81,0x8d,0x5b,0xcd,0xe9,0xc9,0xf9,0xdb,0x5f,0x9a,0xed,0xaf,0xfd,0x5f,0xc7,0x06,
0x09,0xb6,0x83,0x80,0x82,0x88,0xaa,0xad,0x60,0xfa,0x59,0x7b,0x48,0x83,0x4d,0x75,
0x79,0xa0,0x0f,0xd8,0x4f,0x4b,0x1e,0xb9,0x0b,0x34,0x98,0xde,0x9c,0x73,0x58,0xe8,
0x55,0x96,0x6e,0x93,0xa8,0xb3,0x48,0xe3,0x34,0x1b,0xa1,0x1f,0xd7,0x69,0x4e,0xff,
0xb6,0x26,0x54,0xef,0x54,0x03,0xa5,0x7b,0x45,0x72,0x32,0x27,0x31,0xa1,0x4c,0x8f,
0x80,0x7f,0xc5,0x05,0x20,0x57,0xb0,0x1b,0x6e,0x36,0xe0,0x48,0x6f,0xd6,0x24,0x8e,
0x5a,0x0c,0x55,0x4b,0xc8,0x02,0x2d,0x9c,0x1d,0xa0,0x28,0xe6,0x31,0x9f,0xe2,0x1b,
0xda,0x09,0x63,0xb2,0x4a,0x46,0x72,0x74,0x0c,0x1b,0x33,0x8a,0xc0,0x41,0x47,0xe8,
0xf9,0xe6,0x26,0xb0,0x11,0x63,0xbc,0xa4,0x65,0x34,0x36,0xe6,0x41,0xe2,0x16,0x8f,
0x66,0xca,0xf1,0xb5,0xc9,0xde,0x16,0x26,0x53,0xd3,0xd2,0x6c,0xd5,0x75,0xac,0x00,
0x2a,0xc3,0x49,0x71,0xab,0x1b,0x5b,0x6d,0x6e,0xc0,0x10,0x7b,0xc5,0xc7,0x98,0xcd,
0x4a,0xbe,0xec,0xa7,0x8f,0x2d,0x07,0x53,0x5c,0xb9,0xb6,0xb5,0x3c,0x59,0x58,0xf2,
0xf3,0x64,0xb3,0x92,0x27,0xfb,0xe9,0xe3,0xc9,0xc1,0x1e,0xa2,0xe9,0x65,0x1a,0xcd,
0xb7,0xf9,0x4c,0x6f,0x1c,0xaf,0x00,0x25,0x40,0x29,0x4b,0x69,0xd4,0x27,0x56,0x19,
0xf9,0x41,0x12,0x46,0xf3,0x99,0x4c,0x35,0x7c,0xa2,0x49,0x08,0x5b,0x26,0x36,0x02,
0x29,0x81,0x57,0x24,0x85,0xf4,0x10,0x59,0x96,0x71,0xb8,0xca,0xfd,0x82,0xf0,0x69,
0x29,0x05,0xff,0xed,0x63,0x2e,0x00,0xf7,0xf7,0x8d,0x08,0x43,0x46,0xe9,0xe7,0xcb,
0xa7,0x25,0x5f,0xfe,0xdb,0xc7,0x57,0x00,0xee,0xcf,0x97,0xc7,0x7e,0x3f,0x5f,0x3e,
0x2d,0xf9,0xf2,0xdf,0x3e,0xbe,0x02,0x50,0x87,0xae,0xfa,0x30,0x30,0x46,0xd7,0x69,
0x16,0x75,0xae,0xb3,0x70,0x33,0x42,0x73,0xe0,0xfc,0xad,0xc3,0x06,0x82,0x0a,0xbd,
0x45,0x0c,0xa7,0x46,0xc1,0x1b,0xa2,0x60,0x50,0xaf,0x4e,0x16,0x5e,0xef,0x52,0x49,
0x83,0x48,0xb5,0xf4,0xb7,0x4f,0xb5,0x02,0x61,0xff,0x7c,0xcb,0x10,0x69,0xbe,0xa5,
0x14,0x42,0xa4,0x5f,0x20,0x09,0x20,0xc5,0x91,0x5f,0x3e,0x61,0x14,0xf0,0xfe,0x2b,
0x0c,0xd7,0x97,0x64,0x65,0xda,0xe3,0xe4,0xcb,0x74,0x7a,0x7e,0x66,0x89,0x20,0x60,
0x2c,0x09,0x7c,0x02,0x48,0x50,0xb8,0x10,0xbd,0xa6,0x90,0xae,0x00,0x30,0x90,0xe4,
0x09,0xd7,0xa1,0x42,0x75,0x91,0x4e,0x13,0xc8,0x8f,0x79,0x5a,0xa7,0x6f,0x25,0x70,
0x23,0x11,0x93,0x2c,0x35,0x6a,0x19,0xe1,0xfe,0x10,0x95,0x96,0xa9,0x7d,0x5f,0x25,
0x68,0x1e,0x64,0xa5,0x0b,0xcd,0x14,0x9c,0xef,0x2c,0x8d,0x40,0xae,0x37,0x42,0x77,
0x4e,0x27,0x68,0xdb,0xa7,0xb1,0x49,0x40,0x31,0xaf,0x85,0x60,0xd1,0xbe,0x16,0x80,
0x59,0xa1,0x16,0xa0,0x14,0x24,0xeb,0x61,0x65,0xf4,0xaa,0x05,0xe2,0x51,0xa6,0x16,
0x82,0xc7,0x83,0x5a,0x08,0x6e,0x9c,0x5a,0x08,0xbd,0x14,0xb5,0x50,0xd2,0x33,0xcd,
0xcc,0xf4,0x5f,0x2c,0x19,0x17,0x61,0x92,0x05,0x04,0x9e,0xa1,0x75,0x85,0xcc,0xc2,
0xaf,0xce,0x97,0x2d,0xc8,0xe2,0xc3,0x08,0xae,0xca,0x31,0x24,0xab,0x3a,0x21,0xbb,
0xc7,0x71,0x8e,0x75,0xea,0x6a,0xb8,0x7d,0x89,0xa7,0xf0,0x85,0x72,0x4a,0x79,0xaf,
0x73,0xc0,0xc2,0x83,0xd1,0xe3,0x22,0xe7,0x73,0x6d,0x16,0x7f,0xb6,0x08,0x7f,0x12,
0x9c,0x7d,0x98,0x7e,0xfa,0x08,0xce,0x5b,0xd9,0x12,0x25,0x5e,0x3c,0xad,0xd8,0x8f,
0x55,0x25,0xb1,0x70,0x70,0x62,0x30,0x6e,0x46,0x3c,0x97,0xd8,0x8f,0x51,0x25,0x9b,
0xb0,0x19,0xa9,0x0b,0x93,0xb1,0x44,0xa6,0x23,0x97,0xf8,0x96,0xb3,0x89,0xc7,0x8e,
0x54,0x5a,0xe7,0x0a,0xe0,0xc3,0x2c,0xa0,0xac,0xd3,0x98,0x85,0xfe,0x99,0xba,0x09,
0x06,0x12,0x36,0xbf,0x26,0x74,0xb1,0x46,0x4e,0xc2,0x6d,0x7d,0xfd,0x6b,0x2c,0xc2,
0x1c,0xb7,0x06,0xed,0x91,0xfa,0x6e,0x94,0xe9,0x2f,0x52,0x12,0x57,0x88,0x37,0x1a,
0xfc,0x60,0xb1,0x88,0x0c,0x6b,0x88,0x40,0x48,0x87,0xe8,0x41,0xf1,0x5e,0x84,0x9e,
0xd6,0x10,0xf2,0x69,0xeb,0xa2,0xf3,0xac,0x86,0x0e,0x49,0x36,0x5b,0xba,0x8b,0x4a,
0x84,0x97,0xe1,0x36,0xa6,0x0f,0x97,0xe6,0xbe,0xd6,0x65,0xea,0x92,0x3e,0xdb,0x7b,
0x4c,0x7e,0xb5,0x1e,0x23,0x93,0xb5,0x8a,0xc3,0xd8,0x1c,0x5d,0x29,0x9d,0x63,0x5f,
0xd8,0xd0,0x6e,0xbe,0x22,0x93,0xdb,0x6f,0x8b,0x54,0x53,0xb9,0xea,0x1e,0x71,0xc7,
0x31,0x37,0x6f,0x1e,0x75,0xa1,0x3e,0x58,0xda,0x21,0x35,0x12,0x54,0x93,0x3a,0x87,
0xda,0x06,0x59,0x37,0x5f,0x91,0xfa,0xec,0xc7,0xb1,0x9a,0xf3,0x54,0x75,0x16,0x30,
0x1b,0x56,0xff,0x34,0x62,0xe9,0x21,0x2a,0x98,0xb5,0xc7,0xb5,0x4c,0xdc,0xc9,0x95,
0x43,0x35,0x0e,0x54,0x77,0xdf,0x2f,0x8b,0x43,0x84,0x14,0xca,0x8f,0x58,0xdc,0x91,
0x77,0x3a,0x3b,0xbe,0x12,0x83,0xd9,0xd8,0xb2,0x96,0x28,0x95,0x24,0x61,0x52,0x54,
0x49,0x64,0xad,0x32,0x38,0x7b,0x7d,0xa6,0x92,0x51,0xeb,0x28,0x52,0x05,0x54,0xc0,
0xde,0x25,0x6b,0x51,0x2f,0x33,0x24,0xac,0xaf,0x6c,0xf1,0x62,0xd6,0xa1,0xb5,0xb0,
0xb2,0xa8,0xa2,0x6b,0x71,0x0a,0xa1,0xed,0x29,0x9d,0xb1,0x02,0xd1,0x6c,0x67,0xd5,
0x08,0x0c,0x60,0x03,0x3e,0x66,0x96,0xf8,0x2c,0x68,0xcf,0x70,0x96,0xa5,0x59,0xc5,
0x26,0x16,0x42,0xd7,0x2e,0x01,0xdb,0x46,0x92,0x32,0x5f,0xe0,0xec,0x0a,0x4a,0x09,
0x24,0x87,0x24,0x59,0xd5,0xd3,0x58,0x28,0x2a,0xb2,0xc3,0x30,0xc6,0xd9,0x5e,0x80,
0x52,0x84,0xfe,0x2e,0x9b,0x17,0xc7,0x18,0x3f,0xc1,0xee,0xdc,0x36,0xd2,0x50,0x92,
0x9c,0x38,0x86,0x34,0x8a,0x0a,0xce,0x68,0xa0,0xa2,0xaa,0x46,0x60,0x01,0xf5,0xcb,
0x2b,0x7d,0x4d,0xe1,0x91,0x78,0x6c,0x60,0x0c,0xdd,0x18,0x83,0x17,0x7e,0x94,0x67,
0x6e,0x94,0xa7,0x43,0x3f,0xca,0x2b,0x37,0xca,0x8b,0x67,0x7e,0x94,0xc1,0x0b,0x27,
0xce,0x69,0x8d,0x2e,0x4f,0xdd,0xca,0x5c,0xd4,0x29,0xf3,0xc2,0xad,0xcd,0x45,0x9d,
0x36,0x83,0xa1,0x5b,0x9f,0x8b,0x3a,0x7d,0x86,0xcf,0xdd,0x0a,0xbd,0xff,0x78,0xfe,
0x7a,0xea,0x47,0x7b,0x3e,0x70,0x2b,0xf5,0xf6,0xfc,0xcb,0xc9,0xc7,0x77,0x35,0x32,
0xf6,0x87,0x6e,0xcd,0x4e,0xcf,0x6a,0xb8,0x0d,0xfb,0xcf,0xdc,0xaa,0x4d,0x4f,0x3f,
0xb9,0x99,0x95,0x4e,0x73,0x0b,0xe9,0xdd,0xe7,0xcf,0xe7,0x9f,0x03,0x07,0x92,0x8c,
0x14,0x72,0x8f,0x94,0x9c,0xdb,0xb1,0x49,0x8c,0x73,0x4c,0x1c,0x61,0x9e,0x6d,0x52,
0xc0,0x19,0x2d,0x20,0x16,0x3a,0xc4,0xc1,0xfa,0x84,0xf7,0x7b,0x06,0x90,0xb3,0x1b,
0x80,0x07,0xec,0x3e,0xad,0xf2,0x79,0x14,0x8c,0xef,0x9d,0x48,0xc3,0x2a,0x52,0x1e,
0x5e,0xe1,0x08,0x05,0x6e,0xf8,0xe7,0x55,0x78,0x38,0x06,0x29,0x59,0x84,0xb1,0x46,
0x91,0xea,0x17,0x60,0x4e,0xfd,0x8d,0xfb,0xa6,0x3c,0xc9,0xc4,0x21,0x76,0x67,0x45,
0x5b,0x3e,0x05,0xc5,0xeb,0x03,0x71,0x92,0x94,0x8f,0xac,0x52,0x03,0x84,0x53,0x28,
0xa0,0x05,0x41,0x1b,0xda,0xdd,0x60,0x58,0xcd,0x9c,0x07,0x96,0x87,0x69,0xe5,0x14,
0x2b,0x8e,0x06,0x7e,0xf0,0x31,0x09,0x14,0xcd,0xb6,0xbf,0x03,0xc4,0x40,0xe0,0xee,
0x2e,0xae,0xbb,0x50,0xdf,0x56,0xc5,0x12,0x37,0x57,0xc9,0x8a,0xd5,0xda,0xc5,0xd9,
0xcb,0x2e,0x7e,0x79,0xcb,0x6f,0x17,0x0f,0x57,0x7e,0x0c,0x41,0x53,0xe4,0xf2,0x01,
0xed,0xb1,0x92,0xd0,0x4c,0xaf,0xc0,0x4e,0x1c,0xd4,0xb0,0x1f,0x71,0x19,0x46,0x7b,
0xb4,0xbb,0x1c,0x9d,0x05,0x53,0x5b,0xa1,0xe7,0xdd,0x1f,0xd4,0xab,0x1a,0xb8,0x3a,
0x55,0x6c,0xbb,0x08,0x9e,0x95,0x7e,0x95,0xf0,0xd3,0xda,0x5e,0x95,0x48,0x66,0x1c,
0x67,0xbd,0x0b,0x98,0xeb,0x04,0xd0,0x83,0xb1,0x23,0x75,0xec,0xc6,0x38,0x59,0xd1,
0x75,0x71,0x9b,0x93,0xc0,0x15,0x88,0xb1,0x2b,0x4b,0x09,0xb3,0x2c,0xbc,0x45,0x39,
0xf9,0xce,0x9d,0x91,0xe3,0xfa,0xa4,0x28,0x75,0x5b,0x94,0x57,0xd6,0x76,0x5d,0x02,
0xde,0x75,0x09,0x8c,0xae,0xcb,0x35,0xd4,0x01,0x70,0x4b,0x34,0x36,0x16,0xac,0x26,
0x20,0x91,0xde,0x40,0xcf,0x84,0x6a,0x25,0xc4,0x7c,0x86,0x2f,0xd3,0x2b,0x2c,0x2b,
0x07,0x05,0x06,0xab,0x13,0xe5,0x40,0xd1,0x48,0xe4,0x5d,0xf2,0x12,0x4f,0x63,0x8f,
0x2b,0xa9,0x3b,0x7a,0xbe,0x2a,0xdc,0x83,0x1a,0x43,0x64,0x77,0xc3,0xa6,0xae,0xc7,
0x20,0xe2,0xb5,0xaf,0xc7,0x00,0xb3,0xaa,0xc7,0x00,0x3f,0x2d,0x6e,0x72,0xd6,0x59,
0xc2,0xcd,0xc8,0x6a,0x0d,0x35,0xdc,0x6b,0x12,0xd1,0xf5,0x08,0xbd,0xea,0x1b,0x9d,
0x1c,0x85,0xb7,0x57,0x3d,0x0e,0x5c,0x88,0xf1,0x3c,0x08,0xc0,0x41,0xea,0xcb,0x71,
0x8c,0xe6,0x0e,0x55,0xf9,0x05,0xb9,0xae,0x79,0x04,0xd3,0x4a,0x59,0xfe,0x69,0xaa,
0xeb,0x2e,0x01,0x02,0x46,0x1d,0xd3,0x32,0xc7,0xd3,0xb3,0x9f,0xbf,0x4c,0x0b,0xa6,
0x82,0xa3,0xec,0xf2,0x04,0xb0,0xdb,0xe7,0x70,0xdf,0xb6,0x27,0xb5,0x75,0xa5,0x29,
0x07,0xcf,0x99,0x2d,0x2d,0x10,0x79,0x28,0x28,0x5b,0xd9,0xf8,0xaa,0x17,0xc6,0x4a,
0x03,0xe6,0x64,0xdd,0x26,0x96,0xa8,0x2a,0xd2,0x17,0x80,0x5f,0x89,0x6e,0x5f,0xda,
0x05,0x37,0x1f,0x42,0xb9,0x75,0xab,0xad,0x6c,0x1a,0x52,0x5b,0xd1,0xb8,0x0c,0x14,
0x9a,0xf1,0xa5,0x3f,0x30,0x18,0xd4,0x36,0x79,0xe5,0xa5,0x83,0x07,0x87,0x16,0x3b,
0x3f,0xbc,0x39,0xfd,0xef,0x8e,0x27,0x0f,0x0a,0xee,0x4c,0x94,0xae,0xfc,0x12,0x6a,
0x94,0xc3,0x36,0x87,0x10,0x05,0x6d,0x7b,0x5e,0x44,0xf9,0xd1,0xd7,0x5f,0xed,0x8b,
0x1e,0xc0,0xb7,0x8a,0x77,0x11,0x3c,0xb6,0x78,0xe2,0x9a,0xe3,0xfd,0x80,0x58,0xf6,
0xcd,0x36,0x5f,0x17,0x5d,0x6f,0xe9,0x21,0x6d,0x97,0x95,0x1b,0xee,0x6b,0xe1,0x9a,
0x44,0x11,0xd6,0xf6,0xe6,0xe7,0x75,0xab,0xed,0x3c,0x22,0x9d,0x90,0x77,0xbf,0xf1,
0xf8,0xdf,0xf1,0x66,0xc5,0x7b,0x86,0x2b,0x3c,0xf5,0xb4,0xca,0x14,0xb0,0xa4,0xe0,
0xdd,0x1e,0xb7,0xe5,0xba,0x1b,0xb5,0x74,0x84,0x9f,0x2e,0xce,0xcf,0x40,0x10,0xf6,
0x48,0x83,0x2c,0x6f,0x6b,0x8f,0x59,0xf5,0x1c,0x06,0x4a,0x31,0xf8,0x1a,0xfd,0xfd,
0xd3,0xc7,0x0f,0x94,0x6e,0xe4,0x0d,0x5b,0x3f,0xa1,0x51,0x1e,0x94,0xc2,0xee,0x69,
0x35,0x7f,0x7c,0x37,0x6d,0x1e,0xf2,0x37,0x57,0x07,0xcd,0x1e,0x4b,0x0e,0x18,0xeb,
0xee,0x62,0x45,0x60,0x74,0x19,0xc6,0xc5,0x7b,0x08,0x85,0x06,0xa6,0x97,0x14,0x3f,
0x40,0xce,0x8d,0xb3,0x56,0xf3,0x27,0xc0,0x68,0x1e,0x6a,0x91,0x77,0x23,0xbc,0x5e,
0x2c,0xf0,0x86,0x36,0x0f,0x9b,0x2c,0xde,0xf6,0xd8,0x43,0xb9,0x66,0x15,0x29,0x89,
0x5a,0x45,0xe5,0x00,0xb5,0xd4,0x04,0x4b,0xf4,0x6f,0x2f,0x28,0x7f,0x6b,0x06,0xf5,
0x83,0x67,0xe8,0xc9,0x13,0xa4,0x91,0x60,0x78,0x9b,0xf3,0xf1,0x61,0xbf,0xdf,0x2e,
0x95,0x06,0x72,0x71,0xe3,0x57,0x96,0x17,0xa9,0xab,0x22,0x2a,0xc6,0xa6,0x32,0xfe,
0x73,0xb4,0x72,0x55,0x83,0xaf,0x83,0x28,0x03,0xd5,0xe2,0x79,0xd8,0x05,0x25,0x00,
0x8b,0x7c,0xbb,0xa6,0xa4,0x21,0x8d,0x87,0x64,0x85,0xc4,0x27,0x5e,0xa9,0x92,0xe2,
0xba,0xa6,0x95,0x3d,0x4f,0x01,0x4c,0x7a,0xc6,0xc3,0xc2,0x89,0xf0,0x79,0x76,0xa0,
0x1c,0xf1,0xe3,0xb8,0xb7,0xc8,0x73,0xf1,0xfe,0xb0,0xd1,0x65,0xdb,0xa9,0xb3,0x49,
0x37,0xdb,0x8d,0x14,0xd2,0xda,0x17,0xea,0x8e,0x20,0xdf,0xdb,0x8d,0xd0,0x92,0xdc,
0xe0,0x48,0x8e,0xd2,0x14,0xda,0xae,0xec,0xe0,0xd1,0xef,0xb1,0x96,0x94,0x0d,0x14,
0x23,0xba,0x59,0x3b,0x2c,0xc6,0xe6,0xd0,0xa1,0xc5,0x59,0x87,0xcb,0x34,0x42,0x79,
0x1a,0x93,0xc8,0x9e,0x91,0x47,0x1a,0x5d,0x13,0x55,0x9d,0xac,0x3e,0x60,0xb9,0x66,
0x6f,0x57,0xe4,0xec,0xf7,0x8e,0xbc,0x68,0x0d,0xcd,0x18,0xd3,0x9d,0xc7,0x80,0xd5,
0x81,0x5c,0x2d,0x03,0x6d,0x7e,0x9b,0x6e,0xfd,0xbf,0x58,0x9a,0xe9,0x4f,0x75,0xe8,
0xf6,0xf5,0xc8,0x1a,0xb3,0xcc,0xc6,0x1a,0xaa,0x0a,0xcd,0x25,0x2a,0x0b,0xad,0xf2,
0xe7,0xce,0x65,0xfa,0xbd,0x93,0xc2,0x6e,0x23,0x14,0xe4,0xeb,0x77,0x5f,0xca,0x71,
0x35,0xd4,0x7d,0xa9,0xca,0x56,0x4b,0x12,0x43,0x01,0x7e,0x84,0xc2,0x78,0xb3,0x0e,
0x5b,0x72,0xfe,0xe8,0x65,0xbf,0x6d,0xab,0x2f,0x3a,0x5d,0xd6,0xd2,0x8a,0x05,0x2b,
0xaf,0x57,0x75,0xb5,0x8a,0xa1,0x5a,0xcb,0x2b,0x4f,0x63,0x2b,0xc9,0x5f,0xc3,0xf6,
0xd4,0x73,0xd8,0x09,0x3f,0x46,0xe1,0x36,0x92,0x86,0xd1,0x51,0xe0,0x7b,0xce,0x2a,
0x9f,0xc0,0xae,0x07,0xc7,0xd7,0x64,0x49,0x10,0x4d,0x51,0x1e,0x93,0x0d,0x34,0xf2,
0x49,0xb4,0xc2,0x40,0x6c,0x20,0xe6,0x23,0x72,0x85,0x08,0x90,0x91,0x1a,0x05,0x88,
0x77,0xf3,0xf5,0xb7,0xd0,0x50,0xba,0xf3,0x44,0xa4,0x56,0x5c,0xa4,0xa3,0xca,0x35,
0x0d,0x35,0x18,0x1d,0xc6,0x8c,0x1d,0x37,0x14,0x74,0x05,0x6a,0x62,0x5b,0xc8,0x8e,
0xb3,0xb8,0xc7,0x57,0x60,0x64,0xd7,0xf9,0x28,0x88,0xd3,0x45,0xc8,0xdf,0xa5,0xac,
0x33,0xbc,0x3c,0x6a,0xf6,0x2c,0xb8,0x2e,0x8f,0x7b,0x52,0xab,0x1e,0x88,0x2d,0x7e,
0xcd,0xb3,0x63,0x16,0xdb,0x77,0x08,0xa7,0xb3,0x62,0xee,0x42,0x48,0x39,0xd4,0x90,
0xed,0x26,0x29,0xb8,0x7e,0xb1,0x29,0x4a,0x3a,0x15,0xa1,0xdf,0xf0,0xce,0xf8,0x1a,
0x67,0x98,0xd9,0x12,0xa0,0x51,0x7a,0x9d,0xc0,0x64,0x46,0x58,0xfa,0x62,0xaa,0x61,
0xbf,0x11,0x0d,0x94,0x7c,0x3c,0xcd,0xe1,0xb6,0x16,0x09,0x8f,0x12,0xd5,0x92,0x4c,
0xc9,0xcd,0x8f,0x4d,0x89,0x09,0xa8,0x72,0xe9,0xc5,0xfb,0xb3,0x09,0xcd,0x14,0x6e,
0xed,0xe3,0x33,0x8d,0xc0,0x08,0x94,0xb8,0xf1,0x8c,0xf6,0xf8,0x94,0xed,0x92,0x49,
0x8f,0xae,0xeb,0x40,0x45,0xf6,0x7b,0xcc,0xb2,0xb1,0x5d,0xa0,0x2f,0x39,0xe4,0x14,
0xcc,0xb7,0x1f,0xe4,0x27,0xde,0x21,0xd2,0xed,0xa9,0xbd,0x91,0x4e,0x00,0x49,0xb6,
0x95,0xf6,0x94,0xfe,0x3d,0x2b,0x39,0xed,0x82,0x7d,0x2a,0x60,0xdf,0x16,0x0d,0x9c,
0x3d,0x31,0xfe,0xca,0x5c,0xc4,0x0b,0x6b,0xe7,0x42,0xc7,0xba,0xdb,0xe2,0x45,0xb8,
0x24,0x89,0x8a,0xd3,0x03,0x7e,0x8d,0x3b,0xb6,0x40,0xe1,0x23,0xd3,0xae,0xd1,0xb3,
0x7c,0x63,0xc2,0xd3,0x51,0x63,0xd2,0xf8,0x84,0x2f,0xe6,0x77,0x7f,0xe0,0x76,0x31,
0x5f,0x64,0xef,0xbd,0x6b,0x04,0x52,0x5e,0xbb,0x73,0xac,0x57,0xe1,0xbe,0x0d,0xa4,
0xde,0x72,0xff,0xb9,0x8f,0xfe,0xdc,0x47,0xff,0x9f,0x7d,0xa4,0x0f,0x50,0x79,0x66,
0x16,0xd9,0x5e,0xc0,0xdd,0xb4,0xb8,0x51,0xc9,0xf3,0x73,0x3d,0xe0,0xe3,0xba,0x52,
0xaa,0x04,0x75,0x3c,0xe3,0x65,0xb2,0xca,0x33,0xba,0x31,0x61,0x34,0x34,0xa6,0xba,
0xcb,0x05,0xbc,0xa2,0x5c,0x1e,0x3b,0x9e,0x3c,0xee,0x74,0x50,0xc9,0xbc,0x43,0x6e,
0x5e,0xd4,0xe9,0x08,0x7a,0x6a,0xf3,0x17,0x39,0x6b,0x41,0x4b,0xdc,0x9d,0xd5,0xf6,
0xe5,0x1d,0x44,0x20,0x1e,0xc8,0xcb,0xa4,0xc2,0x2f,0xb6,0xa2,0x7d,0x89,0x97,0xf3,
0xc5,0x4e,0x6b,0xc8,0xcf,0x4c,0xff,0x86,0x8f,0xe8,0x58,0x0a,0x60,0x12,0x10,0x57,
0x72,0x29,0x93,0x2c,0xcb,0x48,0xa9,0xe4,0x94,0xc3,0x21,0x8b,0x8d,0x2e,0x9d,0x02,
0x16,0xc7,0x60,0xdb,0x2b,0xf8,0x16,0x0b,0x2c,0x85,0x90,0x2b,0xaa,0x26,0x8d,0xcf,
0x62,0x81,0x2b,0xb6,0x2a,0x47,0xb7,0x30,0x59,0xe0,0xb8,0x2c,0xd9,0xab,0x21,0x33,
0xb6,0x0e,0x6a,0xce,0x6b,0x78,0x50,0x47,0xdd,0xb9,0xf5,0x14,0xcf,0x0b,0xd6,0x43,
0x87,0xdb,0x2a,0x6b,0x29,0x88,0x32,0x45,0xc1,0xca,0xaa,0xc4,0xf8,0xfe,0x53,0x52,
0xd3,0x94,0xa4,0xd9,0x6e,0x9b,0x46,0x9b,0xf4,0xd8,0x60,0x39,0xc7,0x52,0x39,0x22,
0xbf,0xe4,0xeb,0x04,0xd1,0xbc,0x01,0x30,0x1a,0x12,0x7c,0xd2,0x93,0xa6,0x9c,0xf0,
0x9b,0xea,0xf1,0xff,0x00,};

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

