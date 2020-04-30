/*
/******************************************************************************/
/*! \file
 *
 **  PROJECT: M2M.2020
 **
 **  NAME: encoder.h
 **
 **  AUTHOR: Group 4
 **
 ******************************************************************************/
 #include <pthread.h>
/* ! Sampling timer in us                                                     */
#define TIMER (unsigned long  int) 20000



/*******************************************************************************
 **  Structure declaration
 ******************************************************************************/
typedef struct _s_encoder_data
{
    /* Flash counter                                                          */
    unsigned long  int u32_encoder_counter;
    /* Spinning direction                                                     */
    unsigned       char u8_direction;
    /* Mutex                                                                  */
    pthread_mutex_t mutex;

}s_encoder_data;

/*******************************************************************************
 **  Function declaration
 ******************************************************************************/
/*!  Encoder function. Set a timer and check the number of the encoder counter
 * when the timer is up
ps_data: input data                                                           */
void* encoder(void);

/*!  Poll function. Configure the IRQ associate to one of the diods and polling
 * it incrementing a counter
 * ps_data: input data                                                        */
void* poll_irq(void);

/*!  Initializate GPIO for the encoder                                        */
void init_encoder(void);


