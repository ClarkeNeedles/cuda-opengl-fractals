/******************************************************************************
 * @file    main.cpp
 * @author  Clarke Needles
 * @brief   Main loop for the fractal generation application.
 ******************************************************************************/

#include "config.h"
#include "app.h"

int main()
{
   App app = App(
      MAX_ITERS, 
      SCREEN_WIDTH, 
      SCREEN_HEIGHT
   );

   app.run();

   return 0;
}