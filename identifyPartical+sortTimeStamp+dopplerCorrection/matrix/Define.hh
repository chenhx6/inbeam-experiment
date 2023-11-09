/*
 * @Date: 2023-11-07 01:43:26
 * @LastEditTime: 2023-11-09 08:15:20
 */
#ifndef _DEFINE_H_
#define _DEFINE_H_

/**
 * // raw_file
 * -=> TString::Format("%s%s_%d_W%d%s%s", RAWFILEPATH, RAWFILENAME, run, EVENTTIMEWINDOWSWIDTH, ADDBACKFILE, DOPPLERFILE)
 * !!!      "%s%s_%d_W%d%s%s"       !!!
 * if raw file have doppler -= > open elsewise close
 * if raw file have addback -=> open elsewise close
 * tree(name) -=> HIT[HITLENGTH](title)
 */
/* RAW FILE*/
#define RAWFILEPATH "/mnt/e/GC_event/"
#define RAWFILENAME "event_hit"
#define DOPPLERFILE "_doppler"
#define ADDBACKFILE "_addback"

#define DOPPLERCORRECTION
#define ADDBACK
#define EVENTTIMEWINDOWSWIDTH 1000
#define HITLENGTH 100
/* RAW FILE*/

/*ROOT FILE MATRIX*/
#define ROOTFILEPATH "/mnt/e/GC_matrix/"
#define ROOTFILENAME "matrix"
/*ROOT FILE MATRIX*/

/**  | - AM - | - PM - | - AM -|
 *   | - - - - - PAM - - - - - |
 *  PM(瞬时符合矩阵) = PAM(全矩阵) - BM(本底矩阵) - AM(瞬时偶然符合矩阵)
 */
/*coincidence windows*/
/*prompt + accidental windows*/
// PAM WINDOWS
#define CSICSICOINWINDOWS 40  // function -=> TcoinLR
#define GEGECOINWINDOWS 70    // function -=> TcoinLR
#define GECSICOINWINDOWSL -70 // function -=> TcoinL
#define GECSICOINWINDOWSR 200 // function -=> TcoinR

/*prompt + accidental windows*/
/*accidental windows: be expected to decline x-ray or coulomp excitation*/
// AM WINDOWS
#define GEGE // switch about two fold matrix
// #define GEGEAM // switch
#define GEGEAMWINDOWL1 -150
#define GEGEAMWINDOWR1 -100
#define GEGEAMWINDOWL2 100
#define GEGEAMWINDOWR2 150

/*accidental windows*/
/*coincidence windows*/
/*histogram*/
#define EBINNUMBER 2000 // energy hitstogram bin numbers
#define EMIN 0          // minimum energy
#define EMAX 2000       // maximum energy
#define LEASTENTRY 0

#define TOTALPTCMIN 0 //
#define TOTALPTCMAX 8
#define ALPHAPTCMIN 0
#define ALPHAPTCMAX 5
#define PROTONPTCMIN 0
#define PROTONPTCMAX 8

/*histogram*/
/*three-foder*/
// #define CUBEMATRIX
#define CUBEMATRIXAM // switch, minus AM of cube matrix, if you want to use it, you must open CUBEMATRIX
/*three-foder*/

#endif
