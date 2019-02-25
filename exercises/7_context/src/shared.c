#include <string.h>
#include "LED_Test.h"
#include <avr/interrupt.h>
#include <util/atomic.h>

/**
 * \file shared.c
 * \brief A Skeleton Implementation of an RTOS
 *
 * \mainpage A Skeleton Implementation of a "Self-Served" RTOS Model
 * This is an example of how to implement context-switching based on a
 * self-served model. That is, the RTOS is implemented by a collection of
 * user-callable functions. The kernel executes its functions using the calling
 * task's stack.
 *
 * \author Dr. Mantis Cheng
 * \date 2 October 2006
 *
 * ChangeLog: Modified by Alexander M. Hoole, October 2006.
 *			  -Rectified errors and enabled context switching.
 *			  -LED Testing code added for development (remove later).
 *
 * \section Implementation Note
 * This example uses the ATMEL AT90USB1287 instruction set as an example
 * for implementing the context switching mechanism.
 * This code is ready to be loaded onto an AT90USBKey.  Once loaded the
 * RTOS scheduling code will alternate lighting of the GREEN LED light on
 * LED D2 and D5 whenever the correspoing PING and PONG tasks are running.
 * (See the file "cswitch.S" for details.)
 */

//Comment out the following line to remove debugging code from compiled version.
#define DEBUG

typedef void (*voidfuncptr) (void);      /* pointer to void f(void) */

#define WORKSPACE     256
#define MAXPROCESS   4


/*===========
  * RTOS Internal
  *===========
  */

/**
  * This internal kernel function is the context switching mechanism.
  * Fundamentally, the CSwitch() function saves the current task CurrentP's
  * context, selects a new running task, and then restores the new CurrentP's
  * context.
  * (See file "switch.S" for details.)
  */
void CSwitch();

/* Prototype */
void Task_Terminate(void);

/**
  * Exit_kernel() is used when OS_Start() or Task_Terminate() needs to
  * switch to a new running task.
  */
void Exit_Kernel();

#define Disable_Interrupt()         asm volatile ("cli"::)
#define Enable_Interrupt()          asm volatile ("sei"::)

/**
  *  This is the set of states that a task can be in at any given time.
  */
typedef enum process_states
{
  DEAD = 0,
  READY,
  RUNNING
} PROCESS_STATES;


/**
  * Each task is represented by a process descriptor, which contains all
  * relevant information about this task. For convenience, we also store
  * the task's stack, i.e., its workspace, in here.
  * To simplify our "CSwitch()" assembly code, which needs to access the
  * "sp" variable during context switching, "sp" MUST BE the first entry
  * in the ProcessDescriptor.
  * (See file "cswitch.S" for details.)
  */
typedef struct ProcessDescriptor
{
  unsigned char *sp;
  unsigned char workSpace[WORKSPACE];
  PROCESS_STATES state;
} PD;

/**
  * This table contains ALL process descriptors. It doesn't matter what
  * state a task is in.
  */
static PD Process[MAXPROCESS];

/**
  * The process descriptor of the currently RUNNING task.
  */
  //??? Removed static because it was blocking external access.
  //??? Rename Cp to CurrentP because 'cp' is reserved in assembly.
volatile PD* CurrentP;

/** index to next task to run */
volatile static unsigned int NextP;

/** 1 if kernel has been started; 0 otherwise. */
volatile static unsigned int KernelActive;

/** number of tasks created so far */
volatile static unsigned int Tasks;



/**
 * When creating a new task, it is important to initialize its stack just like
 * it has called "Enter_Kernel()"; so that when we switch to it later, we
 * can just restore its execution context on its stack.
 * (See file "cswitch.S" for details.)
 */
void Kernel_Create_Task_At( PD *p, voidfuncptr f ) {
   unsigned char *sp;
#ifdef DEBUG
   int counter = 0;
#endif

   sp = (unsigned char *) &(p->workSpace[WORKSPACE-1]);

   /*----BEGIN of NEW CODE----*/
   //Initialize the workspace (i.e., stack) and PD here!

   //Clear the contents of the workspace
   memset(&(p->workSpace),0,WORKSPACE);

   //Notice that we are placing the address (16-bit) of the functions
   //onto the stack in reverse byte order (least significant first, followed
   //by most significant).  This is because the "return" assembly instructions
   //(rtn and rti) pop addresses off in BIG ENDIAN (most sig. first, least sig.
   //second), even though the AT90 is LITTLE ENDIAN machine.

   //Store terminate at the bottom of stack to protect against stack underrun.
   *(unsigned char *)sp-- = ((unsigned int)Task_Terminate) & 0xff;
   *(unsigned char *)sp-- = (((unsigned int)Task_Terminate) >> 8) & 0xff;
   *(unsigned char *)sp-- = (((unsigned int)Task_Terminate) >> 16) & 0xff;

   //Place return address of function at bottom of stack
   *(unsigned char *)sp-- = ((unsigned int)f) & 0xff;
   *(unsigned char *)sp-- = (((unsigned int)f) >> 8) & 0xff;
   *(unsigned char *)sp-- = (((unsigned int)f) >> 16) & 0xff;

#ifdef DEBUG
   //Fill stack with initial values for development debugging
   //Registers 0 -> 31 and the status register and EIND
   for (counter = 0; counter < 34; counter++)
   {
      *(unsigned char *)sp-- = counter;
   }
#else
   //Place stack pointer at top of stack
   sp = sp - 34;
#endif

   p->sp = sp;		/* stack pointer into the "workSpace" */

   /*----END of NEW CODE----*/



   p->state = READY;
}


/**
  *  Create a new task
  */
static void Kernel_Create_Task( voidfuncptr f ) {
   int x;

   if (Tasks == MAXPROCESS) return;  /* Too many task! */

   /* find a DEAD PD that we can use  */
   for (x = 0; x < MAXPROCESS; x++) {
       if (Process[x].state == DEAD) break;
   }

   ++Tasks;
   Kernel_Create_Task_At( &(Process[x]), f );
}

/**
  * This internal kernel function is a part of the "scheduler". It chooses the
  * next task to run, i.e., CurrentP.
  */
  //Remobed static because it was blocking external access from assembly file cswitch.S.
  //We desire to see a 'T' not a 't' in the avr-nm output from the object file.
void Dispatch() {
     /* find the next READY task
       * Note: if there is no READY task, then this will loop forever!.
       */
   while(Process[NextP].state != READY) {
      NextP = (NextP + 1) % MAXPROCESS;
   }

     /* we have a new CurrentP */
   CurrentP = &(Process[NextP]);
   CurrentP->state = RUNNING;

   //Moved to bottom (this was in the wrong place).
   NextP = (NextP + 1) % MAXPROCESS;
}


/*================
  * RTOS  API  and Stubs
  *================
  */

/**
  * This function initializes the RTOS and must be called before any other
  * system calls.
  */
void OS_Init() {
   int x;

   Tasks = 0;
   KernelActive = 0;
   NextP = 0;

   for (x = 0; x < MAXPROCESS; x++) {
      memset(&(Process[x]),0,sizeof(PD));
      Process[x].state = DEAD;
   }
}


/**
  * This function starts the RTOS after creating a few tasks.
  */
void OS_Start() {
   if ( (! KernelActive) && (Tasks > 0)) {
      Disable_Interrupt();

      /* here we go...  */
      KernelActive = 1;
      asm ( "jmp Exit_Kernel":: );
   }
}


/**
  * For this example, we only support cooperatively multitasking, i.e.,
  * each task gives up its share of the processor voluntarily by calling
  * Task_Next().
  */
void Task_Create( voidfuncptr f) {
   Disable_Interrupt();
   Kernel_Create_Task( f );
   Enable_Interrupt();
}

/**
  * The calling task gives up its share of the processor voluntarily.
  */
void Task_Next() {
   if (KernelActive) {
     Disable_Interrupt();
     CurrentP ->state = READY;
     CSwitch();
     /* resume here when this task is rescheduled again later */
     Enable_Interrupt();
  }
}

/**
  * The calling task terminates itself.
  */
void Task_Terminate() {
   if (KernelActive) {
      Disable_Interrupt();
      CurrentP -> state = DEAD;
        /* we will NEVER return here! */
      asm ( "jmp Exit_Kernel":: );
   }
}


/*============
  * A Simple Test
  *============
  */

/**
  * A cooperative "Ping" task.
  * Added testing code for LEDs.
  */
void Ping() {
  int x ;
  Enable_Interrupt();

  for(;;) {
  	//LED RX on
    // enable_LED_RX();
    PORTE = 0b00100000;

    // for( x=0; x < 32000; ++x );   /* do nothing */
    // for( x=0; x < 32000; ++x );   /* do nothing */
    // for( x=0; x < 32000; ++x );   /* do nothing */
    // for( x=0; x < 32000; ++x );   /* do nothing */

	//LED off
	// disable_LEDs();
    // PORTE = 0x00;

    // printf( "*" );
    // Task_Next();
  }
}


/**
  * A cooperative "Pong" task.
  * Added testing code for LEDs.
  */
void Pong() {
  int x;
  Enable_Interrupt();

  for(;;) {
	//LED TX on
	// enable_LED_TX();
    PORTE = 0b00010000;

    // for( x=0; x < 32000; ++x );   /* do nothing */
    // for( x=0; x < 32000; ++x );   /* do nothing */
    // for( x=0; x < 32000; ++x );   /* do nothing */
    // for( x=0; x < 32000; ++x );   /* do nothing */

	//LED off
	// disable_LEDs();
    // PORTE = 0x00;

    // printf( "." );
    // Task_Next();
  }
}

#define TIMER_COUNT 250
#define BV(bit) (1 << (bit))

/*
 *  Timer ISR
 */
ISR (TIMER1_COMPA_vect) {
    static volatile unsigned char milli_count = 0;

    // disable interrupts
    Disable_Interrupt();

    // Change time
    milli_count++;
    if (milli_count == 10) {
        milli_count = 0;
        // PORTE = PORTE ? 0x00 : 0xFF;
        Task_Next();
    }

    // enable interrupts
    Enable_Interrupt();
}

/**
  * This function creates two cooperative tasks, "Ping" and "Pong". Both
  * will run forever.
  */
int main() {
   OS_Init();

//    init_LED_MAIN();
//    enable_LED_MAIN();

   DDRE = 0xFF;
   PORTE |= 0b00000000;

   // Configure ISR
   Disable_Interrupt();
   // We are using timer 1
   // Configuring timer counter control register A and B
   TCCR1A = 0x00;                 // Clear control register A
   TCCR1B = 0x00;                 // Clear control register B
   TCNT1  = 0x00;                 // Clear the counter
   OCR1A  = TIMER_COUNT;          // The value we are waiting for
   TCCR1B |= BV(WGM12);           // Use CTC mode
   TCCR1B |= BV(CS11) | BV(CS10); // Scale by 64
   TIMSK1 |= BV(OCIE1A);          // Enable timer compare interrupt
   Enable_Interrupt();

    // for (;;) {}

   Task_Create( Pong );
   Task_Create( Ping );

   OS_Start();
}

