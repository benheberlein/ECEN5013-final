/******************************************************************************
* Copyright (C) 2017 by Ben Heberlein
*
* Redistribution, modification or use of this software in source or binary
* forms is permitted as long as the files maintain this copyright. This file
* was created for the University of Colorado Boulder course Advanced Practical
* Embedded Software Development. Ben Heberlein and the University of Colorado 
* are not liable for any misuse of this material.
*
*******************************************************************************/
/**
 * @file defs.h
 * @brief Global definitions for system
 * 
 * This file contains global definitions like device and task IDs.
 *
 * @author Ben Heberlein
 * @date Nov 2 2017
 * @version 1.0
 *
 */

#ifndef __DEFS_H__
#define __DEFS_H__

#define DEFS_ID_BBG         0
#define DEFS_ID_TIVA        1
#define DEFS_TASK_TIVA      0
#define DEFS_TASK_TEMP      1
#define DEFS_TASK_GAS       2
#define DEFS_TASK_BBG       0
#define DEFS_TASK_SPEAK     1
#define DEFS_TASK_LOG       2

#define DEFS_NUM_TASKS_TIVA 3
#define DEFS_NUM_TASKS_BBG  3

#endif /*__DEFS_H__*/
