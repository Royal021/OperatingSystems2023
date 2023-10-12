#pragma once

#define SUCCESS     0
#define EIO         -1      //generic I/O error
#define EMFILE      -2      // reading max files
#define ENOENT       -3        //no such directory entry
#define EINVAL         -4
#define ENOSYS         -5
#define COMMAND_IS_MULTIBLOCK                   ( 1<<5  )
#define COMMAND_WRITES_TO_CARD                  ( 0     )

#define COMMAND_READ_SINGLE                     ( COMMAND_INDEX(17) | COMMAND_INVOLVES_DATA | COMMAND_HAS_RESPONSE_6_BYTES | COMMAND_READS_FROM_CARD    )
#define COMMAND_WRITE_SINGLE                    ( COMMAND_INDEX(24) | COMMAND_INVOLVES_DATA | COMMAND_HAS_RESPONSE_6_BYTES | COMMAND_WRITES_TO_CARD     )

