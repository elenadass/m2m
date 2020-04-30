/*
/******************************************************************************/
/*! \file
 *
 **  PROJECT: M2M.2020
 **
 **  NAME: regulator
 **
 **  AUTHOR: Group 4
 **
 ******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include "regulator.h"

/*******************************************************************************
 ** Global variables
 *******************************************************************************/

int main(void)
{
  /* Local variable definition                                        */
  int i32_read_value = 0;
  float kp = 4;
  float ki = 0.2;
  float kd = 0;
  int last_error = 0;
  int error = 0;
  long long int pid =0;
  int input_Mhz = 6600;
  float integral = 0;
  float derivative = 0;
  int output = 0; 
  //int integral = 0;
    

output =  f_get_motor_values(input_Mhz + pid);
printf("%d 1\\", output);
fflush(stdout);

while(1)
{
  
   scanf("%d",&i32_read_value);   
   fprintf(stderr, "Read value %d\n", i32_read_value);

   /* PID is used to calculate the error                              */
   error = input_Mhz - i32_read_value;
   fprintf(stderr, "Error: %d\n", error);
  
   integral = (integral + error);
   
   derivative = (error - last_error);
   last_error = error;
   
   pid = (int)(kp * error + ki*integral + kd*derivative);
   
   fprintf(stderr, "PID: %d\n", pid);


    /* Transform the value the motor to get the value in the range,
     * adjusting the value according to the PID result                */
   output = (int)f_get_motor_values(pid);
   
   
   /* If output is out of range, it is adapted                        */
   
   if(output > 1237)
   {
     output = 1236;
   }
   else if(output < 237)
   {
     output = 238;
   }
   fprintf(stderr, "Output: %d\n", output);
   /* Print the output for the motor                                  */
   printf("%d 1\\", output);
   fflush(stdout);
}
 

  return EXIT_SUCCESS;
}/* main                                                              */

/***********************************************************************
 ** FUNCTION: f_get_motor_values
 ** INPUTS:
 **  input_value: MHz
 ** OUTPUTS:
 **  int: motor value
 ** COMMENTS:
 ** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 **  Calculate the output for the motor according to the MHz
 **********************************************************************/
 int f_get_motor_values (long long int input_value)
{
  int motor_value = 0;
  
  if(input_value>0)
  {
    motor_value=(input_value+15262)/19.8;
  }
  else if(input_value<0)
  {
    motor_value=(input_value+7870)/10;
  }
  else
  {
    motor_value=737;
  }
  
  return motor_value;
}/* f_get_motor_values                                                */


