/***************************************************************************//*!
*
* @file   freemaster_cfg.h
*
* @brief  FreeMASTER Serial Communication Driver configuration file
*
*******************************************************************************/

#ifndef __FREEMASTER_CFG_H
#define __FREEMASTER_CFG_H

#include "MKL27Z644.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

//! Select interrupt or poll-driven serial communication
#define FMSTR_LONG_INTR         0   //!< Complete message processing in interrupt
#define FMSTR_SHORT_INTR        0   //!< Queuing done in interrupt
#define FMSTR_POLL_DRIVEN       1   //!< No interrupt needed, polling only

//! Select communication interface
#define FMSTR_DISABLE           0   //!< To disable all FreeMASTER functionalities
#define FMSTR_USE_SCI           0   //!< To select UART/LPSCI communication interface
#define FMSTR_USE_LPUART        1   //!< To select LPUART communication interface */
#define FMSTR_USE_FLEXCAN       0   //!< To select FlexCAN communication interface */
#define FMSTR_USE_USB_CDC       0   //!< To select USB CDC Class Driver - virtual serial

//! Define communication interface base address or leave undefined for runtime setting
#undef FMSTR_SCI_BASE   //!< SCI base will be assigned in runtime (when FMSTR_USE_SCI)
// #undef FMSTR_CAN_BASE   //!< CAN base will be assigned in runtime (when FMSTR_USE_FLEXCAN)
#define FMSTR_SCI_BASE          LPUART0_BASE

//! FlexCAN-specific, communication message buffers
#define FMSTR_FLEXCAN_TXMB      0
#define FMSTR_FLEXCAN_RXMB      1

//! Input/output communication buffer size
#define FMSTR_COMM_BUFFER_SIZE  0   //!< Set to 0 for "automatic"

//! Receive FIFO queue size (use with FMSTR_SHORT_INTR only)
#define FMSTR_COMM_RQUEUE_SIZE  32  //!< Set to 0 for "default"

//! When RAM resources are limited, exclude some of the FreeMASTER features by default
#ifdef FMSTR_SMALL_RAM_CONFIG
#define FMSTR_DEMO_ENOUGH_RAM   0
#define FMSTR_DEMO_ENOUGH_ROM   1
#define FMSTR_DEMO_LARGE_ROM    0
#else
#define FMSTR_DEMO_ENOUGH_RAM   1
#define FMSTR_DEMO_ENOUGH_ROM   1
#define FMSTR_DEMO_LARGE_ROM    1
#endif

//! Support for Application Commands
#define FMSTR_USE_APPCMD        FMSTR_DEMO_ENOUGH_ROM  //!< Enable/disable App.Commands support
#define FMSTR_APPCMD_BUFF_SIZE  32  //!< App.Command data buffer size
#define FMSTR_MAX_APPCMD_CALLS  4   //!< How many app.cmd callbacks? (0=disable)

//! Oscilloscope support
#define FMSTR_USE_SCOPE         FMSTR_DEMO_ENOUGH_RAM   //!< Enable/Disable scope support
#define FMSTR_MAX_SCOPE_VARS    8   //!< Max. number of scope variables (2..8)

//! Recorder support
#define FMSTR_USE_RECORDER      FMSTR_DEMO_ENOUGH_RAM   //!< Enable/Disable recorder support
#define FMSTR_MAX_REC_VARS      8   //! Max. number of recorder variables (2..8)
#define FMSTR_REC_OWNBUFF       0   //! Use user-allocated rec. buffer (1=yes)

//! Built-in recorder buffer (use when FMSTR_REC_OWNBUFF is 0)
#define FMSTR_REC_BUFF_SIZE     1024    //!< Built-in buffer size

//! Recorder time base, specifies how often the recorder is called in the user app.
#define FMSTR_REC_TIMEBASE      FMSTR_REC_BASE_MILLISEC(0)  //!< 0 = "unknown"
#define FMSTR_REC_FLOAT_TRIG    0   //!< Enable/disable floating point triggering

//!< Target-side address translation (TSA)
#define FMSTR_USE_TSA           0  //!< Enable TSA functionality
#define FMSTR_USE_TSA_INROM     1   //!< TSA tables declared as const (put to ROM)
#define FMSTR_USE_TSA_SAFETY    FMSTR_DEMO_ENOUGH_ROM   //!< Enable/Disable TSA memory protection
#define FMSTR_USE_TSA_DYNAMIC   FMSTR_DEMO_ENOUGH_ROM   //!< Enable/Disable TSA entries to be added also in runtime

//!< Pipes as data streaming over FreeMASTER protocol
#define FMSTR_USE_PIPES         FMSTR_DEMO_ENOUGH_RAM   //!< Enable/Disable pipes
#define FMSTR_MAX_PIPES_COUNT   3   //!< 3 pipes for demo purposes

//!< Enable/Disable read/write memory commands
#define FMSTR_USE_READMEM       1   //!< Enable read memory commands
#define FMSTR_USE_WRITEMEM      1   //!< Enable write memory commands
#define FMSTR_USE_WRITEMEMMASK  1   //!< Enable write memory bits commands

//! Enable/Disable read/write variable commands (a bit faster than Read Mem)
#define FMSTR_USE_READVAR       1   //!< Enable read variable fast commands
#define FMSTR_USE_WRITEVAR      1   //!< Enable write variable fast commands
#define FMSTR_USE_WRITEVARMASK  1   //!< Enable write variable bits fast commands

#endif /* __FREEMASTER_CFG_H */

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
