/*
   main.c

   NuSYSTEM sample nu2

   Copyright (C) 1997-1999, NINTENDO Co,Ltd.	
   */

#include <nusys.h>
#include "main.h"

#include "game.h"

#include <malloc.h>

/* The stage number  */
volatile int stage;

/* Declaration of the prototype  */
void stage00(int);
void stage01(int);

/* Declaration of the external function  */
void initStage00(void);
void makeDL00(void);
void updateGame00(void);

void initStage01(void);
void makeDL01(void);
void updateGame01(void);

NUContData	contdata[1]; /* Read data of 1 controller  */

Game* game = NULL;

void nusys_game_update(int pendingGfx);

char memory_heap[1024*512*1];

void mainproc(void) {
  nuGfxInit();
  InitHeap(memory_heap, 1024*512*1);
  nuContInit();
  game = game_create();
  nuGfxFuncSet((NUGfxFunc)nusys_game_update);
  nuGfxDisplayOn();
}

///*------------------------
//	Main 
//--------------------------*/
//void mainproc(void)
//{
//  /* The initialization of graphic  */
//  nuGfxInit();
//  InitHeap(memory_heap, 1024*512*1);
//
//  /* The initialization of the controller manager  */
//  nuContInit();
//  
//
//  game = game_create();
//	
//  /* Set the stage number to 0 */
//  stage = 0;
//
//  while(1)
//    {
//      switch(stage)
//	{
//	  /* 
//	     Register the corresponding call-back function according to the 
//	     stage number.  
//		 The call-back function sets the value to the stage when another 
//	     call-back function needs the register change.  
//	     */
//	case 0:
//	  /* Set the stage value to -1 first, to wait for that the call-back 
//	     function sets the value */
//	  stage = -1;
//	  /* The initialization of stage 0  */
//	  initStage00();
//	  /* The call-back register  */
//	  nuGfxFuncSet((NUGfxFunc)stage00);
//	  /* Start to display  */
//	  nuGfxDisplayOn();
//	  break;
//	case 1:
//	  stage = -1;
//	  initStage01();
//	  nuGfxFuncSet((NUGfxFunc)stage01);
//	  nuGfxDisplayOn();
//	  break;
//	default:
//	  break;
//	}
//      
//      /* Wait for that the call-back function switches values (switch the scene) */
//      while(stage == -1)
//	;
//      /* Clear the display  */
//      nuGfxDisplayOff();
//    }
//}
//
//
///*-----------------------------------------------------------------------------
//  The call-back function 
//
//  pendingGfx which is passed from Nusystem as the argument of the call-back   
//  function is the total number of RCP tasks that are currently processing
//  and waiting for the process. 
//-----------------------------------------------------------------------------*/

void nusys_game_update(int pendingGfx) {
  if (pendingGfx < 1) {
    game_update(game, 0.0f);
    game_draw(game);
  }
}

///* Stage 0 */
//void stage00(int pendingGfx)
//{
//  /* Provide the display process if 2 or less RCP tasks are processing or 
//	 waiting for the process.  */
//  if(pendingGfx < 3)
//    makeDL00();		
//
//  /* The process of game progress  */
//  updateGame00(); 
//}
//
///* Stage 1 */
//void stage01(int pendingGfx)
//{
//  /* Provide the display process if 2 or less RCP tasks processing or 
//	waiting for the process.   */
//  if(pendingGfx < 3)
//    makeDL01();
//
//  /* The process of game progress  */
//  updateGame01();
//}
