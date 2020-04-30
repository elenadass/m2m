/*
/******************************************************************************/
/*! \file
 *
 **  PROJECT: M2M.2020
 **
 **  NAME: encoder.c
 **
 **  AUTHOR: Group 4
 **
 ******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <poll.h>
#include <pthread.h>
#include "encoder.h"

/*******************************************************************************
 ** Global variables
 *******************************************************************************/
s_encoder_data vg_encounter_data =
{
  /* Global variable initialiazation                                          */
  0,
  2,
  PTHREAD_MUTEX_INITIALIZER,
};

int main(void)
{
  /* Local variable definition                                                */
  pthread_t thread1, thread2;
  int iret1, iret2;

  /* Create two threads: encoder and polling GPIO                              */
  iret1 = pthread_create(&thread1, NULL, (void*)encoder, NULL);
  iret2 = pthread_create(&thread2, NULL, (void*)poll_irq, NULL);

  /* Wait until threads are complete before main continues                     */
  pthread_join(thread1,0);
  pthread_join(thread2,0);

  return EXIT_SUCCESS;
}/* main                                                                      */

/*******************************************************************************
 ** FUNCTION: init_encoder
 ** INPUTS:
 **  None
 ** OUTPUTS:
 **  None
 ** COMMENTS:
 ** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 **  Initializate GPIO for the encoder
 ******************************************************************************/
void init_encoder(void)
{
  /* Configure PIN 9 as GPIO associate to the IRQ and PIN9 to check spinning
   * direction                                                                */
  system("echo 9 > /sys/class/gpio/unexport;\
   echo 9 > /sys/class/gpio/export;\
   echo in > /sys/class/gpio/gpio9/direction;\
   echo rising > /sys/class/gpio/gpio9/edge;\
   echo 10 > /sys/class/gpio/unexport;\
   echo 10 > /sys/class/gpio/export;\
   echo in > /sys/class/gpio/gpio10/direction;\
   echo none > /sys/class/gpio/gpio10/edge");

  return;
}/* init_encoder                                                              */

/*******************************************************************************
 ** FUNCTION: encoder
 ** INPUTS:
 **  ps_data: encoder data
 ** OUTPUTS:
 **  None
 ** COMMENTS:
 ** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 **  Set a timer and check the number of the encoder counter
 **  when the timer is up
 ******************************************************************************/
void* encoder(void)
{
  unsigned long int u32_total_cycles = 0;
  int i32_cycles_mhz   = 0;
  int fd2 = 0;
   int direction = 1; 
   int counter = 0; 

  //printf("Encoder\n");
  while(1)
  {
    u32_total_cycles = 0;
    usleep(TIMER);

    /* get the mutex                                                          */
    pthread_mutex_lock (&vg_encounter_data.mutex);
    /* Read the counter                                                       */
    u32_total_cycles = vg_encounter_data.u32_encoder_counter;
    /* Reset the counter                                                      */
    vg_encounter_data.u32_encoder_counter = 0;
    /* Unlocked the mutex                                                     */
    pthread_mutex_unlock (&vg_encounter_data.mutex);
    
    if(direction != vg_encounter_data.u8_direction)
    {
      counter++;
      if(counter > 5)
      {
        direction = (direction + 1)%2;
      }
    }
    else
    {
      counter = 0;
    }
    
    if(direction == 1)
    {
      /* Compute result                                                         */
      i32_cycles_mhz=u32_total_cycles*100;
    }
    else
    {
      /* Compute result                                                         */
      i32_cycles_mhz= -(u32_total_cycles*100);
    }
       /* Write result in output file                                         */
    //printf("%ld\n", vg_encounter_data.u8_direction);
    /* Write result in output file                                            */
    printf("%d\n", i32_cycles_mhz);
    fflush(stdout);
  }
}/* encoder                                                                   */

/*******************************************************************************
 ** FUNCTION: poll_irq
 ** INPUTS:
 **  ps_data: encoder data
 ** OUTPUTS:
 **  None
 ** COMMENTS:
 ** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 **  Configure the IRQ associate to one of the diods and polling
 ** it incrementing a counter
 ******************************************************************************/
void* poll_irq(void)
{
  /* Local variable definition                                                */
  struct pollfd pfd0;
  unsigned char u8_value = 0;
  int fd1 = 0;

  /* Configuring GPIO                                                         */
  init_encoder();

  /* Configuring POLL events                                                   */
  pfd0.events = POLLPRI | POLLERR;
  pfd0.fd = open("/sys/class/gpio/gpio9/value", O_RDONLY|O_NONBLOCK);
  fd1 = open("/sys/class/gpio/gpio10/value", O_RDONLY | O_NONBLOCK);

  //printf("POLL IRQ\n");
  while(1)
  {
    /* Read from the beginning (offset 0 )                                    */
    lseek(pfd0.fd, 0, SEEK_SET);
    /* Empty the read buffer                                                  */
    read (pfd0.fd, &u8_value, 1);
    /* Wait until the next interruption. This function blocks the process     */
    poll(&pfd0, 1, 1000);

    /* Read PIN10 for direction                                               */
    lseek(fd1, 0, SEEK_SET);
    /* read the value                                                         */
    read (fd1, &u8_value, 1);
    
    /* Get the mutex                                                          */
    pthread_mutex_lock (&vg_encounter_data.mutex);

    
    /* Increase the counter                                                   */
    vg_encounter_data.u32_encoder_counter++;

    /*Release the mutex                                                       */
    pthread_mutex_unlock (&vg_encounter_data.mutex);
  

    /* Check if the direction has changed                                     */
    if(vg_encounter_data.u8_direction != (u8_value%2))
    {
        vg_encounter_data.u8_direction = u8_value%2;
      
    }
  }
}/* poll                                                                      */
